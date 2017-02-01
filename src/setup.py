from distutils.core import setup, Extension
from Cython.Build import cythonize

setup(
    ext_modules=cythonize(Extension('py_bag_oa',
                                    sources=['py_bag_oa.pyx', 'bag_oa.cpp'],
                                    language='c++',
                                    include_dirs=['/tools/projects/erichang/BAG_2.0/oa_dist/include/oa'],
                                    libraries=['oaCommon', 'oaBase', 'oaPlugIn', 'oaDM', 'oaTech', 'oaDesign', 'dl'],
                                    library_dirs=['/tools/projects/erichang/BAG_2.0/oa_dist/lib/linux_rhel50_gcc44x_64/opt'],
    )
    )
)
