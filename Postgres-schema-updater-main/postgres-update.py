#!/usr/bin/env python3

# This script is designed to manage PostgreSQL database schema updates. 
# It includes functionalities such as:
# - Parsing environment variables from a .env file to retrieve database credentials.
# - Checking for the existence of specified databases.
# - Creating new databases if they do not already exist.
# - Extracting SQL statements from schema files for execution.
# - Filtering existing database objects to avoid conflicts during updates.
# - Modifying SQL statements to ensure safety during execution.
# - Backing up the current database state before applying updates.
# - Applying schema updates to the database based on the provided SQL statements.

# **Setup Instructions:**
# 1. Create a file named `.env` in the same directory as this script.
# 2. Open the `.env` file and paste the following format:
#    ```
#    DATABASE_URL=postgres://<username>:<password>@<host>/<dbname>
#    ```
#    Replace `<username>`, `<password>`, `<host>`, and `<dbname>` with your PostgreSQL credentials.
# 3. Save the `.env` file.
# 4. **Important Note:** Do not run this script in a production environment without testing it first. Always ensure you have backups and test in a safe environment before applying changes.
#
# This script is free to use and modify. There is no copyright and no warranty.


import os
import sys
import subprocess
import platform
import getpass
import time
from datetime import datetime
import re
import tempfile
import difflib

def parse_env_file():
    if not os.path.exists(".env"):
        return None
    
    try:
        with open(".env", "r") as f:
            for line in f:
                if line.startswith('DATABASE_URL='):
                    url = line.strip().split('DATABASE_URL=')[1]
                    match = re.search(r'postgres://([^:]+):([^@]+)@([^/]+)/([^\s]+)', url)
                    if match:
                        return {
                            'user': match.group(1),
                            'password': match.group(2),
                            'host': match.group(3),
                            'dbname': match.group(4)
                        }
    except Exception as e:
        print(f"Error reading .env file: {e}")
    return None

def run_command(command, shell=False, capture_stderr=False):
    try:
        stderr = subprocess.PIPE if capture_stderr else None
        result = subprocess.run(
            command,
            shell=shell,
            check=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=stderr
        )
        return result.stdout
    except subprocess.CalledProcessError as e:
        if capture_stderr:
            print(f"Command failed: {e.stderr}")
        else:
            print(f"Command failed: {e}")
        return None

def check_database_exists(db_user, db_password, db_name, host='localhost'):
    cmd = f"PGPASSWORD='{db_password}' psql -U {db_user} -h {host} -lqt | cut -d \\| -f 1 | grep -w {db_name}"
    result = run_command(cmd, shell=True)
    return result is not None and db_name in result

def create_database(db_user, db_password, db_name, host='localhost'):
    if not check_database_exists(db_user, db_password, db_name, host):
        cmd = f"PGPASSWORD='{db_password}' createdb -U {db_user} -h {host} {db_name}"
        return run_command(cmd, shell=True) is not None
    return True

def extract_statements(schema_content):
    statements = []
    current_statement = []
    in_function_body = False
    
    for line in schema_content.splitlines():
        line = line.strip()
        if not line or line.startswith('--'):
            continue
            
        if line.startswith('CREATE OR REPLACE FUNCTION') or line.startswith('CREATE FUNCTION'):
            if current_statement:
                statements.append('\n'.join(current_statement))
                current_statement = []
            current_statement.append(line)
            in_function_body = True
        elif in_function_body:
            current_statement.append(line)
            if line.endswith('LANGUAGE plpgsql;'):
                statements.append('\n'.join(current_statement))
                current_statement = []
                in_function_body = False
        elif line.startswith('CREATE'):
            if current_statement:
                statements.append('\n'.join(current_statement))
                current_statement = []
            current_statement.append(line)
        elif current_statement:
            current_statement.append(line)
            if line.endswith(';') and not in_function_body:
                statements.append('\n'.join(current_statement))
                current_statement = []
    
    if current_statement:
        statements.append('\n'.join(current_statement))
    
    return statements

