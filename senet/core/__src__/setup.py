from setuptools import setup
from distutils.core import Extension
from Cython.Build import cythonize

setup(
    #name='xtc',
    #ext_modules=cythonize("xtc.pyx"),
    ext_modules=cythonize(Extension("xtc", ["state/*.pyx"])),
    package_data={"xtc":['state/*.pxd']},
    zip_safe=False,
)
#Extension("simulate_fast", ["matrix.c", "random_generator.c", "simulate_fast_c.pyx"])

""" from distutils.core import setup, Extension
setup(name="extc", version="1.0",
      ext_modules=[Extension("extc", ["src/_senetmodule.c"])]) """