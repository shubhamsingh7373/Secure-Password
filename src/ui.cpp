#include "ui.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <conio.h> // for _getch() on Windows
#include <windows.h> // for console colors

namespace UI {

static bool colorEnabled = true;

void enableColor(bool on) {
    colorEnabled = on;
}

std::string colorize(const std::string& text, const std::string& colorCode) {
    if (!colorEnabled) return text;
    return "\033[" + colorCode + "m" + text + "\033[0m";
}

void clearScreen() {
    system("cls");
}

void printBanner() {
    std::cout << colorize(R"(
 ____   ____    .__   __    __
|    | |    |   |__| |  | _|__|
|    | |    |   |  | |  |/ /  |
|    | |    |   |  | |    < \  |
|____| |____|   |__| |__|_ \ \/
     \_/                 \_/

Password Vault Manager
)", "36") << std::endl;
}

void printSeparator(char ch, int width) {
    std::cout << std::string(width, ch) << std::endl;
}

void printSuccess(const std::string& msg) {
    std::cout << colorize("[SUCCESS] ", "32") << msg << std::endl;
}

void printError(const std::string& msg) {
    std::cout << colorize("[ERROR] ", "31") << msg << std::endl;
}

void printWarning(const std::string& msg) {
    std::cout << colorize("[WARNING] ", "33") << msg << std::endl;
}

void printInfo(const std::string& msg) {
    std::cout << colorize("[INFO] ", "34") << msg << std::endl;
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

std::string readPassword(const std::string& prompt) {
    std::cout << prompt;
    std::string password;
    char ch;
    while ((ch = _getch()) != '\r') { // Enter key
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                password.pop_back();
                std::cout << "\b \b";
            }
        } else {
            password += ch;
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}

int readInt(const std::string& prompt, int lo, int hi) {
    while (true) {
        std::cout << prompt;
        int val;
        std::cin >> val;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (val >= lo && val <= hi) return val;
        printError("Invalid input. Please enter a number between " + std::to_string(lo) + " and " + std::to_string(hi) + ".");
    }
}

bool confirm(const std::string& prompt) {
    while (true) {
        std::cout << prompt << " (y/n): ";
        char ch;
        std::cin >> ch;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (ch == 'y' || ch == 'Y') return true;
        if (ch == 'n' || ch == 'N') return false;
    }
}

void printEntryTable(const std::vector<Entry>& entries, bool showPasswords) {
    if (entries.empty()) {
        printInfo("No entries found.");
        return;
    }

    std::cout << std::left << std::setw(5) << "ID"
              << std::setw(20) << "Website"
              << std::setw(15) << "Username"
              << std::setw(15) << "Password"
              << std::setw(15) << "Category"
              << "Notes" << std::endl;
    printSeparator('-', 80);

    for (const auto& e : entries) {
        std::cout << std::left << std::setw(5) << e.id
                  << std::setw(20) << e.website
                  << std::setw(15) << e.username
                  << std::setw(15) << (showPasswords ? e.password : std::string(e.password.size(), '*'))
                  << std::setw(15) << e.category
                  << e.notes << std::endl;
    }
}

void printEntry(const Entry& e, bool showPassword) {
    std::cout << "ID: " << e.id << std::endl;
    std::cout << "Website: " << e.website << std::endl;
    std::cout << "Username: " << e.username << std::endl;
    std::cout << "Password: " << (showPassword ? e.password : std::string(e.password.size(), '*')) << std::endl;
    std::cout << "Category: " << e.category << std::endl;
    std::cout << "Notes: " << e.notes << std::endl;
    std::cout << "Created: " << e.createdAt << std::endl;
    std::cout << "Updated: " << e.updatedAt << std::endl;
}

int showMainMenu() {
    clearScreen();
    printBanner();
    printSeparator();
    std::cout << "1. View All Entries" << std::endl;
    std::cout << "2. Search Entries" << std::endl;
    std::cout << "3. Add New Entry" << std::endl;
    std::cout << "4. Edit Entry" << std::endl;
    std::cout << "5. Delete Entry" << std::endl;
    std::cout << "6. Generate Password" << std::endl;
    std::cout << "7. Save and Exit" << std::endl;
    printSeparator();
    return readInt("Choose an option (1-7): ", 1, 7);
}

int showSubMenu(const std::string& title, const std::vector<std::string>& items) {
    clearScreen();
    printBanner();
    printSeparator();
    std::cout << title << std::endl;
    printSeparator('-', 40);
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << (i + 1) << ". " << items[i] << std::endl;
    }
    printSeparator();
    return readInt("Choose an option: ", 1, items.size());
}

} // namespace UI