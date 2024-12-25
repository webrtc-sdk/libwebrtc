#ifndef LIB_WEBRTC_RTC_BASE_LIST_HXX
#define LIB_WEBRTC_RTC_BASE_LIST_HXX

#include "rtc_types.h"

namespace libwebrtc {

template <class T>
class RTCBaseList : public RefCountInterface {
 protected:
  using raw_type = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

 private:
  T* m_items;
  size_t m_count;

 public:
  RTCBaseList() : m_items(0), m_count(0) {}

  RTCBaseList(const vector<T>& o) : m_items(0), m_count(0) {
    m_count = o.size();
    if (m_count != 0) {
      m_items = new T[m_count];
      for (size_t i = 0; i < m_count; ++i) {
        m_items[i] = o[i];
      }
    }
  }

  vector<T> to_vector() const { return vector<T>(m_items, m_count); }

  ~RTCBaseList() { destroy_all(); }

 public:
  size_t count() const { return m_count; }

  T& item(size_t index) { return m_items[index]; }

  const T& item(size_t index) const { return m_items[index]; }

  void clear() { destroy_all(); }

 protected:
  void destroy(T* rt) { reinterpret_cast<const T*>(rt)->~T(); }

  void destroy_all() {
    for (size_t i = 0; i < m_count; ++i) {
      destroy(&m_items[i]);
    }
    m_items = 0;
    m_count = 0;
  }
};  // end class RTCBaseList

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_BASE_LIST_HXX
