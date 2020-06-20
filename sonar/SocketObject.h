#ifndef CPPWEBLOGGER_SOCKETOBJECT_H
#define CPPWEBLOGGER_SOCKETOBJECT_H

#include <string>
#include "../utils/platforms.h"

#ifdef WIN
#include <winsock2.h>
#include <sys/types.h>
#else
#endif

namespace CPPSocket {

    typedef int netType_t;
    typedef int sockType_t;

    struct SocketBuffer {
        int len;
        char* content;
    };

    struct IP {
        unsigned int IP[4];
        unsigned int port;
    };

    /**
     * 封装Socket库
     */
    class SocketObject {
    protected:
        bool connected = false;

        bool closed = false;

        int domain, type;

        int socketHandle;

        sockaddr_in targetAddr;

        SocketObject(const SocketObject &server, int clientHandle, const void* addr);
    public:

        unsigned int clientIP[4]{};

        unsigned int clientPort = 0;

        class NetworkError : public std::exception {};

        class FailToBind : public NetworkError {};

        class FailToListen : public NetworkError {};

        class FailToConnect : public NetworkError {};

        class FailToAccept : public NetworkError {};

        class NotConnected : public NetworkError {};

        class TimeOut : public std::exception {};

        SocketObject(netType_t net, sockType_t sock, int protocol);

        void bind(int port);

        void listen(int queue = 20);

        void connect(const unsigned long IP[4], unsigned long port);

        SocketObject accept(bool async = false);

        SocketObject& operator <<(const std::string& str);

        SocketObject& operator <<(const char* buffer);

        SocketObject& operator <<(const SocketBuffer& buffer);

        SocketObject& operator >>(std::string& target);

        SocketObject& operator >>(char* buffer);

        SocketObject& operator >>(SocketBuffer& buffer);

        void close();

        bool isClosed() { return closed; }
    };
}

#endif //CPPWEBLOGGER_SOCKETOBJECT_H
