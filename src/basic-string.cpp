#include <cstring>
#include "basic-string.h"

String::String(const schar *buffer) : data(nullptr) {
    assign(buffer);
}

String::String(const String &other) : data(nullptr) {
    assign(other);
}

void String::assign(const schar *buffer) {
    if(data) {
        delete[] data;
    }
    auto length = StrLen(buffer);
    data = new schar[length];
    StrCpy(data, buffer);
}

String::~String() {
    delete[] data;
}