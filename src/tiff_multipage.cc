// Copyright (c) 2015 Anatoly Ikorsky

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include "tiff_multipage.h"

const char * join(const std::string & output_name, const std::vector<std::string> & input_names) {
    uint16_t page_number = 0;
    TIFF * output = TIFFOpen(output_name.c_str(), "w");
    if (output == NULL) {
        return "Could not open output file";
    }

    for (std::vector<std::string>::const_iterator it = input_names.begin(); it != input_names.end(); ++it) {
        TIFF * input = TIFFOpen(it->c_str(), "r");
        if (input == NULL) {
            TIFFClose(output);
            return "Could not open input file";
        }

        do {
            uint32_t imageWidth, imageLength;
            uint16_t bitsPerSample, samplesPerPixel, planarConfig, photometric, orientation;
            int32_t scanlineSize;
            tdata_t buf;

            TIFFGetField(input, TIFFTAG_IMAGEWIDTH, &imageWidth);
            TIFFGetField(input, TIFFTAG_IMAGELENGTH, &imageLength);
            TIFFGetField(input, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
            TIFFGetField(input, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
            TIFFGetField(input, TIFFTAG_PLANARCONFIG, &planarConfig);
            TIFFGetField(input, TIFFTAG_PHOTOMETRIC, &photometric);
            TIFFGetFieldDefaulted(input, TIFFTAG_ORIENTATION, &orientation);
            scanlineSize = TIFFScanlineSize(input);

            TIFFSetField(output, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
            TIFFSetField(output, TIFFTAG_IMAGEWIDTH, imageWidth);
            TIFFSetField(output, TIFFTAG_IMAGELENGTH, imageLength);
            TIFFSetField(output, TIFFTAG_BITSPERSAMPLE, bitsPerSample);
            TIFFSetField(output, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
            TIFFSetField(output, TIFFTAG_PLANARCONFIG, planarConfig);
            TIFFSetField(output, TIFFTAG_PHOTOMETRIC, photometric);
            TIFFSetField(output, TIFFTAG_ORIENTATION, orientation);
            TIFFSetField(output, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
            TIFFSetField(output, TIFFTAG_PAGENUMBER, page_number, page_number + 1);

            buf = _TIFFmalloc(scanlineSize);
            if (planarConfig == PLANARCONFIG_CONTIG) {
                for (uint32_t i = 0; i < imageLength; i++) {
                    TIFFReadScanline(input, buf, i);
                    TIFFWriteScanline(output, buf, i);
                }
            } else if (planarConfig == PLANARCONFIG_SEPARATE) {
                for (uint16_t s = 0; s < samplesPerPixel; s++) {
                    for (uint32_t i = 0; i < imageLength; i++) {
                        TIFFReadScanline(input, buf, i, s);
                        TIFFWriteScanline(output, buf, i, s);
                    }
                }
                for (uint32_t i = 0; i < imageLength; i++) {
                    TIFFReadScanline(input, buf, i, 0);
                    TIFFWriteScanline(output, buf, i, 0);
                }
            }
            _TIFFfree(buf);

            TIFFWriteDirectory(output);
        } while (TIFFReadDirectory(input));

        TIFFClose(input);
        page_number++;
    }

    TIFFClose(output);
    return NULL;
}
