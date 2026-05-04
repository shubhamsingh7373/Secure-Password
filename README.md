# 🔐 Healix Vault — Secure Password Manager

A full-stack, locally-hosted password manager built with a **C++ backend** (Mongoose HTTP server, AES-256 encryption, PBKDF2-SHA-256 authentication) and a **modern web frontend** (HTML/CSS/JS glassmorphism UI).

---

## 🏗️ Architecture Overview

```
┌─────────────────────────────────────────────────────┐
│              Browser (HTML + CSS + JS)               │
│  Login │ Dashboard │ Generator │ Search │ CRUD       │
└────────────────────┬────────────────────────────────┘
                     │  HTTP REST API (localhost:8080)
                     │  Authorization: <derived-key>
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
└─────────────────────────────────────────────────────┘
```

---

## 🔒 Security Implementation

| Layer | Implementation |
|-------|---------------|
| Password Hashing | PBKDF2-SHA-256, 200,000 iterations, random 16-byte salt |
| Vault Encryption | AES-256-CBC, random IV + salt per save, PKCS#7 padding |
| Key Derivation | PBKDF2 with `:enc` context separator (auth key ≠ vault key) |
| Token | Derived 32-byte hex key used as session Authorization header |
| Data at Rest | vault.dat is fully AES-256 encrypted — unreadable without master password |
| Auto-lock | Clears token after 5 minutes of inactivity |

All crypto (SHA-256, AES-256, PBKDF2, Base64) is implemented **from scratch in C++** — no OpenSSL dependency.

---

## 📁 Project Structure

```
CP/
├── CMakeLists.txt              # Build configuration
├── vault.dat                   # AES-256 encrypted vault (auto-created)
├── vault_config.txt            # PBKDF2 hash + salt (auto-created)
│
├── include/
│   ├── auth.h                  # Authentication interface
│   ├── crypto.h                # AES-256, SHA-256, PBKDF2, Base64
│   ├── vault.h                 # Vault CRUD interface + Entry struct
│   ├── password_gen.h          # Password generator interface
│   ├── ui.h                    # (unused in web mode)
│   ├── mongoose.h              # Embedded HTTP server
│   └── nlohmann/json.hpp       # JSON library
│
├── src/
│   ├── main.cpp                # HTTP server + REST API routes
│   ├── auth.cpp                # PBKDF2 login/setup implementation
│   ├── crypto.cpp              # Full AES-256-CBC + SHA-256 + PBKDF2
│   ├── vault.cpp               # Entry CRUD + AES encrypt/decrypt storage
│   ├── password_gen.cpp        # Password generator + strength scorer
│   ├── ui.cpp                  # (legacy console UI)
│   └── mongoose.c              # Mongoose HTTP server implementation
│
└── public/                     # Frontend (served by C++ server)
    ├── index.html              # Full SPA markup
    ├── style.css               # Glassmorphism design system
    └── app.js                  # All frontend logic
```

---

## 🛠️ Prerequisites

| Tool | Version | Download |
|------|---------|----------|
| CMake | ≥ 3.10 | https://cmake.org/download/ |
| MinGW-w64 (GCC) | ≥ 8.0 | https://www.mingw-w64.org/ |
| Modern browser | Chrome/Edge/Firefox | — |

Verify tools are installed:
```powershell
cmake --version
g++ --version
```

---

## ⚙️ Build Instructions (Windows)

### Step 1 — Clone / open the project
```powershell
cd C:\Users\shubh\Desktop\CP
```

### Step 2 — Configure with CMake
```powershell
cmake -S . -B build -G "MinGW Makefiles"
```

### Step 3 — Build
```powershell
cmake --build build --config Release
```
> Build output: `build/password_vault.exe`

### Step 4 — Run the server
```powershell
.\build\password_vault.exe
```
Expected output:
```
Starting Healix Vault Web Server on http://localhost:8080
Open your browser and navigate to http://localhost:8080
Press Ctrl+C to stop.
```

### Step 5 — Open in browser
```
http://localhost:8080
```

---

## 🚀 Usage Guide

### First Run — Setup Master Password
1. Open `http://localhost:8080`
2. The app detects **First Run** and shows the Setup form
3. Enter a master password (minimum 8 characters)
4. Watch the **password strength meter** update in real-time
5. Confirm the password and click **Initialize Vault**

