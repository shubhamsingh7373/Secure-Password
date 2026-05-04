#pragma once
#ifndef PASSWORD_GEN_H
#define PASSWORD_GEN_H

#include <string>

struct GenOptions {
    int  length         = 16;
    bool uppercase      = true;
    bool lowercase      = true;
    bool digits         = true;
    bool symbols        = true;
    bool noAmbiguous    = false;   // exclude 0,O,l,1,|
};

class PasswordGenerator {
public:
    static std::string generate(const GenOptions& opts);
    static int         strength(const std::string& password);  // 0-100
    static std::string strengthLabel(int score);               // Weak/Fair/Strong/Very Strong
};

#endif // PASSWORD_GEN_H
