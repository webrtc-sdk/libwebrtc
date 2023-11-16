#include "include/base/portable.h"

namespace portable {

static int strncpy_safe(char* dest, size_t numberOfElements, const char* src,
                        size_t count) {
  if (!count) return 0;
  if (!dest || !src || !numberOfElements) return -1;
  size_t end = count != _TRUNCATE && count < numberOfElements
                   ? count
                   : numberOfElements - 1;
  size_t i = 0;
  for (; i < end && src[i]; ++i) {
    dest[i] = src[i];
  }
  if (!src[i] || end == count || count == _TRUNCATE) {
    dest[i] = '\0';
    return 0;
  }
  dest[0] = '\0';
  return -1;
}

string::string() : m_dynamic(0), m_length(0) { m_buf[0] = 0; }

void string::init(const char* str, size_t len) {
  m_length = len;
  if (len < PORTABLE_STRING_BUF_SIZE) {
    strncpy_safe(m_buf, PORTABLE_STRING_BUF_SIZE, str, len);
    m_dynamic = 0;
  } else {
    m_dynamic = new char[len + 1];
    strncpy_safe(m_dynamic, len + 1, str, len);
  }
}

void string::destroy() {
  if (m_dynamic != 0) delete[] m_dynamic;
}

string ::~string() {
  if (m_dynamic != 0) delete[] m_dynamic;
}
}  // namespace portable