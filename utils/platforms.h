#ifndef CPPWEBLOGGER_PLATFORMS_H
#define CPPWEBLOGGER_PLATFORMS_H

#if defined(__linux__) || defined(__linux)
#   define LINUX
#endif
#if !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#   define WIN
#endif
#if !defined(WIN) && !defined(LINUX)
#   error "Platform not supported"
#endif

#endif //CPPWEBLOGGER_PLATFORMS_H
