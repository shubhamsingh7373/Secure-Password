# 🔐 HEALIX VAULT - SECURE PASSWORD MANAGER
## Academic Project Report

---

## 📋 ACKNOWLEDGMENTS & STUDENT INFORMATION

### 👥 Project Team

| Role | Name | Roll Number | Details |
|------|------|-------------|---------|
| **Lead Developer** | Shubham Kumar | 24322147017 | Primary Developer & Project Lead |
| **Co-Developer** | Anish Kaushal | - | Project Partner |

### 🏫 Institution Information

- **University**: Desh Bhagat University
- **Branch**: B.Tech Computer Science Engineering (CSE)
- **Semester**: 4th Semester
- **Academic Year**: 2025-2026
- **Project Date**: May 2026

### 🙏 Acknowledgments

We acknowledge the following:

1. **Desh Bhagat University** for providing the academic platform and resources
2. **Our Faculty Members** for their guidance and mentorship throughout this project
3. **Open-Source Community** for cryptographic standards and best practices
4. **NIST (National Institute of Standards and Technology)** for cryptographic algorithms
5. **RFC Standards** for industry best practices

---

## 📊 PROJECT OVERVIEW

### Project Title
**Healix Vault — Secure Password Manager**

### Project Tagline
> **Your passwords. Your device. Your control.**
> A full-stack, locally-hosted password manager with military-grade encryption and zero-trust architecture.

### Project Description

Healix Vault is an educational password management system that demonstrates secure cryptographic implementation through a complete full-stack application. The project combines:

- **C++ Backend**: Mongoose HTTP server with AES-256 encryption and PBKDF2-SHA-256 authentication
- **Modern Frontend**: HTML/CSS/JavaScript with glassmorphism design and real-time UI updates
- **Zero-Cloud Architecture**: All data remains local on the user's device
- **No External Dependencies**: Cryptography implemented from scratch for full control

### Key Statistics

| Metric | Value |
|--------|-------|
| **Primary Language** | C++ (Backend), JavaScript/HTML/CSS (Frontend) |
| **Repository** | https://github.com/shubhamsingh7373/Secure-Password |
| **License** | MIT License |
| **Repository ID** | 1229009004 |
| **Status** | Active Development |
| **Version** | 1.0 (Educational Release) |

---

## ✨ KEY FEATURES

### Security Features

| Feature | Description |
|---------|-------------|
| 🔐 **AES-256 Encryption** | Military-grade encryption for all passwords at rest |
| 🔑 **PBKDF2-SHA-256 Hashing** | Master password secured with 200,000 iterations + random salt |
| 💻 **Zero-Trust Architecture** | All cryptographic operations happen locally; no data leaves your device |
| 🔄 **Smart Key Derivation** | Separate authentication and encryption keys using context separation |
| 🛡️ **Cryptographic Randomness** | True random IV and salt generated per encryption operation |

### User Experience Features

| Feature | Description |
|---------|-------------|
| 🎨 **Modern UI** | Glassmorphism design with intuitive navigation |
| 📊 **Real-time Password Strength Meter** | Color-coded strength indicator (Red → Green) |
| 🔄 **Smart Password Generator** | Customizable length, character sets, real-time strength scoring |
| 🔎 **Instant Search** | Filter by website, username, category, or notes in real-time |
| 📂 **Category Filtering** | Auto-organized credentials (Banking, Social, Work, Custom) |
| ⏱️ **Auto-Lock Feature** | 5-minute inactivity timer protects against physical theft |
| 📋 **One-Click Copy** | Copy passwords to clipboard with toast notifications |
| 👁️ **Show/Hide Toggle** | Reveal/hide passwords in the UI with eye icon |

### Developer Features

| Feature | Description |
|---------|-------------|
| 🌐 **REST API** | Full CRUD operations via HTTP REST with token authentication |
| 📈 **Server-side Search** | Optimized search endpoint for large credential lists |
| 🔐 **Session Management** | Token-based authentication with automatic expiration |
| 📁 **Persistent Storage** | AES-256 encrypted vault survives application restart |

---

## 🏗️ ARCHITECTURE OVERVIEW

### System Architecture Diagram

