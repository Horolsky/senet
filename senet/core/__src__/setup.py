from setuptools import setup
from distutils.core import Extension
from Cython.Build import cythonize

setup(
    name="xtc",
    ext_modules=cythonize(Extension("xtc", ["state/*.pyx"])),
    package_data={"xtc":['state/*.pxd']},
    zip_safe=False,
)