def get_existing_objects(db_user, db_password, db_name, host='localhost'):
    objects = {
        'tables': set(),
        'functions': set(),
        'triggers': set(),
        'indexes': set(),
        'constraints': set(),
        'policies': set(),
        'types': set()
    }
    
    cmd = f"PGPASSWORD='{db_password}' psql -U {db_user} -h {host} -d {db_name} -t -c \"SELECT tablename FROM pg_tables WHERE schemaname = 'public';\""
    result = run_command(cmd, shell=True)
    if result:
        objects['tables'].update(line.strip() for line in result.splitlines() if line.strip())
    
    cmd = f"PGPASSWORD='{db_password}' psql -U {db_user} -h {host} -d {db_name} -t -c \"SELECT proname FROM pg_proc WHERE pronamespace = (SELECT oid FROM pg_namespace WHERE nspname = 'public');\""
    result = run_command(cmd, shell=True)
    if result:
        objects['functions'].update(line.strip() for line in result.splitlines() if line.strip())
    
    cmd = f"PGPASSWORD='{db_password}' psql -U {db_user} -h {host} -d {db_name} -t -c \"SELECT tgname FROM pg_trigger;\""
    result = run_command(cmd, shell=True)
    if result:
        objects['triggers'].update(line.strip() for line in result.splitlines() if line.strip())
    
    cmd = f"PGPASSWORD='{db_password}' psql -U {db_user} -h {host} -d {db_name} -t -c \"SELECT indexname FROM pg_indexes WHERE schemaname = 'public';\""
    result = run_command(cmd, shell=True)
    if result:
        objects['indexes'].update(line.strip() for line in result.splitlines() if line.strip())
    
    cmd = f"PGPASSWORD='{db_password}' psql -U {db_user} -h {host} -d {db_name} -t -c \"SELECT polname FROM pg_policy;\""
    result = run_command(cmd, shell=True)
    if result:
        objects['policies'].update(line.strip() for line in result.splitlines() if line.strip())
    
    cmd = f"PGPASSWORD='{db_password}' psql -U {db_user} -h {host} -d {db_name} -t -c \"SELECT typname FROM pg_type WHERE typtype = 'e' AND typnamespace = (SELECT oid FROM pg_namespace WHERE nspname = 'public');\""
    result = run_command(cmd, shell=True)
    if result:
        objects['types'].update(line.strip() for line in result.splitlines() if line.strip())
    
    return objects

def filter_statements(statements, existing_objects):
    filtered_statements = []
    for stmt in statements:
        skip = False
        
        if 'CREATE INDEX' in stmt:
            index_name = re.search(r'CREATE INDEX (?:IF NOT EXISTS )?(?:public\.)?(\w+)', stmt)
            if index_name and index_name.group(1) in existing_objects['indexes']:
                skip = True
        
        elif 'CREATE TABLE' in stmt:
            table_name = re.search(r'CREATE TABLE (?:IF NOT EXISTS )?(?:public\.)?(\w+)', stmt)
            if table_name and table_name.group(1) in existing_objects['tables']:
                skip = True
        
        elif 'CREATE FUNCTION' in stmt or 'CREATE OR REPLACE FUNCTION' in stmt:
            func_name = re.search(r'CREATE (?:OR REPLACE )?FUNCTION (?:public\.)?(\w+)', stmt)
            if func_name and func_name.group(1) in existing_objects['functions']:
                if not 'OR REPLACE' in stmt:
                    skip = True
        
        elif 'CREATE TRIGGER' in stmt:
            trigger_name = re.search(r'CREATE TRIGGER (?:public\.)?(\w+)', stmt)
            if trigger_name and trigger_name.group(1) in existing_objects['triggers']:
                skip = True
        
        elif 'CREATE POLICY' in stmt:
            policy_match = re.search(r'CREATE POLICY (?:public\.)?(\w+) ON (\w+)', stmt)
            if policy_match and policy_match.group(1) in existing_objects['policies']:
                skip = True
        
        elif 'CREATE TYPE' in stmt:
            type_match = re.search(r'CREATE TYPE (?:public\.)?(\w+)', stmt)
            if type_match and type_match.group(1) in existing_objects['types']:
                skip = True
        
        if not skip:
            filtered_statements.append(stmt)
    
    return filtered_statements

