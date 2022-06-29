# distutils: language = c++
import numpy as np
cimport numpy as np

from CoefficientDecoder cimport CoefficientDecoder, _JColorSpace, _JDctMethod
from libcpp.string cimport string


cdef class PyCoefficientDecoder:
    cdef CoefficientDecoder decoder

    def __cinit__(self, filename, dct_method=JDctMethod.JDCT_ISLOW, do_fancy_upsampling=True, do_block_smoothing=True):
        self.decoder = CoefficientDecoder(<string> filename.encode('utf-8'))
        self.decoder.load(dct_method, do_fancy_upsampling, do_block_smoothing)

    def __dealloc__(self):
        self.decoder.unload()

    # Attribute access
    @property
    def filename(self):
        return self.decoder.filename.decode("utf-8")

    @property
    def image_width(self):
        return self.decoder.image_width

    @property
    def image_height(self):
        return self.decoder.image_height

    @property
    def output_width(self):
        return self.decoder.output_width

    @property
    def output_height(self):
        return self.decoder.output_height

    @property
    def output_components(self):
        return self.decoder.output_components

    @property
    def max_v_samp_factor(self):
        return self.decoder.max_v_samp_factor

    @property
    def max_h_samp_factor(self):
        return self.decoder.max_h_samp_factor

    @property
    def jpeg_color_space(self):
        return self.decoder.jpeg_color_space

    def h_samp_factor(self, channel):
        return self.decoder.get_h_samp_factor(channel)

    def v_samp_factor(self, channel):
        return self.decoder.get_v_samp_factor(channel)

    def get_quantization_table(self, channel):
        cdef np.ndarray[np.uint16_t, ndim=1, mode="c"] output = np.zeros(64, dtype=np.uint16)
        self.decoder.get_quantization_table(channel, &output[0])
        return output.reshape((8, 8))

    def get_width_in_blocks(self, channel):
        return self.decoder.get_width_in_blocks(channel)

    def get_height_in_blocks(self, channel):
        return self.decoder.get_height_in_blocks(channel)

    def get_MCU_height(self, channel):
        return self.decoder.get_MCU_height(channel)

    def get_MCU_width(self, channel):
        return self.decoder.get_MCU_width(channel)

    def get_dct_coefficients(self, channel):
        width_in_blocks = self.decoder.get_width_in_blocks(channel)
        height_in_blocks = self.decoder.get_height_in_blocks(channel)
        num_blocks = width_in_blocks * height_in_blocks
        cdef np.ndarray[np.int16_t, ndim=2, mode="c"] output = np.zeros((num_blocks, 64), dtype=np.int16)
        self.decoder.get_dct_coefficients(channel, &output[0, 0])
        return output

    def get_decompressed_image(self):
        row_stride = self.output_width * self.output_components
        # Add singleton dimension for color channel reshaping later
        output_shape = (self.output_height, row_stride, 1)
        cdef np.ndarray[np.uint8_t, ndim=3, mode="c"] output = np.zeros(output_shape, dtype=np.uint8)
        self.decoder.get_decompressed_image(&output[0, 0, 0])
        output = output.reshape((self.output_height, self.output_width, self.output_components))
        return output


cpdef enum JColorSpace:
    JCS_UNKNOWN = _JColorSpace.JCS_UNKNOWN, # error/unspecified
    JCS_GRAYSCALE = _JColorSpace.JCS_GRAYSCALE, # monochrome
    JCS_RGB = _JColorSpace.JCS_RGB, # red/green/blue, standard RGB (sRGB)
    JCS_YCbCr = _JColorSpace.JCS_YCbCr, # Y/Cb/Cr (also known as YUV), standard YCC
    JCS_CMYK = _JColorSpace.JCS_CMYK, # C/M/Y/K
    JCS_YCCK = _JColorSpace.JCS_YCCK, # Y/Cb/Cr/K
    JCS_BG_RGB = _JColorSpace.JCS_BG_RGB, # big gamut red/green/blue, bg-sRGB
    JCS_BG_YCC = _JColorSpace.JCS_BG_YCC # big gamut Y/Cb/Cr, bg-sYCC

cpdef enum JDctMethod:
    JDCT_ISLOW = _JDctMethod.JDCT_ISLOW # slow but accurate integer algorithm
    JDCT_IFAST = _JDctMethod.JDCT_IFAST # faster, less accurate integer method
    JDCT_FLOAT = _JDctMethod.JDCT_FLOAT # floating-point: accurate, fast on fast HW
