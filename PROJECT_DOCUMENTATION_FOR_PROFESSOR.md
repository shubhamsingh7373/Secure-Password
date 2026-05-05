# 🔐 HEALIX VAULT
## Secure Password Manager

---

### PROJECT DOCUMENTATION

**Submitted by:**
- **Shubham Kumar** (Lead Developer)
- **Rani** (Project Partner)

**Institute:** Desh Bhagat University  
**Program:** B.Tech Computer Science & Engineering  
**Semester:** 4th Semester  
**Date:** May 5, 2026

---

## TABLE OF CONTENTS

1. Project Overview
2. Quick Start Guide
3. Key Features
4. Architecture
5. Security Implementation
6. Build Instructions
7. Usage Guide
8. REST API Reference
9. Troubleshooting
10. Acknowledgements & Credits

---

## 1. PROJECT OVERVIEW

### What is Healix Vault?

Healix Vault is a full-stack, locally-hosted password manager built with:
- **Backend:** C++ with Mongoose HTTP server, AES-256 encryption, PBKDF2-SHA-256 authentication
- **Frontend:** Modern web UI with HTML5/CSS3 (Glassmorphism design) and vanilla JavaScript

### Key Tagline
> Your passwords. Your device. Your control.
> 
> No cloud. No tracking. No dependencies. Just pure security. 🛡️

### Project Goals
✅ Demonstrate secure password management principles  
✅ Implement cryptographic best practices from scratch  
✅ Create a full-stack application with modern UI  
✅ Educational learning platform for cybersecurity  

---

## 2. QUICK START GUIDE (5 Minutes)

### Installation & Setup

**Step 1: Download Prerequisites**
- CMake ≥ 3.10 from https://cmake.org/download/
- MinGW-w64 (GCC) from https://www.mingw-w64.org/

**Step 2: Verify Installation**
```
cmake --version
g++ --version
```

**Step 3: Build the Project**
```
cd C:\path\to\Secure-Password
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --config Release
```

**Step 4: Run the Server**
```
.\build\password_vault.exe
```

**Step 5: Open in Browser**
```
http://localhost:8080
```

**That's it! Your secure vault is ready.** ✅

---

## 3. KEY FEATURES

| Feature | Description | Technology |
|---------|-------------|-----------|
| 🔐 **AES-256 Encryption** | Military-grade encryption for all passwords at rest | FIPS 197 Standard |
| 🔑 **PBKDF2-SHA-256 Hashing** | Master password secured with 200,000 iterations + random salt | RFC 2898 |
| 💻 **Zero-Trust Architecture** | All crypto happens locally; no data leaves your device | Local-only |
| 🎨 **Modern UI** | Glassmorphism design, real-time password strength meter | HTML5/CSS3/JS |
| 🔄 **Password Generator** | Customizable length, character sets, real-time strength scoring | C++ Backend |
| 🔎 **Instant Search** | Filter by website, username, category, or notes in real-time | Server-side |
| 📂 **Category Filtering** | Auto-organized credentials (Banking, Social, Work, Custom) | Local Storage |
| ⏱️ **Auto-Lock** | 5-minute inactivity timer protects against physical theft | Session Mgmt |
| 📋 **One-Click Copy** | Copy passwords to clipboard with toast notifications | JavaScript |
| 🌐 **REST API** | Full CRUD operations via HTTP REST (with token auth) | C++ Mongoose |

---

## 4. SYSTEM ARCHITECTURE

### Architecture Diagram

```
┌─────────────────────────────────────────────────────┐
│              BROWSER (Frontend Layer)                │
│         HTML + CSS + JavaScript (Vanilla)            │
│                                                      │
│  🔓 Login  │  📊 Dashboard  │  ⚙️ Generator  │ 🔍 Search
└────────────────────┬────────────────────────────────┘
                     │
        ↓ HTTP REST API (localhost:8080) ↓
         Authorization: <derived-session-token>
                     │
┌────────────────────▼────────────────────────────────┐
│     C++ MONGOOSE HTTP SERVER (Backend Layer)        │
│                                                      │
│  ┌──────────────┐  ┌──────────────┐  ┌───────────┐ │
│  │     AUTH     │  │    VAULT     │  │ PASSWORD  │ │
│  │             │  │              │  │ GENERATOR │ │
│  │  PBKDF2     │  │  AES-256-CBC │  │           │ │
│  │  SHA-256    │  │  Mode        │  │ Charset   │ │
│  └──────────────┘  └──────────────┘  │ + RNG     │ │
│                           │            └───────────┘ │
│                           ↓                          │
│                                                      │
│         vault.dat (AES-256 Encrypted)               │
│         vault_config.txt (PBKDF2 Hash + Salt)       │
└─────────────────────────────────────────────────────┘
```

