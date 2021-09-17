#include "include/base/portable.h"

namespace portable {
string::string() : m_dynamic(0), m_length(0) {
  m_buf[0] = 0;
}

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
  if (m_dynamic != 0)
    delete[] m_dynamic;
}

string ::~string() {
  if (m_dynamic != 0)
    delete[] m_dynamic;
}
}  // namespace portable