#include "jpeg_util.h"

#include <stdio.h>
#include <stdlib.h>

extern "C" {
#if defined(USE_SYSTEM_LIBJPEG)
#include <jpeglib.h>
#else
// Include directory supplied by gn
#include "jpeglib.h"  // NOLINT
#endif
}

namespace libwebrtc {

std::vector<unsigned char> EncodeRGBToJpeg(const unsigned char* rgb_buf,
                                           int width,
                                           int height,
                                           int color_planes,
                                           int quality) {
  std::vector<unsigned char> result;
  unsigned char* out_buffer = NULL;
  unsigned long out_size = 0;
  // Invoking LIBJPEG
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  jpeg_mem_dest(&cinfo, &out_buffer, &out_size);

  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = color_planes;
  cinfo.in_color_space = JCS_EXT_BGR;
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);

  jpeg_start_compress(&cinfo, TRUE);
  int row_stride = width * color_planes;
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] =
        &((unsigned char*)rgb_buf)[cinfo.next_scanline * row_stride];
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  result.resize(out_size);
  std::copy(out_buffer, out_buffer + out_size, result.begin());
  free(out_buffer);

  return result;
}

}  // namespace libwebrtc
