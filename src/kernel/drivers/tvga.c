#include <drivers/tvga.h> // Include the TVGA driver header

#include <stdint.h>       // Include standard integer types
#include <stddef.h>       // Include standard definitions
#include <tools.h>        // Include utility functions
#include <x86.h>          // Include x86 architecture-specific definitions

// Define constants for VGA dimensions and buffer address
#define VGA_WIDTH 80                // Width of the VGA display (characters)
#define VGA_HEIGHT 25               // Height of the VGA display (lines)
#define VGA_BUFFER_ADDRESS 0xB8000  // Memory address for VGA buffer
#define DEFAULT_COLOR 0x0F          // Default text color (white on black)
#define TABSPACES 4                  // Number of spaces for a tab

// Define static variables for the VGA buffer and cursor attributes
static uint16_t* const tvga_vbuffer = (uint16_t*)VGA_BUFFER_ADDRESS; // Pointer to the VGA buffer
static uint8_t tvga_curattr = DEFAULT_COLOR;                          // Current text attribute (color)
static uint16_t cursor_pos = 0;                                       // Current cursor position

// Function to create an attribute entry combining character and color
static inline uint16_t attr_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t) uc | (uint16_t) color << 8; // Combine character and color into a single entry
}

// Update the cursor position in the VGA hardware
void tvga_updatecursorio(uint16_t pos)
{
    outb(0x3D4, 0x0F);                    // Set cursor position high byte index
    outb(0x3D5, (uint8_t) (pos & 0xFF));  // Write low byte of cursor position
    outb(0x3D4, 0x0E);                    // Set cursor position low byte index
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF)); // Write high byte of cursor position
}

// Update the internal cursor position and hardware cursor
static void tvga_updatecursor(uint32_t position)
{
    cursor_pos = position;                // Set the new cursor position
    tvga_updatecursorio(cursor_pos);     // Update the hardware with the new position
}

// Set the current text attribute (color)
static void tvga_setattr(uint8_t color)
{
    tvga_curattr = color;                 // Update current text attribute to new color
}

// Clear the screen by filling it with spaces and resetting cursor position
static void tvga_clr()
{
    tmemsetw(tvga_vbuffer, attr_entry(' ', tvga_curattr), VGA_HEIGHT * VGA_WIDTH); // Fill buffer with spaces and current attribute
    cursor_pos = 0;                      // Reset cursor position to start
}

// Reset the display by setting default attributes and clearing the screen
static void tvga_reset()
{
    tvga_setattr(DEFAULT_COLOR);          // Set attribute to default color
    tvga_clr();                           // Clear the screen
}

// Initialize the TVGA display by resetting it
void tvga_init()
{
    tvga_reset();                         // Call reset function to initialize display
}

// Handle commands related to TVGA operations such as setting cursor, color, or clearing screen
int tvga_command(cmd_t cmd, ...)
{
    va_list args;                        // Variable argument list for command handling
    va_start(args, cmd);                 // Initialize variable argument list

    switch(cmd) {
        case VGA_CMD_SETCURSOR:          // Command to set cursor position
            uint32_t cursor = va_arg(args, uint32_t);
            if(cursor >= VGA_WIDTH * VGA_HEIGHT)   // Validate cursor position range
                return 2;
            
            tvga_updatecursor(cursor);   // Update cursor position in display
            break;
        case VGA_CMD_SETCOLOR:           // Command to set text color attribute
            tvga_setattr((uint8_t)va_arg(args, uint32_t));
            break;
        case VGA_CMD_CLEAR:               // Command to clear the screen
            tvga_clr();
            break;
        default:
            return 1;                    // Invalid command case handling
            break;
    }

    va_end(args);                        // Clean up variable argument list
    return 0;                            // Successful command execution return value
}

// Handle line feed operation by moving cursor to next line start position
static void tvga_linefeed()
{
    cursor_pos = (cursor_pos / VGA_WIDTH + 1) * VGA_WIDTH; // Move to start of next line
}

// Handle carriage return operation by moving cursor to start of current line
static void tvga_carriagereturn()
{
    cursor_pos -= (cursor_pos % VGA_WIDTH);   // Move cursor to start of current line
}

// Handle tab operation by moving cursor forward by tab spaces defined 
static void tvga_tab()
{
    cursor_pos = cursor_pos - (cursor_pos % TABSPACES) + TABSPACES;  // Move to next tab stop 
}

// Handle backspace operation by moving the cursor one position back if possible 
static void tvga_backspace()
{
    if (cursor_pos > 0) {
        cursor_pos--;                       // Decrement cursor position if not at start 
    }
}

// Scroll the screen up by one line when bottom is reached 
static void tvga_scrollscreen()
{
    tmemcpyw(tvga_vbuffer, tvga_vbuffer + VGA_WIDTH, (VGA_HEIGHT - 1) * VGA_WIDTH); 
    tmemsetw(tvga_vbuffer + (VGA_HEIGHT - 1) * VGA_WIDTH, attr_entry(' ', tvga_curattr), VGA_WIDTH); 
    cursor_pos -= VGA_WIDTH;               // Adjust cursor position after scrolling 
}

// Output a single character to the TVGA display with appropriate handling for control characters 
static void tvga_putch(char c)
{
    switch(c) {
        case '\n':                          // Newline character handling 
            tvga_linefeed();
            break;
        case '\r':                          // Carriage return character handling 
            tvga_carriagereturn();
            break;
        case '\t':                          // Tab character handling 
            tvga_tab();
            break;
        case '\b':                          // Backspace character handling 
            tvga_backspace();
            break;
        default:                            // Regular character output 
            tvga_vbuffer[cursor_pos++] = attr_entry(c, tvga_curattr);
            break;
    }

    if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT) {   // Check if we need to scroll screen 
        tvga_scrollscreen();
    }

    tvga_updatecursor(cursor_pos);             // Update hardware with new cursor position 
}

// Write a string of characters to the TVGA display with specified length 
void tvga_write(const char* str, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {      // Loop through each character in string 
        tvga_putch(str[i]);                     // Output each character using putch function 
    }
}
