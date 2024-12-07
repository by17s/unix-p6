#include <log.h>        // Include logging header for debug output
#include <drivers/tvga.h> // Include TVGA driver header for video output
#include <x86.h>         // Include x86 architecture-specific definitions


int __log_out = 1; // Output destination for logs (0: serial, 1: terminal)

// Function to output a character to TVGA display
void __tvga_putch(char c)
{
    tvga_write(&c, 1); // Write character to TVGA display
}

// Set the logging output destination
void log_set_output(int out) {
    __log_out = out; // Update the logging output destination
}

// Logging function that formats and outputs messages
void LOG(const char* __fmt, ...) {
    va_list args; // Variable argument list for formatted output
    va_start(args, __fmt); // Initialize argument list

    if(__log_out) // Check the output destination
        __doprnt(__fmt, args, __tvga_putch); // Output to TVGA if set
    else
        __doprnt(__fmt, args, com_write); // Output to serial if not

    va_end(args); // Clean up argument list
}
