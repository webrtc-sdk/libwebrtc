// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Copyright (C) 2020 Intel Corporation


#ifndef OPENCV_GAPI_RENDER_TYPES_HPP
#define OPENCV_GAPI_RENDER_TYPES_HPP

#include <string>
#include <vector>

#include <opencv2/gapi/opencv_includes.hpp>
#include <opencv2/gapi/util/variant.hpp>
#include <opencv2/gapi/own/exports.hpp>

namespace cv
{
namespace gapi
{
namespace wip
{
namespace draw
{

/**
 * @brief This structure specifies which FreeType font to use by FText primitives.
 */
struct freetype_font
{
    /*@{*/
    std::string path; //!< The path to the font file (.ttf)
    /*@{*/
};

//! @addtogroup gapi_draw_prims
//! @{
/**
 * @brief This structure represents a text string to draw.
 *
 * Parameters match cv::putText().
 */
struct GAPI_EXPORTS_W_SIMPLE Text
{
    /**
     * @brief Text constructor
     *
     * @param text_               The text string to be drawn
     * @param org_                The bottom-left corner of the text string in the image
     * @param ff_                 The font type, see #HersheyFonts
     * @param fs_                 The font scale factor that is multiplied by the font-specific base size
     * @param color_              The text color
     * @param thick_              The thickness of the lines used to draw a text
     * @param lt_                 The line type. See #LineTypes
     * @param bottom_left_origin_ When true, the image data origin is at the bottom-left corner. Otherwise, it is at the top-left corner
     */
    GAPI_WRAP
    Text(const std::string& text_,
         const cv::Point& org_,
         int ff_,
         double fs_,
         const cv::Scalar& color_,
         int thick_ = 1,
         int lt_ = 8,
         bool bottom_left_origin_ = false) :
        text(text_), org(org_), ff(ff_), fs(fs_),
        color(color_), thick(thick_), lt(lt_), bottom_left_origin(bottom_left_origin_)
    {
    }

    GAPI_WRAP
    Text() = default;

    /*@{*/
    GAPI_PROP_RW std::string text;               //!< The text string to be drawn
    GAPI_PROP_RW cv::Point   org;                //!< The bottom-left corner of the text string in the image
    GAPI_PROP_RW int         ff;                 //!< The font type, see #HersheyFonts
    GAPI_PROP_RW double      fs;                 //!< The font scale factor that is multiplied by the font-specific base size
    GAPI_PROP_RW cv::Scalar  color;              //!< The text color
    GAPI_PROP_RW int         thick;              //!< The thickness of the lines used to draw a text
    GAPI_PROP_RW int         lt;                 //!< The line type. See #LineTypes
    GAPI_PROP_RW bool        bottom_left_origin; //!< When true, the image data origin is at the bottom-left corner. Otherwise, it is at the top-left corner
    /*@{*/
};

/**
 * @brief This structure represents a text string to draw using
 * FreeType renderer.
 *
 * If OpenCV is built without FreeType support, this primitive will
 * fail at the execution stage.
 */
struct FText
{
    /**
     * @brief FText constructor
     *
     * @param text_ The text string to be drawn
     * @param org_  The bottom-left corner of the text string in the image
     * @param fh_   The height of text
     * @param color_ The text color
     */
    FText(const std::wstring& text_,
          const cv::Point& org_,
          int fh_,
          const cv::Scalar& color_) :
        text(text_), org(org_), fh(fh_), color(color_)
    {
    }

    FText() = default;

