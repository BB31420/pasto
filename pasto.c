#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <limits.h>
#include <errno.h>

void print_ascii_art() {
    printf("┌─┐┌─┐┌─┐┌┬┐┌─┐\n");
    printf("├─┘├─┤└─┐ │ │ │\n");
    printf("┴  ┴ ┴└─┘ ┴ └─┘\n");
}

void usage(char *program_name) {
    printf("Usage: %s [-o] [-a] [-n] directory/filename\n", program_name);
    exit(1);
}

int mkdirp(const char *dir) {
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST)
                return -1;
            *p = '/';
        }
    }
    return (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST) ? -1 : 0;
}

char *get_clipboard_content(Display *display) {
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);
    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    Atom utf8_string = XInternAtom(display, "UTF8_STRING", False);
    Atom property = XInternAtom(display, "XSEL_DATA", False);

    XSelectInput(display, window, PropertyChangeMask);
    XConvertSelection(display, clipboard, utf8_string, property, window, CurrentTime);

    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        if (event.type == SelectionNotify && event.xselection.selection == clipboard) {
            if (event.xselection.property) {
                break;
            } else {
                fprintf(stderr, "Clipboard data is not available\n");
                XCloseDisplay(display);
                return NULL;
            }
        }
    }

    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop_return = NULL;

    if (XGetWindowProperty(display, window, property, 0, LONG_MAX, False, utf8_string,
                           &actual_type, &actual_format, &nitems, &bytes_after, &prop_return) != Success) {
        fprintf(stderr, "Failed to get window property\n");
        XCloseDisplay(display);
        return NULL;
    }

    if (actual_type != utf8_string) {
        fprintf(stderr, "Failed to retrieve clipboard content\n");
        XFree(prop_return);
        XCloseDisplay(display);
        return NULL;
    }

    char *content = strdup((char *)prop_return);
    XFree(prop_return);
    XCloseDisplay(display);

    return content;
}

int main(int argc, char **argv) {
    int overwrite = 0;
    int append = 0;
    int open_after = 0;
    char *filepath = NULL;

    print_ascii_art();

    // Parse options and arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            overwrite = 1;
        } else if (strcmp(argv[i], "-a") == 0) {
            append = 1;
        } else if (strcmp(argv[i], "-n") == 0) {
            open_after = 1;
        } else {
            filepath = argv[i];
        }
    }

    if (filepath == NULL) {
        usage(argv[0]);
    }

    char directory[PATH_MAX];
    char *last_slash = strrchr(filepath, '/');
    if (last_slash != NULL) {
        size_t dir_length = last_slash - filepath;
        strncpy(directory, filepath, dir_length);
        directory[dir_length] = '\0';
    } else {
        strcpy(directory, ".");
    }

    // Create the directory if it does not exist
    if (mkdirp(directory) != 0) {
        fprintf(stderr, "Error: Failed to create directory '%s'.\n", directory);
        return 1;
    }

    // Check if file already exists and handle overwrite flag
    if (access(filepath, F_OK) == 0 && !overwrite && !append) {
        fprintf(stderr, "Error: The file '%s' already exists. Use -o to overwrite or -a to append.\n", filepath);
        return 1;
    }

    // Get clipboard content
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }

    char *clipboard_content = get_clipboard_content(display);
    if (clipboard_content == NULL) {
        return 1;
    }

    // Write clipboard content to file
    const char *mode = append ? "a" : "w";
    FILE *fp = fopen(filepath, mode);
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to open file '%s'.\n", filepath);
        free(clipboard_content);
        return 1;
    }
    fwrite(clipboard_content, strlen(clipboard_content), 1, fp);
    fclose(fp);

    free(clipboard_content);

    printf("File '%s' %s successfully with clipboard content.\n", filepath, append ? "appended" : "created");

    // Open the file after writing if the -n flag is set
    if (open_after) {
        char open_command[PATH_MAX + 10];
#ifdef _WIN32
        snprintf(open_command, sizeof(open_command), "start %s", filepath);
#elif __APPLE__
        snprintf(open_command, sizeof(open_command), "open %s", filepath);
#else
        snprintf(open_command, sizeof(open_command), "xdg-open %s", filepath);
#endif
        system(open_command);
    }

    return 0;
}
