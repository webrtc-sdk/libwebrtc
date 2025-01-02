#ifndef LIB_WEBRTC_RTC_BASE_LIST_HXX
#define LIB_WEBRTC_RTC_BASE_LIST_HXX

#include "rtc_types.h"

namespace libwebrtc {

template <typename TKey, typename TVal>
class RTCBasePair : public RefCountInterface {
 private:
  TKey key_;
  TVal value_;

 public:
  RTCBasePair() : key_{}, value_{} {}

  RTCBasePair(const TKey key, const TVal value) : key_(key), value_(value) {}

  RTCBasePair(const std::pair<TKey, TVal> pair)
      : key_(pair.first), value_(pair.second) {}

  ~RTCBasePair() {}

  TKey& key() { return key_; }
  const TKey& key() const { return key_; }

  TVal& value() { return value_; }
  const TVal& value() const { return value_; }
}; // end class RTCBasePair

template <typename T>
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

  vector<T> to_vector() const { 
    if (m_count == 0) {
      return vector<T>();
    }
    else {
      T* items = new T[m_count];
      for (size_t i = 0; i < m_count; ++i) {
        items[i] = m_items[i];
      } 
      return vector<T>(items, m_count);
    }
  }

  ~RTCBaseList() { destroy_all(); }

 public:
  size_t count() const { return m_count; }

  T& item(size_t index) { return m_items[index]; }

  const T& item(size_t index) const { return m_items[index]; }

  void clear() { destroy_all(); }

  bool removeAt(size_t index) {
    if (index >= m_count) {
      return false;
    }
    destroy(&m_items[index]);
    m_count--;
    for (size_t i = index; i < m_count; i++) {
      m_items[i] = m_items[i + 1];
    }
  }

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