```
┌──────────────────────────────────────────────────────────┐
│                Browser (Web Frontend)                     │
│      HTML + CSS + JavaScript (Vanilla, No Frameworks)    │
│  🔓 Login │ 📊 Dashboard │ ⚙️ Generator │ 🔍 Search      │
└──────────────────────┬──────────────────────────────────┘
                       │
                       │  HTTP REST API
                       │  Host: localhost:8080
                       │  Authorization: <session-token>
                       │
┌──────────────────────▼──────────────────────────────────┐
│         C++ Mongoose Embedded HTTP Server                │
│                                                          │
│  ┌────────────────┐  ┌────────────────┐  ┌───────────┐ │
│  │ Auth Module    │  │ Vault Module   │  │ Password  │ │
│  │ • PBKDF2       │  │ • AES-256-CBC  │  │ Generator │ │
│  │ • Session Mgmt │  │ • Entry CRUD   │  │ • Charset │ │
│  │ • SHA-256      │  │ • Encryption   │  │ • Entropy │ │
│  └────────────────┘  └────────────────┘  └───────────┘ │
│                           │                              │
│                  Persistent Storage                      │
│         ├── vault.dat (AES-256 encrypted)               │
│         └── vault_config.txt (PBKDF2 hash + salt)       │
└─��────────────────────────────────────────────────────────┘
```

### Technology Stack

#### Backend
- **Language**: C++14/17
- **HTTP Server**: Mongoose Embedded HTTP Server
- **Cryptography**: Custom C++ implementation (no OpenSSL)
- **JSON Serialization**: nlohmann/json library
- **Build System**: CMake 3.10+

#### Frontend
- **HTML5**: Semantic markup
- **CSS3**: Glassmorphism design with modern visual effects
- **JavaScript**: Vanilla (ES6+), no frameworks
- **HTTP**: Fetch API for REST communication

#### Build & Deployment
- **Compiler**: GCC/MinGW-w64 8.0+
- **Build Tools**: CMake
- **Executable Size**: ~2-3 MB (Release build)
- **Memory Usage**: ~10-20 MB at runtime

---

## 🔒 SECURITY IMPLEMENTATION

### Cryptographic Standards Compliance

| Layer | Implementation | Standard | Strength |
|-------|---|---|---|
| **Password Hashing** | PBKDF2-SHA-256, 200,000 iterations, random 16-byte salt | RFC 2898 | ⭐⭐⭐⭐⭐ |
| **Vault Encryption** | AES-256-CBC, random IV + salt per save, PKCS#7 padding | FIPS 197 | ⭐⭐⭐⭐⭐ |
| **Key Derivation** | PBKDF2 with context separator (auth key ≠ vault key) | NIST SP 800-132 | ⭐⭐⭐⭐⭐ |
| **Random Number Generation** | Cryptographically secure randomness | /dev/urandom | ⭐⭐⭐⭐⭐ |
| **Session Tokens** | Derived 32-byte hex keys | Custom Implementation | ⭐⭐⭐⭐ |
| **Data at Rest** | Full AES-256 encryption | Unreadable without master password | ⭐⭐⭐⭐⭐ |
| **Auto-lock** | 5-minute inactivity timer | Session Management Best Practice | ⭐⭐⭐⭐ |

### Security Highlights

✅ **All cryptography implemented from scratch in C++**
- Full control over implementation
- No vendor lock-in
- Transparent and auditable code
- No hidden dependencies

✅ **Zero-Knowledge Architecture**
- Only your device knows your passwords
- No cloud synchronization
- No telemetry or tracking
- No external servers

✅ **Industry-Standard Algorithms**
- PBKDF2: Key derivation function (RFC 2898)
- AES-256: Symmetric encryption (FIPS 197)
- SHA-256: Cryptographic hash (NIST-approved)
- All NIST-recommended standards

✅ **Secure Key Management**
- Separate authentication and encryption keys
- Context separation using `:enc` and `:auth` suffixes
- Random IV generated per encryption
- Random salt generated per PBKDF2 operation

### Security Assessment

#### ✅ Strengths
- Industry-standard cryptographic algorithms
- Zero external crypto dependencies
- Secure key derivation with context separation
- Strong password hashing (200,000 PBKDF2 iterations)
- Session-based token authentication
- Automatic inactivity timeout

