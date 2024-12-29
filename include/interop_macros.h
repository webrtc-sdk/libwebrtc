#ifndef LIB_WEBRTC_INTROP_MACROS_HXX
#define LIB_WEBRTC_INTROP_MACROS_HXX

#define CHECK_POINTER_EX(p, r) \
  if ((p) == nullptr) {        \
    return (r);                \
  }

#define CHECK_POINTER(p) CHECK_POINTER_EX(p, rtcResultU4::kInvalidPointer)

#define RESET_OUT_POINTER_EX(p, v) \
  if ((p) != nullptr) {            \
    *(p) = (v);                    \
  }

#define RESET_OUT_POINTER(p) RESET_OUT_POINTER_EX(p, nullptr)

#define CHECK_OUT_POINTER(p)             \
  if ((p) != nullptr) {                  \
    *(p) = nullptr;                      \
  } else {                               \
    return rtcResultU4::kInvalidPointer; \
  }

#define CHECK_NATIVE_HANDLE(h) \
  CHECK_POINTER_EX(h, rtcResultU4::kInvalidNativeHandle)

#define ZERO_MEMORY(p, sz)                                     \
  if (static_cast<void*>(p) && static_cast<int>(sz) > 0) {     \
    memset(static_cast<void*>(p), 0, static_cast<size_t>(sz)); \
  }

#define CHECK_AND_ZERO_MEMORY(p, sz)     \
  CHECK_POINTER(p);                      \
  if (static_cast<int>(sz) < 1) {        \
    return rtcResultU4::kBufferTooSmall; \
  }                                      \
  ZERO_MEMORY(p, sz)

#define DECLARE_GET_STRING(handle, value, sz_value, class_name, fun_name) \
  CHECK_NATIVE_HANDLE(handle);                                            \
  CHECK_AND_ZERO_MEMORY(value, sz_value);                                 \
  scoped_refptr<class_name> p = static_cast<class_name*>(handle);         \
  string strValue = p->fun_name();                                        \
  size_t len = strValue.copy_to(value, static_cast<size_t>(sz_value));    \
  return strValue.size() > len ? rtcResultU4::kBufferTooSmall             \
                               : rtcResultU4::kSuccess;

#define DECLARE_GET_VALUE(handle, value, value_type_name, class_name, \
                          fun_name)                                   \
  CHECK_POINTER(value);                                               \
  *(value) = static_cast<value_type_name>(0);                         \
  CHECK_NATIVE_HANDLE(handle);                                        \
  scoped_refptr<class_name> p = static_cast<class_name*>(handle);     \
  *(value) = static_cast<value_type_name>(p->fun_name());             \
  return rtcResultU4::kSuccess;

#define DECLARE_SET_VALUE(handle, value, value_type_name, class_name, \
                          fun_name)                                   \
  CHECK_NATIVE_HANDLE(handle);                                        \
  scoped_refptr<class_name> p = static_cast<class_name*>(handle);     \
  p->fun_name(static_cast<value_type_name>(value));                   \
  return rtcResultU4::kSuccess;

#define DECLARE_GET_LIST_HANDLE(handle, value, value_type_name,               \
                                owner_class_name, list_class_name, fun_name)  \
  CHECK_OUT_POINTER(value);                                                   \
  CHECK_NATIVE_HANDLE(handle);                                                \
  scoped_refptr<owner_class_name> p = static_cast<owner_class_name*>(handle); \
  scoped_refptr<list_class_name> pList =                                      \
      list_class_name::Create(p->fun_name());                                 \
  if (pList == nullptr) {                                                     \
    return rtcResultU4::kUnknownError;                                        \
  }                                                                           \
  *pOutRetVal = static_cast<value_type_name>(pList.release());                \
  return rtcResultU4::kSuccess;

#define DECLARE_SET_LIST_HANDLE(handle, value, owner_class_name,               \
                                list_class_name, fun_name)                     \
  CHECK_NATIVE_HANDLE(handle);                                                 \
  scoped_refptr<owner_class_name> pOwner =                                     \
      static_cast<owner_class_name*>(handle);                                  \
  scoped_refptr<list_class_name> pList = static_cast<list_class_name*>(value); \
  pOwner->fun_name(pList->to_vector());                                        \
  return rtcResultU4::kSuccess;

#define DECLARE_LIST_GET_COUNT(handle, list_type_name)                        \
  CHECK_POINTER_EX(handle, 0);                                                \
  scoped_refptr<list_type_name> pList = static_cast<list_type_name*>(handle); \
  return static_cast<int>(pList->count());

#define DECLARE_LIST_GET_ITEM(handle, index, out_val, out_val_type_name,      \
                              list_type_name, item_type_name)                 \
  CHECK_OUT_POINTER(out_val);                                                 \
  CHECK_NATIVE_HANDLE(handle);                                                \
  if (static_cast<int>(index) < 0) {                                          \
    return rtcResultU4::kOutOfRange;                                          \
  }                                                                           \
  scoped_refptr<list_type_name> pList = static_cast<list_type_name*>(handle); \
  if (static_cast<size_t>(index) >= pList->count()) {                         \
    return rtcResultU4::kOutOfRange;                                          \
  }                                                                           \
  scoped_refptr<item_type_name> pItem =                                       \
      pList->item(static_cast<size_t>(index));                                \
  *(out_val) = static_cast<out_val_type_name>(pItem.release());               \
  return rtcResultU4::kSuccess;

#endif  // LIB_WEBRTC_INTROP_MACROS_HXX
