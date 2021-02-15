#ifndef __WIO_PROPERTIES_SSD_CPP__
#define __WIO_PROPERTIES_SSD_CPP__

#include <SPI.h>
#include <Seeed_FS.h>

#include "SD/Seeed_SD.h"

#define PROP_FILE_BUFF_LEN 40
#define PROP_NAME_BUFF_LEN 40
#define PROP_FILE_SUFFIX "txt"
#define PROP_FAIL_NO_REBOOT false;

class Properties {
private:
    const char propFileNameSuffix[6] = PROP_FILE_SUFFIX;
    char readFileBuffer[PROP_FILE_BUFF_LEN];
    char fileNameBuff[PROP_NAME_BUFF_LEN];
    int readFileLineSD(char *buffer, const char *fileName);
    void writeFileSD(const char *fileName, const char *content);
    bool initFail;

public:
    Properties();
    ~Properties();
    int readString(char *buffer, const char *propName, const char *defaultValue);
    long readNum(const char *propName, int defaultValue);
    bool readBool(const char *propName);
    void writeString(const char *propName, const char *propValue);
    void writeNum(const char *propName, const long propValue);
    void writeBool(const char *propName, const bool propValue);
    bool initFailed();
    char *getDataBuffer();
};

#endif