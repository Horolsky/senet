from setuptools import setup
from distutils.core import Extension
from Cython.Build import cythonize

ext_modules=[
    Extension("xstate", ["xstate.pyx"]),
    Extension("xply", ["xply.pyx"])
]

ext_data = {
    "xstate":['xstate.pxd']
}

setup(
    name="xply",
    version="0.0.1",
    ext_modules=cythonize(ext_modules), 
    package_data=ext_data, 
    zip_safe=False,
)

#python setup.py build_ext --inplace