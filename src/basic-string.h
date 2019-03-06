// Header name is just to avoid conflicts with standard string.h
#ifndef _INC_BASIC_STRING_H
#define _INC_BASIC_STRING_H

#include "efi-cpp.h"
#include <cstring>

class String {
public:
    typedef CHAR16 schar;

    String(const schar *buffer);
    String(const String &other);
    ~String();

    size_t length() const { return StrLen(data); }

    operator const schar *() const { return data; }

private:
    void construct(const schar *buffer);

    schar *data;
};

#endif//_INC_BASIC_STRING_H