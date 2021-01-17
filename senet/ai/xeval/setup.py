from setuptools import setup
from distutils.core import Extension
from Cython.Build import cythonize

ext_modules=[
    Extension("emm", ["emm.pyx"])
]

ext_data = {
    "emm":['emm.pxd']
}

setup(
    name="emm",
    version="0.0.0",
    ext_modules=cythonize(ext_modules), 
    package_data=ext_data, 
    zip_safe=False,
)

#python setup.py build_ext --inplace