# DCT coefficient decoder
Python wrapper for decoding DCT coefficients from JPEG-compressed images.

**Note**: This is research code and has not been designed for use in production. Tested with Python 3.5 and *libjpeg 9a*.

## Prerequisites

* Header files for building Python extensions
```
apt-get install python3-dev
```

* You need to have *libjpeg* installed. If you don't have a global installation, you can download *libjpeg* from [here](https://ijg.org/files/) and install it by following the build instructions. Please update the include and lib paths in `setup.py` such that they point to your installation.

```python
libjpeg_include_dir = "path/to/libjpeg/build/include"
libjpeg_lib_dir = "path/to/libjpeg/build/lib"
```

* Python dependencies
```
pip install Cython numpy
```

## Compile

**Global installation**
```bash
pip install .
```

This will automatically install the python dependencies.

**Local installation**

To compile the extension for use in the project's root directory, run the following:
```bash
python setup.py build_ext --inplace
```

This will place a shared object in the current working directory.  For use from another directory, make sure to include this module in your PYTHONPATH.

## Usage
```python
from decoder import PyCoefficientDecoder 

filename = "image.jpg"

# Load and decode image
d = PyCoefficientDecoder(filename) 

# Access image width
d.image_width

# Read (quantized) DCT coefficients of luminance channel
d.get_dct_coefficients(0)

# Read quantization table for luminance channel
d.get_quantization_table(0)
```

Example how to vary the DCT implementation. Supported options are *JDCT_ISLOW* (default), *JDCT_IFAST*, *JDCT_FLOAT*.
```python
from decoder import PyCoefficientDecoder, JDctMethod
filename = "image.jpg"
d = PyCoefficientDecoder(filename, dct_method=JDctMethod.JDCT_FLOAT)
img = d.get_decompressed_image()
```

## Known issues
* Attempting to read uncompressed image after reading DCT coefficients results in `Improper call to JPEG library in state 210`. Workaround: Initialize another instance of `PyCoefficientDecoder`.

## Misc
1. Another way to quickly read an image's quantization tables:
```bash
djpeg -verbose -verbose image.jpg > /dev/null
```

2. A visual explanation of the *Discrete Cosine Transform* (DCT) is given in [this notebook](discrete_cosine_transform_visual_explanation.ipynb).