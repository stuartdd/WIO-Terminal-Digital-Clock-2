/*
 * Copyright (C) 2021 Stuart Davies
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * TimeServer.h
 * TimeServer.cpp
 *
 * Created on 18 January 2021
 *
 * @file TimeServer.cpp
*/

#include "TimeServer.h"

static WiFiUDP udp;
static WiFiClient client;

TimeStatus::TimeStatus() {
    timeCode = TC_TIME_NOT_CONNECTED_YET;
    unixtimeFound = false;
    dstState = DST_UNKNOWN;
    timeSource = TS_UNKNOWN;
    errorSource = TS_UNKNOWN;
    tzOffset = TZ_PAIRS_GMT;
    overrideDST = false;
    dayOfWeek = 0;
    dayOfMonth = 0;
    monthOfYear = 1;
}

TimeStatus::~TimeStatus() {
}

const char *TimeStatus::getTimeSourceStr(TimeSource ts) {
    switch (ts) {
    case TS_WWW:
        return "WWW";
    case TS_NTP:
        return "NTP";
    default:
        return "UNK";
    }
}

const char *TimeStatus::getDstStr(DSTState dst) {
    switch (dst) {
    case DST_ON:
        return "+";
    case DST_OFF:
        return "-";
    default:
        return "?";
    }
}

const char *TimeStatus::getMonthStr() {
    switch (monthOfYear) {
    case 1:
        return "Jan";
    case 2:
        return "Feb";
    case 3:
        return "Mar";
    case 4:
        return "Apr";
    case 5:
        return "May";
    case 6:
        return "Jun";
    case 7:
        return "Jul";
    case 8:
        return "Aug";
    case 9:
        return "Sep";
    case 10:
        return "Oct";
    case 11:
        return "Nov";
    default:
        return "Dec";
    }
}

const char *TimeStatus::getDayOfMonthSuffix(int dom) {
    if ((dom == 11) || (dom == 12) || (dom == 12)) {
        return "th";
    }
    switch (dom % 10) {
    case 1:
        return "st";
    case 2:
        return "nd";
    case 3:
        return "rd";
    }
    return "th";
}

const char *TimeStatus::getDayOfWeekStr() {
    switch (dayOfWeek) {
    case 0:
        return "Sunday";
    case 1:
        return "Monday";
    case 2:
        return "Tuesday";
    case 3:
        return "Wednesday";
    case 4:
        return "Thursday";
    case 5:
        return "Friday";
    default:
        return "Saturday";
    }
}

bool TimeStatus::timeFailed() {
    return timeCode != TC_TIME_FOUND;
}

const char *TimeStatus::getDayOfMonthStr(char *buffer) {
    sprintf(buffer, "%d%s", dayOfMonth, getDayOfMonthSuffix(dayOfMonth));
    return buffer;
}

const char *TimeStatus::getMessageStr(char *buffer) {
    switch (timeCode) {
    case TC_TIME_NOT_CONNECTED_YET:
        sprintf(buffer, "%s:TIME[%i]:Not Connected yet", getTimeSourceStr(errorSource), timeout);
        break;
    case TC_TIME_FAIL_RECEIVE:
        sprintf(buffer, "%s:TIME[%i]:FAIL:Receive", getTimeSourceStr(errorSource), timeout);
        break;
    case TC_TIME_FAIL_NOT_CONNECTED:
        sprintf(buffer, "%s:TIME[%i]:FAIL:No Connection", getTimeSourceStr(errorSource), timeout);
        break;
    case TC_TIME_UDP_NO_SOCKETS:
        sprintf(buffer, "%s:TIME[%i]:FAIL:No UDP sockets", getTimeSourceStr(errorSource), timeout);
        break;
    case TC_TIME_FAIL_CLIENT_CONN:
        sprintf(buffer, "%s:TIME[%i]:FAIL:Client to Host", getTimeSourceStr(errorSource), timeout);
        break;
    case TC_TIME_FAIL_TIMEOUT:
        sprintf(buffer, "%s:TIME[%i]:FAIL:With TIMEOUT", getTimeSourceStr(errorSource), timeout);
        break;
    case TC_TIME_FAIL_INVALID:
        sprintf(buffer, "%s:TIME[%i]:INVALID Response", getTimeSourceStr(errorSource), timeout);
        break;
    case TC_TIME_FAIL_EMPTY_RESP:
        sprintf(buffer, "%s:TIME[%i]:EMPTY Response", getTimeSourceStr(errorSource), timeout);
        break;
    case TC_TIME_FOUND:
        sprintf(buffer, "%s:OK[%i]:DST:%s", getTimeSourceStr(timeSource), timeout, getDstStr(dstState));
        break;
    }
    return buffer;
}

