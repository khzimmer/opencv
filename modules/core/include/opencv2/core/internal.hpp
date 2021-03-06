/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

/* The header is for internal use and it is likely to change.
   It contains some macro definitions that are used in cxcore, cv, cvaux
   and, probably, other libraries. If you need some of this functionality,
   the safe way is to copy it into your code and rename the macros.
*/
#ifndef __OPENCV_CORE_INTERNAL_HPP__
#define __OPENCV_CORE_INTERNAL_HPP__

#include <vector>

#if defined WIN32 || defined _WIN32
#  ifndef WIN32
#    define WIN32
#  endif
#  ifndef _WIN32
#    define _WIN32
#  endif
#endif

#if !defined WIN32 && !defined WINCE
#  include <pthread.h>
#endif

#ifdef __BORLANDC__
#  ifndef WIN32
#    define WIN32
#  endif
#  ifndef _WIN32
#    define _WIN32
#  endif
#  define CV_DLL
#  undef _CV_ALWAYS_PROFILE_
#  define _CV_ALWAYS_NO_PROFILE_
#endif

#ifndef FALSE
#  define FALSE 0
#endif
#ifndef TRUE
#  define TRUE 1
#endif

#define __BEGIN__ __CV_BEGIN__
#define __END__ __CV_END__
#define EXIT __CV_EXIT__

#ifdef HAVE_IPP
#  include "ipp.h"

CV_INLINE IppiSize ippiSize(int width, int height)
{
    IppiSize size = { width, height };
    return size;
}
#endif

#ifndef IPPI_CALL
#  define IPPI_CALL(func) CV_Assert((func) >= 0)
#endif

#if defined __SSE2__ || defined _M_X64  || (defined _M_IX86_FP && _M_IX86_FP >= 2)
#  include "emmintrin.h"
#  define CV_SSE 1
#  define CV_SSE2 1
#  if defined __SSE3__ || (defined _MSC_VER && _MSC_VER >= 1500)
#    include "pmmintrin.h"
#    define CV_SSE3 1
#  endif
#  if defined __SSSE3__  || (defined _MSC_VER && _MSC_VER >= 1500)
#    include "tmmintrin.h"
#    define CV_SSSE3 1
#  endif
#  if defined __SSE4_1__ || (defined _MSC_VER && _MSC_VER >= 1500)
#    include <smmintrin.h>
#    define CV_SSE4_1 1
#  endif
#  if defined __SSE4_2__ || (defined _MSC_VER && _MSC_VER >= 1500)
#    include <nmmintrin.h>
#    define CV_SSE4_2 1
#  endif
#  if defined __AVX__ || (defined _MSC_FULL_VER && _MSC_FULL_VER >= 160040219)
// MS Visual Studio 2010 (2012?) has no macro pre-defined to identify the use of /arch:AVX
// See: http://connect.microsoft.com/VisualStudio/feedback/details/605858/arch-avx-should-define-a-predefined-macro-in-x64-and-set-a-unique-value-for-m-ix86-fp-in-win32
#    include <immintrin.h>
#    define CV_AVX 1
#    if defined(_XCR_XFEATURE_ENABLED_MASK)
#      define __xgetbv() _xgetbv(_XCR_XFEATURE_ENABLED_MASK)
#    else
#      define __xgetbv() 0
#    endif
#  endif
#endif

#ifdef __ARM_NEON__
#  include <arm_neon.h>
#  define CV_NEON 1
#  define CPU_HAS_NEON_FEATURE (true)
#endif

#ifndef CV_SSE
#  define CV_SSE 0
#endif
#ifndef CV_SSE2
#  define CV_SSE2 0
#endif
#ifndef CV_SSE3
#  define CV_SSE3 0
#endif
#ifndef CV_SSSE3
#  define CV_SSSE3 0
#endif
#ifndef CV_SSE4_1
#  define CV_SSE4_1 0
#endif
#ifndef CV_SSE4_2
#  define CV_SSE4_2 0
#endif
#ifndef CV_AVX
#  define CV_AVX 0
#endif
#ifndef CV_NEON
#  define CV_NEON 0
#endif

#ifdef HAVE_TBB
#  include "tbb/tbb_stddef.h"
#  if TBB_VERSION_MAJOR*100 + TBB_VERSION_MINOR >= 202
#    include "tbb/tbb.h"
#    include "tbb/task.h"
#    undef min
#    undef max
#  else
#    undef HAVE_TBB
#  endif
#endif

#ifdef HAVE_EIGEN
#  if defined __GNUC__ && defined __APPLE__
#    pragma GCC diagnostic ignored "-Wshadow"
#  endif
#  include <Eigen/Core>
#  include "opencv2/core/eigen.hpp"
#endif

