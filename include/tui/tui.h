#ifndef TUI_H
#define TUI_H
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/ioctl.h>

// ANSI escape codes
#define ANSI_CLEAR_LINE "\033[2K"
#define ANSI_CURSOR_UP "\033[%dA"
#define ANSI_HIDE_CURSOR "\033[?25l"
#define ANSI_SHOW_CURSOR "\033[?25h"
#define ANSI_BOLD "\033[1m"
#define ANSI_RESET "\033[0m"
#define ANSI_GREEN "\033[32m"
#define ANSI_RED "\033[31m"
#define ANSI_BLUE "\033[34m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_CYAN "\033[36m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_BRIGHT_GREEN "\033[92m"
#define ANSI_BRIGHT_RED "\033[91m"
#define ANSI_BRIGHT_BLUE "\033[94m"
#include <stddef.h>

// Progress bar configuration
typedef struct {
    size_t current;
    size_t total;
    int width;
    char* label;
    int show_percentage;
    int show_count;
} progress_bar_t;

// Spinner configuration
typedef struct {
    int frame;
    char* label;
    int active;
} spinner_t;
static const char* spinner_frames[] = {"|", "/", "-", "\\", "|", "/", "-", "\\"};
static const int spinner_frame_count = 8;

void tui_clear_line(void) {
    printf(ANSI_CLEAR_LINE "\r");
    fflush(stdout);
}

void tui_move_cursor_up(int lines) {
    printf(ANSI_CURSOR_UP, lines);
    fflush(stdout);
}

void tui_hide_cursor(void) {
    printf(ANSI_HIDE_CURSOR);
    fflush(stdout);
}

void tui_show_cursor(void) {
    printf(ANSI_SHOW_CURSOR);
    fflush(stdout);
}

// Initialize progress bar
static int get_terminal_width(void) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_col;
    }
    return 80; // Default fallback
}


spinner_t* spinner_create(const char* label) {
    spinner_t* spinner = static_cast<spinner_t *>(malloc(sizeof(spinner_t)));
    if (!spinner) return NULL;

    spinner->frame = 0;
    spinner->label = label ? strdup(label) : NULL;
    spinner->active = 1;

    tui_hide_cursor();
    return spinner;
}

void spinner_update(spinner_t* spinner) {
    if (!spinner || !spinner->active) return;

    // Clear line and move to beginning
    printf("\r" ANSI_CLEAR_LINE);

    // Print spinner
    printf(ANSI_YELLOW "%s" ANSI_RESET, spinner_frames[spinner->frame]);

    // Print label
    if (spinner->label) {
        printf(" %s", spinner->label);
    }

    // Update frame
    spinner->frame = (spinner->frame + 1) % spinner_frame_count;

    fflush(stdout);
}

void spinner_stop(spinner_t* spinner) {
    if (!spinner) return;

    spinner->active = 0;

    // Clear line and show completion
    printf("\r" ANSI_CLEAR_LINE);
    if (spinner->label) {
        printf(ANSI_GREEN "✓" ANSI_RESET " %s\n", spinner->label);
    }

    tui_show_cursor();
}

void spinner_free(spinner_t* spinner) {
    if (!spinner) return;
    free(spinner->label);
    free(spinner);
}

void spinner_set_label(spinner_t* spinner, const char* label) {
    if (!spinner) return;
    free(spinner->label);
    spinner->label = label ? strdup(label) : NULL;
}



// Colored output functions
void tui_success(const char* message) {
    printf(ANSI_BRIGHT_GREEN "✓" ANSI_RESET " %s\n", message);
}

void tui_error(const char* message) {
    printf(ANSI_BRIGHT_RED "✗" ANSI_RESET " %s\n", message);
}

void tui_warning(const char* message) {
    printf(ANSI_YELLOW "⚠" ANSI_RESET " %s\n", message);
}

void tui_info(const char* message) {
    printf(ANSI_BRIGHT_BLUE "ℹ" ANSI_RESET " %s\n", message);
}

void tui_header(const char* message) {
    printf(ANSI_BOLD ANSI_CYAN "=== %s ===" ANSI_RESET "\n", message);
}
void tui_success_no_newl(const char* message) {
    printf(ANSI_BRIGHT_GREEN "✓" ANSI_RESET " %s", message);
}

void tui_error_no_newl(const char* message) {
    printf(ANSI_BRIGHT_RED "✗" ANSI_RESET " %s", message);
}

void tui_warning_no_newl(const char* message) {
    printf(ANSI_YELLOW "⚠" ANSI_RESET " %s", message);
}

void tui_info_no_newl(const char* message) {
    printf(ANSI_BRIGHT_BLUE "ℹ" ANSI_RESET " %s", message);
}

void tui_header_no_newl(const char* message) {
    printf(ANSI_BOLD ANSI_CYAN "=== %s ===" ANSI_RESET, message);
}
#endif // TUI_H