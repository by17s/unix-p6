#include <drivers/vbe.h> // Include the VBE driver header

// Structure to hold VBE information including framebuffer, dimensions, and color palette
struct __vbe_info_s {
    void *_fb;                             // Pointer to the framebuffer
    uint32_t* fb;                          // Pointer to framebuffer as a 32-bit integer array
    int h, w;                              // Height and width of the framebuffer
    int bpp;                               // Bits per pixel
    enum vbe_color_palette palette;        // Color palette used
} __vbe_info;                             // Global instance of VBE information structure

// Initialize the VBE with framebuffer, dimensions, bits per pixel, and color palette
int vbe_init(void* fb, int w, int h, int bpp, enum vbe_color_palette p) {
    __vbe_info._fb = fb;                  // Set framebuffer pointer
    __vbe_info.fb = (uint32_t*)fb;       // Cast framebuffer to a 32-bit pointer
    __vbe_info.w = w;                     // Set width
    __vbe_info.h = h;                     // Set height
    __vbe_info.bpp = bpp;                 // Set bits per pixel
    __vbe_info.palette = p;               // Set color palette
}

// Clear the framebuffer to a specific color
void __vbe_clear(uint32_t color) {
    tmemsetd(__vbe_info._fb, color, __vbe_info.w *  __vbe_info.h); // Fill the framebuffer with the specified color
}

// Fill a specific line in the framebuffer with a color
void __vbe_fill_line(uint32_t color, uint32_t ln) {
    tmemsetd(&__vbe_info.fb[ln * __vbe_info.w], color, __vbe_info.w); // Fill the specified line with the given color
}

// Handle commands related to VBE operations
int vbe_sendcmd(cmd_t cmd, ...) {
    va_list args;                         // Variable argument list for command handling
    va_start(args, cmd);                  // Initialize variable argument list
    
    switch (cmd) {
    case VBE_CLEAR:                        // Command to clear the screen
        __vbe_clear(va_arg(args, uint32_t)); // Get color from arguments and clear screen
        break;
    case VBE_FILL_LINE:                   // Command to fill a line with color
        __vbe_fill_line(va_arg(args, uint32_t), va_arg(args, uint32_t)); // Fill specified line with color from arguments
        break;
    case VBE_GET_RES:                     // Command to get current resolution
        *(va_arg(args, int*)) = __vbe_info.w;  // Return width via pointer argument
        *(va_arg(args, int*)) = __vbe_info.h;  // Return height via pointer argument
        break;
    default:
        va_end(args);                     // Clean up variable argument list on error
        return 1;                         // Return error code for unknown command
        break;
    }
    
    va_end(args);                          // Clean up variable argument list after processing command
    return 0;                              // Return success code for command execution
}

// Set a pixel in the framebuffer at a specific position with a specific color data
int vbe_set_px(size_t pos, uint32_t data) {
    __vbe_info.fb[pos] = data;            // Set pixel data at specified position in framebuffer
}

// Get the color data of a pixel at a specific position in the framebuffer
uint32_t vbe_get_px(size_t pos) {
    return __vbe_info.fb[pos];             // Return pixel data from specified position in framebuffer
}

// Function to draw a character bitmap (implementation not provided)
int vbe_draw_char_bitmap0(char* font, char ch, int w, int h) {
    // Implementation for drawing character bitmap will go here...
}