bool TimeStatus::isDstOn() {
    return dstState == DST_ON;
}

void TimeStatus::setDSTOverriden(bool active) {
    overrideDST = active;
}

bool TimeStatus::isDSTOverriden() {
    return overrideDST;
}

const byte *TimeStatus::updateDateTime() {
    long tim = 0;
    if (unixtimeFound) {
        tim = timeSecEpoch;
        if (isDstOn() || isDSTOverriden()) {
            tim = (timeSecEpoch + SEC_IN_HOUR);
        }
        tim = tim + ((tZonePairs[tzOffset * 2] * SEC_IN_HOUR) + (tZonePairs[(tzOffset * 2) + 1] * SEC_IN_MIN));
        tim = tim + ((millis() - timeMillisAdjust) / 1000);
        dateTime = DateTime(tim);
    }
    int hh = dateTime.hour();
    int mm = dateTime.minute();
    int ss = dateTime.second();
    timeBytes[0] = (hh / 10);
    timeBytes[1] = (hh % 10);
    timeBytes[2] = (mm / 10);
    timeBytes[3] = (mm % 10);
    timeBytes[4] = (ss / 10);
    timeBytes[5] = (ss % 10);
    dayOfWeek = dateTime.dayOfTheWeek();
    dayOfMonth = dateTime.day();
    monthOfYear = dateTime.month();
    return timeBytes;
}

void TimeStatus::setTimeZone(int tz) {
    if ((tz < 0) || (tz >= TZ_PAIRS_MAX)) {
        tzOffset = TZ_PAIRS_GMT;
        return;
    }
    tzOffset = tz;
}

int TimeStatus::getTimeZone() {
    return tzOffset;
}

const char *TimeStatus::getTimeStr(char *strTime, TimeFormat fmt) {
    if (!unixtimeFound) {
        switch (fmt) {
        case TF_HHMMSS:
            return "00:00:00";
        case TF_HHMM:
            return "00:00";
        default:  // Return shortest response TF_HH, TF_SEC
            return "00";
        }
    }
    updateDateTime();

    switch (fmt) {
    case TF_HHMMSS:
        strTime[0] = CHAR_0 + timeBytes[0];
        strTime[1] = CHAR_0 + timeBytes[1];
        strTime[2] = ':';
        strTime[3] = CHAR_0 + timeBytes[2];
        strTime[4] = CHAR_0 + timeBytes[3];
        strTime[5] = ':';
        strTime[6] = CHAR_0 + timeBytes[4];
        strTime[7] = CHAR_0 + timeBytes[5];
        strTime[8] = 0;
        break;
    case TF_HHMM:
        strTime[0] = CHAR_0 + timeBytes[0];
        strTime[1] = CHAR_0 + timeBytes[1];
        strTime[2] = ':';
        strTime[3] = CHAR_0 + timeBytes[2];
        strTime[4] = CHAR_0 + timeBytes[3];
        strTime[5] = 0;
        break;
    case TF_HH:
        strTime[0] = CHAR_0 + timeBytes[0];
        strTime[1] = CHAR_0 + timeBytes[1];
        strTime[2] = 0;
        break;
    default:  // Return shortest response TF_SEC
        strTime[0] = CHAR_0 + timeBytes[4];
        strTime[1] = CHAR_0 + timeBytes[5];
        strTime[2] = 0;
    }
    return strTime;
}

