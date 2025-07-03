#ifndef MENU_HPP
#define MENU_HPP

#include <iostream>
#include <string>
#include <vector>

class MenuSystem {
public:
    static void showMainMenu(const std::string& cpu_brand, bool has_avx, bool has_avx2, 
                           bool has_fma, bool has_aes, bool has_sha) {
        clearScreen();
        drawBox();
        
        std::cout << "│ " << centerText("SIFT v0.9.0 - System Intensive Function Tester", 46) << " │\n";
        std::cout << "│ " << centerText("CPU: " + cpu_brand, 46) << " │\n";
        
        std::string features = "Features: AVX" + std::string(has_avx ? "+" : "-") +
                              " AVX2" + std::string(has_avx2 ? "+" : "-") +
                              " FMA" + std::string(has_fma ? "+" : "-") +
                              " AES" + std::string(has_aes ? "+" : "-") +
                              " SHA" + std::string(has_sha ? "+" : "-");
        std::cout << "│ " << centerText(features, 46) << " │\n";
        
        std::cout << "├" << std::string(48, '-') << "┤\n";
        std::cout << "│                                                │\n";
        std::cout << "│  NORMAL TESTS                                  │\n";
        std::cout << "│  [1] AVX Stress Test                           │\n";
        std::cout << "│  [2] CPU Rendering (Death Mode)                │\n";
        std::cout << "│  [3] Memory Stress + Rowhammer                 │\n";
        std::cout << "│  [4] Full System Torture                       │\n";
        std::cout << "│                                                │\n";
        std::cout << "│  REAL-WORLD TESTS                              │\n";
        std::cout << "│  [5] Branch Prediction                         │\n";
        std::cout << "│  [6] Cache Hierarchy (L1/L2/L3)                │\n";
        std::cout << "│  [7] 3n+1 Collatz Conjecture                   │\n";
        std::cout << "│  [8] Prime Factorization                       │\n";
        std::cout << "│                                                │\n";
        std::cout << "│  🛡️ SECURITY TESTS                              │\n";
        std::cout << "│  [9] AES Encryption                            │\n";
        std::cout << "│  [A] AES Decryption                            │\n";
        std::cout << "│  [B] SHA-256 Hashing                           │\n";
        std::cout << "│                                                │\n";
        std::cout << "│  💾 I/O TESTS                                  │\n";
        std::cout << "│  [C] Disk Write Stress                         │\n";
        std::cout << "│  [D] LZMA Compression                          │\n";
        std::cout << "│                                                │\n";
        std::cout << "│  ⚙️ UTILITIES                                   │\n";
        std::cout << "│  [R] Show Recommendation                       │\n";
        std::cout << "│  [Q] Quit                                      │\n";
        std::cout << "│                                                │\n";
        std::cout << "└" << std::string(48, '-') << "┘\n";
        std::cout << "\nSelect option: ";
    }
    
    // static void showTestProgress(const std::string& test_name, int progress,
    //                            const std::string& status = "") {
    //     clearScreen();
    //     drawProgressBox();
    //
    //     std::cout << "│ " << centerText("SIFT - " + test_name, 46) << " │\n";
    //     std::cout << "├" << std::string(48, '-') << "┤\n";
    //     std::cout << "│                                                │\n";
    //
    //     // Progress bar
    //     std::string progressBar = "Progress: ";
    //     int barWidth = 20;
    //     int filled = (progress * barWidth) / 100;
    //     progressBar += "[";
    //     for (int i = 0; i < barWidth; ++i) {
    //         if (i < filled) progressBar += "█";
    //         else progressBar += "░";
    //     }
    //     progressBar += "] " + std::to_string(progress) + "%";
    //
    //     std::cout << "│ " << padText(progressBar, 46) << " │\n";
    //     std::cout << "│                                                │\n";
    //
    //     if (!status.empty()) {
    //         std::cout << "│ " << padText("Status: " + status, 46) << " │\n";
    //         std::cout << "│                                                │\n";
    //     }
    //
    //     std::cout << "│ " << centerText(" TESTING IN PROGRESS ", 46) << " │\n";
    //     std::cout << "│ " << centerText("Monitor temperatures!", 46) << " │\n";
    //     std::cout << "│                                                │\n";
    //     std::cout << "└" << std::string(48, '-') << "┘\n";
    //     std::cout.flush();
    // }
    
    static char getMenuChoice() {
        char choice;
        std::cin >> choice;
        return std::toupper(choice);
    }
    
private:
    static void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    static void drawBox() {
        std::cout << "┌" << std::string(48, '-') << "┐\n";
    }
    
    static void drawProgressBox() {
        std::cout << "┌" << std::string(48, '-') << "┐\n";
    }
    
    static std::string centerText(const std::string& text, int width) {
        if (text.length() >= width) return text.substr(0, width);
        int padding = (width - text.length()) / 2;
        return std::string(padding, ' ') + text + std::string(width - text.length() - padding, ' ');
    }
    
    static std::string padText(const std::string& text, int width) {
        if (text.length() >= width) return text.substr(0, width);
        return text + std::string(width - text.length(), ' ');
    }
};

#endif // MENU_HPP