#### ⚠️ Limitations (Educational Project)
- **NOT professionally audited** - Educational purpose only
- Local-only deployment - Single device only
- No biometric authentication
- No 2FA/MFA support
- Limited threat model - Assumes local device security
- No protection against keyloggers or malware on host system

### Recommended Use Cases

✅ **Suitable For:**
- Learning cryptography and security design
- Storing non-critical passwords locally
- Personal use on trusted devices
- Educational demonstrations
- Portfolio projects

❌ **NOT Suitable For:**
- Production use with sensitive/critical passwords
- Business-critical credentials
- Compliance-regulated environments (HIPAA, SOC 2, etc.)
- Multi-user or enterprise deployments

---

## 📁 PROJECT STRUCTURE

```
Secure-Password/
│
├── 📄 CMakeLists.txt                    # Build configuration for CMake
├── 📄 README.md                         # Comprehensive project documentation
├── 📄 LICENSE                           # MIT License
├── 📄 PROJECT_REPORT.md                 # This academic report
│
├── 📁 include/                          # C++ Header Files
│   ├── auth.h                           # Authentication & session management
│   ├── crypto.h                         # AES-256, SHA-256, PBKDF2, Base64
│   ├── vault.h                          # Vault CRUD + Entry data structure
│   ├── password_gen.h                   # Password generator + strength scoring
│   ├── mongoose.h                       # Embedded HTTP server
│   └── nlohmann/json.hpp                # JSON serialization library
│
├── 📁 src/                              # C++ Implementation Files
│   ├── main.cpp                         # HTTP server + REST API routes
│   ├── auth.cpp                         # PBKDF2 login/setup implementation
│   ├── crypto.cpp                       # AES-256-CBC + SHA-256 + PBKDF2
│   ├── vault.cpp                        # Entry CRUD + encryption/decryption
│   ├── password_gen.cpp                 # Password generator + strength scorer
│   └── mongoose.c                       # Embedded HTTP server core
│
├── 📁 public/                           # Frontend Assets (Served by C++ Server)
│   ├── index.html                       # Single-page app markup + structure
│   ├── style.css                        # Glassmorphism UI design
│   └── app.js                           # All frontend logic (vanilla JavaScript)
│
└── 📁 build/                            # Build Output Directory (Auto-generated)
    └── password_vault.exe               # Compiled executable (Windows)
```

### Directory Descriptions

#### `include/` - Header Files
Contains all C++ header files defining interfaces and data structures.

#### `src/` - Implementation
Contains all C++ implementation files (.cpp) and compiled object files.

#### `public/` - Web Frontend
- **index.html**: Single-page application markup with responsive design
- **style.css**: Modern UI with glassmorphism and animations
- **app.js**: Vanilla JavaScript handling all frontend logic

#### `build/` - Build Artifacts
Generated automatically by CMake during the build process.

---

## 🛠️ PREREQUISITES & INSTALLATION

### System Requirements

