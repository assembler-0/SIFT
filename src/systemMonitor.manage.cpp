#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <filesystem>
#include <memory>
#include <array>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

namespace esst {

struct Metrics {
    float cpu_temp{0};
    float cpu_usage{0};
    float memory_usage{0};
    float ram_gb_used{0};
    float ram_gb_total{0};
    int cpu_freq{0};
    int base_freq{0};
    bool thermal_throttling{false};
};

struct History {
    static constexpr size_t MAX_SAMPLES = 120;
    std::array<float, MAX_SAMPLES> cpu_temp{};
    std::array<float, MAX_SAMPLES> cpu_usage{};
    std::array<float, MAX_SAMPLES> memory_usage{};
    std::array<int, MAX_SAMPLES> cpu_freq{};
    size_t index{0};

    void push(float cpu_t, float cpu_u, float mem_u, int freq) {
        cpu_temp[index] = cpu_t;
        cpu_usage[index] = cpu_u;
        memory_usage[index] = mem_u;
        cpu_freq[index] = freq;
        index = (index + 1) % MAX_SAMPLES;
    }
};

class Monitor {
    GLFWwindow* window_{nullptr};
    bool running_{false};
    Metrics metrics_;
    History history_;
    std::string cpu_sensor_;
    std::string gpu_sensor_;

