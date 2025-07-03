#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <vector>

class ConfigParser {
public:
    static bool loadConfig(const std::string& filename, std::unordered_map<std::string, std::string>& config) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;
            
            // Find = separator
            size_t pos = line.find('=');
            if (pos == std::string::npos) continue;
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            config[key] = value;
        }
        
        return true;
    }
    
    static std::vector<std::string> getTestOrder(const std::unordered_map<std::string, std::string>& config) {
        std::vector<std::string> tests;
        auto it = config.find("test_order");
        if (it != config.end()) {
            std::stringstream ss(it->second);
            std::string test;
            while (std::getline(ss, test, ',')) {
                // Trim whitespace
                test.erase(0, test.find_first_not_of(" \t"));
                test.erase(test.find_last_not_of(" \t") + 1);
                if (!test.empty()) {
                    tests.push_back(test);
                }
            }
        }
        return tests;
    }
};

#endif // CONFIG_HPP