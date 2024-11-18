#include <tools.h> // Include the tools header for utility functions

// Function to output a string using a custom character output function
void __doprnt_put_string(char* str, void (*_putch)(char)) {
    for(; *str != '\0'; str++) // Iterate through each character in the string
        _putch(*str);         // Call the provided function to output the character
}

// Function to process formatted output based on a format string and variable arguments
void __doprnt(const char* fmt, va_list argp, void (*_putch)(char)) {
    char buffer[32]; // Buffer to hold converted numbers or strings
    for (uint32_t i = 0; fmt[i] != '\0'; i++) { // Iterate through the format string
        if (fmt[i] == '%' && fmt[i+1] != '\0') { // Check for format specifier
            i++; // Move to the next character after '%'
            switch (fmt[i]) {
                case 's': // String format
                    __doprnt_put_string(va_arg(argp, char *), _putch); // Output string
                    break;
                case 'c': // Character format
                    _putch((char)va_arg(argp, int)); // Output character
                    break;
                case 'i': // Integer format (decimal)
                case 'd':
                    int_to_str(va_arg(argp, int), 10, buffer); // Convert integer to string
                    __doprnt_put_string(buffer, _putch); // Output converted string
                    break;
                case 'u': // Unsigned integer format
                    uint_to_str(va_arg(argp, int), 10, buffer); // Convert unsigned integer to string
                    __doprnt_put_string(buffer, _putch); // Output converted string
                    break;
                case 'p': // Pointer format (address)
                case 'X': // Hexadecimal format (uppercase)
                case 'x': // Hexadecimal format (lowercase)
                    uint_to_str(va_arg(argp, int), 16, buffer); // Convert integer to hexadecimal string
                    __doprnt_put_string(buffer, _putch); // Output converted string
                    break;
                case 'l': // Long integer specifier handling
                    switch (fmt[i+1]) {
                        case 'l': // Check for long long specifier
                            switch (fmt[i+2]) {
                                case 'u':
                                    // uint64_to_str(va_arg(argp, uint64_t), 10, buffer);
                                    __doprnt_put_string(buffer, _putch); 
                                    break;
                                case 'x':
                                    // uint64_to_str(va_arg(argp, uint64_t), 16, buffer);
                                    __doprnt_put_string(buffer, _putch);
                                    break;
                                case 'd':
                                default:
                                    // int64_to_str(va_arg(argp, uint64_t), 10, buffer);
                                    __doprnt_put_string(buffer, _putch);
                                    break;
                            }
                            i += 2; // Skip over the long long specifier
                            break;
                        default:
                            break; 
                    }
                    i++; // Skip over the 'l'
                    break;
                default: 
                    _putch(fmt[i]); // Output the character as is if not a recognized specifier
            }
        } else {
            _putch(fmt[i]); // Output regular characters directly
        }
    }
}
