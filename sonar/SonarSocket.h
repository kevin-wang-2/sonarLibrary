#ifndef SONARLIBRARY_SONARSOCKET_H
#define SONARLIBRARY_SONARSOCKET_H

#include <thread>
#include "windef.h"
#include "SocketObject.h"

const int maxQueueLen = 10;
const int maxBufLen = 2000;

struct sonarCmd_t {
    unsigned char ucHeader = 0xfe;			// 0xFE
    unsigned char ucID = 0x1;				// 0x1 to 0xfc
    unsigned char ucWorkStatus = 0x1;		// Bit 0 - 1 = Transmit if Bit 6 = 1
    // (Slave Mode)
    // Bit 1 - 1 = Send Data if Bit 6 = 1
    // (Slave Mode)
    // Bit 2 - 1 = Reverse, 0 = Normal
    // operation
    // Bit 3 - 1 = Hold (or Pause) Head, 0 =
    // Resume
    // Bit 4 - 1 Calibrate sonar head
    // (move to 0 degrees) (Slave Mode)
    // Bit 5 - 0 Normal sector, 1 one
    // direction sector
    // Bit 6 - 0 = Master, 1 = Slave
    // Bit 7 - 0 Nomal Work, 1 Command
    unsigned char ucRange = 2; 			// 1 to 20m

    unsigned char ucStartGain = 20; 		// 0 - 40dB
    unsigned char ucLOGF = 40; 			// 10,20,30,40log(r)
    unsigned char ucAbsorption = 10; 	// dB/100m
    unsigned char ucStepSize = 1; 		// ���࣬1

    short int nSoundSpeed = 1485;			// Sound speed, 1300-1700m/s
    short int nTrainAngle = 0; 			// 0 - 359��

    short int nSectorWidth = 3600; 		// 0-3600(0 - 360��)(0��ʾ�̶���ת��360��ʾ������)
    short int nDataLen = 1000; 			// ȱʡ1000�����峤�Ȼ�������������仯�������

    unsigned char ucPulseType = 0;		// ������������
    unsigned char ucRes1 = 0;			// ����
    short int nGate = 500;				// ������������

    short int nMinRange = 200; 			// ����������С���루����������2.5usÿ����
    short unsigned int nDelay = 0; 		// ������ʱ��Ϊ�˱�֤���ܻ���Ӱ�죬��Ҫ�����η���֮�������ʱ

    unsigned char ucRes2 = 0;			// ����
    unsigned char ucRes3 = 0;			// ����
    unsigned char ucFreq = 0;			// 0 -> �Զ��������������Զ�ѡ��
    unsigned char ucEnd = 0xfd;			// Termination, 0xFD
};

struct sonarReplyHeader_t {
    unsigned char ucFlag;			// 0xFE
    unsigned char ucID;				// 0x1 to 0xfc
    unsigned char ucStatus;			// Bit1:0 - 00 8bits���ݣ�һ�ֽڴ���һ���ݵ�

    unsigned char ucDataLo;			// Echo Package length
    unsigned char ucDataHi;			// len = (((ucDataHi&0x7f)<<7)|(ucDataLo&0x7f))
    unsigned char ucAngleLo;		// angle = (((ucAngleHi&0x7f)<<7)|(ucAngleLo&0x7f))
    unsigned char ucAngleHi;

    unsigned char ucEnd;			// Termination, 0xFD

};

struct sonarReplyData_t {
    sonarReplyHeader_t header;
    int angle;
    BYTE data[maxBufLen];
};

class SonarSocket;

class SonarRecvLoop {
    SonarSocket &parent;
public:
    explicit SonarRecvLoop(SonarSocket& ss) : parent(ss) {}

    void operator()();
};


class SonarHeartbeat {
    SonarSocket &parent;
public:
    explicit SonarHeartbeat(SonarSocket& ss) : parent(ss) {}

    void operator()();
};

class SonarSocket {
    CPPSocket::SocketObject sock;

    sonarCmd_t hbCmd {};

    sonarReplyData_t dataQueue[maxQueueLen] {};

    int front = 1, rear = 0;

    std::thread recvThread, hbThread;

    bool run = true;

    friend SonarRecvLoop;
    friend SonarHeartbeat;
public:
    SonarSocket();

    ~SonarSocket();

    void connectSonar(const unsigned long ip[4], unsigned long port);

    void sendCmd(const sonarCmd_t& cmd);

    const sonarReplyData_t *query();
};

#endif //SONARLIBRARY_SONARSOCKET_H
