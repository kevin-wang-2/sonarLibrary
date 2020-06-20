#ifndef SONARLIBRARY_SONARRECORDAPI_H
#define SONARLIBRARY_SONARRECORDAPI_H

#include "../../sonar/SonarRecord.h"
#include "../../utils/DLLUtils.h"

DLLEXPORT void openRecord(const char* fn);

DLLEXPORT bool readRecord(sonarRecordCb_t cb);

#endif //SONARLIBRARY_SONARRECORDAPI_H