const char *TimeServer::getMessageStr(char *buffer) {
    switch (serverCode) {
    case TSC_WIFI_CONNECTED:
        sprintf(buffer, "WIFI:[%i]:IP %d.%d.%d.%d", serverTimeout, WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
        break;
    case TSC_WIFI_NOT_CONNECTED_YET:
        sprintf(buffer, "WIFI:[%i]:FAIL:Not Connected yet", serverTimeout);
        break;
    case TSC_WIFI_DIS_CONNECTED:
        sprintf(buffer, "WIFI:[%i]:FAIL:Dis-connected", serverTimeout);
        break;
    case TSC_WIFI_FAIL_SSID_NOT_FOUND:
        sprintf(buffer, "WIFI:[%i]:FAIL:SSID Not found", serverTimeout);
        break;
    case TSC_WIFI_FAIL_NO_NETWORKS:
        sprintf(buffer, "WIFI:[%i]:FAIL:No networks found", serverTimeout);
        break;
    case TSC_WIFI_FAIL_TIMEOUT:
        sprintf(buffer, "WIFI:[%i]:Timeout:", serverTimeout);
        break;
    }
    return buffer;
}

TimeServer::TimeServer() {
    serverCode = TSC_WIFI_NOT_CONNECTED_YET;
    serverTimeout = 0;
}

TimeServer::~TimeServer() {
}

bool TimeServer::connect(char *ssid, char *pw, unsigned long timeoutMs) {
    unsigned long startTime = millis();
    serverTimeout = 0;
    serverCode = TSC_WIFI_NOT_CONNECTED_YET;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    WiFi.begin(ssid, pw);
    unsigned long endTime = millis() + timeoutMs;
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() > endTime) {
            serverCode = TSC_WIFI_FAIL_TIMEOUT;
            serverTimeout = millis() - startTime;
            return false;
        }
        delay(100);
    }
    serverCode = TSC_WIFI_CONNECTED;
    serverTimeout = millis() - startTime;
    delay(100);
    return true;
}

void TimeServer::disconnect() {
    serverCode = TSC_WIFI_NOT_CONNECTED_YET;
    WiFi.disconnect();
    delay(100);
}

bool TimeServer::isNotConnected() {
    if (WiFi.status() != WL_CONNECTED) {
        serverCode = TSC_WIFI_DIS_CONNECTED;
    }
    return serverCode != TSC_WIFI_CONNECTED;
}

bool TimeServer::getTimeFromWWW(TimeStatus &timeStatus, unsigned int timeoutMs) {
    timeStatus.errorSource = TS_WWW;
    timeStatus.timeout = 0;
    unsigned long startTime = millis();

    if (isNotConnected()) {
        timeStatus.timeCode = TC_TIME_FAIL_NOT_CONNECTED;
        timeStatus.timeout = millis() - startTime;
        return false;
    }

    int conCount = 5;
    while (!client.connect(timeDataHost, 80)) {
        conCount--;
        if (conCount == 0) {
            //
            // Could not connect so return false;
            //
            timeStatus.timeCode = TC_TIME_FAIL_CLIENT_CONN;
            timeStatus.timeout = millis() - startTime;
            return false;
        }
        delay(50);
    }

    unsigned long endTime = millis() + timeoutMs;
    client.printf("GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", timeDataUrl, timeDataHost);
    while (client.available() == 0) {
        if (millis() > endTime) {
            client.stop();
            timeStatus.timeCode = TC_TIME_FAIL_TIMEOUT;
            timeStatus.timeout = millis() - startTime;
            return false;
        }
    }
    int count = 0;
    int pos = 0;
    // Read all the lines of the reply from server
    while (client.available()) {
        byte b = client.read();
        if (b < 0) {
            //
            // A -1 is returned for the end of the file.
            //
            break;
        }
        count++;
        if (b < 32) {
            readFileBuff[pos] = 0;
            if (pos > 0) {
                if (matchResponsePrefix(dstResponseMatch)) {
                    //
                    // We found 'dst: ' so get the data value 'true' or 'false'
                    //
                    timeStatus.dstState = (getResponseValue(dstResponseMatch) == "true" ? DST_ON : DST_OFF);
                }
                if (matchResponsePrefix(unixtimeResponseMatch)) {
                    //
                    // We found 'unixtime: ' so get the data value, the number of seconds since the epoch
                    //
                    long tmp = getResponseValue(unixtimeResponseMatch).toInt();
                    if (tmp > 0) {
                        timeStatus.unixtimeFound = true;
                        timeStatus.timeSource = TS_WWW;
                        timeStatus.timeSecEpoch = tmp;
                        timeStatus.timeMillisAdjust = millis();
                        timeStatus.timeCode = TC_TIME_FOUND;
                    } else {
                        //
                        // The unixtime value is invalid so return false.
                        //
                        timeStatus.timeCode = TC_TIME_FAIL_INVALID;
                    }
                }
            }
            pos = 0;
        } else {
            if (pos < FILE_BUFF_LEN) {
                //
                // Only add chars if we are NOT past the end of the data buffer.
                //
                readFileBuff[pos] = b;
            }
            pos++;
        }
    }
    client.stop();
    if (count == 0) {
        timeStatus.timeCode = TC_TIME_FAIL_EMPTY_RESP;
    }
    timeStatus.timeout = millis() - startTime;
    return timeStatus.timeCode == TC_TIME_FOUND;
}

