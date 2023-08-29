import os
import shutil
from distutils.core import Extension, setup
from distutils.sysconfig import get_config_vars
from sys import platform

import numpy as np
import requests
from Cython.Build import cythonize
from Cython.Distutils import build_ext
from invoke import Context

# Remove the -Wstrict-prototypes flag from the OPT environment variable as it does not make sense when compiling C++ sources and throws warnings.
(opt,) = get_config_vars("OPT")
if opt is not None:
    os.environ["OPT"] = " ".join(
        flag for flag in opt.split() if flag != "-Wstrict-prototypes"
    )

if not os.path.exists("jpeg-9a/build/include"):
    if platform == "windows":
        target_file = "jpegsrc91.zip"
    else:
        target_file = "jpegsrc.v9a.tar.gz"

    download_result = requests.get(f"http://ijg.org/files/{target_file}")
    with open(target_file, "wb") as f:
        for chunk in download_result.iter_content():
            f.write(chunk)

    shutil.unpack_archive(target_file)

    context = Context()
    with context.cd("jpeg-9a"):
        context.run("mkdir -p build")
        context.run(
            f"./configure --prefix {os.path.join(os.getcwd(), context.cwd, 'build')}"
        )
        context.run("make")
        context.run("make install")


libjpeg_include_dir = os.path.join(os.getcwd(), "jpeg-9a/build/include")
libjpeg_lib_dir = os.path.join(os.getcwd(), "jpeg-9a/build/lib")

decoder_ext = Extension(
    "decoder",
    sources=["decoder.pyx"],
    language="c++",
    include_dirs=[np.get_include(), libjpeg_include_dir],
    library_dirs=[libjpeg_lib_dir],
    libraries=["jpeg"],
    runtime_library_dirs=[libjpeg_lib_dir],
    extra_compile_args=["-std=c++11"],
)

setup(
    cmdclass={"build_ext": build_ext}, ext_modules=cythonize(decoder_ext), version="0.2"
)
