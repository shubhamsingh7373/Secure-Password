# 🔐 Healix Vault — Secure Password Manager

> **Your passwords. Your device. Your control.**
>
> A full-stack, locally-hosted password manager built with a **C++ backend** (Mongoose HTTP server, AES-256 encryption, PBKDF2-SHA-256 authentication) and a **modern web frontend** (HTML/CSS/JS glassmorphism UI).
>
> **No cloud. No tracking. No dependencies. Just pure security.** 🛡️

---

## ⚡ Quick Start (5 Minutes)

### 1️⃣ Install Prerequisites
```bash
# Windows PowerShell
# Download & install CMake: https://cmake.org/download/
# Download & install MinGW-w64: https://www.mingw-w64.org/

# Verify installation
cmake --version
g++ --version
```

### 2️⃣ Build & Run
```bash
cd C:\Users\YourName\Desktop\Secure-Password
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --config Release
.\build\password_vault.exe
```

### 3️⃣ Open in Browser
```
http://localhost:8080
```

**That's it!** Your vault is ready. 🚀

---

## ✨ Key Features

| Feature | Description |
|---------|-------------|
| 🔐 **AES-256 Encryption** | Military-grade encryption for all passwords at rest |
| 🔑 **PBKDF2-SHA-256 Hashing** | Master password secured with 200,000 iterations + random salt |
| 💻 **Zero-Trust Architecture** | All crypto happens locally; no data leaves your device |
| 🎨 **Modern UI** | Glassmorphism design, real-time password strength meter |
| 🔄 **Smart Password Generator** | Customizable length, character sets, real-time strength scoring |
| 🔎 **Instant Search** | Filter by website, username, category, or notes in real-time |
| 📂 **Category Filtering** | Auto-organized credentials (Banking, Social, Work, Custom) |
| ⏱️ **Auto-Lock** | 5-minute inactivity timer protects against physical theft |
| 📋 **One-Click Copy** | Copy passwords to clipboard with toast notifications |
| 🌐 **REST API** | Full CRUD operations via HTTP REST (with token auth) |

---

## 🏗️ Architecture Overview

```
┌─────────────────────────────────────────────────────┐
│              Browser (HTML + CSS + JS)               │
│  🔓 Login │ 📊 Dashboard │ ⚙️ Generator │ 🔍 Search  │
└────────────────────┬────────────────────────────────┘
                     │  HTTP REST API (localhost:8080)
                     │  Authorization: <session-token>
┌────────────────────▼────────────────────────────────┐
│           C++ Mongoose HTTP Server                   │
│                                                      │
│  ┌──────────┐  ┌──────────┐  ┌───────────────────┐  │
│  │  Auth    │  │  Vault   │  │  PasswordGenerator │  │
│  │ PBKDF2   │  │ AES-256  │  │  (charset + RNG)   │  │
│  │ SHA-256  │  │ CBC mode │  └───────────────────┘  │
│  └──────────┘  └──────────┘                         │
│                    │                                 │
│              vault.dat (AES-256 encrypted)           │
│              vault_config.txt (PBKDF2 hash + salt)   │
└─────────────────���───────────────────────────────────┘
```

---

## 🔒 Security Implementation

| Layer | Implementation | Standard |
|-------|---|---|
| **Password Hashing** | PBKDF2-SHA-256, 200,000 iterations, random 16-byte salt | RFC 2898 |
| **Vault Encryption** | AES-256-CBC, random IV + salt per save, PKCS#7 padding | FIPS 197 |
| **Key Derivation** | PBKDF2 with `:enc` context separator (auth key ≠ vault key) | NIST SP 800-132 |
| **Session Token** | Derived 32-byte hex key used as Authorization header | Custom |
| **Data at Rest** | vault.dat is fully AES-256 encrypted | Unreadable without master password |
| **Auto-lock** | Clears token after 5 minutes of inactivity | Session management best practice |

✅ **All crypto implemented from scratch in C++** — No OpenSSL dependency, full control, no vendor lock-in.

---

## 📁 Project Structure

```
Secure-Password/
├── CMakeLists.txt              # Build configuration
├── vault.dat                   # AES-256 encrypted vault (auto-created)
├── vault_config.txt            # PBKDF2 hash + salt (auto-created)
│
├── include/                    # C++ Headers
│   ├── auth.h                  # Authentication & session management
│   ├── crypto.h                # AES-256, SHA-256, PBKDF2, Base64
│   ├── vault.h                 # Vault CRUD + Entry struct
│   ├── password_gen.h          # Password generator + strength scoring
│   ├── mongoose.h              # Embedded HTTP server
│   └── nlohmann/json.hpp       # JSON serialization
│
├── src/                        # C++ Implementation
│   ├── main.cpp                # HTTP server + REST API routes
│   ├── auth.cpp                # PBKDF2 login/setup
│   ├── crypto.cpp              # Full AES-256-CBC + SHA-256 + PBKDF2
│   ├── vault.cpp               # Entry CRUD + encryption/decryption
│   ├── password_gen.cpp        # Generator + strength scorer
│   └── mongoose.c              # HTTP server implementation
│
└── public/                     # Frontend (served by C++ server)
    ├── index.html              # Single-page app markup
    ├── style.css               # Glassmorphism UI design
    └── app.js                  # All frontend logic (vanilla JS)
```

