#ifndef COEFFICIENTDECODER_H
#define COEFFICIENTDECODER_H

#include <string>
#include <jpeglib.h>
#include <cstdint>

namespace decoding {
    class CoefficientDecoder {
        public:
            std::string filename;
            CoefficientDecoder();
            CoefficientDecoder(std::string filename);
            ~CoefficientDecoder();

            void load(J_DCT_METHOD dct_method, bool do_fancy_upsampling, bool do_block_smoothing);
            void unload();
            void get_quantization_table(int channel, unsigned short int* output);
            void get_dct_coefficients(int channel, int16_t* output);
            int get_width_in_blocks(int channel);
            int get_height_in_blocks(int channel);
            int get_output_width();
            int get_output_height();
            void get_decompressed_image(uint8_t* output);
            int get_MCU_width(int channel);
            int get_MCU_height(int channel);
            int get_h_samp_factor(int channel);
            int get_v_samp_factor(int channel);

            int image_width = -1;
            int image_height = -1;
            int output_width = -1;
            int output_height = -1;
            int output_components = -1;
            int max_v_samp_factor = -1;
            int max_h_samp_factor = -1;
            J_COLOR_SPACE jpeg_color_space;

        private:
            int count_quantization_tables();

            FILE* infile;
            struct jpeg_decompress_struct cinfo;
            struct jpeg_error_mgr jerr;
    };
}

#endif