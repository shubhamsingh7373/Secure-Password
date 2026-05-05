# 🙏 Acknowledgements

## Healix Vault — Secure Password Manager

This project stands on the shoulders of brilliant open-source projects, cryptographic research, and the collaborative efforts of the security community.

---

## 💻 Core Dependencies & Libraries

### **[Mongoose HTTP Server](https://github.com/cesanta/mongoose)**
- **Purpose:** Lightweight embedded HTTP/HTTPS server
- **Role:** Powers the REST API backend and static file serving
- **Why Chosen:** Minimal dependencies, perfect for embedded systems
- **License:** Dual licensing (commercial and open-source)

### **[nlohmann/json](https://github.com/nlohmann/json)**
- **Purpose:** Modern C++ JSON library
- **Role:** Serializes/deserializes API requests and vault data
- **Why Chosen:** Header-only, zero external dependencies, excellent performance
- **License:** MIT License
- **Version:** Compatible with C++11+

### **[MinGW-w64 (GCC)](https://www.mingw-w64.org/)**
- **Purpose:** Cross-platform C++ compiler toolchain
- **Role:** Compiles C++ source to Windows executables
- **Why Chosen:** Free, open-source, industry standard
- **License:** GPL v3 + GCC Runtime Exception

### **[CMake](https://cmake.org/)**
- **Purpose:** Build system and project configuration
- **Role:** Manages compilation, linking, and cross-platform builds
- **Why Chosen:** Universal standard, minimal boilerplate
- **License:** BSD 3-Clause License

---

## 🔐 Cryptographic Foundations

