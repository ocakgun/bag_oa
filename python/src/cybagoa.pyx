# distutils: language = c++

from libcpp.string cimport string
from libcpp cimport bool

cdef extern from "bagoa.hpp" namespace "bagoa":
    cdef cppclass LibDefObserver:
        pass
    cdef cppclass OAWriter:
        OAWriter()
        void open(const string & lib_path, const string & library,
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
    cdef OAWriter c_writer
    cdef unicode encoding
    def __init__(self, unicode lib_path, unicode library, unicode cell, unicode view,
                 unicode encoding=u'utf-8'):
        self.encoding = encoding
        self.c_writer = OAWriter()
        self.c_writer.open(lib_path.encode(encoding), library.encode(encoding),
                           cell.encode(encoding), view.encode(encoding))

    def __del__(self):
        self.close()
            
    def close(self):
        self.c_writer.close()
        
    def add_purpose(self, unicode purp_name, int purp_num):
        self.c_writer.add_purpose(purp_name.encode(self.encoding), purp_num)

    def add_layer(self, unicode lay_name, int lay_num):
        self.c_writer.add_layer(lay_name.encode(self.encoding), lay_num)


    def create_rect(self, layer, bbox, int arr_nx=1, int arr_ny=1, double arr_spx=0.0, double arr_spy=0.0):
        lay = layer[0].encode(self.encoding)
        purp = layer[1].encode(self.encoding)
        (xl, yb), (xr, yt) = bbox
        self.c_writer.create_rect(lay, purp, xl, yb, xr, yt,
                                  arr_nx, arr_ny, arr_spx, arr_spy)

    def create_pin(self, unicode net_name, unicode pin_name, unicode label, layer, bbox):
        lay = layer[0].encode(self.encoding)
        purp = layer[1].encode(self.encoding)
        (xl, yb), (xr, yt) = bbox
        self.c_writer.create_pin(net_name.encode(self.encoding), pin_name.encode(self.encoding),
                                 label.encode(self.encoding), lay, purp, xl, yb, xr, yt)

    def create_via(self, unicode id, loc, unicode orient, int num_rows, int num_cols,
                   double sp_rows, double sp_cols, enc1, enc2,
                   double cut_width=-1, double cut_height=-1, int arr_nx=1,
                   int arr_ny=1, double arr_spx=0.0, double arr_spy=0.0):
        self.c_writer.create_via(id.encode(self.encoding), loc[0], loc[1], orient.encode(self.encoding),
                                 num_rows, num_cols, sp_rows, sp_cols,
                                 enc1[0], enc1[3], enc1[1], enc1[2],
                                 enc2[0], enc2[3], enc2[1], enc2[2],
                                 cut_width, cut_height, arr_nx, arr_ny, arr_spx, arr_spy)
