#include "core.hpp"
#include "pcg_random.hpp"
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <functional>
#include <thread>
#include <chrono>
#include <vector>
#include <immintrin.h>
#include <cpuid.h>
#include <sched.h>
#include <sys/mman.h>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <optional>
class sift {
public:
    void init() {
        detect_cpu_features();
        std::cout << "SIFT version " << APP_VERSION << " | CPU: " << cpu_brand << "\n";
        std::cout << "Features: AVX" << (has_avx ? "+" : "-")
                  << " | AVX2" << (has_avx2 ? "+" : "-")
                  << " | AES" << (has_aes ? "+" : "-")
                  << " | SHA" << (has_sha ? "+" : "-")
                  << " | AVX2" << (has_avx2 ? "+" : "-")
                  << " | FMA" << (has_fma ? "+" : "-") << "\n";

        while (running) {
            std::cout << "[SIFT] >> ";
            if (!std::getline(std::cin, op_mode)) break;

            if (auto it = command_map.find(op_mode); it != command_map.end()) {
                it->second();
            }
            else if (!op_mode.empty()) {
                std::cout << "Invalid command\n";
            }
        }
    }

private:
    bool running = true;
    std::string op_mode;
    std::string cpu_brand;
    bool has_avx = false, has_avx2 = false, has_fma = false, has_aes = false, has_sha = false;
    const unsigned int num_threads = std::thread::hardware_concurrency();

    static constexpr auto APP_VERSION = "0.9.0";
    static constexpr int AVX_BUFFER_SIZE = 64; // 256 bytes (L1 cache line optimized)
    static constexpr int COLLATZ_BATCH_SIZE = 10000000;

    const std::unordered_map<std::string, std::function<void()>> command_map = {
        {"exit", [this]() { running = false; }},
        {"menu", [this]() { showMenu(); }},
        {"avx",  [this]() { initAvx(); }},
        {"3np1", [this]() { init3np1(); }},
        {"primes", [this]() { initPrimes(); }},
        {"disk", [this]() { initDiskWrite(); }},
        {"full", [this]() { nuclearOption(); }},
        {"mem", [this]() { initMem(); }},
        {"sha", [this]() { initSHA256(); }},
        {"lzma", [this]() { initLZMA(); }},
        {"aesenc", [this]() { initAESENC(); }},
        {"aesdec", [this]() { initAESDEC(); }},
        {"render", [this]() { initRender(); }},
        {"branch", [this]() { initBranch(); }}
    };

    void detect_cpu_features() {
        char brand[0x40] = {0};
        unsigned int eax, ebx, ecx, edx;

        __get_cpuid(0x80000002, &eax, &ebx, &ecx, &edx);
        memcpy(brand, &eax, 4); memcpy(brand+4, &ebx, 4);
        memcpy(brand+8, &ecx, 4); memcpy(brand+12, &edx, 4);

        __get_cpuid(0x80000003, &eax, &ebx, &ecx, &edx);
        memcpy(brand+16, &eax, 4); memcpy(brand+20, &ebx, 4);
        memcpy(brand+24, &ecx, 4); memcpy(brand+28, &edx, 4);

        __get_cpuid(0x80000004, &eax, &ebx, &ecx, &edx);
        memcpy(brand+32, &eax, 4); memcpy(brand+36, &ebx, 4);
        memcpy(brand+40, &ecx, 4); memcpy(brand+44, &edx, 4);

        __get_cpuid(1, &eax, &ebx, &ecx, &edx);
        has_aes = ecx & bit_AES;        // Missing AES-NI detection
        has_sha = false;                // Add SHA-NI detection

        __get_cpuid(7, &eax, &ebx, &ecx, &edx);
        has_sha = ebx & (1 << 29);      // SHA-NI support
        cpu_brand = brand;

        __get_cpuid(1, &eax, &ebx, &ecx, &edx);
        has_avx = ecx & bit_AVX;
        has_fma = ecx & bit_FMA;

        __get_cpuid(7, &eax, &ebx, &ecx, &edx);
        has_avx2 = ebx & bit_AVX2;
    }