### Component Breakdown

**Frontend Layer:**
- User login & authentication interface
- Dashboard for credential management
- Password generator with live strength meter
- Search functionality with category filtering

**Backend Layer:**
- HTTP REST API server (Mongoose)
- Authentication module (PBKDF2-SHA-256)
- Vault encryption/decryption (AES-256-CBC)
- Password generator with entropy
- Session management with auto-lock

**Data Persistence:**
- vault_config.txt: PBKDF2 hash + salt
- vault.dat: AES-256 encrypted credentials

---

## 5. SECURITY IMPLEMENTATION

### Cryptographic Standards Used

| Layer | Algorithm | Standard | Details |
|-------|-----------|----------|---------|
| **Password Hashing** | PBKDF2-SHA-256 | RFC 2898 | 200,000 iterations + 16-byte random salt |
| **Vault Encryption** | AES-256-CBC | FIPS 197 | 256-bit key, random IV per save, PKCS#7 padding |
| **Key Derivation** | PBKDF2 (:enc separator) | NIST SP 800-132 | Auth key ≠ Vault key (separation of concerns) |
| **Hashing** | SHA-256 | FIPS 180-4 | Message authentication & integrity verification |
| **Encoding** | Base64 | RFC 4648 | Binary data serialization for JSON |
| **Session Token** | Derived 32-byte hex | Custom | Used as Authorization header |

### Security Architecture

**What's Right ✅**
- Industry-standard algorithms (PBKDF2, AES-256, SHA-256)
- Cryptographic randomness (IV, salt per encryption)
- Zero-knowledge design (local-only encryption)
- Session timeout (5-minute auto-lock)
- Separate auth & vault encryption keys
- No external crypto dependencies (full control)

**Important Limitations ⚠️**
- Educational project (not professionally audited)
- Local-only (no cross-device sync)
- No biometric authentication
- No two-factor authentication (2FA)
- Master password is single point of failure
- Not for production use with critical data

---

## 6. BUILD INSTRUCTIONS

### Prerequisites

| Tool | Version | Download |
|------|---------|----------|
| CMake | ≥ 3.10 | https://cmake.org/download/ |
| MinGW-w64 (GCC) | ≥ 8.0 | https://www.mingw-w64.org/ |
| Modern Browser | Chrome/Edge/Firefox | Any recent browser |

### Step-by-Step Build Process

**Step 1: Verify CMake Installation**
```
cmake --version
# Expected output: cmake version 3.x.x
```

**Step 2: Verify GCC Installation**
```
g++ --version
# Expected output: g++ (GCC) 8.0 or higher
```

**Step 3: Navigate to Project Directory**
```
cd C:\Users\YourName\Desktop\Secure-Password
```

**Step 4: Create Build Directory with CMake**
```
cmake -S . -B build -G "MinGW Makefiles"
# This creates the build/ directory and generates Makefiles
```

**Step 5: Compile Release Build**
```
cmake --build build --config Release
# Compilation takes 1-2 minutes
# Output: build\password_vault.exe (~2-3 MB)
```

**Step 6: Run the Executable**
```
.\build\password_vault.exe
# Expected console output:
# Starting Healix Vault Web Server on http://localhost:8080
# Open your browser and navigate to http://localhost:8080
# Press Ctrl+C to stop.
```

**Step 7: Access in Web Browser**
```
http://localhost:8080
```

### Successful Build Indicators
✅ Executable created: build\password_vault.exe  
✅ Console shows startup message  
✅ Browser loads login page at http://localhost:8080  
✅ No compilation errors in console  

---

## 7. USAGE GUIDE

### Initial Setup - Master Password

1. **Open Application**
   - Navigate to http://localhost:8080
   - App detects first run automatically

2. **Create Master Password**
   - Enter password (minimum 8 characters)
   - Observe real-time password strength meter
   - Color coding: Red (Weak) → Yellow (Medium) → Green (Strong)

