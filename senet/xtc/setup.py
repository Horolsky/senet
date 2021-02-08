from setuptools import setup
from distutils.core import Extension
from Cython.Build import cythonize

ext_modules=[
    Extension("xtc", sources=["xtc.pyx", "src-xtc.c"]),#, libraries=["senetply"])
]

ext_data = {
    "xtc":['xtc.pxd'],
}

setup(
    name="xtc",
    version="0.0.1",
    ext_modules=cythonize(ext_modules), 
    package_data=ext_data, 
    zip_safe=False,
)

#python setup.py build_ext --inplace