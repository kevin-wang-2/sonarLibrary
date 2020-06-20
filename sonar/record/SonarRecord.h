#ifndef SONARLIBRARY_SONARRECORD_H
#define SONARLIBRARY_SONARRECORD_H

#include <string>
#include <cstdio>
#include "windef.h"

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

class SonarRecord {
    FILE* fp = nullptr;
public:
    class FileDoesntExist : std::exception {};
    class FileCorrupt : std::exception {};

    ~SonarRecord();

    void open(const std::string& fn);

    bool read(sonarRecordCb_t cb);
};

#endif //SONARLIBRARY_SONARRECORDAPI_H
