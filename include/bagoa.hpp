#ifndef BAGOA_H_
#define BAGOA_H_

#include <iostream>
#include <exception>
#include <stdexcept>
#include <math.h>
#include <map>

#include "oaDesignDB.h"

// techID = techOpenTechFile(lib_name "tech.oa" "r")
// techGetPurposeNum(techID "pin")

namespace bagoa {
    
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
    
    struct Rect {
        std::string layer;
        std::string purpose;
        double bbox[4];
        int nx, ny;
        double spx, spy;
    };

    struct Via {
        oa::oaString via_id;
        oa::oaOrient orient;
        double loc[2];
        int num_rows, num_cols;
        double spacing[2];
        double enc1[2];
        double off1[2];
        double enc2[2];
        double off2[2];
        double cut_width, cut_height;
        int nx, ny;
        double spx, spy;
    };

    struct Pin {
        std::string layer;
        std::string purpose;
        double bbox[4];
        oa::oaString term_name;
        oa::oaString pin_name;
        oa::oaString label;
    };

    typedef std::vector<Rect> RectList;
    typedef RectList::const_iterator RectIter;
    typedef std::vector<Via> ViaList;
    typedef ViaList::const_iterator ViaIter;
    typedef std::vector<Pin> PinList;
    typedef PinList::const_iterator PinIter;
    
    class OALayout {
    public:
        OALayout() {}
        ~OALayout() {}
        
        
        void add_rect(const std::string & lay_name, const std::string & purp_name,
                      double xl, double yb, double xr, double yt,
                      unsigned int nx=1, unsigned int ny=1,
                      double spx=0, double spy=0);

        void add_via(const std::string & via_name, double xc, double yc,
                     const std::string & orient, unsigned int num_rows,
                     unsigned int num_cols, double sp_rows, double sp_cols,
                     double enc1_xl, double enc1_yb, double enc1_xr, double enc1_yt,
                     double enc2_xl, double enc2_yb, double enc2_xr, double enc2_yt,
                     double cut_width=-1, double cut_height=-1,
                     unsigned int nx=1, unsigned int ny=1,
                     double spx=0, double spy=0);

        void add_pin(const std::string & net_name, const std::string & pin_name,
                     const std::string & label, const std::string & lay_name,
                     const std::string & purp_name, double xl, double yb,
                     double xr, double yt);
        
        RectList rect_list;
        ViaList via_list;
        PinList pin_list;
    };
    
    class OALayoutLibrary {
    public:
        OALayoutLibrary() : is_open(false), lib_def_obs(1) {}
        ~OALayoutLibrary() {}

        void open_library(const std::string & lib_path, const std::string & library);

        void add_purpose(const std::string & purp_name, unsigned int purp_num);

        void add_layer(const std::string & lay_name, unsigned int lay_num);
        
        void close();
        
        void create_layout(const std::string & cell, const std::string & view,
                           const OALayout & layout);
        
    private:
        oa::oaCoord double_to_oa(double val);
        void array_figure(oa::oaFig * fig_ptr, unsigned int nx, unsigned int ny,
                          double spx, double spy);
        void create_rect(oa::oaBlock * blk_ptr, const Rect & inst);
        void create_via(oa::oaBlock * blk_ptr, const Via & inst);
        void create_pin(oa::oaBlock * blk_ptr, const Pin & inst);
        
        bool is_open;
        oa::oaUInt4 dbu_per_uu;
        LayerMap lay_map;
        PurposeMap purp_map;
        LibDefObserver lib_def_obs;

        oa::oaLib * lib_ptr;
        oa::oaTech * tech_ptr;
        oa::oaScalarName lib_name;
    };
}

#endif
