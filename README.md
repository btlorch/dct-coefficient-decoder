# DCT coefficient decoder
Python wrapper for decoding DCT coefficients from JPEG-compressed images.

**Note**: This is research code and has not been designed for use in production. Tested with Python 3.5 and *libjpeg 9a*.

## Prerequisites

* Header files for building Python extensions
```
apt-get install python-dev
```

* You need to have *libjpeg* installed. Please update the include and lib paths in `setup.py` such that they point to your installation.
```python
libjpeg_include_dir = "path/to/libjpeg/build/include"
libjpeg_lib_dir = "path/to/libjpeg/build/lib"
```

* Python dependencies
```
pip install Cython numpy
```

## Compile
To compile the extension for use in the project's root directory, run the following:
```bash
python setup.py build_ext --inplace
```

This will place a shared object in the current working directory.

For use from another directory, make sure to include this module in your Python path.
This can be done by either including the project's root directory in your `PYTHONPATH` environment variable, or by installing the module with `python setup.py install`.

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

## Known issues
* Attempting to read uncompressed image after reading DCT coefficients results in `Improper call to JPEG library in state 210`. Workaround: Initialize another instance of `PyCoefficientDecoder`.
