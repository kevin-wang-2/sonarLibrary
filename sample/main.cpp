#include <iostream>
#include "../lib/libSonarLibrary.h"

int main() {
    initializeLibrary();

    unsigned long ip[] = {192, 168, 0, 183};
    openSonar(ip, 23);
    while(1) {
        const sonarReplyData_t* d = queryData();
        if(d != nullptr)
            std::cout << (((d->header.ucDataHi&0x7f) << 7) | (d->header.ucDataLo&0x7f)) << std::endl;
    }
    return 0;
}