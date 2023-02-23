#ifndef INTERNAL_JPEG_UTIL_HXX
#define INTERNAL_JPEG_UTIL_HXX

#include <vector>
#include <inttypes.h>

namespace libwebrtc {
// Encodes the given RGB data into a JPEG image.
std::vector<unsigned char> EncodeRGBToJpeg(const unsigned char * data,
                                      int width,
                                      int height,
                                      int color_planes,
                                      int quality);
} // namespace libwebrtc

#endif //INTERNAL_JPEG_UTIL_HXX
