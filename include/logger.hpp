#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <sys/utsname.h>
#include <thread>
#include <thread>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void logTestResult(const std::string& test_name, 
                      const std::vector<double>& thread_scores,
                      double avg_score,
                      double median_score,
                      const std::string& cpu_brand) {
        
        std::ofstream log_file("results.log", std::ios::app);
        if (!log_file.is_open()) return;

        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        // Get hostname
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        
        // Get system info
        struct utsname sys_info;
        uname(&sys_info);
        
        // Write log entry
        log_file << "=== SIFT TEST RESULT ===\n";
        log_file << "Timestamp: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n";
        log_file << "Host: " << hostname << "\n";
        log_file << "System: " << sys_info.sysname << " " << sys_info.release << " " << sys_info.machine << "\n";
        log_file << "CPU: " << cpu_brand << "\n";
        log_file << "Test: " << test_name << "\n";
        log_file << "Threads: " << thread_scores.size() << "\n";
        
        // Individual thread scores
        for (size_t i = 0; i < thread_scores.size(); ++i) {
            log_file << "Thread_" << i << ": " << std::fixed << std::setprecision(2) << thread_scores[i] << " IPS\n";
        }
        
        log_file << "Average: " << std::fixed << std::setprecision(2) << avg_score << " IPS\n";
        log_file << "Median: " << std::fixed << std::setprecision(2) << median_score << " IPS\n";
        log_file << "========================\n\n";
        
        log_file.close();
    }

    void logCacheResult(const std::vector<std::array<double, 4>>& cache_scores,
                       const std::string& cpu_brand) {
        
        std::ofstream log_file("results.log", std::ios::app);
        if (!log_file.is_open()) return;

        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        // Get hostname
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        
        // Get system info
        struct utsname sys_info;
        uname(&sys_info);
        
        const char* cache_names[] = {"L1_Cache", "L2_Cache", "L3_Cache", "Memory_Latency"};
        
        log_file << "=== SIFT CACHE TEST RESULT ===\n";
        log_file << "Timestamp: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n";
        log_file << "Host: " << hostname << "\n";
        log_file << "System: " << sys_info.sysname << " " << sys_info.release << " " << sys_info.machine << "\n";
        log_file << "CPU: " << cpu_brand << "\n";
        log_file << "Test: Cache_Hierarchy\n";
        log_file << "Threads: " << cache_scores.size() << "\n";
        
        // Log each cache level
        for (int cache_level = 0; cache_level < 4; ++cache_level) {
            log_file << cache_names[cache_level] << ":\n";
            double total = 0;
            for (size_t thread = 0; thread < cache_scores.size(); ++thread) {
                double score = cache_scores[thread][cache_level];
                log_file << "  Thread_" << thread << ": " << std::fixed << std::setprecision(2) << score << " IPS\n";
                total += score;
            }
            double avg = total / cache_scores.size();
            log_file << "  Average: " << std::fixed << std::setprecision(2) << avg << " IPS\n";
        }
        
        log_file << "===============================\n\n";
        log_file.close();
    }

    void logSystemInfo(const std::string& cpu_brand, bool has_avx, bool has_avx2, 
                      bool has_fma, bool has_aes, bool has_sha) {
        
        std::ofstream log_file("results.log", std::ios::app);
        if (!log_file.is_open()) return;

        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        // Get hostname
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        
        // Get system info
        struct utsname sys_info;
        uname(&sys_info);
        
        log_file << "=== SIFT SESSION START ===\n";
        log_file << "Timestamp: " << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\n";
        log_file << "SIFT_Version: 0.9.0\n";
        log_file << "Host: " << hostname << "\n";
        log_file << "System: " << sys_info.sysname << " " << sys_info.release << " " << sys_info.machine << "\n";
        log_file << "CPU: " << cpu_brand << "\n";
        log_file << "CPU_Features: AVX" << (has_avx ? "+" : "-") 
                 << " AVX2" << (has_avx2 ? "+" : "-")
                 << " FMA" << (has_fma ? "+" : "-")
                 << " AES" << (has_aes ? "+" : "-")
                 << " SHA" << (has_sha ? "+" : "-") << "\n";
        log_file << "Threads: " << std::thread::hardware_concurrency() << "\n";
        log_file << "==========================\n\n";
        
        log_file.close();
    }

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

#endif // LOGGER_HPP