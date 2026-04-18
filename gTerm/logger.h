#ifndef LOGGER_H
#define LOGGER_H

// Change this to 0 to disable all logging
#define DEBUG_MACRO 1

#define LOG_ERROR_SYS(...) do { std::cout << "[ERROR]: " << __VA_ARGS__ << std::endl; } while(0)
#define LOG_ERROR_ERRNO(msg) do { \
        std::cout << "[ERROR]: " << msg << ": " << strerror(errno) << std::endl; \
    } while(0)

#ifdef IS_WINDOWS
#define LOG_ERROR_WIN32(msg) do { \
        DWORD err = GetLastError(); \
        LPVOID lpMsgBuf = nullptr; \
        FormatMessageA( \
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, \
            NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
            (LPSTR)&lpMsgBuf, 0, NULL); \
        std::cout << "[ERROR]: " << msg << " (code " << err << "): " << (lpMsgBuf ? (char*)lpMsgBuf : "Unknown error") << std::endl; \
        if (lpMsgBuf) LocalFree(lpMsgBuf); \
    } while(0)
#elif defined(IS_LINUX)
//TODO: Need to test this on Linux
//#include <cstring>   // for strerror()
//#include <cerrno>    // for errno

#define LOG_ERROR_UNIX(msg) do { \
        int err = errno; \
        std::cout << "[ERROR]: " << msg << " (code " << err << "): " \
                  << (err != 0 ? strerror(err) : "Unknown error") << std::endl; \
    } while(0)
#endif


#if DEBUG_MACRO
#define LOG_SUCCESS(...)  do { std::cout << "[SUCCESS]: "  << __VA_ARGS__ << std::endl; } while(0)
#define LOG_INFO(...)  do { std::cout << "[INFO]: "  << __VA_ARGS__ << std::endl; } while(0)
#define LOG_WARN(...)  do { std::cout << "[WARN]: "  << __VA_ARGS__ << std::endl; } while(0)
#define LOG_ERROR(...) do { std::cout << "[ERROR]: " << __VA_ARGS__ << std::endl; } while(0)
#define LOG_DEBUG(...) do { std::cout << "[DEBUG]: " << __VA_ARGS__ << std::endl; } while(0)

#else
#define LOG_SUCCESS(...)  ((void)0)
#define LOG_INFO(...)  ((void)0)
#define LOG_WARN(...)  ((void)0)
#define LOG_ERROR(...) ((void)0)
#define LOG_DEBUG(...) ((void)0)
#endif

#endif // LOGGER_H

/*
-- USAGE --
LOG_SUCCESS("gTerm terminal rxQueue Cleared");
LOG_INFO("gTerm terminal rxQueue Cleared");
LOG_WARN("Low memory warning, free: " << free_bytes << " bytes");
LOG_ERROR("Failed to open file: " << filename);
LOG_DEBUG("rxQueue size = " << rxQueue.size() << " bytes");
LOG_ERROR_ERRNO("Serial read failed");
*/