String TimeServer::getResponseValue(const char with[]) {
    char c[20];
    int pos = with[0];
    int cPos = 0;
    for (int i = 0; i < 19; i++) {
        char ch = readFileBuff[pos + i];
        if (ch <= ' ') {
            c[cPos] = 0;
            return String(c);
        }
        c[cPos] = ch;
        cPos++;
    }
    c[cPos] = 0;
    return String(c);
}

bool TimeServer::matchResponsePrefix(const char with[]) {
    for (int i = 1; i <= with[0]; i++) {
        if (readFileBuff[i - 1] != with[i]) {
            return false;
        }
    }
    return true;
}

bool TimeServer::testForSsid(const char *ssid) {
    int n = WiFi.scanNetworks();
    if (n == 0) {
        serverCode = TSC_WIFI_FAIL_NO_NETWORKS;
        return false;
    } else {
        for (int i = 0; i < n; ++i) {
            if (WiFi.SSID(i) == ssid) {
                return true;
            }
            delay(10);
        }
    }
    serverCode = TSC_WIFI_FAIL_SSID_NOT_FOUND;
    return false;
}

/*
   Get the NTP time in seconds from the EPOC.

   0 Indicates failure. See the logs!

   Positive numbers are assumed to be the seconds since the EPOCH.
*/
bool TimeServer::getNTPTime(TimeStatus &timeStatus, const char *server, unsigned int timeoutMs) {
    timeStatus.errorSource = TS_NTP;
    timeStatus.timeout = 0;
    unsigned long startTime = millis();

    if (isNotConnected()) {
        timeStatus.timeCode = TC_TIME_FAIL_NOT_CONNECTED;
        timeStatus.timeout = millis() - startTime;
        return false;
    }

    int n = udp.begin(NTP_LOCAL_PORT);
    if (n == 0) {
        timeStatus.timeCode = TC_TIME_UDP_NO_SOCKETS;
        timeStatus.timeout = millis() - startTime;
        return false;
    }

    while (udp.parsePacket() > 0)
        ;
    //initializes the UDP state
    //This initializes the transfer buffer
    IPAddress timeServerIP;

    WiFi.hostByName(server, timeServerIP);

    sendNTPpacket(timeServerIP);  // send an NTP packet to a time server
                                  //    Serial.println("NTP:5 " + (String(WiFi.status())));

    // wait to see if a reply is available
    delay(100);
    uint32_t endTime = millis() + timeoutMs;
    while (millis() < endTime) {
        if (udp.parsePacket() >= NTP_PACKET_LEN) {
            // We've received a packet, read the data from it
            udp.read(ntpPacketBuffer, NTP_PACKET_LEN);  // read the packet into the buffer
            unsigned long highWord = word(ntpPacketBuffer[40], ntpPacketBuffer[41]);
            unsigned long lowWord = word(ntpPacketBuffer[42], ntpPacketBuffer[43]);
            // combine the four bytes (two words) into a long integer
            // this is NTP time (seconds since Jan 1 1900):
            unsigned long secsSince1900 = highWord << 16 | lowWord;
            // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
            const unsigned long seventyYears = 2208988800UL;
            // subtract seventy years:
            timeStatus.timeSecEpoch = secsSince1900 - seventyYears;
            timeStatus.timeMillisAdjust = millis();
            timeStatus.unixtimeFound = true;
            timeStatus.timeSource = TS_NTP;
            timeStatus.timeCode = TC_TIME_FOUND;
            timeStatus.timeout = millis() - startTime;
            return true;
        }
        delay(10);
    }
    timeStatus.timeCode = TC_TIME_FAIL_TIMEOUT;
    timeStatus.timeout = millis() - startTime;
    return false;
}

// send an NTP request to the time server at the given address
void TimeServer::sendNTPpacket(IPAddress &address) {
    // set all bytes in thTimeServer::e buffer to 0
    for (int i = 0; i < NTP_PACKET_LEN; ++i) {
        ntpPacketBuffer[i] = 0;
    }
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    ntpPacketBuffer[0] = 0b11100011;  // LI, Version, Mode
    ntpPacketBuffer[1] = 0;           // Stratum, or type of clock
    ntpPacketBuffer[2] = 6;           // Polling Interval
    ntpPacketBuffer[3] = 0xEC;        // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    ntpPacketBuffer[12] = 49;
    ntpPacketBuffer[13] = 0x4E;
    ntpPacketBuffer[14] = 49;
    ntpPacketBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123);  //NTP requests are to port 123
    udp.write(ntpPacketBuffer, NTP_PACKET_LEN);
    delay(100);
    udp.endPacket();
}
