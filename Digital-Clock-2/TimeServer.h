#ifndef __WIO_TIME_SERVER_CPP__
#define __WIO_TIME_SERVER_CPP__

#include "DateTime.h"
#include "rpcWiFi.h"
#define TIME_DATA_URL_CONST "/api/ip.txt"
#define TIME_DATA_HOST_CONST "worldtimeapi.org"
#define FILE_BUFF_LEN 60
#define MESSAGE_BUFF_LEN 40
#define NTP_PACKET_LEN 48
#define NTP_LOCAL_PORT 2390
#define SEC_IN_HOUR 3600
#define SEC_IN_MIN 60
#define CHAR_0 48

const char timeDataHost[] = TIME_DATA_HOST_CONST;
const char timeDataUrl[] = TIME_DATA_URL_CONST;
const char dstResponseMatch[] = {5, 'd', 's', 't', ':', ' '};
const char unixtimeResponseMatch[] = {10, 'u', 'n', 'i', 'x', 't', 'i', 'm', 'e', ':', ' '};
/*
  Time zone pairs.
  tZonePairs[0]:tZonePairs[1] defines offset 0, -12 hours + 0 minutes
  tZonePairs[2]:tZonePairs[3] defines offset 1, -11  hours + 0 minutes
  ...
  tZonePairs[27]:tZonePairs[28] defines offset 12, 0  hours + 0 minutes (London)
  ...
*/
#define TZ_PAIRS_MAX 37
#define TZ_PAIRS_GMT 14
const int tZonePairs[] = {-12, 0, -11, 0, -10, 0, -9, -30, -9, 0, -8, 0, -7, 0, -6, 0, -5, 0, -4, 0, -3, -30, -3, 0, -2, 0, -1, 0, +0, 0, +1, 0, +2, 0, +3, 0, +3, 30, +4, 0, +4, 30, +5, 0, +5, 30, +5, 45, +6, 0, +6, 30, +7, 0, +8, 0, +8, 45, +9, 0, +9, 30, +10, 0, +10, 30, +11, 0, +12, 0, +12, 45, +13, 0, +14, 0};

enum TimeServerCode {
    TSC_WIFI_NOT_CONNECTED_YET,
    TSC_WIFI_CONNECTED,
    TSC_WIFI_DIS_CONNECTED,
    TSC_WIFI_FAIL_NO_NETWORKS,
    TSC_WIFI_FAIL_SSID_NOT_FOUND,
    TSC_WIFI_FAIL_TIMEOUT,
};

enum TimeCode {
    TC_TIME_NOT_CONNECTED_YET,
    TC_TIME_UDP_NO_SOCKETS,
    TC_TIME_FAIL_CLIENT_CONN,
    TC_TIME_FAIL_RECEIVE,
    TC_TIME_FAIL_NOT_CONNECTED,
    TC_TIME_FAIL_TIMEOUT,
    TC_TIME_FAIL_INVALID,
    TC_TIME_FAIL_EMPTY_RESP,
    TC_TIME_FOUND
};

enum TimeFormat {
    TF_HH,
    TF_HHMM,
    TF_HHMMSS,
    TF_SEC
};

enum DSTState {
    DST_ON,      // Dalight Saving is ON according to worldtimeapi.org.
    DST_OFF,     // Dalight Saving is OFF according to worldtimeapi.org.
    DST_UNKNOWN  // Could not get data from worldtimeapi.org. This is why we need overrideDST.
};

enum TimeSource {
    TS_UNKNOWN,  // Not yet decided.
    TS_WWW,      // www.worldtime.com.
    TS_NTP       // Networ Time Protocol
};

class TimeStatus {
private:
    DateTime dateTime;
    char strTime[9];
    byte timeBytes[6];
    const char *getTimeSourceStr(TimeSource ts);
    const char *getDstStr(DSTState ts);
    const char *getDayOfMonthSuffix(int dom);
    int tzOffset;
    bool overrideDST;

public:
    TimeStatus();
    ~TimeStatus();
    TimeCode timeCode;
    DSTState dstState;
    TimeSource timeSource;
    TimeSource errorSource;
    int timeout;

    int timeSecEpoch;
    long timeMillisAdjust;
    bool unixtimeFound;
    int dayOfWeek;
    int dayOfMonth;
    int monthOfYear;
    const byte *updateDateTime();
    const char *getMessageStr(char *buffer);
    const char *getTimeStr(char *strTime, TimeFormat fmt);
    const char *getDayOfMonthStr(char *buffer);
    const char *getMonthStr();
    const char *getDayOfWeekStr();
    void setTimeZone(int tz);
    int getTimeZone();
    bool timeFailed();
    void setDSTOverriden(bool active);
    bool isDSTOverriden();
    bool isDstOn();
};

class TimeServer {
private:
    int serverTimeout;
    char readFileBuff[FILE_BUFF_LEN];
    byte ntpPacketBuffer[NTP_PACKET_LEN];
    bool matchResponsePrefix(const char with[]);
    String getResponseValue(const char with[]);
    void sendNTPpacket(IPAddress &address);

public:
    TimeServer();
    ~TimeServer();
    TimeServerCode serverCode;
    bool connect(char *ssid, char *pw, unsigned long timeoutMs);
    bool testForSsid(const char *ssid);
    void disconnect();
    bool isNotConnected();
    bool getTimeFromWWW(TimeStatus &timeStatus, unsigned int timeoutMs);
    bool getNTPTime(TimeStatus &timeStatus, const char *server, unsigned int timeoutMs);
    const char *getMessageStr(char *buffer);
};

#endif