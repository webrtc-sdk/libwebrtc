#ifndef WEBRTC_CONFIG_H_
#define WEBRTC_CONFIG_H_

#ifdef SHARED_BUILD

#ifdef LIB_WEBRTC_API_EXPORTS
#define LIB_WEBRTC_API __declspec(dllexport)
#elif defined(LIB_WEBRTC_API_DLL)
#define LIB_WEBRTC_API __declspec(dllimport)
#elif !defined(WIN32)
#define LIB_WEBRTC_API __attribute__((visibility("default")))
#else
#define LIB_WEBRTC_API
#endif

#else

#define LIB_WEBRTC_API 

#endif

#endif // WEBRTC_CONFIG_H_