#ifdef __cplusplus

namespace cv
{
#ifdef HAVE_TBB

    typedef tbb::blocked_range<int> BlockedRange;

    template<typename Body> static inline
    void parallel_for( const BlockedRange& range, const Body& body )
    {
        tbb::parallel_for(range, body);
    }

    template<typename Iterator, typename Body> static inline
    void parallel_do( Iterator first, Iterator last, const Body& body )
    {
        tbb::parallel_do(first, last, body);
    }

    typedef tbb::split Split;

    template<typename Body> static inline
    void parallel_reduce( const BlockedRange& range, Body& body )
    {
        tbb::parallel_reduce(range, body);
    }

    typedef tbb::concurrent_vector<Rect> ConcurrentRectVector;
    typedef tbb::concurrent_vector<double> ConcurrentDoubleVector;
#else
    class BlockedRange
    {
    public:
        BlockedRange() : _begin(0), _end(0), _grainsize(0) {}
        BlockedRange(int b, int e, int g=1) : _begin(b), _end(e), _grainsize(g) {}
        int begin() const { return _begin; }
        int end() const { return _end; }
        int grainsize() const { return _grainsize; }

    protected:
        int _begin, _end, _grainsize;
    };

    template<typename Body> static inline
    void parallel_for( const BlockedRange& range, const Body& body )
    {
        body(range);
    }
    typedef std::vector<Rect> ConcurrentRectVector;
    typedef std::vector<double> ConcurrentDoubleVector;

    template<typename Iterator, typename Body> static inline
    void parallel_do( Iterator first, Iterator last, const Body& body )
    {
        for( ; first != last; ++first )
            body(*first);
    }

    class Split {};

    template<typename Body> static inline
    void parallel_reduce( const BlockedRange& range, Body& body )
    {
        body(range);
    }
#endif
} //namespace cv

#define CV_INIT_ALGORITHM(classname, algname, memberinit) \
    static ::cv::Algorithm* create##classname##_hidden() \
    { \
        return new classname; \
    } \
    \
    static ::cv::AlgorithmInfo& classname##_info() \
    { \
        static ::cv::AlgorithmInfo classname##_info_var(algname, create##classname##_hidden); \
        return classname##_info_var; \
    } \
    \
    static ::cv::AlgorithmInfo& classname##_info_auto = classname##_info(); \
    \
    ::cv::AlgorithmInfo* classname::info() const \
    { \
        static volatile bool initialized = false; \
        \
        if( !initialized ) \
        { \
            initialized = true; \
            classname obj; \
            memberinit; \
        } \
        return &classname##_info(); \
    }

#endif //__cplusplus

/* maximal size of vector to run matrix operations on it inline (i.e. w/o ipp calls) */
#define  CV_MAX_INLINE_MAT_OP_SIZE  10

/* maximal linear size of matrix to allocate it on stack. */
#define  CV_MAX_LOCAL_MAT_SIZE  32

/* maximal size of local memory storage */
#define  CV_MAX_LOCAL_SIZE  \
    (CV_MAX_LOCAL_MAT_SIZE*CV_MAX_LOCAL_MAT_SIZE*(int)sizeof(double))

/* default image row align (in bytes) */
#define  CV_DEFAULT_IMAGE_ROW_ALIGN  4

/* matrices are continuous by default */
#define  CV_DEFAULT_MAT_ROW_ALIGN  1

/* maximum size of dynamic memory buffer.
   cvAlloc reports an error if a larger block is requested. */
#define  CV_MAX_ALLOC_SIZE    (((size_t)1 << (sizeof(size_t)*8-2)))

/* the alignment of all the allocated buffers */
#define  CV_MALLOC_ALIGN    16

/* default alignment for dynamic data strucutures, resided in storages. */
#define  CV_STRUCT_ALIGN    ((int)sizeof(double))

/* default storage block size */
#define  CV_STORAGE_BLOCK_SIZE   ((1<<16) - 128)

/* default memory block for sparse array elements */
#define  CV_SPARSE_MAT_BLOCK    (1<<12)

/* initial hash table size */
#define  CV_SPARSE_HASH_SIZE0    (1<<10)

/* maximal average node_count/hash_size ratio beyond which hash table is resized */
#define  CV_SPARSE_HASH_RATIO    3

/* max length of strings */
#define  CV_MAX_STRLEN  1024

#if 0 /*def  CV_CHECK_FOR_NANS*/
#  define CV_CHECK_NANS( arr ) cvCheckArray((arr))
#else
#  define CV_CHECK_NANS( arr )
#endif

