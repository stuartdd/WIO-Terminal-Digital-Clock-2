#include "properties.h"

Properties::Properties() {
}

Properties::~Properties() {
}

bool Properties::initFailed() {
    return initFail;
}

int Properties::readString(char *buffer, const char *propName, const char *defaultValue) {
    int count = readFileLineSD(buffer, propName);
    if (count == 0) {
        sprintf(buffer, "%s", defaultValue);
    }
    return count;
}

bool Properties::readBool(const char *propName) {
    long i = readNum(propName, 1);
    return (i != 0);
}

long Properties::readNum(const char *propName, int defaultValue) {
    int count = readFileLineSD(readFileBuffer, propName);
    if (count == 0) {
        return defaultValue;
    }
    long l = 0;
    int sign = 1;
    for (int i = 0; i < count; i++) {
        char c = readFileBuffer[i];
        if ((c == '-') && (l == 0)) {
            sign = -1;
        } else {
            if (c >= '0' && c <= '9') {
                l = l * 10;
                l = l + (c - 48);
            }
        }
    }
    l = l * sign;
    return l;
}

void Properties::writeString(const char *propName, const char *propValue) {
    writeFileSD(propName, propValue);
}

void Properties::writeNum(const char *propName, const long propValue) {
    ltoa(propValue, readFileBuffer, 10);
    writeFileSD(propName, readFileBuffer);
}

void Properties::writeBool(const char *propName, const bool propValue) {
    writeFileSD(propName, propValue ? "0" : "1");
}

//
// Write a String to a text file on the SSD.
// All files have a PROP_FILE_SUFFIX suffix so we just provide the name and the content.
//
void Properties::writeFileSD(const char *fileName, const char *content) {
    sprintf(fileNameBuff, "%s.%s", fileName, propFileNameSuffix);
    File f = SD.open(fileNameBuff, FILE_WRITE);
    f.println(content);
    f.close();
}

//
// Read a single line of text from a text file on the SSD.
// Returns the number of characters read.
// Reads a max of PROP_FILE_BUFF_LEN characters in to the readFileBuff[] buffer.
// Reads ONLY the first line. The rest of the file is discarded.
// Any chars < ' ' are considered EOL (End of Line)
//
int Properties::readFileLineSD(char *buffer, const char *fileName) {
    sprintf(fileNameBuff, "%s.%s", fileName, propFileNameSuffix);  //
    // Open the file.
    //
    File fil = SD.open(fileNameBuff, FILE_READ);
    if (fil) {
        //
        // Read each char in to the buffer untill full of an EOL char is found.
        // We always read the whole file!
        //
        int index = -1;
        bool notEol = true;
        while (fil.available()) {
            byte b = fil.read();
            if (b < 32) {
                notEol = false;
            }
            if (notEol) {
                index++;
                if (index < PROP_FILE_BUFF_LEN) {
                    buffer[index] = char(b);
                }
            }
        }
        //
        // Close the file and return the char count. If file is empty this will be 0.
        //
        fil.close();
        return index + 1;
    } else {
                    Serial.println(String(fileNameBuff) + ":" +String(fil) );
        //
        // Return 0 to indicate that no reading was possible.
        //
        return 0;
    }
}
