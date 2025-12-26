#ifndef utf8To_h
#define utf8To_h

#include <cstddef>  // size_t definícióhoz
#include "../../core/audiohelpers.h"

size_t strlen_utf8(const char *s);
String fixSlovakUTF8(const String &in);
char  *utf8To(const char *str, bool uppercase);

#endif