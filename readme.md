#简介
用于与Std46C以及弋洋智能科技有限公司、世纳电子技术有限公司声呐通讯与数据存储的API。
#编译
该API目前仅兼容Windows操作系统，可在支持Posix线程的MinGW下利用cmake编译。默认生成有动态链接库libSonarLibrary.dll与样例测试程序sonarLibTest.exe。
#使用
当使用C/C++进行编程，可利用windows系统API来载入该动态链接库。该动态链接库包括以下接口
1. 基于基于网络的声呐通信接口
    - `void openSonar(const unsigned long ip[4],  unsigned long port)` 连接声呐，由于声呐使用UDP协议，该指令不会失败。
    - `void sendCmd(sonarCmd_t cmd)` 向声呐发送指令，`sonarCmd_t`结构体的定义可在sonar\/SonarSocket.h中找到。
    - `const sonarReplyData_t* queryData()` 请求一个声呐回波消息，若目前没有在队列中的回波消息，则返回`nullptr`。消息队列最大容量为10。
2. 声呐回放数据接口
    - `void openRecord(const char* fn)` 打开声呐回放文件，若文件不存在，将会返回一个异常。
    - `bool readRecord(sonarRecordCb_t cb)` 读取声呐文件中一次回波信号，并调用回调函数。回调函数的定义可在sonar\/SonarRecord.h中找到。