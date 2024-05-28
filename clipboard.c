#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <string.h>
#include <limits.h>

int main(void) {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }

    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);
    Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
    Atom utf8_string = XInternAtom(display, "UTF8_STRING", False);
    Atom targets = XInternAtom(display, "TARGETS", False);
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
                return 1;
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
        return 1;
    }

    if (actual_type == utf8_string) {
        printf("%s", (char *)prop_return);
    } else {
        fprintf(stderr, "Failed to retrieve clipboard content\n");
    }

    XFree(prop_return);
    XCloseDisplay(display);

    return 0;
}