    static void showMenu() {
        std::cout << "\n========= SIFT =========\n"
                  << "Extreme System Stability Test\n"
                  << "avx   - AVX/FMA Stress Test\n"
                  << "3np1  - Collatz Conjecture bruteforce\n"
                  << "primes  - Prime bruteforce\n"
                  << "mem   - Extreme memory testing\n"
                  << "aesenc   - Vector AES Encrypt stressing\n"
                  << "aesdec   - Vector AES Decrypt stressing\n"
                  << "sha   - SHA_NI stressing\n"
                  << "disk   - Disk stressing\n"
                  << "lzma   - CPU compression and decompression using LZMA\n"
                  << "render - BRUTAL CPU Ray-tracing (Cinebench killer)\n"
                  << "branch - Branch Prediction (Real-world patterns)\n"
                  << "gpu   - GPU stressing with HIP\n"
                  << "full  - Combined Full System Stress\n"
                  << "exit  - Exit Program\n\n";
    }
    std::string formatIPS(double flops) const {
        if (flops >= 1e9) {
            return std::to_string(flops / 1e9) + " GIPS";
        }
        if (flops >= 1e6) {
            return std::to_string(flops / 1e6) + " MIPS";
        }
        if (flops >= 1e3) {
            return std::to_string(flops / 1e3) + " KIPS";
        }
        return std::to_string(flops) + " IPS";
    }

    static void initLZMA (std::optional<int> duration_o = std::nullopt){
        if (!duration_o.has_value()) {
            std::cout << "Duration (s)?: ";
            if (!(std::cin >> duration_o.emplace())) return;
        }
        spawn_system_monitor();
        const int duration = duration_o.value();
        if (duration_o.value() == 0) return;
        startLZMA(duration);
        stop_system_monitor();
    }

    void init3np1(std::optional<unsigned long> iterations_o = std::nullopt, std::optional<unsigned long> lower_o = std::nullopt, std::optional<unsigned long> upper_o = std::nullopt) const {
        if (!iterations_o.has_value()) {
            std::cout << "Iterations?: ";
            if (!(std::cin >> iterations_o.emplace())) return;
        }
        if (!lower_o.has_value()) {
            std::cout << "Lower bound?: ";
            if (!(std::cin >> lower_o.emplace())) return;
        }
        if (!upper_o.has_value()) {
            std::cout << "Upper bound?: ";
            if (!(std::cin >> upper_o.emplace())) return;
        }
        const unsigned long iterations = iterations_o.value();
        const unsigned long lower = lower_o.value();
        const unsigned long upper = upper_o.value();
        if (iterations_o.value() == 0) return;
        spawn_system_monitor();
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        std::vector<double> scores(num_threads);

        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back([=, &scores]() {
                scores[i] = collatzWorker(iterations, lower, upper, i);
            });
        }
        for (auto& t : threads) t.join();

        const double total = std::accumulate(scores.begin(), scores.end(), 0.0);
        const double avg   = total / scores.size();
        std::ranges::sort(scores);
        const double median = scores[scores.size() / 2];