    /*@{*/
    std::wstring text;              //!< The text string to be drawn
    cv::Point    org;               //!< The bottom-left corner of the text string in the image
    int          fh;                //!< The height of text
    cv::Scalar   color;             //!< The text color
    /*@{*/
};

/**
 * @brief This structure represents a rectangle to draw.
 *
 * Parameters match cv::rectangle().
 */
struct GAPI_EXPORTS_W_SIMPLE Rect
{
    /**
     * @brief Rect constructor
     *
     * @param rect_   Coordinates of the rectangle
     * @param color_  The bottom-left corner of the text string in the image
     * @param thick_  The thickness of lines that make up the rectangle. Negative values, like #FILLED, mean that the function has to draw a filled rectangle
     * @param lt_     The type of the line. See #LineTypes
     * @param shift_  The number of fractional bits in the point coordinates
     */
    Rect(const cv::Rect& rect_,
         const cv::Scalar& color_,
         int thick_ = 1,
         int lt_ = 8,
         int shift_ = 0) :
        rect(rect_), color(color_), thick(thick_), lt(lt_), shift(shift_)
    {
    }

    GAPI_WRAP
    Rect() = default;

    /*@{*/
    GAPI_PROP_RW cv::Rect   rect;  //!< Coordinates of the rectangle
    GAPI_PROP_RW cv::Scalar color; //!< The rectangle color or brightness (grayscale image)
    GAPI_PROP_RW int        thick; //!< The thickness of lines that make up the rectangle. Negative values, like #FILLED, mean that the function has to draw a filled rectangle
    GAPI_PROP_RW int        lt;    //!< The type of the line. See #LineTypes
    GAPI_PROP_RW int        shift; //!< The number of fractional bits in the point coordinates
    /*@{*/
};

/**
 * @brief This structure represents a circle to draw.
 *
 * Parameters match cv::circle().
 */
struct GAPI_EXPORTS_W_SIMPLE Circle
{
    /**
     * @brief Circle constructor
     *
     * @param  center_ The center of the circle
     * @param  radius_ The radius of the circle
     * @param  color_  The color of the  circle
     * @param  thick_  The thickness of the circle outline, if positive. Negative values, like #FILLED, mean that a filled circle is to be drawn
     * @param  lt_     The Type of the circle boundary. See #LineTypes
     * @param  shift_  The Number of fractional bits in the coordinates of the center and in the radius value
     */
    GAPI_WRAP
    Circle(const cv::Point& center_,
           int radius_,
           const cv::Scalar& color_,
           int thick_ = 1,
           int lt_ = 8,
           int shift_ = 0) :
        center(center_), radius(radius_), color(color_), thick(thick_), lt(lt_), shift(shift_)
    {
    }

    GAPI_WRAP
    Circle() = default;

    /*@{*/
    GAPI_PROP_RW cv::Point  center; //!< The center of the circle
    GAPI_PROP_RW int        radius; //!< The radius of the circle
    GAPI_PROP_RW cv::Scalar color;  //!< The color of the  circle
    GAPI_PROP_RW int        thick;  //!< The thickness of the circle outline, if positive. Negative values, like #FILLED, mean that a filled circle is to be drawn
    GAPI_PROP_RW int        lt;     //!< The Type of the circle boundary. See #LineTypes
    GAPI_PROP_RW int        shift;  //!< The Number of fractional bits in the coordinates of the center and in the radius value
    /*@{*/
};

/**
 * @brief This structure represents a line to draw.
 *
 * Parameters match cv::line().
 */
struct GAPI_EXPORTS_W_SIMPLE Line
{
    /**
     * @brief Line constructor
     *
     * @param  pt1_    The first point of the line segment
     * @param  pt2_    The second point of the line segment
     * @param  color_  The line color
     * @param  thick_  The thickness of line
     * @param  lt_     The Type of the line. See #LineTypes
     * @param  shift_  The number of fractional bits in the point coordinates
    */
    GAPI_WRAP
    Line(const cv::Point& pt1_,
         const cv::Point& pt2_,
         const cv::Scalar& color_,
         int thick_ = 1,
         int lt_ = 8,
         int shift_ = 0) :
        pt1(pt1_), pt2(pt2_), color(color_), thick(thick_), lt(lt_), shift(shift_)
    {
    }

    GAPI_WRAP
    Line() = default;

