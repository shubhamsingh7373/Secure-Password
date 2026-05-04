#pragma once
#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
#include <vector>
#include <cstdint>

namespace Crypto {

    // ─── SHA-256 ────────────────────────────────────────────────────────────
    std::string sha256(const std::string& input);

    // ─── AES-256-CBC (key derived from master password via SHA-256) ─────────
    // Returns Base64-encoded ciphertext (IV prepended)
    std::string aes256Encrypt(const std::string& plaintext, const std::string& key);
    // Decrypts Base64-encoded ciphertext produced by aes256Encrypt
    std::string aes256Decrypt(const std::string& ciphertext_b64, const std::string& key);

    // ─── Base64 helpers ──────────────────────────────────────────────────────
    std::string base64Encode(const std::vector<uint8_t>& data);
    std::vector<uint8_t> base64Decode(const std::string& encoded);

    // ─── PBKDF2-SHA256 key derivation ────────────────────────────────────────
    // Derives a 32-byte key from a password + salt, iterations times
    std::vector<uint8_t> pbkdf2(const std::string& password,
                                 const std::vector<uint8_t>& salt,
                                 int iterations,
                                 int keyLen);

    // ─── Secure random bytes ─────────────────────────────────────────────────
    std::vector<uint8_t> randomBytes(size_t n);

} // namespace Crypto

#endif // CRYPTO_H