        std::cout << "\n====== 3n+1 STRESS SCORE ======\n";
        for (size_t i = 0; i < scores.size(); ++i) {
            std::cout << "Thread " << i << ": "
                      << formatIPS(scores[i]) << "\n";
        }
        std::cout << "-------------------------------\n";
        std::cout << "Avg:    " << formatIPS(avg) << "\n";
        std::cout << "Median: " << formatIPS(median) << "\n";
        std::cout << "================================\n";
        stop_system_monitor();

    }

    void initPrimes(std::optional<unsigned long> iterations_o = std::nullopt, std::optional<float> lower_o = std::nullopt, std::optional<float> upper_o = std::nullopt) const {
        if (!iterations_o.has_value()) {
            std::cout << "Iterations?: ";
            if (!(std::cin >> iterations_o.emplace())) return;
        }
        if (!lower_o.has_value()) {
            std::cout << "Lower bound?: ";
            if (!(std::cin >> lower_o.emplace())) return;
        }
        if (!upper_o.has_value()) {
            std::cout << "Upper bound?: ";
            if (!(std::cin >> upper_o.emplace())) return;
        }
        const unsigned long iterations = iterations_o.value();
        const unsigned long lower = lower_o.value();
        const unsigned long upper = upper_o.value();
        if (iterations_o.value() == 0) return;
        spawn_system_monitor();
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        std::vector<double> scores(num_threads);

        for (unsigned i = 0; i < num_threads; ++i) {
            threads.emplace_back([=, &scores]() {
                scores[i] = primesWorker(iterations, lower, upper, i);
            });
        }
        for (auto& t : threads) t.join();

        const double total = std::accumulate(scores.begin(), scores.end(), 0.0);
        const double avg   = total / scores.size();
        std::ranges::sort(scores);
        const double median = scores[scores.size() / 2];

        std::cout << "\n====== PRIMES STRESS SCORE ======\n";
        for (size_t i = 0; i < scores.size(); ++i) {
            std::cout << "Thread " << i << ": "
                      << formatIPS(scores[i]) << "\n";
        }
        std::cout << "-------------------------------\n";
        std::cout << "Avg:    " << formatIPS(avg) << "\n";
        std::cout << "Median: " << formatIPS(median) << "\n";
        std::cout << "==================================\n";
        stop_system_monitor();
        
    }

    void initAvx(std::optional<unsigned long> iterations_o = std::nullopt, std::optional<float> lower_o = std::nullopt, std::optional<float> upper_o = std::nullopt) const {
        if (!iterations_o.has_value()) {
            std::cout << "Iterations?: ";
            if (!(std::cin >> iterations_o.emplace())) return;
        }
        if (!lower_o.has_value()) {
            std::cout << "Lower bound?: ";
            if (!(std::cin >> lower_o.emplace())) return;
        }
        if (!upper_o.has_value()) {
            std::cout << "Upper bound?: ";
            if (!(std::cin >> upper_o.emplace())) return;
        }
        const unsigned long iterations = iterations_o.value();
        const unsigned long lower = lower_o.value();
        const unsigned long upper = upper_o.value();
        if (iterations_o.value() == 0) return;
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        std::vector<double> scores(num_threads);
        spawn_system_monitor();
        for (unsigned i = 0; i < num_threads; ++i) {
            threads.emplace_back([=, &scores]() {
                scores[i] = avxWorker(iterations, lower, upper, i);
            });
        }
        for (auto& t : threads) t.join();

        const double total = std::accumulate(scores.begin(), scores.end(), 0.0);
        const double avg   = total / scores.size();
        std::sort(scores.begin(), scores.end());
        double median = scores[scores.size() / 2];

        std::cout << "\n====== AVX STRESS SCORE ======\n";
        for (size_t i = 0; i < scores.size(); ++i) {
            std::cout << "Thread " << i << ": "
                      << formatIPS(scores[i]) << "\n";
        }
        std::cout << "-------------------------------\n";
        std::cout << "Avg:    " << formatIPS(avg) << "\n";
        std::cout << "Median: " << formatIPS(median) << "\n";
        std::cout << "===============================\n";
        stop_system_monitor();
        
    }


    void initMem(std::optional<unsigned long> user_iterations = std::nullopt) const {
        char status;
        std::cout << "ONE TIME WARNING, THIS TEST CONTAINS ROWHAMMER ATTACK, PROCEED? (yY/nN): ";
        std::cin >> status;
        switch (status) {
        case 'y': case 'Y': break;
        default: return;
        }
        if (!user_iterations.has_value()) {
            std::cout << "Iterations?: ";
            if (!(std::cin >> user_iterations.emplace())) return;
        }
        if (user_iterations.value() == 0) return;
        const unsigned long iterations = user_iterations.value();
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        std::vector<double> scores(num_threads);
        spawn_system_monitor();
        for (unsigned i = 0; i < num_threads; ++i) {
            threads.emplace_back([=, &scores]() {
                scores[i] = memoryWorker(iterations, i);
            });
        }
        for (auto& t : threads) t.join();

        const double total = std::accumulate(scores.begin(), scores.end(), 0.0);
        const double avg   = total / scores.size();
        std::sort(scores.begin(), scores.end());
        const double median = scores[scores.size() / 2];

        std::cout << "\n====== MEM STRESS SCORE ======\n";
        for (size_t i = 0; i < scores.size(); ++i) {
            std::cout << "Thread " << i << ": "
                      << formatIPS(scores[i]) << "\n";
        }
        std::cout << "-------------------------------\n";
        std::cout << "Avg:    " << formatIPS(avg) << "\n";
        std::cout << "Median: " << formatIPS(median) << "\n";
        std::cout << "=================================\n";
        stop_system_monitor();
        
    }

    void initAESENC(std::optional<unsigned long> iterations_o = std::nullopt, std::optional<unsigned long> blksize_o = std::nullopt) const {
        if (!iterations_o.has_value()) {
            std::cout << "Iterations?: ";
            if (!(std::cin >> iterations_o.emplace())) return;
        }
        if (!blksize_o.has_value()) {
            std::cout << "Blocksize?: ";
            if (!(std::cin >> blksize_o.emplace())) return;
        }
        if (iterations_o.value() == 0) return;
        unsigned long iterations = iterations_o.value();
        unsigned int block_size = blksize_o.value();
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        std::vector<double> scores(num_threads);
        spawn_system_monitor();
        for (unsigned i = 0; i < num_threads; ++i) {
            threads.emplace_back([=, &scores]() {
                scores[i] = aesENCWorker(iterations, i, block_size);
            });
        }
        for (auto& t : threads) t.join();

        const double total = std::accumulate(scores.begin(), scores.end(), 0.0);
        const double avg   = total / scores.size();
        std::sort(scores.begin(), scores.end());
        const double median = scores[scores.size() / 2];

        std::cout << "\n====== AESENC STRESS SCORE ======\n";
        for (size_t i = 0; i < scores.size(); ++i) {
            std::cout << "Thread " << i << ": "
                      << formatIPS(scores[i]) << "\n";
        }
        std::cout << "-------------------------------\n";
        std::cout << "Avg:    " << formatIPS(avg) << "\n";
        std::cout << "Median: " << formatIPS(median) << "\n";
        std::cout << "==================================\n";
        stop_system_monitor();
        
    }

    void initAESDEC(std::optional<unsigned long> iterations_o = std::nullopt, std::optional<unsigned long> blksize_o = std::nullopt) {
        if (!iterations_o.has_value()) {
            std::cout << "Iterations?: ";
            if (!(std::cin >> iterations_o.emplace())) return;
        }
        if (!blksize_o.has_value()) {
            std::cout << "Blocksize?: ";
            if (!(std::cin >> blksize_o.emplace())) return;
        }
        if (iterations_o.value() == 0) return;
        spawn_system_monitor();
        unsigned long iterations = iterations_o.value();
        unsigned int block_size = blksize_o.value();
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        std::vector<double> scores(num_threads);
        spawn_system_monitor();
        for (unsigned i = 0; i < num_threads; ++i) {
            threads.emplace_back([=, &scores]() {
                scores[i] = aesDECWorker(iterations, i, block_size);
            });
        }
        for (auto& t : threads) t.join();

        const double total = std::accumulate(scores.begin(), scores.end(), 0.0);
        const double avg   = total / scores.size();
        std::sort(scores.begin(), scores.end());
        const double median = scores[scores.size() / 2];

        std::cout << "\n====== AESDEC STRESS SCORE ======\n";
        for (size_t i = 0; i < scores.size(); ++i) {
            std::cout << "Thread " << i << ": "
                      << formatIPS(scores[i]) << "\n";
        }
        std::cout << "-------------------------------\n";
        std::cout << "Avg:    " << formatIPS(avg) << "\n";
        std::cout << "Median: " << formatIPS(median) << "\n";
        std::cout << "==================================\n";
        stop_system_monitor();
        
    }

    void initDiskWrite(std::optional<unsigned long> iterations_o = std::nullopt){
        if (!iterations_o.has_value()) {
            std::cout << "Iterations?: ";
            if (!(std::cin >> iterations_o.emplace())) return;
        }
        unsigned long iterations = iterations_o.value();
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        std::vector<double> scores(num_threads);
        spawn_system_monitor();
        for (unsigned i = 0; i < num_threads; ++i) {
            threads.emplace_back([=, &scores]() {
                scores[i] = diskWriteWorker(iterations, i);
            });
        }
        for (auto& t : threads) t.join();

        const double total = std::accumulate(scores.begin(), scores.end(), 0.0);
        const double avg   = total / scores.size();
        std::sort(scores.begin(), scores.end());
        const double median = scores[scores.size() / 2];

        std::cout << "\n====== DISK STRESS SCORE ======\n";
        for (size_t i = 0; i < scores.size(); ++i) {
            std::cout << "Thread " << i << ": "
                      << formatIPS(scores[i]) << "\n";
        }
        std::cout << "-------------------------------\n";
        std::cout << "Avg:    " << formatIPS(avg) << "\n";
        std::cout << "Median: " << formatIPS(median) << "\n";
        std::cout << "================================\n";
        stop_system_monitor();
        
    }

    void initSHA256(std::optional<unsigned long> iterations_o = std::nullopt){
        if (!iterations_o.has_value()) {
            std::cout << "Iterations?: ";
            if (!(std::cin >> iterations_o.emplace())) return;
        }
        const unsigned long iterations = iterations_o.value();
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        std::vector<double> scores(num_threads);
        spawn_system_monitor();
        for (unsigned i = 0; i < num_threads; ++i) {
            threads.emplace_back([=, &scores]() {
                scores[i] = sha256Worker(iterations, i);
            });
        }
        for (auto& t : threads) t.join();

        const double total = std::accumulate(scores.begin(), scores.end(), 0.0);
        const double avg   = total / scores.size();
        std::ranges::sort(scores);
        const double median = scores[scores.size() / 2];

        std::cout << "\n====== SHA STRESS SCORE ======\n";
        for (size_t i = 0; i < scores.size(); ++i) {
            std::cout << "Thread " << i << ": "
                      << formatIPS(scores[i]) << "\n";
        }
        std::cout << "-------------------------------\n";
        std::cout << "Avg:    " << formatIPS(avg) << "\n";
        std::cout << "Median: " << formatIPS(median) << "\n";
        std::cout << "================================\n";
        stop_system_monitor();
        
    }

    void initRender(std::optional<int> resolution_o = std::nullopt, std::optional<int> samples_o = std::nullopt) const {
        if (!resolution_o.has_value()) {
            std::cout << "Resolution (1=720p, 2=1080p, 3=4K): ";
            if (!(std::cin >> resolution_o.emplace())) return;
        }
        if (!samples_o.has_value()) {
            std::cout << "Samples: ";
            if (!(std::cin >> samples_o.emplace())) return;
        }
        int width = 0, height = 0;

        switch (resolution_o.value()) {
            case 1: width = 1280; height = 720; break;
            case 2: width = 1920; height = 1080; break;
            case 3: width = 3840; height = 2160; break;
            default: break;
        }
        
        std::cout << "\nCPU TRACER TEST\n";
        std::cout << "Resolution: " << width << "x" << height << "\n";
        std::cout << "Samples per pixel: " << (64 * samples_o.value()) << "\n";
        
        spawn_system_monitor();
        
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        std::vector<double> scores(num_threads);
        
        for (unsigned i = 0; i < num_threads; ++i) {
            threads.emplace_back([=, &scores]() {
                scores[i] = renderWorker(width, height, samples_o.value(), i);
            });
        }
        
        for (auto& t : threads) t.join();
        
        const double total = std::accumulate(scores.begin(), scores.end(), 0.0);
        const double avg = total / scores.size();
        std::ranges::sort(scores);
        const double median = scores[scores.size() / 2];
        
        std::cout << "\n========== RENDER SCORE ==========\n";
        for (size_t i = 0; i < scores.size(); ++i) {
            std::cout << "Thread " << i << ": " << formatIPS(scores[i]) << "\n";
        }
        std::cout << "-------------------------------\n";
        std::cout << "Avg:    " << formatIPS(avg) << "\n";
        std::cout << "Median: " << formatIPS(median) << "\n";
        std::cout << "===================================\n";
        
        stop_system_monitor();
    }

    void initBranch(std::optional<unsigned long> iterations_o = std::nullopt, std::optional<int> pattern_o = std::nullopt) {
        if (!iterations_o.has_value()) {
            std::cout << "Iterations?: ";
            if (!(std::cin >> iterations_o.emplace())) return;
        }
        if (!pattern_o.has_value()) {
            std::cout << "Pattern (1=Gaming, 2=Database, 3=Compiler, 4=Mixed): ";
            if (!(std::cin >> pattern_o.emplace())) return;
        }
        
        const char* pattern_names[] = {"", "Gaming AI", "Database Queries", "Compiler Parsing", "Mixed Workload"};
        std::cout << "\n🎯 BRANCH PREDICTION : " << pattern_names[pattern_o.value()] << "\n\n";
        
        spawn_system_monitor();
        
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        std::vector<double> scores(num_threads);
        
        for (unsigned i = 0; i < num_threads; ++i) {
            threads.emplace_back([=, &scores]() {
                scores[i] = branchWorker(iterations_o.value(), pattern_o.value(), i);
            });
        }
        
        for (auto& t : threads) t.join();
        
        const double total = std::accumulate(scores.begin(), scores.end(), 0.0);
        const double avg = total / scores.size();
        std::ranges::sort(scores);
        const double median = scores[scores.size() / 2];
        
        std::cout << "\n==== BRANCH PREDICTION SCORE ====\n";
        for (size_t i = 0; i < scores.size(); ++i) {
            std::cout << "Thread " << i << ": " << formatIPS(scores[i]) << "\n";
        }
        std::cout << "-------------------------------\n";
        std::cout << "Avg:    " << formatIPS(avg) << "\n";
        std::cout << "Median: " << formatIPS(median) << "\n";
        std::cout << "===================================\n";
        
        stop_system_monitor();
    }

    void nuclearOption() {
        unsigned long intensity = 0;
        std::cout << "Intensity (1 = default): ";
        std::cin >> intensity;
        std::cout << "Launching full stress test...\n";
        unsigned long nuke_iterations_avx = 200000 * intensity;
        unsigned long nuke_iterations_3np1 = 20000000 * intensity;
        unsigned long nuke_iterations_primes = 3 * intensity;
        unsigned long nuke_iterations_aes = 20 * intensity;
        unsigned long nuke_iterations_disk = 20 * intensity;
        unsigned long nuke_iterations_mem = 20 * intensity;
        unsigned long nuke_iterations_sha = 100000000 * intensity;
        unsigned long nuke_duration_lzma = 60 * intensity;
        unsigned long nuke_resolution_render = 3;
        unsigned long nuke_samples_render = 5 * intensity;
        unsigned long nuke_iterations_branch = 5000000000 * intensity;
        constexpr unsigned long lower_avx = 0.0001, upper_avx = 1000000000000000;
        constexpr unsigned long lower = 1, upper = 1000000000000000;
        constexpr int block_size = 24;
        const auto start = std::chrono::high_resolution_clock::now();
        spawn_system_monitor();
        initMem(nuke_iterations_mem);
        initAvx(nuke_iterations_avx, lower_avx, upper_avx);
        init3np1(nuke_iterations_3np1, lower, upper);
        initPrimes(nuke_iterations_primes, lower, upper);
        initAESENC(nuke_iterations_aes, block_size);
        initAESDEC(nuke_iterations_aes, block_size);
        initDiskWrite(nuke_iterations_disk);
        initSHA256(nuke_iterations_sha);
        initLZMA(nuke_duration_lzma);
        initRender(nuke_resolution_render, nuke_samples_render);
        initBranch(nuke_iterations_branch, 1);
        initBranch(nuke_iterations_branch, 2);
        initBranch(nuke_iterations_branch, 3);
        initBranch(nuke_iterations_branch, 4);
        const auto duration = std::chrono::high_resolution_clock::now() - start;
        std::cout << "Full test complete! Time: "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
                  << " ms\n";
        stop_system_monitor();
    }

    static void pinThread(int core) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        int target_core = core % std::thread::hardware_concurrency();
        CPU_SET(target_core, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    }
    static void* allocate_huge_buffer(size_t size) {
    #ifdef __linux__
        void* ptr = mmap(nullptr, size, PROT_READ|PROT_WRITE,
                        MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB, -1, 0);
        if (ptr != MAP_FAILED) return ptr;
    #endif
        return aligned_alloc(1 << 21, size); // Fallback to 2MB aligned
    }

    static void free_buffer(void* buf, size_t size) {
    #ifdef __linux__
        munmap(buf, size);
    #else
        free(buf);
    #endif
    }

    static double memoryWorker(unsigned long iterations, const int thread_id) {
        pinThread(thread_id);
        const auto start = std::chrono::high_resolution_clock::now();
        constexpr size_t size = 1 << 30; // 1GB
        constexpr size_t buffer_size = size;

        // Allocate buffer once outside the loop
        void* buffer = allocate_huge_buffer(size);
        if (!buffer) {
            std::cerr << "Failed to allocate memory buffer for thread " << thread_id << std::endl;
            return 0.0;
        }

        // Run the stress tests
        for (unsigned long i = 0; i < iterations; ++i) {
            floodL1L2(buffer, &iterations, buffer_size);
            floodMemory(buffer, &iterations, buffer_size);
            floodNt(buffer, &iterations, buffer_size);
            rowhammerAttack(buffer, &iterations, buffer_size);
        }
        free_buffer(buffer, size);
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed = end - start;
        return iterations / elapsed.count();

    }

    static double sha256Worker(unsigned long iterations, const int thread_id) {
        pinThread(thread_id);
        const auto start = std::chrono::high_resolution_clock::now();
        sha256(iterations);
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed = end - start;
        return iterations / elapsed.count();

    }

    static double aesENCWorker(const long iterations, int tid, const int block_size) {
        pinThread(tid);
        const auto start = std::chrono::high_resolution_clock::now();
        // Allocate aligned buffers
        alignas(16) uint8_t key[32] = {0x01}; // All-zero key (worst-case)
        alignas(16) uint8_t expanded_key[240]; // AES-256 expanded key
        alignas(16) uint8_t plaintext[16] = {0};
        alignas(16) uint8_t ciphertext[16];
        const size_t BLOCKS = 1 << block_size;
        auto buffer = std::make_unique<uint8_t[]>(BLOCKS * 16);
        pcg32 gen(std::random_device{}());
        std::uniform_int_distribution<uint8_t> dist(0, 255);
        for (auto& v : key) v = dist(gen);
        for (long i = 0; i < iterations; i++){
            // Key expansion (stress FPU)
            aes256Keygen(expanded_key);
            // Encrypt individual blocks (stress latency)
            for (size_t i = 0; i < BLOCKS; i++) {
                aes128EncryptBlock(ciphertext, plaintext, key);
                asm volatile("" : : "r"(ciphertext) : "memory");
            }
            // XTS mode (stress throughput)
            uint8_t tweak[16] = {0};
            aesXtsEncrypt(buffer.get(), buffer.get(), expanded_key, tweak, BLOCKS);
        }

        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed = end - start;
        return iterations / elapsed.count();
    }

    static double aesDECWorker(const long iterations, int tid, const int block_size) {
        pinThread(tid);
        const auto start = std::chrono::high_resolution_clock::now();
        // Allocate aligned buffers
        alignas(16) uint8_t key[32] = {0x01}; // All-zero key (worst-case)
        alignas(16) uint8_t expanded_key[240]; // AES-256 expanded key
        alignas(16) uint8_t ciphertext[16] = {0};
        alignas(16) uint8_t plaintext[16];
        const size_t BLOCKS = 1 << block_size;
        auto buffer = std::make_unique<uint8_t[]>(BLOCKS * 16);
        pcg32 gen(std::random_device{}());
        std::uniform_int_distribution<uint8_t> dist(0, 255);
        for (auto& v : key) v = dist(gen);

        for (long i = 0; i < iterations; i++){
            // Key expansion (stress FPU)
            aes256Keygen(expanded_key);
            // Decrypt individual blocks (stress latency)
            for (size_t i = 0; i < BLOCKS; i++) {
                aes128DecryptBlock(plaintext, ciphertext, key);
                asm volatile("" : : "r"(plaintext) : "memory");
            }
            // XTS mode decryption (stress throughput)
            uint8_t tweak[16] = {0};
            aesXtsDecrypt(buffer.get(), buffer.get(), expanded_key, tweak, BLOCKS);
        }
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed = end - start;
        return iterations / elapsed.count();
    }

    static double collatzWorker(unsigned long iterations, unsigned long lower, unsigned long upper, int tid) {
        pinThread(tid);
        pcg32 gen(42u + tid, 54u + tid);
        std::uniform_int_distribution<unsigned long> dist(lower, upper);

        const auto start = std::chrono::high_resolution_clock::now();

        for (unsigned long i = 0; i < iterations; ) {
            const unsigned long batch = std::min(static_cast<unsigned long>(COLLATZ_BATCH_SIZE), iterations - i);

            for (unsigned long j = 0; j < batch; ++j) {
                unsigned long steps = 0;
                p3np1E(dist(gen), &steps);
            }
            i += batch;
        }
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed = end - start;
        return iterations / elapsed.count();  // Standard: iterations/second
    }

    static double primesWorker(unsigned long iterations, unsigned long lower, unsigned long upper, int tid) {
        pinThread(tid);
        pcg32 gen(42u + tid, 54u + tid);
        std::uniform_int_distribution<unsigned long> dist(lower, upper);

        unsigned long total_steps = 0;
        const auto start = std::chrono::high_resolution_clock::now();

        for (unsigned long i = 0; i < iterations; ) {
            const unsigned long batch = std::min(static_cast<unsigned long>(COLLATZ_BATCH_SIZE), iterations - i);
            unsigned long batch_steps = 0;

            for (unsigned long j = 0; j < batch; ++j) {
                unsigned long steps = 0;
                primes(dist(gen), &steps);
                batch_steps += steps;
            }

            total_steps += batch_steps;
            i += batch;
        }
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed = end - start;
        return iterations / elapsed.count();  // it/s
    }

    static double avxWorker(const unsigned long iterations, const float lower, const float upper, int tid) {
        pinThread(tid);
        pcg32 gen(42u + tid, 54u + tid);
        std::uniform_real_distribution<float> dist(lower, upper);

        alignas(32) float n1[AVX_BUFFER_SIZE], n2[AVX_BUFFER_SIZE], n3[AVX_BUFFER_SIZE];

        const auto start = std::chrono::high_resolution_clock::now();

        for (unsigned long i = 0; i < iterations; ++i) {
            for (int j = 0; j < AVX_BUFFER_SIZE; ++j) {
                n1[j] = dist(gen);
                n2[j] = dist(gen);
                n3[j] = dist(gen);
            }

            for (int offset = 0; offset < AVX_BUFFER_SIZE; offset += 8) {
                avx(n1+offset, n2+offset, n3+offset);
            }
        }
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed = end - start;
        return iterations / elapsed.count();  // Standard: iterations/second
    }
    static double diskWriteWorker(unsigned long iterations, int tid){
        pinThread(tid);
        const auto start = std::chrono::high_resolution_clock::now();
        std::string filename = "/tmp/writeTestThread" + std::to_string(tid) + ".bin";
        for (int i = 0; i < iterations; ++i) {
            diskWrite(filename.c_str());
        }
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed = end - start;
        return iterations / elapsed.count();  // Standard: iterations/second
    }

    static double renderWorker(int width, int height, int sample_multiplier, int tid) {
        pinThread(tid);
        const auto start = std::chrono::high_resolution_clock::now();
        
        // Create scene data (16 spheres)
        alignas(16) float scene[64]; // 16 spheres * 4 floats (x,y,z,radius)
        pcg32 gen(42u + tid, 54u + tid);
        std::uniform_real_distribution<float> pos_dist(-10.0f, 10.0f);
        std::uniform_real_distribution<float> rad_dist(0.5f, 2.0f);
        
        for (int i = 0; i < 16; ++i) {
            scene[i*4 + 0] = pos_dist(gen);     // x
            scene[i*4 + 1] = pos_dist(gen);     // y  
            scene[i*4 + 2] = pos_dist(gen);     // z
            scene[i*4 + 3] = rad_dist(gen);     // radius
        }
        
        // Calculate pixels per thread
        int total_pixels = width * height;
        int pixels_per_thread = total_pixels / std::thread::hardware_concurrency();
        int start_pixel = tid * pixels_per_thread;
        int end_pixel = (tid == std::thread::hardware_concurrency() - 1) ? 
                       total_pixels : start_pixel + pixels_per_thread;
        
        alignas(16) float pixel_output;
        long pixels_rendered = 0;

        double total_iterations = (end_pixel - start_pixel) * sample_multiplier;
        renderPixel(total_iterations, tid, &pixel_output);
        pixels_rendered = total_iterations;
        
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed = end - start;
        return pixels_rendered / elapsed.count(); // Pixels per second
    }

    static double branchWorker(unsigned long iterations, int pattern_type, int tid) {
        pinThread(tid);
        const auto start = std::chrono::high_resolution_clock::now();

        branchTorture(iterations, pattern_type);
        
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsed = end - start;
        return iterations / elapsed.count();
    }
};

int main() {
    sift().init();
    return 0;
}