/****************************************************************************************\
*                                  Common declarations                                   *
\****************************************************************************************/

/* get alloca declaration */
#ifdef __GNUC__
#  undef alloca
#  define alloca __builtin_alloca
#  define CV_HAVE_ALLOCA 1
#elif defined WIN32 || defined _WIN32 || \
      defined WINCE || defined _MSC_VER || defined __BORLANDC__
#  include <malloc.h>
#  define CV_HAVE_ALLOCA 1
#elif defined HAVE_ALLOCA_H
#  include <alloca.h>
#  define CV_HAVE_ALLOCA 1
#elif defined HAVE_ALLOCA
#  include <stdlib.h>
#  define CV_HAVE_ALLOCA 1
#else
#  undef CV_HAVE_ALLOCA
#endif

#ifdef __GNUC__
#  define CV_DECL_ALIGNED(x) __attribute__ ((aligned (x)))
#elif defined _MSC_VER
#  define CV_DECL_ALIGNED(x) __declspec(align(x))
#else
#  define CV_DECL_ALIGNED(x)
#endif

#if CV_HAVE_ALLOCA
/* ! DO NOT make it an inline function */
#  define cvStackAlloc(size) cvAlignPtr( alloca((size) + CV_MALLOC_ALIGN), CV_MALLOC_ALIGN )
#endif

#ifndef CV_IMPL
#  define CV_IMPL CV_EXTERN_C
#endif

#define CV_DBG_BREAK() { volatile int* crashMe = 0; *crashMe = 0; }

/* default step, set in case of continuous data
   to work around checks for valid step in some ipp functions */
#define  CV_STUB_STEP     (1 << 30)

#define  CV_SIZEOF_FLOAT ((int)sizeof(float))
#define  CV_SIZEOF_SHORT ((int)sizeof(short))

#define  CV_ORIGIN_TL  0
#define  CV_ORIGIN_BL  1

/* IEEE754 constants and macros */
#define  CV_POS_INF       0x7f800000
#define  CV_NEG_INF       0x807fffff /* CV_TOGGLE_FLT(0xff800000) */
#define  CV_1F            0x3f800000
#define  CV_TOGGLE_FLT(x) ((x)^((int)(x) < 0 ? 0x7fffffff : 0))
#define  CV_TOGGLE_DBL(x) \
    ((x)^((int64)(x) < 0 ? CV_BIG_INT(0x7fffffffffffffff) : 0))

#define  CV_NOP(a)      (a)
#define  CV_ADD(a, b)   ((a) + (b))
#define  CV_SUB(a, b)   ((a) - (b))
#define  CV_MUL(a, b)   ((a) * (b))
#define  CV_AND(a, b)   ((a) & (b))
#define  CV_OR(a, b)    ((a) | (b))
#define  CV_XOR(a, b)   ((a) ^ (b))
#define  CV_ANDN(a, b)  (~(a) & (b))
#define  CV_ORN(a, b)   (~(a) | (b))
#define  CV_SQR(a)      ((a) * (a))

#define  CV_LT(a, b)    ((a) < (b))
#define  CV_LE(a, b)    ((a) <= (b))
#define  CV_EQ(a, b)    ((a) == (b))
#define  CV_NE(a, b)    ((a) != (b))
#define  CV_GT(a, b)    ((a) > (b))
#define  CV_GE(a, b)    ((a) >= (b))

#define  CV_NONZERO(a)      ((a) != 0)
#define  CV_NONZERO_FLT(a)  (((a)+(a)) != 0)

/* general-purpose saturation macros */
#define  CV_CAST_8U(t)  (uchar)(!((t) & ~255) ? (t) : (t) > 0 ? 255 : 0)
#define  CV_CAST_8S(t)  (schar)(!(((t)+128) & ~255) ? (t) : (t) > 0 ? 127 : -128)
#define  CV_CAST_16U(t) (ushort)(!((t) & ~65535) ? (t) : (t) > 0 ? 65535 : 0)
#define  CV_CAST_16S(t) (short)(!(((t)+32768) & ~65535) ? (t) : (t) > 0 ? 32767 : -32768)
#define  CV_CAST_32S(t) (int)(t)
#define  CV_CAST_64S(t) (int64)(t)
#define  CV_CAST_32F(t) (float)(t)
#define  CV_CAST_64F(t) (double)(t)

#define  CV_PASTE2(a,b) a##b
#define  CV_PASTE(a,b)  CV_PASTE2(a,b)

#define  CV_EMPTY
#define  CV_MAKE_STR(a) #a

