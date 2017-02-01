# distutils: language = c++

from libcpp.string cimport string
from libcpp cimport bool

cdef extern from "bagoa.hpp" namespace "bagoa":
    cdef cppclass LibDefObserver:
        pass

    cdef cppclass OALayout:
        OALayout()
        void add_rect(const string & lay_name, const string & purp_name,
                      double xl, double yb, double xr, double yt,
                      unsigned int nx, unsigned int ny,
                      double spx, double spy) except +

        void add_via(const string & via_name, double xc, double yc,
                     const string & orient, unsigned int num_rows,
                     unsigned int num_cols, double sp_rows, double sp_cols,
                     double enc1_xl, double enc1_yb, double enc1_xr, double enc1_yt,
                     double enc2_xl, double enc2_yb, double enc2_xr, double enc2_yt,
                     double cut_width, double cut_height,
                     unsigned int nx, unsigned int ny,
                     double spx, double spy) except +

        void add_pin(const string & net_name, const string & pin_name,
                     const string & label, const string & lay_name,
                     const string & purp_name, double xl, double yb,
                     double xr, double yt) except +

    cdef cppclass OALayoutLibrary:
        OALayoutLibrary()
        void open_library(const string & lib_path, const string & library) except +
        void add_purpose(const string & purp_name, unsigned int purp_num) except +
        void add_layer(const string & lay_name, unsigned int lay_num) except +
        void close() except +
        void create_layout(const string & cell, const string & view, const OALayout & layout) except +


cdef class PyOALayout:
    cdef OALayout c_layout
    def __init__(self):
        pass
    def add_rect(self, unicode lay_name, unicode purp_name

cdef class PyOALayoutLibrary:
    cdef OALayoutLibrary c_lib
    cdef bytes lib_path
    cdef bytes library
    cdef unicode encoding
    def __init__(self, unicode lib_path, unicode library, unicode encoding):
        self.lib_path = lib_path.encode(encoding)
        self.library = library.encode(encoding)
        self.encoding = encoding
    
    def __enter__(self):
        self.c_lib.open_library(self.lib_path, self.library)
        return self

    def __exit__(self, *args):
        self.close()
        
    def __del__(self):
        self.close()
            
    def close(self):
        self.c_lib.close()
        
    def add_purpose(self, unicode purp_name, int purp_num):
        self.c_lib.add_purpose(purp_name.encode(self.encoding), purp_num)

    def add_layer(self, unicode lay_name, int lay_num):
        self.c_lib.add_layer(lay_name.encode(self.encoding), lay_num)

    def create_layout(self, unicode cell, unicode view, PyOALayout layout):
        self.c_lib.create_layout(cell.encode(self.encoding), view.encode(self.encoding),
                                 layout.c_layout)
