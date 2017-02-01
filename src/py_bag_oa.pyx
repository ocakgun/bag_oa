# distutils: language = c++

from libcpp.string cimport string
from libcpp cimport bool

cdef extern from "bag_oa.h" namespace "bag_oa":
    cdef cppclass LibDefObserver:
        pass
    cdef cppclass OAWriter:
        OAWriter(const char * lib_path, const char * library,
                 const char * cell, const char * view) except +

        bool create_rect(const string & lay_name, const string & purp_name,
                         double xl, double yb, double xr, double yt,
                         unsigned int nx=1, unsigned int ny=1,
                         double spx=0, double spy=0) except +

        bool create_via(const string & via_name, double xc, double yc,
                        const string & orient, unsigned int num_rows,
                        unsigned int num_cols, double sp_rows, double sp_cols,
                        const double (&enc1)[4], const double (&enc2)[4],
                        double cut_width=-1, double cut_height=-1,
                        unsigned int nx=1, unsigned int ny=1,
                        double spx=0, double spy=0) except +

        bool create_pin(const string & net_name, const string & pin_name,
                        const string & label, const string & lay_name,
                        const string & purp_name, double xl, double yb,
                        double xr, double yt) except +

        void add_purpose(const string & purp_name, unsigned int purp_num)

        void add_layer(const string & lay_name, unsigned int lay_num)
        
        void close() except +


cdef class PyOAWriter:
    cdef OAWriter * c_writer
    def __cinit__(self, const char * lib_path, const char * library,
                  const char * cell, const char * view):
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
        
    def add_purpose(self, const string & purp_name, unsigned int purp_num):
        self.c_writer.add_purpose(purp_name, purp_num)

    def add_layer(self, const string & lay_name, unsigned int lay_num):
        self.c_writer.add_layer(lay_name, lay_num)


if __name__ == '__main__':
    writer = PyOAWriter("./cds.lib", "AAAFOO", "testoa", "layout")
    writer.add_purpose("pin", 251)
    writer.close()
