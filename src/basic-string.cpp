#include <cstring>
#include "basic-string.h"

void String::construct(const schar *buffer) {
    size_t length = StrLen(buffer);
    data = new schar[length];
    StrCpy(data, buffer);
}

String::String(const schar *buffer) {
    construct(buffer);
}

String::String(const String &other) {
    construct(other);
}

String::~String() {
    delete[] data;
}