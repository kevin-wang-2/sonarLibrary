#ifndef SONARLIBRARY_SONARCOMMUNICATIONAPI_H
#define SONARLIBRARY_SONARCOMMUNICATIONAPI_H

#include "../../utils/DLLUtils.h"
#include "../../sonar/comm/SonarSocket.h"

DLLEXPORT void openSonar(const unsigned long ip[4],  unsigned long port);

DLLEXPORT void sendCmd(sonarCmd_t cmd);

DLLEXPORT const sonarReplyData_t* queryData();

#endif //SONARLIBRARY_SONARCOMMUNICATIONAPI_H
