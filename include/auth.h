#pragma once
#ifndef AUTH_H
#define AUTH_H

#include <string>

// ─── Authentication / master-password management ──────────────────────────────
class Auth {
public:
    explicit Auth(const std::string& configPath);

    bool isFirstRun()    const;
    bool setupMaster(const std::string& password);
    bool verify(const std::string& password);

    // Returns the 32-byte hex key derived from the master password
    std::string derivedKey(const std::string& password) const;

private:
    std::string configPath_;
    std::string storedHash_;   // PBKDF2(SHA-256) stored hash
    std::string storedSalt_;   // hex-encoded random salt

    void loadConfig();
    void saveConfig();
};

#endif // AUTH_H
