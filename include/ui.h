#pragma once
#ifndef UI_H
#define UI_H

#include <string>
#include <vector>
#include "vault.h"

// ─── Terminal UI helpers ──────────────────────────────────────────────────────
namespace UI {

    // Colours / ANSI
    void enableColor(bool on);
    std::string colorize(const std::string& text, const std::string& colorCode);

    // Screen helpers
    void clearScreen();
    void printBanner();
    void printSeparator(char ch = '-', int width = 60);
    void printSuccess(const std::string& msg);
    void printError(const std::string& msg);
    void printWarning(const std::string& msg);
    void printInfo(const std::string& msg);

    // Input helpers
    std::string readLine(const std::string& prompt);
    std::string readPassword(const std::string& prompt);
    int         readInt(const std::string& prompt, int lo, int hi);
    bool        confirm(const std::string& prompt);

    // Entry display
    void printEntryTable(const std::vector<Entry>& entries, bool showPasswords = false);
    void printEntry(const Entry& e, bool showPassword = false);

    // Menu
    int  showMainMenu();
    int  showSubMenu(const std::string& title, const std::vector<std::string>& items);

} // namespace UI

#endif // UI_H
