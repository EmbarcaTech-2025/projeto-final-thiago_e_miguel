#include "SD.h"
#include <string.h>

SD::SD() : pxDisk(nullptr), isInitialized(false), isMounted(false) {
    mountPoint[0] = '\0';
}

SD::~SD() {
    cleanup();
}

bool SD::initialize(const char* diskName) {
    if (isInitialized) {
        return true; // Already initialized
    }
    
    pxDisk = FF_SDDiskInit(diskName);
    if (!pxDisk) {
        return false;
    }
    
    isInitialized = true;
    return true;
}

bool SD::mount(const char* mountPath) {
    if (!isInitialized) {
        return false; // Must initialize first
    }
    
    if (isMounted) {
        return true; // Already mounted
    }
    
    FF_Error_t xError = FF_SDDiskMount(pxDisk);
    if (FF_isERR(xError) != pdFALSE) {
        return false;
    }
    
    FF_FS_Add(mountPath, pxDisk);
    strncpy(mountPoint, mountPath, sizeof(mountPoint) - 1);
    mountPoint[sizeof(mountPoint) - 1] = '\0';
    isMounted = true;
    return true;
}

bool SD::unmount() {
    if (!isMounted) {
        return true; // Already unmounted
    }
    
    FF_FS_Remove(mountPoint);
    FF_Unmount(pxDisk);
    isMounted = false;
    mountPoint[0] = '\0';
    return true;
}

bool SD::isReady() const {
    return isInitialized && isMounted;
}

bool SD::writeToFile(const char* filePath, const char* data) {
    if (!isReady()) {
        return false;
    }
    
    char fullPath[64];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", mountPoint, filePath);
    
    FF_FILE* pxFile = ff_fopen(fullPath, "w");
    if (!pxFile) {
        return false;
    }
    
    int result = ff_fprintf(pxFile, "%s", data);
    bool success = (result >= 0);
    
    ff_fclose(pxFile);
    return success;
}

bool SD::appendToFile(const char* filePath, const char* data) {
    if (!isReady()) {
        return false;
    }
    
    char fullPath[64];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", mountPoint, filePath);
    
    FF_FILE* pxFile = ff_fopen(fullPath, "a");
    if (!pxFile) {
        return false;
    }
    
    int result = ff_fprintf(pxFile, "%s", data);
    bool success = (result >= 0);
    
    ff_fclose(pxFile);
    return success;
}

bool SD::readFromFile(const char* filePath, char* buffer, size_t bufferSize) {
    if (!isReady() || !buffer || bufferSize == 0) {
        return false;
    }
    
    char fullPath[64];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", mountPoint, filePath);
    
    FF_FILE* pxFile = ff_fopen(fullPath, "r");
    if (!pxFile) {
        return false;
    }
    
    size_t bytesRead = ff_fread(buffer, 1, bufferSize - 1, pxFile);
    buffer[bytesRead] = '\0';
    
    ff_fclose(pxFile);
    return true;
}

bool SD::fileExists(const char* filePath) {
    if (!isReady()) {
        return false;
    }
    
    char fullPath[64];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", mountPoint, filePath);
    
    FF_FILE* pxFile = ff_fopen(fullPath, "r");
    if (!pxFile) {
        return false;
    }
    
    ff_fclose(pxFile);
    return true;
}

bool SD::createDirectory(const char* dirPath) {
    if (!isReady()) {
        return false;
    }
    
    char fullPath[64];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", mountPoint, dirPath);
    
    FF_Error_t xError = ff_mkdir(fullPath);
    return (FF_isERR(xError) == pdFALSE);
}

const char* SD::getErrorMessage(FF_Error_t error) {
    return FF_GetErrMessage(error);
}

void SD::cleanup() {
    if (isMounted) {
        unmount();
    }
    
    if (isInitialized && pxDisk) {
        FF_SDDiskDelete(pxDisk);
        pxDisk = nullptr;
        isInitialized = false;
    }
}