def modify_statements_for_safety(statements):
    modified_statements = []
    
    for stmt in statements:
        if stmt.startswith('CREATE INDEX') and 'IF NOT EXISTS' not in stmt:
            stmt = stmt.replace('CREATE INDEX', 'CREATE INDEX IF NOT EXISTS')
        
        elif stmt.startswith('CREATE TABLE') and 'IF NOT EXISTS' not in stmt:
            stmt = stmt.replace('CREATE TABLE', 'CREATE TABLE IF NOT EXISTS')
        
        elif stmt.startswith('CREATE TYPE') and 'IF NOT EXISTS' not in stmt and 'AS ENUM' in stmt:
            stmt = stmt.replace('CREATE TYPE', 'CREATE TYPE IF NOT EXISTS')
            
        elif 'CREATE EXTENSION' in stmt and 'IF NOT EXISTS' not in stmt:
            stmt = stmt.replace('CREATE EXTENSION', 'CREATE EXTENSION IF NOT EXISTS')
            
        modified_statements.append(stmt)
    
    return modified_statements

def backup_database(db_user, db_password, db_name, host='localhost'):
    print("Creating database backup...")
    
    if not os.path.exists("backups"):
        os.makedirs("backups")
    
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    backup_file = f"backups/nuvex_backup_{timestamp}.sql"
    
    backup_cmd = f"PGPASSWORD='{db_password}' pg_dump -U {db_user} -h {host} {db_name} -F p -f {backup_file}"
    result = run_command(backup_cmd, shell=True)
    
    if os.path.exists(backup_file):
        print(f"Database backup created successfully: {backup_file}")
        return backup_file
    else:
        print("Failed to create database backup")
        return None

def apply_schema_updates(db_user, db_password, db_name, schema_file, host='localhost'):
    print("\nAnalyzing current database schema...")
    
    if not create_database(db_user, db_password, db_name, host):
        print("Failed to create database")
        return False
    
    try:
        with open(schema_file, 'r') as f:
            schema_content = f.read()
    except Exception as e:
        print(f"Error reading schema file: {e}")
        return False
    
    existing_objects = get_existing_objects(db_user, db_password, db_name, host)
    
    statements = extract_statements(schema_content)
    
    filtered_statements = filter_statements(statements, existing_objects)
    
    safe_statements = modify_statements_for_safety(filtered_statements)
    
    if not safe_statements:
        print("Database schema is up to date.")
        return True
    
    print("\nThe following schema changes will be applied:")
    for i, stmt in enumerate(safe_statements, 1):
        print(f"\n{i}. {stmt}")
    
    if input("\nDo you want to proceed? (y/N): ").lower() != 'y':
        print("Update cancelled.")
        return False
    
    backup_file = backup_database(db_user, db_password, db_name, host)
    if not backup_file:
        print("Aborting update due to backup failure")
        return False
    
    print("\nApplying schema updates...")
    success_count = 0
    error_count = 0
    
    for stmt in safe_statements:
        with tempfile.NamedTemporaryFile(mode='w', suffix='.sql', delete=False) as temp:
            temp.write(stmt)
            temp_path = temp.name
        
        cmd = f"PGPASSWORD='{db_password}' psql -U {db_user} -h {host} {db_name} -f {temp_path}"
        result = run_command(cmd, shell=True, capture_stderr=True)
        
        try:
            os.unlink(temp_path)
        except:
            pass
        
        if result is not None:
            success_count += 1
        else:
            error_count += 1
            print(f"Warning: Failed to apply statement: {stmt[:100]}...")
    
    if error_count == 0:
        print(f"\nSchema updates completed successfully! Applied {success_count} changes.")
        return True
    else:
        print(f"\nSchema updates completed with warnings: {success_count} successes, {error_count} errors.")
        return success_count > 0

def main():
    print("=== Nuvex Database Schema Update Tool ===")
    
    if not os.path.exists("Nuvex.sql"):
        print("Error: Nuvex.sql not found")
        sys.exit(1)
    
    env_creds = parse_env_file()
    if env_creds:
        db_user = env_creds['user']
        db_password = env_creds['password']
        db_name = env_creds['dbname']
        host = env_creds['host']
        print("Using database credentials from .env file")
    else:
        db_user = input("Enter PostgreSQL username: ")
        db_password = getpass.getpass("Enter PostgreSQL password: ")
        db_name = input("Enter database name: ")
        host = input("Enter database host (default: localhost): ") or "localhost"
    
    if apply_schema_updates(db_user, db_password, db_name, "Nuvex.sql", host):
        print("\n=== Update Complete ===")
        print(f"Database: {db_name}")
        print(f"Username: {db_user}")
        print("Backup files can be found in the 'backups' directory")
    else:
        print("\n=== Update Failed ===")
        print("Please check the error messages above")

if __name__ == "__main__":
    main()