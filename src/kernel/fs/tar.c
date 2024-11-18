#include <fs/tar.h> // Include the TAR file system header

// Initialize a TAR structure with a pointer to the TAR data and its size
tar_t* tar_init(char* tar_ptr, size_t tar_size) {
    tar_t *tar = (tar_t*)kmalloc(sizeof(tar_t)); // Allocate memory for the TAR structure
    tar->ptr = tar_ptr;                          // Set the pointer to the TAR data
    tar->size = tar_size;                        // Set the size of the TAR data
    return tar;                                  // Return the initialized TAR structure
}

// Convert the size field from a TAR header from ASCII to an integer
uint32_t tar_get_size(char *in) {
    uint32_t size = 0;                          // Initialize size to 0
    uint32_t j;
    uint32_t count = 1;

    for (j = 11; j > 0; j--, count *= 8)       // Iterate over the size string in the TAR header
        size += ((in[j - 1] - '0') * count);   // Convert ASCII characters to an integer

    return size;                                // Return the computed size
}

// Retrieve a file from the TAR archive by name
tar_t tar_get_file(tar_t* tar, char* name) {
    size_t offset = 512;                        // Start at the first header offset
    tar_header_t* th;                           // Pointer to the current TAR header
    tar_t file = {                              // Initialize a file structure with default values
        .size = 0,
        .ptr = 0
    };

    for(;;) {                                   // Infinite loop to iterate through entries in the TAR archive
        th = (tar_header_t*)(tar->ptr + offset); // Get the current header
        
        if(th->name[0] == '\0') break;         // Break if we reach the end of the archive

        size_t size = tar_get_size(th->size); // Get the size of the current file
        
        if(strncmp(th->name, name, 100) == 0) { // Check if the current file matches the requested name
            file.size = size;                  // Set file size
            file.ptr = tar->ptr + offset + 512; // Set pointer to file data in TAR archive
            return file;                       // Return found file information
        }
        
        offset += ((size / 512) + 1) * 512;     // Move offset to next header based on current file size

        if(size % 512) {                       // If there's a remainder, skip padding bytes
            offset += 512;
        }
    }
    
    return file;                               // Return an empty file structure if not found
}
