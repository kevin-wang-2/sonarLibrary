#include "SonarRecord.h"

const int maxBufLen = 2000;

void SonarRecord::open(const std::string &fn) {
    fp = fopen(fn.c_str(), "rb");
    if(!fp) throw FileDoesntExist();
}

SonarRecord::~SonarRecord() {
    if(fp) fclose(fp);
}

bool SonarRecord::read(sonarRecordCb_t cb) {
    if(!fp) return false;
    sonarRecordHeader_t header;
    size_t len = fread(&header, sizeof(header), 1, fp);
    if(len == 1) {
        if(header.ucFlag != 0xfe) throw FileCorrupt();

        int dataLen = ((header.ucDataLenHi&0x7f) << 7) | (header.ucDataLenLo&0x7f);
        if(dataLen > maxBufLen) throw FileCorrupt();

        int angle = ((header.ucAngleHi&0x7f) << 7) | (header.ucAngleLo&0x7f);
        if(angle < 0 || angle > 800) throw FileCorrupt();

        BYTE data[maxBufLen + 2];
        fread(data, sizeof(BYTE), dataLen + 2, fp);

        cb(header, angle, data);
        return true;
    } else return false;
}