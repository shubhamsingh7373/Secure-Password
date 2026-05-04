#include "auth.h"
#include "crypto.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

/*
 * Config file format (plain text, two lines):
 *   Line 1: hex-encoded PBKDF2 hash (32 bytes → 64 hex chars)
 *   Line 2: hex-encoded salt        (16 bytes → 32 hex chars)
 */

static std::vector<uint8_t> hexDecode(const std::string& hex) {
    std::vector<uint8_t> out;
    for(size_t i=0; i+1<hex.size(); i+=2) {
        out.push_back((uint8_t)std::stoi(hex.substr(i,2), nullptr, 16));
    }
    return out;
}

static std::string hexEncode(const std::vector<uint8_t>& v) {
    std::ostringstream oss;
    for(auto b : v) oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    return oss.str();
}

#include <iomanip>

Auth::Auth(const std::string& configPath) : configPath_(configPath) {
    loadConfig();
}

bool Auth::isFirstRun() const {
    return storedHash_.empty();
}

bool Auth::setupMaster(const std::string& password) {
    if(password.size() < 8) return false;
    auto saltBytes = Crypto::randomBytes(16);
    storedSalt_ = hexEncode(saltBytes);
    auto keyBytes = Crypto::pbkdf2(password, saltBytes, 200000, 32);
    storedHash_ = hexEncode(keyBytes);
    saveConfig();
    return true;
}

bool Auth::verify(const std::string& password) {
    if(storedHash_.empty() || storedSalt_.empty()) return false;
    auto saltBytes = hexDecode(storedSalt_);
    auto keyBytes  = Crypto::pbkdf2(password, saltBytes, 200000, 32);
    return hexEncode(keyBytes) == storedHash_;
}

std::string Auth::derivedKey(const std::string& password) const {
    // Reuse stored salt to derive the vault encryption key
    // We use a different derivation context (append "enc") so the auth key
    // and the vault encryption key are different even from the same password.
    auto saltBytes = hexDecode(storedSalt_);
    std::string ctx = password + ":enc";
    auto keyBytes  = Crypto::pbkdf2(ctx, saltBytes, 200000, 32);
    return hexEncode(keyBytes);
}

void Auth::loadConfig() {
    std::ifstream f(configPath_);
    if(!f.is_open()) return;
    std::getline(f, storedHash_);
    std::getline(f, storedSalt_);
}

void Auth::saveConfig() {
    std::ofstream f(configPath_);
    if(!f.is_open()) throw std::runtime_error("Cannot write config: " + configPath_);
    f << storedHash_ << "\n" << storedSalt_ << "\n";
}
