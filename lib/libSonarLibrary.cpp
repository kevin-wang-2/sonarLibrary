#include "libSonarLibrary.h"
#include "windows.h"
#include <cassert>

typedef void (*openSonar_t)(const unsigned long [4], unsigned long);
typedef void (*sendCmd_t)(sonarCmd_t);
typedef const sonarReplyData_t *(*queryData_t)();
typedef void (*openRecord_t)(const char*);
typedef bool (*readRecord_t)(sonarRecordCb_t);

openSonar_t openSonar_f;
sendCmd_t sendCmd_f;
queryData_t queryData_f;
openRecord_t openRecord_f;
readRecord_t readRecord_f;

bool initialized = false;

void initializeLibrary() {
    HMODULE hDll = LoadLibrary("libSonarLibrary");
    assert(hDll);

    openSonar_f = (openSonar_t)GetProcAddress(hDll, "openSonar");
    sendCmd_f = (sendCmd_t)GetProcAddress(hDll, "sendCmd");
    queryData_f = (queryData_t)GetProcAddress(hDll, "queryData");
    openRecord_f = (openRecord_t)GetProcAddress(hDll, "openRecord");
    readRecord_f = (readRecord_t)GetProcAddress(hDll, "readRecord");

    initialized = true;
}

void openSonar(const unsigned long ip[4],  unsigned long port) {
    assert(initialized && openSonar_f);
    openSonar_f(ip, port);
}

void sendCmd(sonarCmd_t cmd) {
    assert(initialized && sendCmd_f);
    sendCmd_f(cmd);
}

const sonarReplyData_t* queryData() {
    assert(initialized && queryData_f);
    return queryData_f();
}

void openRecord(const char* fn) {
    assert(initialized && openRecord_f);
    openRecord_f(fn);
}

bool readRecord(sonarRecordCb_t cb) {
    assert(initialized && readRecord_f);
    return readRecord_f(cb);
}