3. **Confirm & Initialize**
   - Re-enter password for confirmation
   - Click "Initialize Vault"
   - Vault is now encrypted and ready

### Login (Subsequent Sessions)

1. Open http://localhost:8080
2. Enter master password
3. Click "Unlock Vault"
4. Access your credentials

### Adding a New Credential

**Workflow:**
1. Click ➕ "New Entry" button (top-right)
2. Fill credential details:
   - Website: `github.com`
   - Username: `your@email.com`
   - Password: `(use generator or paste)`
   - Category: `Work` (or custom)
   - Notes: `(optional)`
3. Click 🔄 "Generate" for secure password
4. Observe strength bar update
5. Click "Save Entry"
6. Toast notification confirms save

### Password Generator

1. Click "Generator" in sidebar
2. Adjust length: 8-64 characters (slider)
3. Toggle character types:
   - Uppercase (A-Z)
   - Lowercase (a-z)
   - Numbers (0-9)
   - Symbols (!@#$%^&*)
4. Click "Use This Password"
5. Password fills into entry form

### Managing Credentials

**View Password:**
- Click 👁️ eye icon on credential card
- Password revealed in plain text

**Copy to Clipboard:**
- Click 📋 copy icon
- Toast notification appears: "Copied!"
- Paste anywhere (Ctrl+V)

**Edit Credential:**
- Click ✏️ edit icon
- Modal opens with pre-filled data
- Make changes and save

**Delete Credential:**
- Click 🗑️ trash icon
- Confirmation dialog appears
- Click "Confirm Delete"
- Credential permanently removed

### Search & Filter

**Search Feature:**
- Type in search bar at top
- Keyboard shortcut: Ctrl+F
- Real-time filtering by:
  - Website name
  - Username
  - Category
  - Notes

**Category Filtering:**
- Categories auto-appear in left sidebar
- Click category to filter entries
- Shows only credentials in that category
- Default categories: General, Social, Banking, Work

### Auto-Lock Security Feature

- **Timer:** Vault locks after 5 minutes of inactivity
- **Reset:** Any mouse movement or keypress resets timer
- **Manual Lock:** Click 🔒 "Lock Vault" button
- **Timer Display:** Shown in bottom-left of sidebar

---

## 8. REST API REFERENCE

### API Overview

**Base URL:** `http://localhost:8080`

**Authentication Required:** All endpoints except `/api/status`, `/api/setup`, `/api/login` require:
```
Authorization: <session-token>
Content-Type: application/json
```

### API Endpoints

| # | Method | Endpoint | Purpose | Auth |
|---|--------|----------|---------|------|
| 1 | GET | `/api/status` | Check if first run | ❌ No |
| 2 | POST | `/api/setup` | Initialize vault with master password | ❌ No |
| 3 | POST | `/api/login` | Authenticate and get session token | ❌ No |
| 4 | GET | `/api/entries` | List all vault entries | ✅ Yes |
| 5 | POST | `/api/entries` | Create new entry | ✅ Yes |
| 6 | GET | `/api/entries/:id` | Get single entry by ID | ✅ Yes |
| 7 | PUT | `/api/entries/:id` | Update entry by ID | ✅ Yes |
| 8 | DELETE | `/api/entries/:id` | Delete entry by ID | ✅ Yes |
| 9 | GET | `/api/search?q=term` | Search entries by keyword | ✅ Yes |
| 10 | GET | `/api/generate?...` | Generate secure password | ✅ Yes |

### Complete API Workflow Examples

#### Example 1: Check Status
```
Request:
GET /api/status

Response (First Run):
{
  "first_run": true
}

Response (Already Initialized):
{
  "first_run": false
}
```

#### Example 2: Setup Master Password
```
Request:
POST /api/setup
Content-Type: application/json

{
  "master_password": "MyStr0ng!Pass"
}

Response:
{
  "success": true,
  "token": "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"
}
```

#### Example 3: Login
```
Request:
POST /api/login
Content-Type: application/json

{
  "master_password": "MyStr0ng!Pass"
}

Response:
{
  "success": true,
  "token": "x9y8z7w6v5u4t3s2r1q0p9o8n7m6l5k4"
}
```

#### Example 4: Add New Credential
```
Request:
POST /api/entries
Authorization: x9y8z7w6v5u4t3s2r1q0p9o8n7m6l5k4
Content-Type: application/json

{
  "website": "GitHub",
  "username": "user@email.com",
  "password": "Str0ng!Pass",
  "category": "Work",
  "notes": "Primary development account"
}

Response:
{
  "success": true,
  "id": 1
}
```

#### Example 5: Get All Entries
```
Request:
GET /api/entries
Authorization: x9y8z7w6v5u4t3s2r1q0p9o8n7m6l5k4

Response:
[
  {
    "id": 1,
    "website": "GitHub",
    "username": "user@email.com",
    "password": "Str0ng!Pass",
    "category": "Work",
    "notes": "Primary development account"
  },
  {
    "id": 2,
    "website": "Gmail",
    "username": "user@gmail.com",
    "password": "Another!Pass",
    "category": "Social",
    "notes": "Personal email"
  }
]
```

#### Example 6: Search Entries
```
Request:
GET /api/search?q=github
Authorization: x9y8z7w6v5u4t3s2r1q0p9o8n7m6l5k4

Response:
[
  {
    "id": 1,
    "website": "GitHub",
    "username": "user@email.com",
    "password": "Str0ng!Pass",
    "category": "Work",
    "notes": "Primary development account"
  }
]
```

#### Example 7: Generate Password
```
Request:
GET /api/generate?length=16&upper=1&lower=1&numbers=1&symbols=1
Authorization: x9y8z7w6v5u4t3s2r1q0p9o8n7m6l5k4

Response:
{
  "password": "aB3$xYz9@Qw5mN2!",
  "strength": "Strong"
}
```

---

## 9. TROUBLESHOOTING

### Common Issues & Solutions

| Issue | Cause | Solution |
|-------|-------|----------|
| **Cannot connect to server** | password_vault.exe not running | Re-run `.\build\password_vault.exe` |
| **Build fails: ws2_32 not found** | Wrong MinGW version installed | Install MinGW-w64 (64-bit), not MinGW-32 |
| **cmake command not found** | CMake not in system PATH | Reinstall CMake, check "Add to PATH" option |
| **Port 8080 already in use** | Another app using same port | Edit main.cpp line with mg_http_listen and change port |
| **Forgot master password** | No password recovery available | Delete vault.dat & vault_config.txt to reset (⚠️ data lost) |
| **Wrong password error** | Typo or password mismatch | Verify Caps Lock is OFF, password is case-sensitive |
| **Vault won't save entries** | Permission denied on file write | Run executable from writable directory (not C:\Program Files) |

---

## 10. ACKNOWLEDGEMENTS & CREDITS

### Project Team

**Developed by:**
- **Shubham Kumar** - Lead Developer & Designer
- **Rani** - Project Partner & Contributor

**Institute:** Desh Bhagat University  
**Program:** B.Tech Computer Science & Engineering  
**Semester:** 4th Semester

### Core Dependencies & Libraries

**Mongoose HTTP Server**
- GitHub: https://github.com/cesanta/mongoose
- Purpose: Embedded HTTP/HTTPS server for REST API
- License: Dual licensing (commercial and open-source)

**nlohmann/json**
- GitHub: https://github.com/nlohmann/json
- Purpose: Modern C++ JSON library for API serialization
- License: MIT License

**MinGW-w64 (GCC)**
- Website: https://www.mingw-w64.org/
- Purpose: Cross-platform C++ compiler toolchain
- License: GPL v3 + GCC Runtime Exception

**CMake**
- Website: https://cmake.org/
- Purpose: Build system and project configuration
- License: BSD 3-Clause License

### Cryptographic Standards

**PBKDF2-SHA-256**
- Standard: RFC 2898
- Purpose: Password-based key derivation
- Implementation: 200,000 iterations + 16-byte salt

**AES-256-CBC**
- Standard: FIPS 197
- Purpose: Vault encryption at rest
- Implementation: Military-grade security

**SHA-256**
- Standard: FIPS 180-4
- Purpose: Message authentication & hashing
- Implementation: NIST-approved algorithm

**Base64 Encoding**
- Standard: RFC 4648
- Purpose: Binary data serialization
- Implementation: Custom C++ encoder/decoder

### Technical References & Standards

**OWASP Password Storage Cheat Sheet**
- Link: https://cheatsheetseries.owasp.org/cheatsheets/Password_Storage_Cheat_Sheet.html
- Influence: Password hashing best practices

**NIST Special Publication 800-63B**
- Link: https://pages.nist.gov/800-63-3/sp800-63b.html
- Influence: Authentication requirements & session management

**RESTful API Design Best Practices**
- Link: https://restfulapi.net/
- Influence: HTTP method semantics, API structure

**Modern Web Development Standards**
- HTML5/CSS3 (W3C standards)
- Glassmorphism UI design pattern
- Vanilla JavaScript ES6+

### Security & Learning Resources

**Learning Resources:**
- Cryptography I (Stanford): https://www.coursera.org/learn/crypto
- CryptoHack: https://cryptohack.org/
- PortSwigger Web Security Academy: https://portswigger.net/web-security
- C++ Reference: https://en.cppreference.com/
- NIST Guidelines: https://www.nist.gov/

**Related & Inspirational Projects:**
- KeePass (https://keepass.info/) - Desktop password manager
- Bitwarden (https://bitwarden.com/) - Cloud password manager
- 1Password (https://1password.com/) - Enterprise solution
- OpenSSL (https://www.openssl.org/) - Cryptographic reference
- GnuPG (https://gnupg.org/) - Cryptographic best practices

### Open Source Community

**Special Thanks To:**
- Global security research community
- Open-source developers and contributors
- Bug bounty researchers
- Educational institutions promoting cybersecurity

### License & Legal

**Project License:** Academic / Educational Project — Healix Vault, 2026

**Third-Party Licenses:**
- Mongoose: Dual licensing (commercial/open-source) ✅ Compatible
- nlohmann/json: MIT License ✅ Fully Compatible
- MinGW-w64: GPL v3 + GCC Runtime Exception ✅ Compatible
- CMake: BSD 3-Clause License ✅ Fully Compatible

**Important Disclaimer:**
- This is an educational project, NOT professionally audited
- Designed for learning cryptography and secure design principles
- NOT recommended for production use with sensitive/critical data
- For critical passwords, use professionally audited managers like Bitwarden or 1Password

### Educational Value

**This Project Demonstrates:**
✅ Secure password management architecture  
✅ Cryptographic principles in practice (AES, SHA-256, PBKDF2)  
✅ Full-stack web application development (C++ backend + web frontend)  
✅ Zero-knowledge design patterns  
✅ Session management and authentication  
✅ RESTful API design  
✅ Modern UI/UX with glassmorphism  

### Final Thanks

**"Standing on the shoulders of giants" — Isaac Newton**

We are grateful for:
- The open-source community's collaborative spirit
- Academic researchers advancing cryptography
- Security professionals establishing best practices
- Educators promoting cybersecurity education
- All contributors to the projects we built upon

**Thank you for using and learning from Healix Vault!** 🔐✨

---

## FUTURE ENHANCEMENTS

**Planned Features for v2.0:**

1. **☁️ Cloud Sync** - End-to-end encrypted sync across devices
2. **👆 Biometric Auth** - Windows Hello, Touch ID support
3. **🔐 2FA/MFA** - TOTP and hardware key support
4. **🌍 Cross-Platform** - macOS, Linux, mobile support
5. **💬 Import/Export** - Data migration from other managers
6. **🎨 Custom Themes** - Dark mode, color customization
7. **📊 Analytics** - Password health & security reports

---

## STUDENT INFORMATION

**Project Submitted By:**

| Detail | Information |
|--------|-------------|
| **Student Name 1** | Shubham Kumar |
| **Student Name 2** | Rani |
| **University** | Desh Bhagat University |
| **Branch** | B.Tech Computer Science & Engineering |
| **Semester** | 4th Semester |
| **Subject** | Advanced Programming / Cryptography / Web Development |
| **Project Type** | Full-Stack Application Development |
| **Submission Date** | May 5, 2026 |

---

## PROJECT REPOSITORY

**GitHub:** https://github.com/shubhamsingh7373/Secure-Password.git

**Clone Repository:**
```
git clone https://github.com/shubhamsingh7373/Secure-Password.git
```

---

## DOCUMENT INFORMATION

**Document Type:** Project Documentation & Submission  
**Document Version:** 1.0 (Complete)  
**Last Updated:** May 5, 2026  
**Prepared For:** Academic Submission  
**Target Audience:** University Professor & Evaluators  

---

**End of Document**

✨ **Healix Vault - Secure Password Manager v1.0** ✨

*Made with ❤️ for security education and academic excellence.*

🔐 **Your passwords. Your device. Your control.** 🔐
