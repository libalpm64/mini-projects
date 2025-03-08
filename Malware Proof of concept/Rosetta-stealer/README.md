## Rosetta-Stealer
### Disclaimer: This tool is intended strictly for educational and research purposes in controlled lab environments. Unauthorized access to systems or misuse of this tool may violate applicable laws, such as the Computer Misuse Act. Use responsibly. This is your final warning. I will not repeat this again; it is your own discretion whether you go to jail or not.

Rosetta-Stealer is a Python-based proof-of-concept (PoC) designed to demonstrate techniques used by modern data exfiltration tools. It incorporates techniques to evade detection by common antivirus software.

This was tested in a lab environment and evaded common AVs such as Windows Defender and Bitdefender. It employs several techniques to mask what the program is actually doing in order to achieve this.

Firstly:

We used PyArmor to pack it into an executable, which heavily obfuscates exactly where the entry to the program is, resulting in bypassing AVs' behavioral detection and signature scans. (This is required to bypass AVs; otherwise, it’s quite obvious what the program is doing.)
We added many fake methods to confuse antivirus software into thinking it is a legitimate program.
It uses SOCKS proxies to avoid IDS/IPS from detecting the end URL.


Steals the following:
Google Chrome passwords, Microsoft Edge passwords, Brave Software passwords, Firefox passwords, Discord tokens

Downloads files on the PC from the Desktop and Documents folders with the tags ".txt, .doc, .docx, .pptx, .ppt, .xlsx, .xls" and uploads them to catbox.moe alongside the browser/token information.


Note: I am not adding features to this, nor should you, nor will I teach you how to set this up.
