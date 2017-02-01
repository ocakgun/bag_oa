#ifndef BAG_OA_H_
#define BAG_OA_H_

#include <iostream>
#include <exception>
#include <stdexcept>
#include <math.h>
#include <map>

#include "oaDesignDB.h"

// techID = techOpenTechFile(lib_name "tech.oa" "r")
// techGetPurposeNum(techID "pin")

namespace bag_oa {
    
    typedef std::map<std::string, oa::oaLayerNum> LayerMap;
    typedef std::map<std::string, oa::oaLayerNum>::iterator LayerIter;
    typedef std::map<std::string, oa::oaPurposeNum> PurposeMap;
    typedef std::map<std::string, oa::oaPurposeNum>::iterator PurposeIter;

    class LibDefObserver : public oa::oaObserver<oa::oaLibDefList> {
    public:
        LibDefObserver(oa::oaUInt4 priority) :
            oa::oaObserver<oa::oaLibDefList>(priority, true) {};

        oa::oaBoolean onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString & msg,
                                     oa::oaLibDefListWarningTypeEnum type);
    };
    
    class OAWriter {
    public:
        OAWriter() : is_open(false), dbu_per_uu(0), lib_def_obs(1) {}
        ~OAWriter() { close(); }
        
        void open(const std::string & lib_path, const std::string & library,
                  const std::string & cell, const std::string & view);
        
        bool create_rect(const std::string & lay_name, const std::string & purp_name,
                         double xl, double yb, double xr, double yt,
                         unsigned int nx=1, unsigned int ny=1,
                         double spx=0, double spy=0);

        bool create_via(const std::string & via_name, double xc, double yc,
                        const std::string & orient, unsigned int num_rows,
                        unsigned int num_cols, double sp_rows, double sp_cols,
                        double enc1_xl, double enc1_yb, double enc1_xr, double enc1_yt,
                        double enc2_xl, double enc2_yb, double enc2_xr, double enc2_yt,
                        double cut_width=-1, double cut_height=-1,
                        unsigned int nx=1, unsigned int ny=1,
                        double spx=0, double spy=0);

        bool create_pin(const std::string & net_name, const std::string & pin_name,
                        const std::string & label, const std::string & lay_name,
                        const std::string & purp_name, double xl, double yb,
                        double xr, double yt);

        void add_purpose(const std::string & purp_name, unsigned int purp_num);

        void add_layer(const std::string & lay_name, unsigned int lay_num);
        
        void close();
        
    private:
        oa::oaCoord double_to_oa(double val);

        void array_figure(oa::oaFig * fig_ptr, unsigned int nx, unsigned int ny,
                          double spx, double spy);
        bool is_open;
        oa::oaUInt4 dbu_per_uu;
        LayerMap lay_map;
        PurposeMap purp_map;
        LibDefObserver lib_def_obs;

        oa::oaLib * lib_ptr;
        oa::oaTech * tech_ptr;
        oa::oaDesign * dsn_ptr;
        oa::oaBlock * blk_ptr;
    };
    
}

#endif