---

## 🛠️ Prerequisites

| Tool | Version | Status |
|------|---------|--------|
| **CMake** | ≥ 3.10 | [Download](https://cmake.org/download/) |
| **MinGW-w64 (GCC)** | ≥ 8.0 | [Download](https://www.mingw-w64.org/) |
| **Modern Browser** | Chrome/Edge/Firefox/Safari | ✅ |

### Verify Installation
```powershell
cmake --version     # Should show 3.10 or higher
g++ --version       # Should show 8.0 or higher
```

---

## ⚙️ Build Instructions (Windows)

### Step 1: Clone / Open Project
```powershell
cd C:\Users\YourName\Desktop\Secure-Password
```

### Step 2: Configure with CMake
```powershell
cmake -S . -B build -G "MinGW Makefiles"
```
> Creates a `build/` directory with CMake configuration

### Step 3: Build Release Binary
```powershell
cmake --build build --config Release
```
> Output: `build/password_vault.exe` (~2-3 MB)

### Step 4: Run the Server
```powershell
.\build\password_vault.exe
```

**Expected output:**
```
Starting Healix Vault Web Server on http://localhost:8080
Open your browser and navigate to http://localhost:8080
Press Ctrl+C to stop.
```

### Step 5: Access in Browser
```
http://localhost:8080
```

---

## 🚀 Usage Guide

### 🔓 First Run — Setup Master Password

1. Open `http://localhost:8080`
2. App detects **First Run** and shows Setup form
3. Enter master password (minimum 8 characters)
4. Watch the **password strength meter** update in real-time
5. Confirm password and click **Initialize Vault**

**First run setup completes in seconds!**

---

### 🔑 Subsequent Runs — Login

1. Open `http://localhost:8080`
2. Enter your master password
3. Click **Unlock Vault**
4. Wrong password? Error message shown, vault stays locked

---

### 💾 Adding a Credential

1. Click **➕ New Entry** (top-right)
2. Fill in:
   - **Website:** `github.com`
   - **Username:** `user@email.com`
   - **Password:** `(auto-generate or paste)`
   - **Category:** `Work`
   - **Notes:** `(optional)`
3. Click **🔄 Generate** to auto-fill secure password
4. Watch the **color-coded strength bar** update
5. Click **Save Entry** — toast confirms success

---

### ⚙️ Password Generator

1. Click **Generator** in left sidebar
2. Adjust **length** slider (8–64 characters)
3. Toggle character types:
   - ✅ Uppercase (A-Z)
   - ✅ Lowercase (a-z)
   - ✅ Numbers (0-9)
   - ✅ Symbols (!@#$%...)
4. Click **Use This Password** to fill entry form

**Real-time strength scoring included!**

---

### 👁️ Managing Credentials

| Action | How To |
|--------|--------|
| **View Password** | Click 👁️ eye icon on card |
| **Copy to Clipboard** | Click 📋 copy button → toast confirms |
| **Edit Entry** | Click ✏️ edit → modal pre-filled |
| **Delete Entry** | Click 🗑️ trash → confirm dialog |
| **Lock Vault** | Click 🔒 lock button or wait 5 minutes |

---

### 🔍 Search & Filter

**Search Bar:**
- Type in search field (Ctrl+F to focus)
- Searches website, username, category, notes in real-time
- Results update as you type

**Category Filtering:**
- Categories auto-appear in left sidebar
- Click category to filter entries
- Default: **General, Social, Banking, Work** (custom categories supported)

---

### ⏱️ Auto-Lock Feature

- Vault **automatically locks after 5 minutes** of inactivity
- Timer shown in bottom-left of sidebar
- Any **mouse movement or keypress** resets timer
- Click **🔒 Lock Vault** to manually lock immediately

---

## 🌐 REST API Reference

All endpoints except `/api/status`, `/api/setup`, `/api/login` require:
```
Authorization: <session-token>
Content-Type: application/json
```

### Endpoint Overview

| Method | Endpoint | Description | Auth |
|--------|----------|-------------|------|
| `GET` | `/api/status` | Check if first run | ❌ |
| `POST` | `/api/setup` | Initialize vault with master password | ❌ |
| `POST` | `/api/login` | Authenticate and get session token | ❌ |
| `GET` | `/api/entries` | List all vault entries | ✅ |
| `POST` | `/api/entries` | Create new entry | ✅ |
| `GET` | `/api/entries/:id` | Get single entry | ✅ |
| `PUT` | `/api/entries/:id` | Update entry | ✅ |
| `DELETE` | `/api/entries/:id` | Delete entry | ✅ |
| `GET` | `/api/search?q=term` | Server-side search | ✅ |
| `GET` | `/api/generate?...` | Generate password | ✅ |

---

### Example: Complete Workflow

#### 1. Check Status (First Run)
```bash
curl http://localhost:8080/api/status
# Response: {"first_run": true}
```

#### 2. Setup Master Password
```bash
curl -X POST http://localhost:8080/api/setup \
  -H "Content-Type: application/json" \
  -d '{"master_password":"MyStr0ng!Pass"}'
# Response: {"success": true, "token": "a1b2c3d4e5f6..."}
```

#### 3. Login (Subsequent Runs)
```bash
curl -X POST http://localhost:8080/api/login \
  -H "Content-Type: application/json" \
  -d '{"master_password":"MyStr0ng!Pass"}'
# Response: {"success": true, "token": "x9y8z7w6v5u4..."}
```

#### 4. Add Entry
```bash
curl -X POST http://localhost:8080/api/entries \
  -H "Authorization: x9y8z7w6v5u4..." \
  -H "Content-Type: application/json" \
  -d '{
    "website":"GitHub",
    "username":"user@email.com",
    "password":"Str0ng!Pass",
    "category":"Work",
    "notes":"Primary development account"
  }'
# Response: {"id": 1, "success": true}
```

#### 5. List All Entries
```bash
curl http://localhost:8080/api/entries \
  -H "Authorization: x9y8z7w6v5u4..."
# Response: [{"id": 1, "website": "GitHub", ...}, ...]
```

#### 6. Search Entries
```bash
curl "http://localhost:8080/api/search?q=github" \
  -H "Authorization: x9y8z7w6v5u4..."
# Response: [{"id": 1, "website": "GitHub", ...}]
```

#### 7. Generate Password
```bash
curl "http://localhost:8080/api/generate?length=16&upper=1&lower=1&numbers=1&symbols=1" \
  -H "Authorization: x9y8z7w6v5u4..."
# Response: {"password": "aB3$xYz9@Qw5mN2!"}
```

---

## 🔄 Data Persistence

- **`vault_config.txt`** — PBKDF2 hash + salt (survives restart)
- **`vault.dat`** — AES-256 encrypted vault (survives restart)
- Both files stored in **project root** (same directory as executable)
- **Deleting `vault_config.txt`** resets to first-run (vault.dat becomes unreadable)

### Data Format
```
vault_config.txt:  [PBKDF2_HASH]:[SALT]:[ITERATIONS]
vault.dat:         [ENCRYPTED_BINARY_DATA]
```

---

## 🐛 Troubleshooting

| Issue | Cause | Solution |
|-------|-------|----------|
| Cannot connect to server | Executable not running | Run `.\build\password_vault.exe` again |
| Build fails: `ws2_32 not found` | Wrong MinGW version | Install MinGW-w64 (not MinGW-32) |
| `cmake` not found | CMake not in PATH | Reinstall CMake, ensure "Add to PATH" is checked |
| Port 8080 in use | Another app using port | Change port in `main.cpp` line with `mg_http_listen` |
| Forgot master password | Master password required | Delete `vault.dat` + `vault_config.txt` to reset (⚠️ data lost) |
| Wrong password error | Typo or different password | Ensure Caps Lock is OFF; password is case-sensitive |
| Vault won't save | File permissions issue | Run executable from a writable directory |

---

## 🔮 Future Improvements

While this version provides a solid, secure local foundation, the following features are planned:

### **☁️ Cloud Sync (Optional, End-to-End Encrypted)**
Sync your encrypted `vault.dat` across multiple devices using Google Drive, Dropbox, or a custom secure backend. The data will remain end-to-end encrypted — cloud providers cannot read your passwords.

### **👆 Biometric Unlock**
Integration with Windows Hello (Fingerprint/Facial Recognition) or Apple Touch ID/Face ID for quick vault access without typing the master password, while keeping encryption keys secure in OS keychain.

### **🔐 2FA / MFA Support**
Add an extra layer of security using:
- TOTP (Time-based One-Time Password with Google Authenticator, Authy)
- Hardware security keys (YubiKey, Windows Hello for Business)
- Prevent unauthorized access even if master password is compromised

### **🌍 Cross-Platform Support**
Expand from Windows-only to:
- macOS (with native Cocoa UI or web frontend)
- Linux (native GTK UI or web frontend)
- Mobile (iOS/Android companion apps)

### **💬 Import/Export**
- Import from CSV, JSON, or other password managers
- Export encrypted backups
- Bulk credential management

---

## 📊 Performance & Specs

| Metric | Value |
|--------|-------|
| **Executable Size** | ~2-3 MB (Release build) |
| **Memory Usage** | ~10-20 MB at runtime |
| **Encryption Speed** | ~50-100 ms per credential (depends on CPU) |
| **Decryption Speed** | ~50-100 ms per credential |
| **Max Entries** | Theoretical limit: 1GB+ (depends on disk space) |
| **Password Length Support** | Up to 1024 characters |
| **Concurrent Users** | 1 (local only) |

---

## 🔐 Security Considerations

### ✅ What This Project Does Right
- **No external crypto dependencies** — Full control, no vendor lock-in
- **Zero-knowledge architecture** — Only your device knows your passwords
- **Industry-standard algorithms** — PBKDF2, AES-256, SHA-256 (NIST-approved)
- **Secure key derivation** — Separate auth and encryption keys
- **Cryptographic randomness** — True random IV and salt per encryption
- **Session timeout** — Auto-lock after 5 minutes of inactivity

### ⚠️ Important Limitations
- **Educational Project** — NOT professionally audited
- **Local-only** — Single device, no synchronization
- **No biometric auth** — Master password required every time
- **No 2FA** — Single factor authentication only
- **Limited threat model** — Assumes local device security

### 🛡️ Recommended Usage
- ✅ Learning cryptography and security design
- ✅ Storing non-critical passwords locally
- ✅ Personal use on trusted devices
- ❌ Production use with sensitive/critical passwords
- ❌ Business-critical credentials
- ❌ Compliance-regulated environments (HIPAA, SOC 2, etc.)

---

## 📝 License

**Academic / Educational Project — Healix Vault, 2026**

This project is an educational initiative demonstrating secure password management principles through a full-stack implementation with cryptographic best practices.

For detailed attributions, see [ACKNOWLEDGEMENTS.md](./ACKNOWLEDGEMENTS.md)

---

## 🤝 Contributing

Contributions are welcome! Areas for improvement:
- 🐛 Bug fixes and stability improvements
- 🎨 UI/UX enhancements
- 🔧 Cross-platform support (macOS, Linux)
- 📚 Documentation improvements
- 🧪 Additional test coverage
- 🔐 Security audit findings

---

## 📚 Learning Resources

- **Cryptography:** [Coursera - Cryptography I](https://www.coursera.org/learn/crypto)
- **Web Security:** [PortSwigger Web Security Academy](https://portswigger.net/web-security)
- **Password Managers:** [OWASP Password Storage Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/Password_Storage_Cheat_Sheet.html)
- **C++ Crypto:** [C++ Reference](https://en.cppreference.com/)

---

## ❓ FAQ

**Q: Is my data safe with Healix Vault?**
> A: For educational purposes on a trusted device, yes. But this is NOT professionally audited. Use established password managers (Bitwarden, 1Password, KeePass) for critical data.

**Q: Can I use this in production?**
> A: Not recommended without professional security audit. This is an educational project.

**Q: Where is my data stored?**
> A: Locally on your device in `vault.dat` (AES-256 encrypted) and `vault_config.txt` (PBKDF2 hash + salt).

**Q: Can I sync my vault across devices?**
> A: Not yet. Planned for future release with end-to-end encryption.

**Q: What if I forget my master password?**
> A: Vault becomes inaccessible. You must reset by deleting `vault.dat` + `vault_config.txt` (all data lost).

**Q: Can I export my passwords?**
> A: Yes, via `/api/entries` endpoint to get JSON. Future release will add export UI.

**Q: Does it work on macOS/Linux?**
> A: Frontend is cross-platform, but backend requires cross-compilation. Future release planned.

---

## 📞 Support

- **Questions?** Open an issue on GitHub
- **Bug Report?** Provide detailed steps to reproduce
- **Security Issue?** Report responsibly (don't open public issue)

---

## 🎉 Credits

Special thanks to the open-source community, cryptographic standards bodies (NIST, RFC), and all contributors.

See [ACKNOWLEDGEMENTS.md](./ACKNOWLEDGEMENTS.md) for comprehensive credits.

---

**Made with ❤️ for security education. Use responsibly.** 🔐✨

---

**Version:** 1.0 (Educational Release)  
**Last Updated:** May 5, 2026  
**Status:** Active Development
