#include <string>
#include <stdexcept>
#include <stdio.h>
#include "CoefficientDecoder.h"

namespace decoding {

    // Default constructor
    CoefficientDecoder::CoefficientDecoder () {}

    // Overloaded constructor
    CoefficientDecoder::CoefficientDecoder (std::string filename) {
        this->filename = filename;
    }

    // Destructor
    CoefficientDecoder::~CoefficientDecoder () {
    }

    void CoefficientDecoder::load(bool use_float_dct, bool do_fancy_upsampling, bool do_block_smoothing) {
        // For some unknown reason we cannot call load in the constructor
        this->infile = fopen(this->filename.c_str(), "rb");
        if (NULL == this->infile) {
            throw std::runtime_error("File not found");
        }

        // Set up error handling
        this->cinfo.err = jpeg_std_error(&this->jerr);
        this->jerr.error_exit = [](j_common_ptr cinfo){
            char jpegLastErrorMsg[JMSG_LENGTH_MAX];
            /* Create the message */
            ( *( cinfo->err->format_message ) ) ( cinfo, jpegLastErrorMsg );
            /* Jump to the setjmp point */
            throw std::runtime_error( jpegLastErrorMsg );
        };
        jpeg_create_decompress(&this->cinfo);
        // Specify data source
        jpeg_stdio_src(&this->cinfo, this->infile);
        (void) jpeg_read_header(&this->cinfo, FALSE);

        // Be aware that bool is explicitly converted to enum integer here.
        this->cinfo.do_fancy_upsampling = (boolean) do_fancy_upsampling;
        // Default behaviour, apparently, block smoothing is enabled
        this->cinfo.do_block_smoothing = (boolean) do_block_smoothing;

        if (use_float_dct) {
            this->cinfo.dct_method = JDCT_FLOAT;
        }

        // Compute output image dimensions
        jpeg_calc_output_dimensions(&this->cinfo);

        this->image_width = this->cinfo.image_width;
        this->image_height = this->cinfo.image_height;
        this->output_width = this->cinfo.output_width;
        this->output_height = this->cinfo.output_height;
        this->output_components = this->cinfo.output_components;
        this->max_h_samp_factor = this->cinfo.max_h_samp_factor;
        this->max_v_samp_factor = this->cinfo.max_v_samp_factor;
        this->jpeg_color_space = this->cinfo.jpeg_color_space;
    }

    void CoefficientDecoder::unload() {
        if (NULL == this->infile) {
            return;
        }

        (void) jpeg_destroy_decompress(&(this->cinfo));
        fclose(this->infile);
    }

    void CoefficientDecoder::get_quantization_table(int channel, unsigned short int* output) {
        if (channel < 0 || channel >= this->cinfo.num_components) {
            throw std::runtime_error("Illegal color channel");
        }

        // jpeg_read_coefficients must have been called before the quantization table can be accessed.
        jpeg_read_coefficients(&(this->cinfo));

        jpeg_component_info* compptr = this->cinfo.comp_info + channel;
        JQUANT_TBL* quantization_table = compptr->quant_table;
        for (int i=0; i<64; i++) {
            output[i] = quantization_table->quantval[i];
        }
    }

    void CoefficientDecoder::get_dct_coefficients(int channel, int16_t* output) {
        // TODO: don't call this per-channel
        jvirt_barray_ptr* src_coef_arrays = jpeg_read_coefficients(&(this->cinfo));

        // JDIMENSION is defined as unsigned int
        JDIMENSION height_in_blocks = this->cinfo.comp_info[channel].height_in_blocks;
        JDIMENSION width_in_blocks = this->cinfo.comp_info[channel].width_in_blocks;

        for (JDIMENSION row_num=0; row_num < height_in_blocks; row_num++) {
            // A pointer to the virtual array of DCT values
            JBLOCKARRAY row_ptr = ((&(this->cinfo))->mem->access_virt_barray)((j_common_ptr) &cinfo, src_coef_arrays[channel], row_num, (JDIMENSION) 1, FALSE);
            // Loop through the blocks to get the DCT values
            for (JDIMENSION block_num=0; block_num < width_in_blocks; block_num++) {
                // Copy over DCT coefficients for the current block
                int block_offset = (row_num * width_in_blocks + block_num) * DCTSIZE2;
                memcpy(&output[block_offset], row_ptr[0][block_num], DCTSIZE2 * sizeof(JCOEF));
                // for (int i=0; i<DCTSIZE2; i++) {
                //    output[blockOffset + i] = rowPtr[0][blockNum][i];
                // }
            }
        }
    }

    int CoefficientDecoder::get_width_in_blocks(int channel) {
        return this->cinfo.comp_info[channel].width_in_blocks;
    }

    int CoefficientDecoder::get_height_in_blocks(int channel) {
        return this->cinfo.comp_info[channel].height_in_blocks;
    }

    int CoefficientDecoder::get_MCU_width(int channel) {
        return this->cinfo.comp_info[channel].MCU_width;
    }

    int CoefficientDecoder::get_MCU_height(int channel) {
        return this->cinfo.comp_info[channel].MCU_height;
    }

    int CoefficientDecoder::get_h_samp_factor(int channel) {
        return this->cinfo.comp_info[channel].h_samp_factor;
    }

    int CoefficientDecoder::get_v_samp_factor(int channel) {
        return this->cinfo.comp_info[channel].v_samp_factor;
    }

    void CoefficientDecoder::get_decompressed_image(uint8_t* output) {
        // TODO this method may not be called twice

        (void) jpeg_start_decompress(&this->cinfo);
        /* We can ignore the return value since suspension is not possible
         * with the stdio data source.
         */

        /* We may need to do some setup of our own at this point before reading
         * the data. After jpeg_start_decompress() we have the correct scaled
         * output image dimensions available, as well as the output colormap
         * if we asked for color quantization.
         * In this example, we need to make an output work buffer of the right size.
         */
        /* JSAMPLEs per row in output buffer */
        // Output components is the number of colors

        int row_stride = this->cinfo.output_width * this->cinfo.output_components;
        /* Make a one-row-high sample array that will go away when done with image */
        // JSAMPARRAY buffer = (*this->cinfo.mem->alloc_sarray) ((j_common_ptr) &this->cinfo, JPOOL_IMAGE, row_stride, 1);

        // Read all scanlines at once
        // (void) jpeg_read_scanlines(&this->cinfo, &output, this->cinfo.output_height);

        /* Step 6: while (scan lines remain to be read) */
        /*           jpeg_read_scanlines(...); */

        /* Here we use the library's state variable cinfo.output_scanline as the
         * loop counter, so that we don't have to keep track ourselves.
         */
        while (this->cinfo.output_scanline < this->cinfo.output_height) {
            /* jpeg_read_scanlines expects an array of pointers to scanlines.
             * Here the array is only one element long, but you could ask for
             * more than one scanline at a time if that's more convenient.
             */
             int output_scanline = this->cinfo.output_scanline;
             uint8_t* output_scaline_ptr = output + output_scanline * row_stride;
            (void) jpeg_read_scanlines(&this->cinfo, &output_scaline_ptr, 1);
        }

        /* Step 7: Finish decompression */
        (void) jpeg_finish_decompress(&this->cinfo);
        /* We can ignore the return value since suspension is not possible
         * with the stdio data source.
         */
    }
}