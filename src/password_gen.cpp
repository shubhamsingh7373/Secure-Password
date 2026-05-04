#include "password_gen.h"
#include <random>
#include <algorithm>
#include <string>
#include <vector>

std::string PasswordGenerator::generate(const GenOptions& opts) {
    std::string charset;
    if (opts.uppercase) charset += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (opts.lowercase) charset += "abcdefghijklmnopqrstuvwxyz";
    if (opts.digits) charset += "0123456789";
    if (opts.symbols) charset += "!@#$%^&*()_+-=[]{}|;:,.<>?";

    if (opts.noAmbiguous) {
        // Remove ambiguous characters: 0, O, l, 1, |
        std::string toRemove = "0Ol1|";
        for (char c : toRemove) {
            charset.erase(std::remove(charset.begin(), charset.end(), c), charset.end());
        }
    }

    if (charset.empty()) return "";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, charset.size() - 1);

    std::string password;
    for (int i = 0; i < opts.length; ++i) {
        password += charset[dis(gen)];
    }

    return password;
}

int PasswordGenerator::strength(const std::string& password) {
    int score = 0;
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSymbol = false;

    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasDigit = true;
        else hasSymbol = true;
    }

    if (hasUpper) score += 25;
    if (hasLower) score += 25;
    if (hasDigit) score += 25;
    if (hasSymbol) score += 25;

    if (password.length() >= 12) score += 20;
    else if (password.length() >= 8) score += 10;

    return std::min(score, 100);
}

std::string PasswordGenerator::strengthLabel(int score) {
    if (score < 25) return "Very Weak";
    if (score < 50) return "Weak";
    if (score < 75) return "Fair";
    if (score < 90) return "Strong";
    return "Very Strong";
}