#define  CV_ZERO_OBJ(x) memset((x), 0, sizeof(*(x)))

#define  CV_DIM(static_array) ((int)(sizeof(static_array)/sizeof((static_array)[0])))

#define  cvUnsupportedFormat "Unsupported format"

CV_INLINE void* cvAlignPtr( const void* ptr, int align CV_DEFAULT(32) )
{
    assert( (align & (align-1)) == 0 );
    return (void*)( ((size_t)ptr + align - 1) & ~(size_t)(align-1) );
}

CV_INLINE int cvAlign( int size, int align )
{
    assert( (align & (align-1)) == 0 && size < INT_MAX );
    return (size + align - 1) & -align;
}

CV_INLINE  CvSize  cvGetMatSize( const CvMat* mat )
{
    CvSize size;
    size.width = mat->cols;
    size.height = mat->rows;
    return size;
}

#define  CV_DESCALE(x,n)     (((x) + (1 << ((n)-1))) >> (n))
#define  CV_FLT_TO_FIX(x,n)  cvRound((x)*(1<<(n)))

/****************************************************************************************\
*                     Structures and macros for integration with IPP                     *
\****************************************************************************************/

/* IPP-compatible return codes */
typedef enum CvStatus
{
    CV_BADMEMBLOCK_ERR          = -113,
    CV_INPLACE_NOT_SUPPORTED_ERR= -112,
    CV_UNMATCHED_ROI_ERR        = -111,
    CV_NOTFOUND_ERR             = -110,
    CV_BADCONVERGENCE_ERR       = -109,

    CV_BADDEPTH_ERR             = -107,
    CV_BADROI_ERR               = -106,
    CV_BADHEADER_ERR            = -105,
    CV_UNMATCHED_FORMATS_ERR    = -104,
    CV_UNSUPPORTED_COI_ERR      = -103,
    CV_UNSUPPORTED_CHANNELS_ERR = -102,
    CV_UNSUPPORTED_DEPTH_ERR    = -101,
    CV_UNSUPPORTED_FORMAT_ERR   = -100,

    CV_BADARG_ERR               = -49,  //ipp comp
    CV_NOTDEFINED_ERR           = -48,  //ipp comp

    CV_BADCHANNELS_ERR          = -47,  //ipp comp
    CV_BADRANGE_ERR             = -44,  //ipp comp
    CV_BADSTEP_ERR              = -29,  //ipp comp

    CV_BADFLAG_ERR              =  -12,
    CV_DIV_BY_ZERO_ERR          =  -11, //ipp comp
    CV_BADCOEF_ERR              =  -10,

    CV_BADFACTOR_ERR            =  -7,
    CV_BADPOINT_ERR             =  -6,
    CV_BADSCALE_ERR             =  -4,
    CV_OUTOFMEM_ERR             =  -3,
    CV_NULLPTR_ERR              =  -2,
    CV_BADSIZE_ERR              =  -1,
    CV_NO_ERR                   =   0,
    CV_OK                       =   CV_NO_ERR
}
CvStatus;

#define CV_NOTHROW throw()

typedef struct CvFuncTable
{
    void*   fn_2d[CV_DEPTH_MAX];
}
CvFuncTable;

typedef struct CvBigFuncTable
{
    void*   fn_2d[CV_DEPTH_MAX*4];
} CvBigFuncTable;

#define CV_INIT_FUNC_TAB( tab, FUNCNAME, FLAG )         \
    (tab).fn_2d[CV_8U] = (void*)FUNCNAME##_8u##FLAG;    \
    (tab).fn_2d[CV_8S] = 0;                             \
    (tab).fn_2d[CV_16U] = (void*)FUNCNAME##_16u##FLAG;  \
    (tab).fn_2d[CV_16S] = (void*)FUNCNAME##_16s##FLAG;  \
    (tab).fn_2d[CV_32S] = (void*)FUNCNAME##_32s##FLAG;  \
    (tab).fn_2d[CV_32F] = (void*)FUNCNAME##_32f##FLAG;  \
    (tab).fn_2d[CV_64F] = (void*)FUNCNAME##_64f##FLAG

namespace cv { namespace ogl {
CV_EXPORTS bool checkError(const char* file, const int line, const char* func = "");
}}

#if defined(__GNUC__)
    #define CV_CheckGlError() CV_DbgAssert( (cv::ogl::checkError(__FILE__, __LINE__, __func__)) )
#else
    #define CV_CheckGlError() CV_DbgAssert( (cv::ogl::checkError(__FILE__, __LINE__)) )
#endif

#endif // __OPENCV_CORE_INTERNAL_HPP__