    /*@{*/
    GAPI_PROP_RW cv::Point  pt1;    //!< The first point of the line segment
    GAPI_PROP_RW cv::Point  pt2;    //!< The second point of the line segment
    GAPI_PROP_RW cv::Scalar color;  //!< The line color
    GAPI_PROP_RW int        thick;  //!< The thickness of line
    GAPI_PROP_RW int        lt;     //!< The Type of the line. See #LineTypes
    GAPI_PROP_RW int        shift;  //!< The number of fractional bits in the point coordinates
    /*@{*/
};

/**
 * @brief This structure represents a mosaicing operation.
 *
 * Mosaicing is a very basic method to obfuscate regions in the image.
 */
struct GAPI_EXPORTS_W_SIMPLE Mosaic
{
    /**
     * @brief Mosaic constructor
     *
     * @param mos_    Coordinates of the mosaic
     * @param cellSz_ Cell size (same for X, Y)
     * @param decim_  Decimation (0 stands for no decimation)
    */
    Mosaic(const cv::Rect& mos_,
           int cellSz_,
           int decim_) :
        mos(mos_), cellSz(cellSz_), decim(decim_)
    {
    }

    GAPI_WRAP
    Mosaic() : cellSz(0), decim(0) {}

    /*@{*/
    GAPI_PROP_RW cv::Rect mos;    //!< Coordinates of the mosaic
    GAPI_PROP_RW int      cellSz; //!< Cell size (same for X, Y)
    GAPI_PROP_RW int      decim;  //!< Decimation (0 stands for no decimation)
    /*@{*/
};

/**
 * @brief This structure represents an image to draw.
 *
 * Image is blended on a frame using the specified mask.
 */
struct GAPI_EXPORTS_W_SIMPLE Image
{
    /**
     * @brief Mosaic constructor
     *
     * @param  org_   The bottom-left corner of the image
     * @param  img_   Image to draw
     * @param  alpha_ Alpha channel for image to draw (same size and number of channels)
    */
    GAPI_WRAP
    Image(const cv::Point& org_,
          const cv::Mat& img_,
          const cv::Mat& alpha_) :
        org(org_), img(img_), alpha(alpha_)
    {
    }

    GAPI_WRAP
    Image() = default;

    /*@{*/
    GAPI_PROP_RW cv::Point org;   //!< The bottom-left corner of the image
    GAPI_PROP_RW cv::Mat   img;   //!< Image to draw
    GAPI_PROP_RW cv::Mat   alpha; //!< Alpha channel for image to draw (same size and number of channels)
    /*@{*/
};

/**
 * @brief This structure represents a polygon to draw.
 */
struct GAPI_EXPORTS_W_SIMPLE Poly
{
    /**
     * @brief Mosaic constructor
     *
     * @param points_ Points to connect
     * @param color_  The line color
     * @param thick_  The thickness of line
     * @param lt_     The Type of the line. See #LineTypes
     * @param shift_  The number of fractional bits in the point coordinate
    */
    GAPI_WRAP
    Poly(const std::vector<cv::Point>& points_,
         const cv::Scalar& color_,
         int thick_ = 1,
         int lt_ = 8,
         int shift_ = 0) :
        points(points_), color(color_), thick(thick_), lt(lt_), shift(shift_)
    {
    }

    GAPI_WRAP
    Poly() = default;

    /*@{*/
    GAPI_PROP_RW std::vector<cv::Point> points;  //!< Points to connect
    GAPI_PROP_RW cv::Scalar             color;   //!< The line color
    GAPI_PROP_RW int                    thick;   //!< The thickness of line
    GAPI_PROP_RW int                    lt;      //!< The Type of the line. See #LineTypes
    GAPI_PROP_RW int                    shift;   //!< The number of fractional bits in the point coordinate
    /*@{*/
};

using Prim  = util::variant
    < Text
    , FText
    , Rect
    , Circle
    , Line
    , Mosaic
    , Image
    , Poly
    >;

using Prims = std::vector<Prim>;
//! @} gapi_draw_prims

} // namespace draw
} // namespace wip
} // namespace gapi
} // namespace cv

#endif // OPENCV_GAPI_RENDER_TYPES_HPP