### **PBKDF2-SHA-256**
- **Algorithm:** Password-Based Key Derivation Function 2 with SHA-256 HMAC
- **Standard:** [RFC 2898](https://datatracker.ietf.org/doc/html/rfc2898)
- **Implementation:** Custom C++ from NIST specifications
- **Use Case:** Master password hashing (200,000 iterations + 16-byte salt)
- **Strength:** Industry standard, NIST-approved

### **AES-256-CBC**
- **Algorithm:** Advanced Encryption Standard, 256-bit key, CBC mode
- **Standard:** [FIPS 197](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf)
- **Implementation:** Custom C++ block cipher + mode
- **Use Case:** Vault encryption with random IV + PKCS#7 padding
- **Strength:** Military-grade, no known practical attacks (as of 2026)

### **SHA-256**
- **Algorithm:** Secure Hash Algorithm 2, 256-bit output
- **Standard:** [FIPS 180-4](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf)
- **Implementation:** Custom C++ implementation
- **Use Case:** Message authentication, integrity verification, hashing
- **Strength:** NIST-approved, widely used in cryptographic systems

### **Base64 Encoding**
- **Purpose:** Binary data serialization for JSON and storage
- **Standard:** [RFC 4648](https://datatracker.ietf.org/doc/html/rfc4648)
- **Implementation:** Custom C++ encoder/decoder
- **Use Case:** Storing binary encrypted data as text-safe format

---

## 📚 Technical & Security References

### OWASP (Open Web Application Security Project)
- **Resource:** [OWASP Password Storage Cheat Sheet](https://cheatsheetseries.owasp.org/cheatsheets/Password_Storage_Cheat_Sheet.html)
- **Contribution:** Best practices for password hashing and storage
- **Key Takeaway:** PBKDF2, bcrypt, scrypt, and Argon2 are approved methods

### NIST Cybersecurity Framework
- **Publication:** [NIST Special Publication 800-63B](https://pages.nist.gov/800-63-3/sp800-63b.html)
- **Focus:** Authentication and lifecycle management
- **Influence:** Master password requirements, session token design

### RESTful API Design
- **Resource:** [REST API Best Practices](https://restfulapi.net/)
- **Contribution:** HTTP method semantics, status codes, error handling
- **Implementation:** Clean separation between authentication and resource endpoints

### Modern Web Development
- **HTML5/CSS3:** Latest W3C standards
- **Glassmorphism:** Contemporary UI design pattern
- **Vanilla JavaScript:** No external framework dependencies, pure ES6+

---

## 🛡️ Security Research & Standards

### IETF Standards
- **RFC 2898:** PBKDF2 specification
- **RFC 4648:** Base64 Data Encodings
- **RFC 5234:** ABNF (for API spec)
- **RFC 7230-7235:** HTTP/1.1 semantics and routing

### NIST Publications
- **SP 800-38A:** Block Cipher Modes (CBC mode specification)
- **SP 800-132:** Password-Based Key Derivation recommendations
- **SP 800-175B:** Guideline for Using Cryptographic Algorithms

### Academic Research
- Special thanks to cryptographers and security researchers whose peer-reviewed work informs industry standards

---

## 🤝 Open Source Community

### Security Researchers
- The global security research community for continuous vulnerability disclosure and best practice evolution

### Password Manager Comparison
- Inspired by the design philosophies of Bitwarden, KeePass, and 1Password
- Demonstrates what secure password management architecture looks like

### Educational Resources
- Countless tutorials, documentation, and educational materials that made this project possible
- GitHub community for sharing knowledge and open-source collaboration

---

## 🙋 Special Mentions

- **Test Communities:** Users and security professionals who tested early versions
- **Documentation Writers:** Technical writers whose guides informed our documentation
- **Stack Overflow Community:** Collective knowledge that solved countless technical challenges

---

## 📖 Learning Resources Used

| Resource | Purpose |
|----------|---------|
| [Cryptography I (Stanford)](https://www.coursera.org/learn/crypto) | Foundational crypto theory |
| [CryptoHack](https://cryptohack.org/) | Practical cryptographic challenges |
| [PortSwigger Web Security Academy](https://portswigger.net/web-security) | Web application security |
| [C++ Reference](https://en.cppreference.com/) | Language standards and best practices |
| [Mongoose Documentation](https://github.com/cesanta/mongoose) | HTTP server integration |

---

## 🔗 Related Projects & Tools

- **[KeePass](https://keepass.info/)** — Desktop password manager (inspired architecture)
- **[Bitwarden](https://bitwarden.com/)** — Cloud-based password manager (inspired features)
- **[OpenSSL](https://www.openssl.org/)** — Though we don't depend on it, it's a reference standard
- **[GnuPG](https://gnupg.org/)** — GNU Privacy Guard (cryptographic best practices)

---

## ⚖️ Legal & License

### License
**Academic / Educational Project — Healix Vault, 2026**

This project is an educational initiative demonstrating secure password management principles through a full-stack implementation with industry-standard cryptographic best practices.

### Third-Party Licenses
- **Mongoose:** Dual licensing (commercial and open-source)
- **nlohmann/json:** MIT License
- **MinGW-w64:** GPL v3 + GCC Runtime Exception
- **CMake:** BSD 3-Clause License

All third-party licenses are compatible with educational use. For production deployment, consult legal counsel regarding licensing compliance.

### Disclaimer
While Healix Vault implements industry-standard cryptographic practices:
- It is designed for **educational purposes**
- It has NOT undergone professional security audits
- For critical systems, use professionally audited password managers

---

## 🎓 Educational Value

This project serves as:
- ✅ A reference implementation of cryptographic principles
- ✅ An example of full-stack web application architecture
- ✅ A demonstration of secure data persistence
- ✅ A learning platform for cryptography and security design

### Not Recommended For:
- ❌ Production use with sensitive data
- ❌ Mission-critical systems
- ❌ Compliance-regulated environments (HIPAA, SOC 2, etc.)

---

## 🚀 Contributing Back to Community

If you use this project or learn from it:
- Consider contributing improvements back to open source
- Share knowledge with others learning cryptography and security
- Participate in security research and bug bounty programs
- Help make the open-source community stronger

---

## 📞 Support & Feedback

- Issues and questions: GitHub Issues section
- Contributions: Pull Requests welcome
- Security concerns: Please report responsibly

---

## 🎯 Final Thanks

To everyone who contributed to the knowledge, tools, and standards that made this educational project possible.

**Together, we make security more accessible and understandable.** 🔐✨

---

**Document Version:** 1.0  
**Last Updated:** May 5, 2026  
**Project Status:** Educational Release (v1.0)

---

> *"Standing on the shoulders of giants" — Isaac Newton*
> 
> This project exemplifies the collaborative nature of the open-source community, where knowledge is shared freely for the benefit of all.
