#pragma once

#include "ff_headers.h"
#include "ff_sddisk.h"
#include "ff_stdio.h"
#include "ff_utils.h"
#include "hw_config.h"
#include <stdbool.h>
#include <stddef.h>
#include <pico/stdlib.h>

#include "FreeRTOS.h"
#include "task.h"


class SD {
private:
    FF_Disk_t* pxDisk;
    bool isInitialized;
    bool isMounted;
    char mountPoint[32];

public:
    // Constructor
    SD();
    
    // Destructor
    ~SD();
    
    // Initialize SD card
    bool initialize(const char* diskName = "sd0");
    
    // Mount SD card
    bool mount(const char* mountPath = "/sd0");
    
    // Unmount SD card
    bool unmount();
    
    // Check if SD card is ready
    bool isReady() const;
    
    // Write data to file
    bool writeToFile(const char* filePath, const char* data);
    
    // Append data to file
    bool appendToFile(const char* filePath, const char* data);
    
    // Read data from file (caller must provide buffer and size)
    bool readFromFile(const char* filePath, char* buffer, size_t bufferSize);
    
    // Check if file exists
    bool fileExists(const char* filePath);
    
    // Create directory
    bool createDirectory(const char* dirPath);
    
    // Get error message
    const char* getErrorMessage(FF_Error_t error);
    
    // Cleanup resources
    void cleanup();
};