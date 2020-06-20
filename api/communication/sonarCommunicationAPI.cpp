//
// Created by 22490 on 2020/6/19.
//

#include "sonarCommunicationAPI.h"

SonarSocket ss;

void openSonar(const unsigned long ip[4],  unsigned long port) {
    ss.connectSonar(ip, port);
}

void sendCmd(sonarCmd_t cmd) {
    ss.sendCmd(cmd);
}

const sonarReplyData_t*queryData() {
    return ss.query();
}