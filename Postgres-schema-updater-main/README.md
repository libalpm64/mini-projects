# Postgres-schema-updater
<img src="https://tokei.rs/b1/github/libalpm64/Postgres-schema-updater?category=code&style=flat" alt="Lines of Code"/>

Manage PostgreSQL database schema updates without any data loss and easy to use. Update your database easily and backup your database with automatic failover.

## It includes functionalities such as:
- Parsing environment variables from a `.env` file to retrieve database credentials.
- Checking for the existence of specified databases.
- Creating new databases if they do not already exist.
- Extracting SQL statements from schema files for execution.
- Filtering existing database objects to avoid conflicts during updates.
- Modifying SQL statements to ensure safety during execution.
- Backing up the current database state before applying updates.
- Applying schema updates to the database based on the provided SQL statements.

## **Setup Instructions:**
1. Create a file named `.env` in the same directory as this script.
2. Open the `.env` file and paste the following format:
    ```
    DATABASE_URL=postgres://<username>:<password>@<host>/<dbname>
    ```
    Replace `<username>`, `<password>`, `<host>`, and `<dbname>` with your PostgreSQL credentials.
3. Save the `.env` file.
4. **Important Note:** Do not run this script in a production environment without testing it first. Always ensure you have backups and test in a safe environment before applying changes.

---

This script is free to use and modify. There is no copyright and no warranty.
