#include "base/string.h"

namespace libwebrtc {

string::string() {
  length_ = 0;
  data_ = new char[0];
}

string::string(const char* data, unsigned size) {
  length_ = size;
  data_ = new char[length_ + 1];
  for (unsigned j = 0; j < size; j++)
    data_[j] = data[j];
  data_[length_] = '\0';
}

string::string(const char* c) {
  if (c) {
    unsigned n = 0;
    while (c[n] != '\0')
      n++;
    length_ = n;
    data_ = new char[n + 1];
    for (unsigned j = 0; j < n; j++)
      data_[j] = c[j];
    data_[length_] = '\0';
  } else {
    length_ = 0;
    data_ = new char[0];
  }
}

string::string(const string& s) {
  length_ = s.size();
  data_ = new char[length_];
  for (unsigned j = 0; j < length_; j++)
    data_[j] = s[j];
}

string::~string() {
  delete[] data_;
}


 char string::operator[](unsigned j) const {
#if 0
    if (j >= length)
      throw 1;
#endif
   return data_[j];
 }

char& string::operator[](unsigned j) {
#if 0
    if (j >= length)
      throw 1;
#endif
  return data_[j];
}

string& string::operator=(const string& s) {
  if (this == &s)
    return *this;

  delete data_;
  length_ = s.size();
  data_ = new char[length_];
  for (unsigned j = 0; j < length_; j++)
    data_[j] = s[j];
  return *this;
}

string& string::operator+=(const string& s) {
  unsigned len = length_ + s.size();
  char* str = new char[len];

  for (unsigned j = 0; j < length_; j++)
    str[j] = data_[j];

  for (unsigned i = 0; i < s.size(); i++)
    str[length_ + i] = s[i];

  delete data_;
  length_ = len;
  data_ = str;
  return *this;
}

}  // namespace libwebrtc