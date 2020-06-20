#include <chrono>
#include <queue>
#include <string>
#include "SonarSocket.h"

SonarSocket::SonarSocket() : sock(AF_INET, SOCK_DGRAM, 0), front(0), rear(0) {
}

SonarSocket::~SonarSocket() {
    run = false;
    if(recvThread.joinable()) recvThread.join();
    if(hbThread.joinable()) hbThread.join();
}

void SonarSocket::connectSonar(const unsigned long *ip, unsigned long port) {
    sock.connect(ip, port);
    recvThread = std::thread(SonarRecvLoop(*this));
    hbThread = std::thread(SonarHeartbeat(*this));
}

void SonarSocket::sendCmd(const sonarCmd_t &cmd) {
    hbCmd = cmd;
}

const sonarReplyData_t* SonarSocket::query() {
    if(front == rear) return nullptr;
    const sonarReplyData_t* rep = &dataQueue[rear];
    rear = (rear + 1) % maxQueueLen;
    return rep;
}

void SonarHeartbeat::operator()() {
    while(parent.run) {
        parent.sock << CPPSocket::SocketBuffer({sizeof(parent.hbCmd), (char*)&parent.hbCmd});
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

/**
 * 利用有限状态机进行数据接收操作
 */
enum recvLoopState_t {
    WAIT_FOR_HEADER,
    WAIT_FOR_CONTENT,
    WAIT_FOR_CLEAR
};

void SonarRecvLoop::operator()() {
    using queue = std::queue<BYTE>;
    static queue buffer;
    static sonarReplyHeader_t curHeader;
    recvLoopState_t state = WAIT_FOR_HEADER;
    while(parent.run) {
        try {
            BYTE temp[2000]{};
            CPPSocket::SocketBuffer sb{2000, (char*)temp};
            parent.sock >> sb;
            for(int i = 0; i < sb.len; i++) buffer.push(temp[i]);
        } catch(CPPSocket::SocketObject::TimeOut &t) {}
        if(!buffer.empty()) {
            if(state == WAIT_FOR_HEADER && buffer.size() >= sizeof(sonarReplyHeader_t)) { // Received header
                BYTE temp[sizeof(sonarReplyHeader_t)];
                for(BYTE & i : temp) {
                    i = buffer.front();
                    buffer.pop();
                }
                curHeader = *(sonarReplyHeader_t*)(temp);
                if((curHeader.ucFlag != 0xfe) || (curHeader.ucEnd != 0xfd)) {
                    state = WAIT_FOR_CLEAR;
                } else state = WAIT_FOR_CONTENT;
            } else if(state == WAIT_FOR_CONTENT && (buffer.size() >= (unsigned int)((((curHeader.ucDataHi & 0x7f) << 7) | (curHeader.ucDataLo & 0x7f)) + 2))) { // Received Data
                if((((curHeader.ucDataHi & 0x7f) << 7) | (curHeader.ucDataLo & 0x7f)) > maxBufLen) {
                    for(int i = 0; i < ((((curHeader.ucDataHi & 0x7f) << 7) | (curHeader.ucDataLo & 0x7f)) + 2); i++) buffer.pop(); // Wrong data
                } else {
                    auto &cur = parent.dataQueue[parent.front];
                    for (int i = 0; i < ((((curHeader.ucDataHi & 0x7f) << 7) | (curHeader.ucDataLo & 0x7f)) + 2); i++) {
                        cur.data[i] = buffer.front();
                        buffer.pop();
                    }
                    cur.angle = ((curHeader.ucAngleHi & 0x7f) << 7) | (curHeader.ucAngleLo & 0x7f);
                    cur.header = curHeader;
                    parent.front = (parent.front + 1) % maxQueueLen;
                    if (parent.rear == parent.front) parent.rear = (parent.rear + 1) % maxQueueLen;
                }
                state = WAIT_FOR_HEADER;
            } else if(state == WAIT_FOR_CLEAR) {
                while((buffer.front() != 0xfe) && !buffer.empty()) buffer.pop();
                if(!buffer.empty()) state = WAIT_FOR_HEADER;
            }
        }
    }
}