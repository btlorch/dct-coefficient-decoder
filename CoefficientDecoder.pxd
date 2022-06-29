from libcpp.string cimport string
from libc.stdint cimport int16_t, uint8_t

cdef extern from "CoefficientDecoder.cpp":
    pass

# Declare the class with cdef
cdef extern from "CoefficientDecoder.h" namespace "decoding":
    cdef cppclass CoefficientDecoder:
        CoefficientDecoder() except +
        CoefficientDecoder(string) except +
        void load(_JDctMethod, bint, bint) except +
        void unload() except +
        void get_quantization_table(int, unsigned short int *) except +
        int get_width_in_blocks(int) except +
        int get_height_in_blocks(int) except +
        void get_dct_coefficients(int, int16_t*) except +
        void get_decompressed_image(uint8_t*) except +
        int get_MCU_height(int) except +
        int get_MCU_width(int) except +
        int get_h_samp_factor(int) except +
        int get_v_samp_factor(int) except +

        string filename
        int image_width
        int image_height
        int output_width
        int output_height
        int output_components
        int max_h_samp_factor
        int max_v_samp_factor
        _JColorSpace jpeg_color_space
        _JDctMethod dct_method

# Enum workaround: see https://github.com/cython/cython/issues/1529
cdef extern from "jpeglib.h":
    ctypedef enum _JColorSpace "J_COLOR_SPACE":
        JCS_UNKNOWN, # error/unspecified
        JCS_GRAYSCALE, # monochrome
        JCS_RGB, # red/green/blue, standard RGB (sRGB)
        JCS_YCbCr, # Y/Cb/Cr (also known as YUV), standard YCC
        JCS_CMYK, # C/M/Y/K
        JCS_YCCK, # Y/Cb/Cr/K
        JCS_BG_RGB, # big gamut red/green/blue, bg-sRGB
        JCS_BG_YCC # big gamut Y/Cb/Cr, bg-sYCC

    ctypedef enum _JDctMethod "J_DCT_METHOD":
        JDCT_ISLOW,		# slow but accurate integer algorithm
        JDCT_IFAST,		# faster, less accurate integer method
        JDCT_FLOAT		# floating-point: accurate, fast on fast HW
