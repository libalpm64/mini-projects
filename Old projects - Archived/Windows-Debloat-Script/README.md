## Libalpm's Window De-bloat script
I’m beyond sick of Windows and its built-in spyware. It’s infuriating the amount of shit Microsoft can get away with. Windows 11 collects an insane amount of information about our systems, making it way too easy to know who we are and where we are without us even knowing. Heck, they probably are collecting my blood type, for all I know. It's unbelievable how much data they collect. We're going to
Fix that starting today.

Remember when Windows 7 and earlier just focused on updates and left us the hell alone? Those were the days! It's time to push back against this invasive bullshit and restore a windows 7 like experience.

``This script works for both Windows 10 and 11. And this script will not affect daily Windows users. I made sure to check each process. You can also revert it via the https://privacy.sexy url. Go through my list and revert the change if a problem persists.``

## User Management
- **Remove Default User**: Deletes the default0 user account.

## Data Clearing
- **Clear SRUM Data**: Resets the System Resource Usage Monitor data.

## App Access 
Changes the default access to disabled for:
- Account Name, Picture, etc.
- Phone
- Location
- Motion Activity
- Trusted Devices
- Unpaired Wireless Devices
- Camera
- Voice Activation
- Information about Other Apps
- Tasks
- Eye Tracking
- Physical Movement
- Human Presence
- Screen Capture

## Telemetry and Data Collection
Manage telemetry and data collection:
- Customer Experience Improvement
- Application Experience Data
- Application Impact
- Steps Recorder (considered spyware)
- Diagnostic Services:
  - DiagTrack
  - Diagsv
- Advertisers and Cloud Diagnostics
- Cortana (includes location pinging)
- Personal Search Data Sharing
- Taskbar Ads (Personal Cloud Content)
- Targeted Advertisements and Marketing
- Windows Insider Program
- Cloud Sync
- Windows Recall
- Cloud Engine-Based Speech Recognition
- Opt-Out of Data Collection
- Windows Feedback Telemetry
- Handwriting Data Collection
- Game Screen Recording (use alternatives like ShadowPlay)
- Internet Access for Windows DRM
- Activity Feed
- Typing Feedback
- Clipboard Data Collection
- Windows Connect Now (remote software access)

## Bloatware Removal
Remove the following applications:
- Copilot
- OneDrive
- Third-Party Apps (e.g., Duolingo, Pandora, Shazam, Candy Crush)
- Office
- Paint 3D (discontinued)
- Mixed Reality Portal
- Cortana
- App Connector
- Phone App
- Minecraft (prebundled Windows 11 BTW)
- Skype, GroupMe
- Steps Recorder Capability
- Widgets from Taskbar
- Meet Now Icon from Taskbar

## Privacy Enhancements
- **NTP Server Change**: Switches to NTP.org.
- **Block Windows Telemetry Hosts**
- **Block Third-Party Telemetry** (e.g., Google, Nvidia, Visual Studio, etc.)
- **Disable Recent Apps**
- **Disable Online Tips**
- **Disable Document/Folders History**
- **Disable Lock Screen Notifications**
- **Disable Sync Provider Notifications**

## Security Improvements
- **App Launch Tracking**
- **Disable Insecure Ciphers**
- **Disable Insecure Hashing Algorithms**
- **Disable Insecure Protocols**
- **RIP Listener**
- **Mitigate Meltdown and Spectre Attacks**
- **DEP (Data Execution Protection)**
- **SEHOP (Structured Exception Handling Overwrite Protection)**
- **Disable Always Install with Elevated Permissions**
- **Prevent PowerShell v2 Downgrade Attacks**
- **Lock Screen Camera Access**

---
This debloat script significantly enhances performance as well; it reduces the service count by a high margin. You can expect to see a few percentages less usage on idle. 
