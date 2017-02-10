# distutils: language = c++

from libcpp.string cimport string
from libcpp.map cimport map
from libcpp cimport bool

cdef extern from "bagLayout.hpp" namespace "baglayout":
    cdef cppclass Layout:
        Layout()

        void add_inst(const string & lib_name, const string & cell_name,
                      const string & view_name, const string & inst_name,
                      double xc, double yc, const string & orient,
                      const map[string, int] int_params, const map[string, string] str_params,
                      const map[string, double] double_params, int num_rows,
                      int num_cols, double sp_rows, double sp_cols) except +
        
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
                     double xr, double yt, bool make_pin_obj) except +


cdef extern from "bagoa.hpp" namespace "bagoa":
    cdef cppclass LibDefObserver:
        pass

    cdef cppclass OALayoutLibrary:
        OALayoutLibrary()
        void open_library(const string & lib_path, const string & library) except +
        void add_purpose(const string & purp_name, unsigned int purp_num) except +
        void add_layer(const string & lay_name, unsigned int lay_num) except +
        void close() except +
        void create_layout(const string & cell, const string & view, const Layout & layout) except +


cdef class PyLayout:
    cdef Layout c_layout
    cdef unicode encoding
    def __init__(self, unicode encoding):
        self.encoding = encoding

    def add_inst(self, unicode lib, unicode cell, unicode view,
                 unicode name, loc, unicode orient, params=None,
                 int num_rows=1, int num_cols=1, double sp_rows=0.0,
                 double sp_cols=0.0):
        cdef map[string, int] int_map
        cdef map[string, string] str_map
        cdef map[string, double] double_map
        lib_name = lib.encode(self.encoding)
        cell_name = cell.encode(self.encoding)
        view_name = view.encode(self.encoding)
        inst_name = name.encode(self.encoding)
        c_orient = orient.encode(self.encoding)
        params = params or {}
        for key, val in params.items():
            key = key.encode(self.encoding)
            if isinstance(val, bytes):
                str_map[key] = val
            elif isinstance(val, unicode):
                str_map[key] = val.encode(self.encoding)
            elif isinstance(val, int):
                int_map[key] = val
            elif isinstance(val, float):
                double_map[key] = val

        self.c_layout.add_inst(lib_name, cell_name, view_name,
                               inst_name, loc[0], loc[1],
                               c_orient, int_map, str_map,
                               double_map, num_rows, num_cols,
                               sp_rows, sp_cols)
        
    def add_rect(self, layer, bbox, int arr_nx=1, int arr_ny=1,
                 double arr_spx=0.0, double arr_spy=0.0):
        lay = layer[0].encode(self.encoding)
        purp = layer[1].encode(self.encoding)
        self.c_layout.add_rect(lay, purp, bbox[0][0], bbox[0][1],
                               bbox[1][0], bbox[1][1], arr_nx, arr_ny,
                               arr_spx, arr_spy)

    def add_via(self, unicode id, loc, unicode orient,
                int num_rows, int num_cols, double sp_rows, double sp_cols,
                enc1, enc2, double cut_width=-1, double cut_height=-1,
                int arr_nx=1, int arr_ny=1, double arr_spx=0.0,
                double arr_spy=0.0):
        via_name = id.encode(self.encoding)
        via_orient = orient.encode(self.encoding)
        self.c_layout.add_via(via_name, loc[0], loc[1], via_orient, 
                              num_rows, num_cols, sp_rows, sp_cols,
                              enc1[0], enc1[3], enc1[1], enc1[2],
                              enc2[0], enc2[3], enc2[1], enc2[2],
                              cut_width, cut_height, arr_nx, arr_ny, arr_spx, arr_spy)

    def add_pin(self, unicode net_name, unicode pin_name, unicode label, layer, bbox,
                bool make_rect=True):
        c_net = net_name.encode(self.encoding)
        c_pin = pin_name.encode(self.encoding)
        c_label = label.encode(self.encoding)
        lay = layer[0].encode(self.encoding)
        purp = layer[1].encode(self.encoding)
        self.c_layout.add_pin(c_net, c_pin, c_label, lay, purp,
                              bbox[0][0], bbox[0][1], bbox[1][0], bbox[1][1],
                              make_rect)


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

    def create_layout(self, unicode cell, unicode view, PyLayout layout):
        self.c_lib.create_layout(cell.encode(self.encoding), view.encode(self.encoding),
                                 layout.c_layout)
