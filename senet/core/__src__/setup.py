from setuptools import setup
from distutils.core import Extension
from Cython.Build import cythonize

ext_modules=[
    Extension("plyx", ["plyx.pyx"]),
]
ext_data = {
    "plyx":['plyx.pxd']
}

setup(
    name="plyx",
    version="0.0.1",
    ext_modules=cythonize(ext_modules), #Extension("plyx", ["*.pyx"])),
    package_data=ext_data, #{"plyx":['plyx.pxd']},
    zip_safe=False,
)