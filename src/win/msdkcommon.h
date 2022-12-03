// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2.0

#ifndef OWT_BASE_WIN_MSDKCOMMON_H_
#define OWT_BASE_WIN_MSDKCOMMON_H_

#include <mutex>

#include <mfxdefs.h>
#include <mfxvideo++.h>
#include <mfxplugin++.h>
#include <mfxcommon.h>

#ifdef WEBRTC_WIN
#include <tchar.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#ifdef __cplusplus
#include <string>
#endif

namespace owt {
namespace base {
#ifndef MFX_VERSION
#define MFX_VERSION (MFX_VERSION_MAJOR * 1000 + MFX_VERSION_MINOR)
#endif


#define MSDK_STRING(x) _T(x)
#define MSDK_CHAR(x) _T(x)

#ifdef __cplusplus
typedef std::basic_string<TCHAR> msdk_tstring;
#endif
typedef TCHAR msdk_char;

#define msdk_printf   _tprintf
#define msdk_fprintf  _ftprintf
#define msdk_sprintf  _stprintf_s // to be removed
#define msdk_vprintf  _vtprintf
#define msdk_strlen   _tcslen
#define msdk_strcmp   _tcscmp
#define msdk_stricmp  _tcsicmp
#define msdk_strncmp  _tcsnicmp
#define msdk_strstr   _tcsstr
#define msdk_atoi     _ttoi
#define msdk_strtol   _tcstol
#define msdk_strtod   _tcstod
#define msdk_strchr   _tcschr
#define msdk_itoa_decimal(value, str)   _itow_s(value, str, 4, 10)
#define msdk_strnlen(str,lenmax) strnlen_s(str,lenmax)
#define msdk_sscanf _stscanf_s

// msdk_strcopy is intended to be used with 2 parmeters, i.e. msdk_strcopy(dst, src)
// for _tcscpy_s that's possible if DST is declared as: TCHAR DST[n];
#define msdk_strcopy _tcscpy_s
#define msdk_strncopy_s _tcsncpy_s

#define MSDK_DEC_WAIT_INTERVAL 300000
#define MSDK_ENC_WAIT_INTERVAL 300000
#define MSDK_VPP_WAIT_INTERVAL 300000
#define MSDK_SURFACE_WAIT_INTERVAL 20000
#define MSDK_DEVICE_FREE_WAIT_INTERVAL 30000
#define MSDK_WAIT_INTERVAL MSDK_DEC_WAIT_INTERVAL+3*MSDK_VPP_WAIT_INTERVAL+MSDK_ENC_WAIT_INTERVAL // an estimate for the longest pipeline we have in samples

#define MSDK_INVALID_SURF_IDX 0xFFFF

#define MSDK_SLEEP(msec) Sleep(msec)

#define MSDK_MEMCPY_BITSTREAM(bitstream, offset, src, count) memcpy_s((bitstream).Data + (offset), (bitstream).MaxLength - (offset), (src), (count))

#define MSDK_MEMCPY_BUF(bufptr, offset, maxsize, src, count) memcpy_s((bufptr)+ (offset), (maxsize) - (offset), (src), (count))

#define MSDK_MEMCPY_VAR(dstVarName, src, count) memcpy_s(&(dstVarName), sizeof(dstVarName), (src), (count))

#define MSDK_MEMCPY(dst, src, count) memcpy_s(dst, (count), (src), (count))


#define MSDK_SAFE_DELETE_ARRAY(P)                {if (P) {delete[] P; P = NULL;}}
#define MSDK_SAFE_RELEASE(X)                     {if (X) { X->Release(); X = NULL; }}
#define MSDK_SAFE_FREE(X)                        {if (X) { free(X); X = NULL; }}

#ifndef MSDK_SAFE_DELETE
#define MSDK_SAFE_DELETE(P)                      {if (P) {delete P; P = NULL;}}
#endif // MSDK_SAFE_DELETE

#define MSDK_ZERO_MEMORY(VAR)                    {memset(&VAR, 0, sizeof(VAR));}
#define MSDK_MAX(A, B)                           (((A) > (B)) ? (A) : (B))
#define MSDK_MIN(A, B)                           (((A) < (B)) ? (A) : (B))
#define MSDK_ALIGN16(value)                      (((value + 15) >> 4) << 4) // round up to a multiple of 16
#define MSDK_ALIGN32(value)                      (((value + 31) >> 5) << 5) // round up to a multiple of 32
#define MSDK_ALIGN(value, alignment)             (alignment) * ( (value) / (alignment) + (((value) % (alignment)) ? 1 : 0))
#define MSDK_ARRAY_LEN(value)                    (sizeof(value) / sizeof(value[0]))
#define ALIGN16(x) ((((x) + 15) >> 4) << 4)

enum class DumpType : int {
  MFX_DEC,
  MFX_VPP,
  MFX_ENC
};

struct MSDKEncConfiguration {
  MSDKEncConfiguration();
  MSDKEncConfiguration(const MSDKEncConfiguration&);

  bool operator ==(const MSDKEncConfiguration& o) const;
  bool operator !=(const MSDKEncConfiguration& o) const;
  // If platform supports, enable MFE (Multi-frame encoding)
  bool enable_mfe;
  // If platform supports, enable Adaptive LTR feature
  bool enable_altr;
  // Enable rolling i (a.k.a., Intra frame refresh)
  bool enable_rolling_i;
  // Enable HEVC GACC encoder plugin
  bool enable_hevc_gacc;
  // Enable HEVC SW encoder plugin
  bool enable_hevc_sw;
  // Enable FEI interface of MSDK
  bool enable_fei;
  // Enabling using QVBR as the BRC.
  bool enable_qvbr;
};
}
}
#endif  // OWT_BASE_WIN_MSDKCOMMON_H_