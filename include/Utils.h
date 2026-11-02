#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <limits>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

// ============================================================================
// OOP CONCEPT: Templates & Generic Programming
// 1. Template function for type-safe and validated user input.
//    Supports int, double, float, etc. with bounds checking and stream error recovery.
// 2. Utility functions for formatting console interface.
// ============================================================================

namespace Utils {

    // Template Function for validated numeric input
    template <typename T>
    T getValidatedInput(const std::string& prompt, T minVal, T maxVal) {
        T value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                if (value >= minVal && value <= maxVal) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    return value;
                } else {
                    std::cout << "   [!] Input out of valid range (" << minVal << " to " << maxVal << "). Please try again.\n";
                }
            } else {
                std::cout << "   [!] Invalid input type! Please enter a valid number.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }

    // Helper for reading non-empty string input
    inline std::string getNonEmptyString(const std::string& prompt) {
        std::string input;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, input);
            if (!input.empty()) {
                return input;
            }
            std::cout << "   [!] Input cannot be blank! Please try again.\n";
        }
    }

    // Helper for date string (YYYY-MM-DD)
    inline std::string getCurrentDateTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        std::tm localTm;
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&localTm, &nowTime);
#else
        localtime_r(&nowTime, &localTm);
#endif
        std::ostringstream ss;
        ss << std::put_time(&localTm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    // UI helpers
    inline void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
        std::system("cls");
#else
        std::system("clear");
#endif
    }

    inline void pause() {
        std::cout << "\n   Press Enter to continue...";
        std::cin.get();
    }

    inline void printLine(char ch = '=', int len = 65) {
        for (int i = 0; i < len; ++i) std::cout << ch;
        std::cout << "\n";
    }

    inline void printHeader(const std::string& title) {
        printLine('=', 65);
        int padding = (65 - static_cast<int>(title.length())) / 2;
        if (padding < 0) padding = 0;
        for (int i = 0; i < padding; ++i) std::cout << " ";
        std::cout << title << "\n";
        printLine('=', 65);
    }

} // namespace Utils

#endif // UTILS_H
