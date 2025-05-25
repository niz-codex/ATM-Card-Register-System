# 💳 Secure Card Authentication System in C

A simple and secure C-based card authentication system that supports **user registration**, **PIN hashing**, **access control** (admin/user), **expiry checking**, and **audit logging**. Built for educational and system-level programming practice.

## 🚀 Features

- 📁 Stores card data in a local text file (`cardsDetails.txt`)
- 🔒 Uses SHA-256 hashing (via OpenSSL) for PINs
- 📜 Logs every access attempt with timestamps (`audit.log`)
- ✅ Verifies expiry dates (MM/YY format)
- 👮 Differentiates between `admin` and `user` roles
- 🔁 Allows 3 attempts for PIN entry
- ✳️ Registers new cards if not found

## 📦 Dependencies

- GCC compiler
- OpenSSL (for SHA-256 hashing)

Install OpenSSL if not already installed:

```sudo apt install libssl-dev```

🛠️ Compilation 
```gcc cardRegisterSystem.c -o card_system -lssl -lcrypto```

🧾 Example Workflow
- Insert card: Enter the card number.
- New card? Register with name, expiry date (MM/YY), PIN, and role.
- Existing card? Enter correct PIN to access.

Role-based access:
   - admin: Full privileges
   - user: Limited access

📄 File Structure
cardsDetails.txt – Stores card details in CSV format.
audit.log – Logs access attempts with timestamps.
main.c – Main source code.

🔐 Security Notes
- PINs are never stored in plain text.
- Hashing is done using SHA-256.
- Audit logs track all attempts for traceability.
- Card expiry is strictly enforced.

📌 Future Improvements
- 🔄 Replace file storage with SQLite or JSON
- 🧪 Add unit tests
- 🧑‍💻 GUI front-end or Web API integration
- 🔐 Multi-factor authentication (MFA)

🙋 Author
Nizath – Ethical hacker and system programmer

📧 Email: nisathnisath606@gmail.com

"Discipline and deep focus will take you where motivation can’t."
