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

int __itoa(int value, char* buffer, int buffer_size) {
    int index = 0;
    if (value < 0) {
        if (index < buffer_size - 1) {
            buffer[index++] = '-';
        }
        value = -value;
    }
    int temp = value;
    int digits = 1;
    while ((temp /= 10) > 0) {
        digits++;
    }

    if (digits >= buffer_size) {
        return 0; //not enough space
    }

    for (int i = digits - 1; i >= 0; i--) {
        buffer[index + i] = '0' + (value % 10);
        value /= 10;
    }
    index += digits;
    if (index < buffer_size) {
        buffer[index] = '\0';
    }
    return index;
}

int __snprintf(char* str, int size, const char* format, ...) {
    if (!str || size <= 0 || !format) {
        return -1;
    }

    va_list args;
    va_start(args, format);

    int total_written = 0;
    int i = 0; 
    int buffer_index = 0;

    while (format[i] != '\0') {
        if (format[i] == '%') {
            i++;
            if (format[i] == '\0') break;

            if (format[i] == 'd') { // numeric
                int value = va_arg(args, int);
                char temp_buffer[32];
                int written = __itoa(value, temp_buffer, sizeof(temp_buffer));
                for (int j = 0; j < written && buffer_index < size - 1; j++) {
                    str[buffer_index++] = temp_buffer[j];
                }
                total_written += written;
            } else if (format[i] == 's') { // string
                const char* value = va_arg(args, const char*);
                for (int j = 0; value[j] != '\0'; j++) {
                    if (buffer_index < size - 1) {
                        str[buffer_index++] = value[j];
                    }
                    total_written++;
                }
            } else if (format[i] == 'c') { // char
                char value = (char)va_arg(args, int);
                if (buffer_index < size - 1) {
                    str[buffer_index++] = value;
                }
                total_written++;
            } else { // undef id
                if (buffer_index < size - 1) {
                    str[buffer_index++] = '%';
                }
                total_written++;
                if (buffer_index < size - 1) {
                    str[buffer_index++] = format[i];
                }
                total_written++;
            }
        } else {
            if (buffer_index < size - 1) {
                str[buffer_index++] = format[i];
            }
            total_written++;
        }
        i++;
    }

    if (buffer_index < size) {
        str[buffer_index] = '\0'; // termination
    } else if (size > 0) {
        str[size - 1] = '\0'; //cut string
    }

    va_end(args);
    return total_written;
}