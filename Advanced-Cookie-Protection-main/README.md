## **Cookies Advanced Data Protection**

Cookies can be stolen by threat actors and used to bypass two-factor authentication (2FA). This write-up outlines a method to make session hijacking **extremely difficult** unless the request originates from the user's network. The model and design can be polymorphic and implemented in multiple ways with different approaches.

### **How Sessions Are Made**

Sessions are hashed and stored as browser cookies that uniquely identify a device, allowing the server to validate a user’s identity.

In most cases, the session is only stored in the database and does not require frequent re-authentication. This is because running a cryptographic algorithm for every request to verify the session would be computationally expensive. Instead, the system simply checks if the cookie matches the one stored in the database and verifies the expiration timestamp. This removes the need to rehash or decrypt the session until the cookie naturally expires.

However, if an attacker replaces the cookie, they could potentially gain unauthorized access to the account using malware or other means.

With this system, if an attacker attempts to use the session from an unauthorized IP address, the session is considered invalid, forcing a full re-authentication.

---

### **Database Setup**

|ID|User Associated|Session Cookie|Time Created|Cookie Expiry|Session Cookie IP|Mobile Device ID (Optional)|
|---|---|---|---|---|---|---|
|UserID|Username|(Secure Hash / HMAC)|(Unix Timestamp)|(Unix Timestamp)|(IPv4/IPv6 Format)|(Mobile Device UID)|

---

### **Social Media**

Social media platforms present a challenge for **IP-based security** due to frequent travel and mobile network changes. However, solutions exist:

#### **Mobile Override**

- Implement **hardware-based identification (HWID)** for mobile devices.
- Update: This could include a file tailored specifically for that mobile device (for example, an encryption key stored on the device), so it doesn't have to rely on a mobile device identifier that could be used for malicious or tracking purposes.

- The mobile cookie flag should **only be applied on mobile devices** and remain separate from desktop-based authentication.
    
- Since mobile devices are **generally more secure than desktops**, we assume they are not compromised.
    
- The session should **only request for reauthenication if the HWID does not match**, avoiding the need for constant IP lookups via **MaxMind** or similar services.

- Previous sessions should still be able to access them until the expiration window is over, which allows older sessions to remain functional if the IP address still matches or the mobile device identifier is the same.    

#### **Tracking IP Changes**

Most social media platforms already **log IP changes** for security and legal reasons. This system can **leverage existing logic** to enhance security without major infrastructure changes:

- **ISP-Based Tracking:** If a user is on a **mobile hotspot** and traveling, the ASN (Autonomous System Number) will likely remain the same and the IP changes slightly.
    
- **Subnet Range Monitoring:** Instead of invalidating sessions **immediately** upon an IP change, monitor for range changes within a **/16 subnet** (or a reasonable threshold).
    
- **Geolocation Awareness:** If a user travels **500+ Kilometers/miles** and connects via a **different ISP**, require re-authentication or **2FA** to confirm the login.
    

---

### **Key Notes**

This security model could be used as an **optional layer of advanced threat protection**, ensuring only you can access your account while also allowing them to disable this option.

This is also separate from the actual session itself. The validation occurs **before the session** is even checked. This means we are verifying IP address changes **before** the session logic, meaning it doesn't alter your existing session or require decryption of the session cookie. The IP address is **not embedded in the cookie**, rather it is stored in the database and checked against the session cookie.

For **managers** or those who need more control over their accounts in different locations, this may require the use of a **VPN** to access their social media accounts or a way to add **trusted IP addresses**. The system should prompt for **2FA** whenever adding a trusted IP address in order to ensure the safety of the account. This would also allow them to **work securely** without being caught in the crossfire of the advanced cookie protection.

**Example:**

[Dashboard]  
**Trusted IPs ->**  
Add a trusted IP  
Select IP (or click for the current location)  
Save  
(Please enter your 2FA code in order to save)

---

### **Law Enforcement**

While this system enhances security, it could also create complications for **law enforcement**. Without access to the **user’s IP network**, **law enforcement** may not be able to log in to an account, which could present legal challenges. To address this, it may be necessary to allow the addition of a **trusted IP address** that would **legally** grant access to law enforcement, as it is illegal to withhold data in criminal investigations, especially those involving time-sensitive matters.

---

## **Conclusion**

This system ensures that stolen session cookies are essentially useless without the correct **IP address**, **device**, or **location**, making **session hijacking extremely difficult**. It provides a high level of security while remaining adaptable for **social media platforms** and other niche use cases.