### Subsequent Runs — Login
1. Open `http://localhost:8080`
2. Enter your master password and click **Unlock Vault**
3. Wrong password → error message shown, vault stays locked

### Adding a Credential
1. Click **New Entry** (top-right button)
2. Fill in: Website, Username, Password, Category, Notes
3. Use the **🔄 Generate** icon to auto-fill a secure password
4. Password strength shown live with color-coded bar
5. Click **Save Entry** — toast notification confirms save

### Password Generator
1. Click **Generator** in the left sidebar
2. Adjust length (8–64) with the slider
3. Toggle: Uppercase / Lowercase / Numbers / Symbols
4. Click **Use This Password** to fill it into the entry form

### Managing Credentials
- **👁 View** — Click the eye icon on a card to reveal password
- **📋 Copy** — Click Copy button → toast confirms clipboard copy
- **✏️ Edit** — Click Edit → modal pre-filled with current data
- **🗑 Delete** — Click trash icon → confirm dialog before deletion

### Search
- Type in the search bar (Ctrl+F to focus)
- Searches website, username, category, and notes in real-time

### Category Filtering
- Categories auto-appear in the left sidebar
- Click any category to filter entries
- Default categories: **General, Social, Banking, Work** (or any custom value)

### Auto-Lock
- Vault automatically locks after **5 minutes of inactivity**
- Timer shown in bottom-left of sidebar
- Any mouse movement or keypress resets the timer
- Click **Lock Vault** to manually lock immediately

---

## 🌐 REST API Reference

All endpoints except `/api/status`, `/api/setup`, `/api/login` require:
```
Authorization: <session-token>
```

| Method | Endpoint | Description |
|--------|----------|-------------|
| `GET` | `/api/status` | Check if first run |
| `POST` | `/api/setup` | Initialize vault with master password |
| `POST` | `/api/login` | Authenticate and get session token |
| `GET` | `/api/entries` | List all vault entries |
| `POST` | `/api/entries` | Create new entry |
| `GET` | `/api/entries/:id` | Get single entry |
| `PUT` | `/api/entries/:id` | Update entry |
| `DELETE` | `/api/entries/:id` | Delete entry |
| `GET` | `/api/search?q=term` | Server-side search |
| `GET` | `/api/generate?length=16&upper=1&lower=1&numbers=1&symbols=0` | Generate password |

### Example: Add Entry
```bash
curl -X POST http://localhost:8080/api/entries \
  -H "Authorization: <token>" \
  -H "Content-Type: application/json" \
  -d '{"website":"GitHub","username":"user@email.com","password":"Str0ng!Pass","category":"Work"}'
```

---

## 🔄 Data Persistence

- `vault_config.txt` — stores PBKDF2 hash + salt (survives restart)
- `vault.dat` — AES-256 encrypted credential store (survives restart)
- Both files are in the **same directory as the executable** (project root)
- Deleting `vault_config.txt` resets to first-run (vault.dat becomes unreadable)

---

## 🐛 Troubleshooting

| Issue | Solution |
|-------|----------|
| `Cannot connect to server` | Make sure `password_vault.exe` is running |
| Build fails: `ws2_32 not found` | Ensure MinGW-w64 is installed, not MinGW-32 |
| `cmake` not found | Add CMake to PATH or reinstall |
| Port 8080 in use | Edit `main.cpp` line `mg_http_listen(..., "http://0.0.0.0:8080", ...)` |
| Forgot master password | Delete `vault.dat` and `vault_config.txt` to reset (data lost) |

---

## 🔮 Future Improvements

While this version provides a solid, secure local foundation, the following features are planned for future releases to enhance usability and security:

*   **☁️ Cloud Sync (Optional, Encrypted):** Sync your encrypted `vault.dat` across multiple devices using Google Drive, Dropbox, or a custom secure backend. The data will remain end-to-end encrypted, meaning the cloud provider cannot read your passwords.
*   **👆 Biometric Unlock:** Integration with Windows Hello (Fingerprint/Facial Recognition) or Apple Touch ID/Face ID to quickly unlock the vault without typing the master password every time, while keeping the encryption key secure in the OS keychain.
*   **🔐 2FA Support (Two-Factor Authentication):** Add an extra layer of security using TOTP (Authenticator apps like Google Authenticator or Authy) or hardware security keys (YubiKey) to prevent unauthorized access even if the master password is compromised.

---

## 📝 License

Academic / Educational Project — Healix Vault, 2026
