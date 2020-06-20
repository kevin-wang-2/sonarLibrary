#ifndef SONARLIBRARY_LIBSONARLIBRARY_H
#define SONARLIBRARY_LIBSONARLIBRARY_H

#include "windef.h"

const int maxBufLen = 1000;

typedef struct sonarCmd_t {
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

    unsigned char ucStartGain = 20; 	// 0 - 40dB
    unsigned char ucLOGF = 40; 			// 10,20,30,40log(r)
    unsigned char ucAbsorption = 10; 	// dB/100m
    unsigned char ucStepSize = 1; 		// ���࣬1

    short int nSoundSpeed = 1485;		// Sound speed, 1300-1700m/s
    short int nTrainAngle = 0; 			// 0 - 359��

    short int nSectorWidth = 3600; 		// 0-3600
    short int nDataLen = 1000; 			// Max Data Length

    unsigned char ucPulseType = 0;
    unsigned char ucRes1 = 0;
    short int nGate = 500;

    short int nMinRange = 200; 			//
    short unsigned int nDelay = 0; 		//

    unsigned char ucRes2 = 0;			//
    unsigned char ucRes3 = 0;			//
    unsigned char ucFreq = 0;			//
    unsigned char ucEnd = 0xfd;			// Termination, 0xFD
} sonarCmd_t;

typedef struct sonarReplyHeader_t {
    unsigned char ucFlag;			// 0xFE
    unsigned char ucID;				// 0x1 to 0xfc
    unsigned char ucStatus;			// Bit1:0 - 00 8bits

    unsigned char ucDataLo;			// Echo Package length
    unsigned char ucDataHi;			// len = (((ucDataHi&0x7f)<<7)|(ucDataLo&0x7f))
    unsigned char ucAngleLo;		// angle = (((ucAngleHi&0x7f)<<7)|(ucAngleLo&0x7f))
    unsigned char ucAngleHi;

    unsigned char ucEnd;			// Termination, 0xFD

} sonarReplyHeader_t;

typedef struct sonarReplyData_t {
    sonarReplyHeader_t header;
    int angle;
    BYTE data[maxBufLen];
} sonarReplyData_t;

typedef struct sonarRecordHeader_t {
    unsigned char ucFlag;			// 0xFE
    unsigned char ucSize;			// Size of RECORDFORMAT
    unsigned char ucVersion;		// Version of RECORDFORMAT
    unsigned char ucParam; 			// Bit3:0 - �࣬1 ~ 15, Bit6:Sector

    DWORD tm;						// Year:6 Day:5 Hour:5 Mon:4 Min:6 Sec:6

    unsigned char ucAngleLo;		//
    unsigned char ucAngleHi;		// ((ucAngleHi&0x7f)<<7)+(ucAngleLo&0x7f)
    unsigned char ucDataLenLo;		//
    unsigned char ucDataLenHi;		// ((ucDataLenHi&0x7f)<<7)+(ucDataLenLo&0x7f)

    unsigned char ucFreq;			// Frequency
    unsigned char ucRange; 			// 1 to 200m
    unsigned char ucGain; 			// 0 - 40dB
    unsigned char ucRes1; 			// 0

    unsigned char ucMaxStepLo; 		//
    unsigned char ucMaxStepHi; 		// ((ucMaxStepHi&0x7f)<<7)+(ucMaxStepLo&0x7f)
    unsigned char ucRes2; 			// 0
    unsigned char ucEnd;
} sonarRecordHeader_t;

typedef void (*sonarRecordCb_t)(const sonarRecordHeader_t&, int, const BYTE*);

void initializeLibrary();

void openSonar(const unsigned long ip[4],  unsigned long port);

void sendCmd(sonarCmd_t cmd);

const sonarReplyData_t* queryData();

void openRecord(const char* fn);

bool readRecord(sonarRecordCb_t cb);

#endif //SONARLIBRARY_LIBSONARLIBRARY_H
