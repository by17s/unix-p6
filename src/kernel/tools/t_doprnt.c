#include <tools.h>

void __doprnt_put_string(char* str, void (*_putch)(char)) {
    for(;*str != '\0'; str++)
        _putch(*str);
}

void __doprnt(const char* fmt, va_list argp, void (*_putch)(char)) {
    char buffer[32];
    for (uint32_t i = 0; fmt[i] != '\0' ; i++)
    {
        if (fmt[i] == '%' && fmt[i+1] != '\0') {
            i++;
            switch (fmt[i])
            {
            case 's':
                __doprnt_put_string(va_arg(argp, char *), _putch);
                break;
            case 'c':
                _putch((char)va_arg(argp, int));
                break;
            case 'i':
            case 'd':
                int_to_str(va_arg(argp, int), 10, buffer);
                __doprnt_put_string(buffer, _putch);
                break;
            case 'u':
                uint_to_str(va_arg(argp, int), 10, buffer);
                __doprnt_put_string(buffer, _putch);
                break;
            case 'p':
            case 'X':
            case 'x':
                uint_to_str(va_arg(argp, int), 16, buffer);
                __doprnt_put_string(buffer, _putch);
                break;
            case 'l':
                switch (fmt[i+1]) {
                case 'l':
                    switch (fmt[i+2]) {
                    case 'u':
                        //uint64_to_str(va_arg(argp, uint64_t), 10, buffer);
                        __doprnt_put_string(buffer, _putch);
                        break;
                    case 'x':
                        //uint64_to_str(va_arg(argp, uint64_t), 16, buffer);
                        __doprnt_put_string(buffer, _putch);
                        break;
                    case 'd':
                    default:
                        //int64_to_str(va_arg(argp, uint64_t), 10, buffer);
                        __doprnt_put_string(buffer, _putch);
                        break;
                    }
                    i++;
                    break;
                default:
                    break;
                }
                i++;
                break;
            default:
                _putch(fmt[i]);
            }
        } else _putch(fmt[i]);
    }
}