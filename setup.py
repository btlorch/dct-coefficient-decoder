from distutils.core import setup, Extension
from distutils.sysconfig import get_config_vars
from Cython.Distutils import build_ext
from Cython.Build import cythonize
import numpy as np
import os

# Remove the -Wstrict-prototypes flag from the OPT environment variable as it does not make sense when compiling C++ sources and throws warnings.
(opt,) = get_config_vars('OPT')
if opt is not None:
    os.environ['OPT'] = " ".join(
        flag for flag in opt.split() if flag != '-Wstrict-prototypes'
    )

libjpeg_include_dir = os.path.expanduser("~/i1/libjpeg/jpeg-9a/build/include")
libjpeg_lib_dir = os.path.expanduser("~/i1/libjpeg/jpeg-9a/build/lib")

decoder_ext = Extension("decoder",
                        sources=["decoder.pyx"],
                        language="c++",
                        include_dirs=[np.get_include(), libjpeg_include_dir],
                        library_dirs=[libjpeg_lib_dir],
                        libraries=["jpeg"],
                        runtime_library_dirs=[libjpeg_lib_dir],
                        extra_compile_args=["-std=c++11"],
                        )

setup(cmdclass={"build_ext": build_ext}, ext_modules=cythonize(decoder_ext), version="0.2")
