# distutils: language = c++

from libcpp.string cimport string
from libcpp cimport bool

cdef extern from "bag_oa.h" namespace "bag_oa":
    cdef cppclass LibDefObserver:
        pass
    cdef cppclass OAWriter:
        OAWriter(const string & lib_path, const string & library,
                 const string & cell, const string & view) except +

        bool create_rect(const string & lay_name, const string & purp_name,
                         double xl, double yb, double xr, double yt,
                         unsigned int nx, unsigned int ny,
                         double spx, double spy) except +

        bool create_via(const string & via_name, double xc, double yc,
                        const string & orient, unsigned int num_rows,
                        unsigned int num_cols, double sp_rows, double sp_cols,
                        double enc1_xl, double enc1_yb, double enc1_xr, double enc1_yt,
                        double enc2_xl, double enc2_yb, double enc2_xr, double enc2_yt,
                        double cut_width, double cut_height,
                        unsigned int nx, unsigned int ny,
                        double spx, double spy) except +

        bool create_pin(const string & net_name, const string & pin_name,
                        const string & label, const string & lay_name,
                        const string & purp_name, double xl, double yb,
                        double xr, double yt) except +

        void add_purpose(const string & purp_name, unsigned int purp_num)

        void add_layer(const string & lay_name, unsigned int lay_num)
        
        void close() except +


cdef class PyOAWriter:
    cdef OAWriter * c_writer
    def __init__(self, bytes lib_path, bytes library, bytes cell, bytes view):
        self.c_writer = new OAWriter(lib_path, library, cell, view)
        if self.c_writer == NULL:
            raise MemoryError("Not enough memory to make C++ OAWriter")

    def __del__(self):
        if self.c_writer != NULL:
            self.close()
            
    def close(self):
        if self.c_writer != NULL:
            self.c_writer.close()
            del self.c_writer
            self.c_writer = NULL
        
    def add_purpose(self, bytes purp_name, int purp_num):
        self.c_writer.add_purpose(purp_name, purp_num)

    def add_layer(self, bytes lay_name, int lay_num):
        self.c_writer.add_layer(lay_name, lay_num)


    def create_rect(self, layer, bbox, int arr_nx=1, int arr_ny=1, double arr_spx=0.0, double arr_spy=0.0):
        lay = layer[0]
        purp = layer[1]
        xl = bbox[0][0]
        yb = bbox[0][1]
        xr = bbox[1][0]
        yt = bbox[1][1]
        self.c_writer.create_rect(lay, purp, xl, yb, xr, yt,
                                  arr_nx, arr_ny, arr_spx, arr_spy)

    def create_pin(self, bytes net_name, bytes pin_name, bytes label, layer, bbox):
        lay, purp = layer
        (xl, yb), (xr, yt) = bbox
        self.c_writer.create_pin(net_name, pin_name, label, lay, purp,
                                 xl, yb, xr, yt)

    def create_via(self, id, loc, orient, num_rows, num_cols, sp_rows, sp_cols,
                   enc1, enc2, double cut_width=-1, double cut_height=-1,
                   int arr_nx=1, int arr_ny=1, double arr_spx=0.0, double arr_spy=0.0):
        self.c_writer.create_via(id, loc[0], loc[1], orient, num_rows, num_cols, sp_rows, sp_cols,
                                 enc1[0], enc1[3], enc1[1], enc1[2],
                                 enc2[0], enc2[3], enc2[1], enc2[2],
                                 cut_width, cut_height, arr_nx, arr_ny, arr_spx, arr_spy)