    void setup_theme() {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();

        // Load fonts with absolute paths
        ImFont* font = io.Fonts->AddFontFromFileTTF("/home/Atheria/workspace/SIFT/include/imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);
        if (!font) {
            font = io.Fonts->AddFontFromFileTTF("/home/Atheria/workspace/SIFT/include/imgui/misc/fonts/Cousine-Regular.ttf", 15.0f);
        }
        if (!font) {
            io.Fonts->AddFontDefault();
        }
        io.Fonts->Build();

        auto& style = ImGui::GetStyle();
        auto* colors = style.Colors;

        // Midnight Theme
        colors[ImGuiCol_Text]                   = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
        colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.35f, 0.79f, 0.98f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.35f, 0.79f, 0.98f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        style.WindowPadding     = ImVec2(8.00f, 8.00f);
        style.FramePadding      = ImVec2(5.00f, 2.00f);
        style.CellPadding       = ImVec2(6.00f, 6.00f);
        style.ItemSpacing       = ImVec2(6.00f, 6.00f);
        style.ItemInnerSpacing  = ImVec2(6.00f, 6.00f);
        style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
        style.IndentSpacing     = 25;
        style.ScrollbarSize     = 15;
        style.GrabMinSize       = 10;
        style.WindowBorderSize  = 1;
        style.ChildBorderSize   = 1;
        style.PopupBorderSize   = 1;
        style.FrameBorderSize   = 1;
        style.TabBorderSize     = 1;
        style.WindowRounding    = 7;
        style.ChildRounding     = 4;
        style.FrameRounding     = 3;
        style.PopupRounding     = 4;
        style.ScrollbarRounding = 9;
        style.GrabRounding      = 3;
        style.LogSliderDeadzone = 4;
        style.TabRounding       = 4;
    }

    std::string find_sensor(const std::string& pattern) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator("/sys/class/hwmon")) {
                auto name_path = entry.path() / "name";
                std::ifstream file(name_path);
                std::string name;
                if (file >> name && name.find(pattern) != std::string::npos) {
                    for (const auto& temp_file : std::filesystem::directory_iterator(entry.path())) {
                        if (temp_file.path().filename().string().contains("temp1_input")) {
                            return temp_file.path();
                        }
                    }
                }
            }
        } catch (...) {}
        return {};
    }

    float read_temp(const std::string& path) {
        if (path.empty()) return 0;
        std::ifstream file(path);
        int temp;
        return file >> temp ? temp / 1000.0f : 0;
    }

    float read_cpu_usage() {
        static long last_idle{0}, last_total{0};
        std::ifstream file("/proc/stat");
        std::string line;
        std::getline(file, line);

        long user, nice, system, idle, iowait, irq, softirq, steal;
        sscanf(line.c_str(), "cpu %ld %ld %ld %ld %ld %ld %ld %ld",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);

        long total = user + nice + system + idle + iowait + irq + softirq + steal;
        long diff_total = total - last_total;
        long diff_idle = idle - last_idle;

        last_total = total;
        last_idle = idle;

        return diff_total > 0 ? 100.0f * (diff_total - diff_idle) / diff_total : 0;
    }

    void read_memory_info() {
        std::ifstream file("/proc/meminfo");
        std::string line;
        long mem_total{0}, mem_available{0};

        while (std::getline(file, line)) {
            if (line.starts_with("MemTotal:")) {
                sscanf(line.c_str(), "MemTotal: %ld", &mem_total);
            } else if (line.starts_with("MemAvailable:")) {
                sscanf(line.c_str(), "MemAvailable: %ld", &mem_available);
            }
        }

        metrics_.ram_gb_total = mem_total / 1024.0f / 1024.0f;
        metrics_.ram_gb_used = (mem_total - mem_available) / 1024.0f / 1024.0f;
        metrics_.memory_usage = mem_total > 0 ? 100.0f * (mem_total - mem_available) / mem_total : 0;
    }

    int read_cpu_frequency() {
        // Try scaling frequency first (more accurate)
        std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
        if (file.is_open()) {
            int freq_khz;
            file >> freq_khz;
            return freq_khz / 1000;
        }
        
        // Fallback to /proc/cpuinfo
        std::ifstream proc_file("/proc/cpuinfo");
        std::string line;
        while (std::getline(proc_file, line)) {
            if (line.starts_with("cpu MHz")) {
                float freq;
                sscanf(line.c_str(), "cpu MHz : %f", &freq);
                return static_cast<int>(freq);
            }
        }
        return metrics_.base_freq; // Return base freq if can't read
    }

    void get_base_frequency() {
        std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/base_frequency");
        if (file.is_open()) {
            int freq_khz;
            file >> freq_khz;
            metrics_.base_freq = freq_khz / 1000;
        } else {
            metrics_.base_freq = 3000; // Default fallback
        }
    }

    void update_metrics() {
        metrics_.cpu_temp = read_temp(cpu_sensor_);
        metrics_.cpu_usage = read_cpu_usage();
        read_memory_info();
        metrics_.cpu_freq = read_cpu_frequency();
        // Disable thermal throttling detection for now
        metrics_.thermal_throttling = false;

        history_.push(metrics_.cpu_temp, metrics_.cpu_usage, metrics_.memory_usage, metrics_.cpu_freq);
    }

    void render_ui() {
        ImGui::Begin("SIFT MONITOR", nullptr, ImGuiWindowFlags_NoDecoration);

        ImGui::Text("SIFT NEXUS");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "[ONLINE]");
        ImGui::Separator();
        ImGui::Spacing();

        // Metrics grid with fixed column widths
        if (ImGui::BeginTable("metrics", 4, ImGuiTableFlags_SizingFixedFit)) {
            ImGui::TableSetupColumn("CPU", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("MEMORY", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("FREQ", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("STATUS", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableNextColumn();
            ImGui::TextDisabled("CPU");
            ImGui::Text("%.0f°C", metrics_.cpu_temp);
            ImGui::Text("%.1f%%", metrics_.cpu_usage);
            ImGui::Text("%d MHz", metrics_.cpu_freq);

            ImGui::TableNextColumn();
            ImGui::TextDisabled("MEMORY");
            ImGui::Text("%.1f/%.1f GB", metrics_.ram_gb_used, metrics_.ram_gb_total);
            ImGui::Text("%.0f%%", metrics_.memory_usage);
            ImGui::ProgressBar(metrics_.memory_usage / 100.0f, {80, 0}, "");

            ImGui::TableNextColumn();
            ImGui::TextDisabled("FREQ");
            ImGui::Text("Base: %d MHz", metrics_.base_freq);
            ImGui::Text("Curr: %d MHz", metrics_.cpu_freq);
            float freq_ratio = (float)metrics_.cpu_freq / metrics_.base_freq;
            ImGui::ProgressBar(freq_ratio, {80, 0}, "");

            ImGui::TableNextColumn();
            ImGui::TextDisabled("STATUS");
            ImGui::Text("STRESS ACTIVE");
            if (metrics_.thermal_throttling) {
                ImGui::TextColored({1.0f, 0.4f, 0.2f, 1.0f}, "THROTTLING");
            } else {
                ImGui::TextColored({0.2f, 0.8f, 0.2f, 1.0f}, "NORMAL");
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Graphs
        ImGui::PlotLines("CPU TEMP (°C)", history_.cpu_temp.data(), History::MAX_SAMPLES,
                        history_.index, nullptr, 0, 100, {0, 80});

        ImGui::PlotLines("CPU USAGE (%)", history_.cpu_usage.data(), History::MAX_SAMPLES,
                        history_.index, nullptr, 0, 100, {0, 80});

        ImGui::PlotHistogram("MEMORY (%)", history_.memory_usage.data(), History::MAX_SAMPLES,
                            history_.index, nullptr, 0, 100, {0, 80});

        ImGui::Spacing();
        if (ImGui::Button("DISCONNECT NEXUS", {180, 30})) {
            running_ = false;
        }

        ImGui::End();
    }

public:
    bool init() {
        if (!glfwInit()) return false;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        window_ = glfwCreateWindow(480, 400, "NEXUS", nullptr, nullptr);
        if (!window_) {
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window_);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO().IniFilename = nullptr;

        setup_theme();
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        cpu_sensor_ = find_sensor("coretemp");
        get_base_frequency();

        return true;
    }

    void run() {
        if (!init()) return;
        running_ = true;

        while (running_ && !glfwWindowShouldClose(window_)) {
            glfwPollEvents();
            update_metrics();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos({0, 0});
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

            render_ui();

            ImGui::Render();
            int w, h;
            glfwGetFramebufferSize(window_, &w, &h);
            glViewport(0, 0, w, h);
            glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window_);

            std::this_thread::sleep_for(std::chrono::milliseconds(15)); // 10 FPS - smoother numbers
        }

        cleanup();
    }

    void cleanup() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        if (window_) glfwDestroyWindow(window_);
        glfwTerminate();
    }

    void stop() { running_ = false; }
};

} // namespace esst

// Simple C API
static std::unique_ptr<esst::Monitor> g_monitor;
static std::thread g_thread;

extern "C" void spawn_system_monitor() {
    if (g_monitor) return;

    g_monitor = std::make_unique<esst::Monitor>();
    g_thread = std::thread([]{ g_monitor->run(); });
}

extern "C" void stop_system_monitor() {
    if (!g_monitor) return;

    g_monitor->stop();
    if (g_thread.joinable()) g_thread.join();
    g_monitor.reset();
}