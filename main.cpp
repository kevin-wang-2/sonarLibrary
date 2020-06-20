#include <iostream>
#include "windows.h"
#include <cassert>

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
    BYTE data[2000];
};

int main() {
    HMODULE hDll = LoadLibrary("libSonarLibrary");
    assert(hDll);

    auto openSonar = (void (*)(const int [4], unsigned int))GetProcAddress(hDll, "openSonar");
    auto query = (const sonarReplyData_t* (*)())GetProcAddress(hDll, "queryData");

    int ip[] = {192, 168, 0, 183};
    openSonar(ip, 23);
    while(1) {
        const sonarReplyData_t* d = query();
        if(d != nullptr)
            std::cout << (((d->header.ucDataHi&0x7f) << 7) | (d->header.ucDataLo&0x7f)) << std::endl;
    }

    return 0;
}