#include "sonarRecordAPI.h"

SonarRecord sonarRecord;

void openRecord(const char* fn) {
    sonarRecord.open(fn);
}

bool readRecord(sonarRecordCb_t cb) {
    return sonarRecord.read(cb);
}
