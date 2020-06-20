#include "../utils/platforms.h"
#ifdef WIN
#include "SocketObject.h"
#include <cstring>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <sys/types.h>

using namespace CPPSocket;
SocketObject::SocketObject(CPPSocket::netType_t net, CPPSocket::sockType_t sock, int protocol = 0) {
    domain = net;
    type = sock;

    if(type == SOCK_DGRAM) {
        WSAData wsaData;
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSAStartup(wVersionRequested, &wsaData);
    }

    socketHandle = socket(domain, type, protocol);

    timeval tm = {2, 0};
    ::setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tm, sizeof(timeval));
    ::setsockopt(socketHandle, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tm, sizeof(timeval));
}

SocketObject::SocketObject(const CPPSocket::SocketObject &server, int clientHandle, const void* _addr) {
    domain = server.domain;
    type = server.type;

    const sockaddr_in *addr = (sockaddr_in *)_addr;
    clientIP[0] = (addr->sin_addr.s_addr & 0xff);
    clientIP[1] = (addr->sin_addr.s_addr >> 8) & 0xff;
    clientIP[2] = (addr->sin_addr.s_addr >> 16) & 0xff;
    clientIP[3] = (addr->sin_addr.s_addr >> 24) & 0xff;

    clientPort = ntohs(addr->sin_port);
    domain = addr->sin_family;
    type = server.type;

    socketHandle = clientHandle;
    connected = true;
}

void SocketObject::bind(int port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(0);
    if(::bind(socketHandle, (sockaddr*)&addr, sizeof(addr)) == -1) throw FailToBind();
}

void SocketObject::listen(int queue) {
    if(::listen(socketHandle, queue)) throw FailToListen();
}

void SocketObject::connect(const unsigned long IP[4], unsigned long port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    std::stringstream ss;
    ss << IP[0] << "." << IP[1] << "." << IP[2] << "." << IP[3];
    std::string IPS;
    ss >> IPS;
    addr.sin_addr.s_addr = inet_addr(IPS.c_str());
    targetAddr = addr;
    if(type == SOCK_STREAM) if(::connect(socketHandle, (sockaddr*)&addr, sizeof(addr)) == -1) throw FailToConnect();
    connected = true;
}

SocketObject SocketObject::accept(bool async) {
    sockaddr_in addr{};
    int length = sizeof(addr);

    int clientHandle;

    if((clientHandle = ::accept(socketHandle, (sockaddr*)&addr, &length)) == -1) {
        if(async) throw FailToAccept();
    } else {
        return  {*this, clientHandle, &addr};
    }

    while((clientHandle = ::accept(socketHandle, (sockaddr*)&addr, &length)) == -1);

    return  {*this, clientHandle, &addr};
}

SocketObject& SocketObject::operator<<(const std::string &str) {
    if(!connected) throw NotConnected();
    if(domain == SOCK_STREAM)
        ::send(socketHandle, str.c_str(), str.length(), 0);
    else
        ::sendto(socketHandle, str.c_str(), str.length(), 0, (LPSOCKADDR)&targetAddr, sizeof(targetAddr));
    return *this;
}

SocketObject& SocketObject::operator<<(const char *buffer) {
    if(!connected) throw NotConnected();
    if(domain == SOCK_STREAM)
        ::send(socketHandle, buffer, strlen(buffer), 0);
    else
        ::sendto(socketHandle, buffer, strlen(buffer), 0, (LPSOCKADDR)&targetAddr, sizeof(targetAddr));

    return *this;
}

SocketObject& SocketObject::operator<<(const CPPSocket::SocketBuffer &buffer) {
    if(!connected) throw NotConnected();
    if(domain == SOCK_STREAM)
        ::send(socketHandle, buffer.content, buffer.len, 0);
    else
        ::sendto(socketHandle, buffer.content, buffer.len, 0, (LPSOCKADDR)&targetAddr, sizeof(targetAddr));

    return *this;
}

SocketObject& SocketObject::operator>>(std::string &target) {
    if(!connected) throw NotConnected();
    char buffer[1024]{};
    int len;
    if(domain == SOCK_STREAM) {
        if ((len = ::recv(socketHandle, buffer, 1024, 0)) == -1 && errno == EAGAIN) throw TimeOut();
    } else {
        int addrLen = sizeof(targetAddr);
        if ((len = ::recvfrom(socketHandle, buffer, 1024, 0, (LPSOCKADDR)&targetAddr, &addrLen)) == -1 && errno == EAGAIN) throw TimeOut();
    }

    buffer[len] = 0;
    target = buffer;

    return *this;
}

SocketObject& SocketObject::operator>>(char *buffer) {
    if(!connected) throw NotConnected();

    if(domain == SOCK_STREAM) {
        if (::recv(socketHandle, buffer, 1024, 0) == -1 && errno == EAGAIN) throw TimeOut();
    } else {
        int len = sizeof(targetAddr);
        if (::recvfrom(socketHandle, buffer, 1024, 0, (LPSOCKADDR)&targetAddr, &len) == -1 && errno == EAGAIN) throw TimeOut();
    }

    return *this;
}

SocketObject& SocketObject::operator>>(SocketBuffer &buffer) {
    if(!connected) throw NotConnected();

    if(domain == SOCK_STREAM) {
        int len;
        if ((len = ::recv(socketHandle, buffer.content, buffer.len, 0)) == -1 && errno == EAGAIN) throw TimeOut();
        buffer.len = len;
    } else {
        int addLen = sizeof(targetAddr);
        int len;
        if ((len = ::recvfrom(socketHandle, buffer.content, buffer.len, 0, (LPSOCKADDR)&targetAddr, &addLen)) == -1 && errno == EAGAIN) throw TimeOut();
        buffer.len = len;
    }

    return *this;
}

void SocketObject::close() {
    ::closesocket(socketHandle);
    closed = true;
}

#endif