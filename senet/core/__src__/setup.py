from setuptools import setup
from distutils.core import Extension
from Cython.Build import cythonize

setup(
    name="plyx",
    ext_modules=cythonize(Extension("plyx", ["*.pyx"])),
    package_data={"plyx":['*.pxd']},
    zip_safe=False,
)