| Component | Requirement | Version | Status |
|-----------|-------------|---------|--------|
| **Operating System** | Windows 10/11 | Latest | ✅ Tested |
| **CMake** | Build Configuration Tool | ≥ 3.10 | [Download](https://cmake.org/download/) |
| **MinGW-w64 (GCC)** | C++ Compiler | ≥ 8.0 | [Download](https://www.mingw-w64.org/) |
| **Modern Browser** | Client Application | Latest | ✅ Chrome/Edge/Firefox/Safari |

### Verify Installation

```powershell
# Check CMake installation
cmake --version
# Expected output: cmake version 3.10 or higher

# Check GCC installation
g++ --version
# Expected output: g++ version 8.0 or higher
```

### Prerequisites Installation Steps

#### 1. Install CMake
- Download from: https://cmake.org/download/
- Windows: Use installer (check "Add CMake to PATH")
- Verify: `cmake --version`

#### 2. Install MinGW-w64
- Download from: https://www.mingw-w64.org/
- Choose: x86_64 architecture
- Verify: `g++ --version`

#### 3. Install Browser
- Any modern browser (Chrome, Edge, Firefox, Safari)
- JavaScript enabled (default)

---

## ⚙️ BUILD INSTRUCTIONS (Windows)

### Step 1: Navigate to Project Directory
```powershell
cd C:\Users\YourName\Desktop\Secure-Password
```

### Step 2: Configure with CMake
```powershell
cmake -S . -B build -G "MinGW Makefiles"
```
- Creates `build/` directory with configuration files
- Generates Makefiles for compilation

### Step 3: Build Release Binary
```powershell
cmake --build build --config Release
```
- Compiles all source files
- Links libraries
- Output: `build/password_vault.exe` (~2-3 MB)

### Step 4: Run the Server
```powershell
.\build\password_vault.exe
```

Expected output:
```
Starting Healix Vault Web Server on http://localhost:8080
Open your browser and navigate to http://localhost:8080
Press Ctrl+C to stop.
```

### Step 5: Access in Browser
Open: `http://localhost:8080`

---

## 🚀 USAGE GUIDE

### 🔓 First Run — Setup Master Password

1. Open `http://localhost:8080` in browser
2. App detects **First Run** and shows setup form
3. Enter master password (minimum 8 characters)
4. Watch the **password strength meter** update in real-time:
   - Red: Weak
   - Yellow: Fair
   - Green: Strong
5. Confirm password and click **Initialize Vault**
6. Setup completes in seconds!

### 🔑 Subsequent Runs — Login

1. Open `http://localhost:8080`
2. Enter your master password
3. Click **Unlock Vault**
4. Wrong password? Error message shown, vault stays locked
5. Correct password? Redirected to dashboard

### 💾 Adding a Credential

1. Click **➕ New Entry** (top-right button)
2. Fill in the form:
   - **Website**: e.g., `github.com`
   - **Username**: e.g., `user@email.com`
   - **Password**: Manually enter or auto-generate
   - **Category**: Select from dropdown (Banking, Social, Work, General, etc.)
   - **Notes**: Optional additional information
3. Click **🔄 Generate** to auto-fill with secure password
4. Watch the **color-coded strength bar** update in real-time
5. Click **Save Entry** — toast notification confirms success

### ⚙️ Password Generator

1. Click **Generator** tab in left sidebar
2. Adjust **length** slider (8–64 characters)
3. Toggle character type checkboxes:
   - ✅ Uppercase (A-Z)
   - ✅ Lowercase (a-z)
   - ✅ Numbers (0-9)
   - ✅ Symbols (!@#$%^&*...)
4. Watch **real-time strength scoring** update
5. Click **Use This Password** to populate entry form

### 👁️ Managing Credentials

| Action | How To | Result |
|--------|--------|--------|
| **View Password** | Click 👁️ eye icon on card | Password revealed on click |
| **Copy to Clipboard** | Click 📋 copy button | Toast confirms copy success |
| **Edit Entry** | Click ✏️ edit button | Modal opens with pre-filled data |
| **Delete Entry** | Click 🗑️ trash button | Confirmation dialog appears |
| **Lock Vault** | Click 🔒 lock button | Vault locked, redirects to login |

### 🔍 Search & Filter

**Search Bar:**
- Type in search field (focus with Ctrl+F)
- Searches: website, username, category, notes
- Results update as you type
- Matches are highlighted

**Category Filtering:**
- Categories auto-appear in left sidebar
- Click category to filter entries
- Shows only entries in selected category
- Default categories: General, Social, Banking, Work
- Custom categories supported

### ⏱️ Auto-Lock Feature

- Vault **automatically locks after 5 minutes** of inactivity
- Inactivity timer shown in bottom-left of sidebar
- Any **mouse movement or keypress** resets timer
- Click **🔒 Lock Vault** to manually lock immediately
- Click **Lock** when leaving your computer

---

## 🌐 REST API REFERENCE

### API Base URL
```
http://localhost:8080/api
```

### Authentication
All endpoints except `/api/status`, `/api/setup`, `/api/login` require:
```
Authorization: <session-token>
Content-Type: application/json
```

### Endpoint Overview

| Method | Endpoint | Description | Auth Required |
|--------|----------|-------------|---|
| `GET` | `/api/status` | Check if first run / vault initialized | ❌ No |
| `POST` | `/api/setup` | Initialize vault with master password | ❌ No |
| `POST` | `/api/login` | Authenticate and get session token | ❌ No |
| `GET` | `/api/entries` | List all vault entries | ✅ Yes |
| `POST` | `/api/entries` | Create new entry | ✅ Yes |
| `GET` | `/api/entries/:id` | Get single entry by ID | ✅ Yes |
| `PUT` | `/api/entries/:id` | Update existing entry | ✅ Yes |
| `DELETE` | `/api/entries/:id` | Delete entry | ✅ Yes |
| `GET` | `/api/search?q=term` | Server-side search | ✅ Yes |
| `GET` | `/api/generate?...` | Generate secure password | ✅ Yes |

### Complete Workflow Example

#### 1. Check Vault Status (First Time)
```bash
curl http://localhost:8080/api/status

# Response:
# {"first_run": true}
```

#### 2. Setup Master Password (First Time Only)
```bash
curl -X POST http://localhost:8080/api/setup \
  -H "Content-Type: application/json" \
  -d '{"master_password":"MyStr0ng!Pass"}'

# Response:
# {"success": true, "token": "a1b2c3d4e5f6g7h8i9j0..."}
```

#### 3. Login (Subsequent Times)
```bash
curl -X POST http://localhost:8080/api/login \
  -H "Content-Type: application/json" \
  -d '{"master_password":"MyStr0ng!Pass"}'

# Response:
# {"success": true, "token": "x9y8z7w6v5u4t3s2r1q0..."}
```

#### 4. Add New Entry
```bash
curl -X POST http://localhost:8080/api/entries \
  -H "Authorization: x9y8z7w6v5u4t3s2r1q0..." \
  -H "Content-Type: application/json" \
  -d '{
    "website":"GitHub",
    "username":"user@email.com",
    "password":"Str0ng!Pass",
    "category":"Work",
    "notes":"Primary development account"
  }'

# Response:
# {"id": 1, "success": true}
```

#### 5. List All Entries
```bash
curl http://localhost:8080/api/entries \
  -H "Authorization: x9y8z7w6v5u4t3s2r1q0..."

# Response:
# [
#   {"id": 1, "website": "GitHub", "username": "user@email.com", ...},
#   {"id": 2, "website": "Gmail", "username": "user@gmail.com", ...}
# ]
```

#### 6. Search Entries
```bash
curl "http://localhost:8080/api/search?q=github" \
  -H "Authorization: x9y8z7w6v5u4t3s2r1q0..."

# Response:
# [{"id": 1, "website": "GitHub", "username": "user@email.com", ...}]
```

#### 7. Generate Secure Password
```bash
curl "http://localhost:8080/api/generate?length=16&upper=1&lower=1&numbers=1&symbols=1" \
  -H "Authorization: x9y8z7w6v5u4t3s2r1q0..."

# Response:
# {"password": "aB3$xYz9@Qw5mN2!"}
```

#### 8. Update Entry
```bash
curl -X PUT http://localhost:8080/api/entries/1 \
  -H "Authorization: x9y8z7w6v5u4t3s2r1q0..." \
  -H "Content-Type: application/json" \
  -d '{
    "website":"GitHub",
    "username":"newuser@email.com",
    "password":"NewStr0ng!Pass",
    "category":"Work",
    "notes":"Updated account"
  }'

# Response:
# {"success": true, "id": 1}
```

#### 9. Delete Entry
```bash
curl -X DELETE http://localhost:8080/api/entries/1 \
  -H "Authorization: x9y8z7w6v5u4t3s2r1q0..."

# Response:
# {"success": true, "message": "Entry deleted"}
```

---

## 🔄 DATA PERSISTENCE

### File Storage

The application creates two files in the project root directory:

1. **`vault_config.txt`** — Configuration and authentication
   - Contains: PBKDF2 hash + salt + iteration count
   - Format: `[PBKDF2_HASH]:[SALT]:[ITERATIONS]`
   - Purpose: Master password verification
   - Survives application restart

2. **`vault.dat`** — Encrypted vault data
   - Contains: All encrypted credential entries
   - Format: Binary AES-256 encrypted data
   - Purpose: Secure storage of passwords
   - Unreadable without correct master password
   - Survives application restart

### Data Recovery

- **All data is local** — No cloud backup
- **Deleting `vault_config.txt`** resets to first-run (vault.dat becomes inaccessible)
- **Deleting `vault.dat`** loses all stored credentials
- **Backup**: Manually copy both files to secure location

### Data Encryption

All sensitive data is encrypted using AES-256-CBC:
- Vault entries encrypted with derived key
- Random IV generated per encryption
- Random salt generated for PBKDF2
- PKCS#7 padding applied

---

## 🐛 TROUBLESHOOTING

| Issue | Cause | Solution |
|-------|-------|----------|
| Cannot connect to server | Executable not running | Run `.\build\password_vault.exe` again |
| Build fails: `ws2_32 not found` | Wrong MinGW version | Install MinGW-w64 (x86_64, not i686) |
| `cmake` not found | CMake not in PATH | Reinstall CMake, ensure "Add to PATH" is checked during installation |
| Port 8080 in use | Another application using port | Change port in `main.cpp` (search for `mg_http_listen`) |
| Forgot master password | Master password required for access | Delete `vault.dat` + `vault_config.txt` to reset (⚠️ all data lost) |
| Wrong password error | Typo or different password entered | Ensure Caps Lock is OFF; password is case-sensitive |
| Vault won't save entries | File permissions issue | Run executable from a writable directory (not Program Files) |
| Frontend not loading | Server not serving static files | Check `public/` directory exists in project root |
| Passwords not showing in dashboard | Browser cache issue | Hard refresh (Ctrl+Shift+R) or clear browser cache |
| API returns 401 Unauthorized | Session token expired or invalid | Log out and log in again to get new token |
| Cannot generate passwords | Browser JavaScript disabled | Enable JavaScript in browser settings |

### Debug Mode

To add verbose logging:
1. Edit `src/main.cpp`
2. Uncomment debug statements (search for `// DEBUG`)
3. Rebuild with `cmake --build build --config Release`

---

## 🔮 FUTURE IMPROVEMENTS

### Planned Features (Post-MVP)

#### ☁️ Cloud Sync (Optional, End-to-End Encrypted)
- Sync encrypted vault across multiple devices
- Supports: Google Drive, Dropbox, custom secure backend
- Data remains end-to-end encrypted
- Cloud providers cannot read passwords

#### 👆 Biometric Unlock
- Windows Hello (Fingerprint/Facial Recognition)
- Apple Touch ID/Face ID (macOS/iOS)
- Quick vault access without typing master password
- Encryption keys remain secure in OS keychain

#### 🔐 2FA / MFA Support
- TOTP (Time-based One-Time Password)
- Google Authenticator / Authy integration
- Hardware security keys (YubiKey, Windows Hello)
- Prevents unauthorized access even if master password compromised

#### 🌍 Cross-Platform Support
- macOS: Native Cocoa UI or web frontend
- Linux: Native GTK UI or web frontend
- Mobile: iOS/Android companion apps
- Unified codebase with platform-specific optimizations

#### 💬 Import/Export Features
- Import from CSV, JSON, other password managers
- Export encrypted backups
- Bulk credential management
- Migration tools for legacy systems

#### 🧪 Advanced Security
- Security audit by professional firm
- Penetration testing
- Code review by cryptography experts
- OWASP compliance verification

---

## 📊 PERFORMANCE & SPECIFICATIONS

### Performance Metrics

| Metric | Value | Notes |
|--------|-------|-------|
| **Executable Size** | ~2-3 MB | Release build (stripped) |
| **Memory Usage** | ~10-20 MB | At runtime, all entries loaded |
| **Startup Time** | ~500-1000 ms | Depends on hardware |
| **Encryption Speed** | ~50-100 ms | Per credential (depends on CPU) |
| **Decryption Speed** | ~50-100 ms | Per credential (depends on CPU) |
| **UI Responsiveness** | <100 ms | Dashboard updates, search |
| **Max Entries** | 1GB+ | Theoretical limit based on disk space |
| **Password Length** | Up to 1024 chars | Support in generator and storage |
| **Concurrent Users** | 1 | Local-only, single user |
| **Session Timeout** | 5 minutes | Inactivity auto-lock |

### System Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| **CPU** | 1.0 GHz | 2.0 GHz+ |
| **RAM** | 512 MB | 2 GB+ |
| **Storage** | 50 MB | 1 GB+ |
| **Internet** | None required | None required (offline-capable) |

---

## 🎓 LEARNING OUTCOMES

### Knowledge Gained

#### Cryptography
- ✅ Symmetric encryption (AES-256-CBC)
- ✅ Cryptographic hashing (SHA-256)
- ✅ Key derivation functions (PBKDF2)
- ✅ Random number generation
- ✅ Encryption modes and padding schemes

#### Software Engineering
- ✅ Full-stack application development
- ✅ RESTful API design
- ✅ Client-server architecture
- ✅ Session management
- ✅ Error handling and validation

#### C++ Programming
- ✅ Advanced C++ features
- ✅ Memory management
- ✅ HTTP server implementation
- ✅ Binary data handling
- ✅ Build system (CMake)

#### Web Development
- ✅ HTML5/CSS3 design
- ✅ JavaScript (Vanilla ES6+)
- ✅ DOM manipulation
- ✅ Fetch API and HTTP requests
- ✅ Real-time UI updates

#### Security Best Practices
- ✅ Zero-knowledge architecture
- ✅ Threat modeling
- ✅ Defense in depth
- ✅ Secure coding principles
- ✅ Cryptographic standards compliance

---

## 📚 REFERENCES & RESOURCES

### Cryptography Standards
- **RFC 2898**: PBKDF2 Key Derivation Function
- **FIPS 197**: Advanced Encryption Standard (AES)
- **FIPS 180-4**: Secure Hash Standard (SHA-256)
- **NIST SP 800-132**: Password-Based Key Derivation

### Learning Resources
- **Coursera**: Cryptography I (Dan Boneh)
- **PortSwigger**: Web Security Academy
- **OWASP**: Password Storage Cheat Sheet
- **C++ Reference**: https://en.cppreference.com/

### Open-Source Projects Studied
- **Mongoose HTTP Server**: Embedded web server
- **nlohmann/json**: JSON serialization
- **KeePass**: Password manager design
- **Bitwarden**: Modern security architecture

### Academic Papers
- Schneier, B. (1996). "Applied Cryptography"
- Ferguson, N., & Schneier, B. (2003). "Practical Cryptography"
- NIST (2013). "Recommendation for Password-Based Key Derivation"

---

## 📞 PROJECT CONTACT & SUPPORT

### Student Information
- **Lead Developer**: Shubham Kumar (Roll: 24322147017)
- **Co-Developer**: Anish Kaushal
- **Institution**: Desh Bhagat University
- **Course**: B.Tech CSE, 4th Semester
- **Academic Year**: 2025-2026

### GitHub Repository
- **URL**: https://github.com/shubhamsingh7373/Secure-Password
- **License**: MIT

### Support
- **GitHub Issues**: For bug reports and feature requests
- **Documentation**: Comprehensive README.md included
- **Code Comments**: All critical sections documented

---

## ✅ PROJECT COMPLETION CHECKLIST

- ✅ Full-stack application completed
- ✅ C++ backend with encryption implemented
- ✅ Modern web frontend developed
- ✅ REST API fully functional
- ✅ AES-256 encryption implemented
- ✅ PBKDF2-SHA-256 hashing implemented
- ✅ Zero-trust architecture implemented
- ✅ Session management implemented
- ✅ Password generator implemented
- ✅ Search and filter functionality
- ✅ Auto-lock feature implemented
- ✅ Comprehensive documentation
- ✅ Build system (CMake) configured
- ✅ Error handling and validation
- ✅ UI/UX design completed
- ✅ Testing completed on Windows

---

## 📝 REVISION HISTORY

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | May 5, 2026 | Initial release - Educational project |

---

## 🎉 CONCLUSION

**Healix Vault** demonstrates a comprehensive understanding of:
- Advanced cryptography and secure coding
- Full-stack application development
- Modern software architecture
- Security best practices
- Professional project documentation

This project serves as an educational foundation for understanding how password managers work internally and showcases best practices in secure software development.

---

## 📄 LEGAL & ACADEMIC INTEGRITY

### Acknowledgments

This project was developed as part of the B.Tech CSE curriculum at Desh Bhagat University.

### Contributors
- **Primary Developer**: Shubham Kumar (Roll: 24322147017)
- **Secondary Developer**: Anish Kaushal

### Academic Integrity
All code is original work developed by the project team. External libraries (Mongoose, nlohmann/json) are properly credited and licensed.

### License
This project is released under the MIT License. See LICENSE file for details.

---

**Made with ❤️ for security education.**

**Status**: ✅ Active Development  
**Version**: 1.0 (Educational Release)  
**Last Updated**: May 5, 2026  
**University**: Desh Bhagat University, B.Tech CSE, 4th Semester
