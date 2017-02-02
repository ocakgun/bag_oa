#include "bagoa.hpp"

namespace bagoa {

    // OA namespace object
    const oa::oaNativeNS ns;
    // Layout pin access direction (all)
    const oa::oaByte pin_dir = oacTop | oacBottom | oacLeft | oacRight;    

    oa::oaBoolean LibDefObserver::onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString & msg,
                                                 oa::oaLibDefListWarningTypeEnum type) {
        throw std::runtime_error("OA Error: " + static_cast<std::string>(msg));
        return true;
    }

    void OALayout::add_inst(const std::string & lib_name, const std::string & cell_name,
                            const std::string & view_name, const std::string & inst_name,
                            double xc, double yc, const std::string & orient,
                            const IntMap & int_params, const StrMap & str_params,
                            const DoubleMap & double_params, int num_rows,
                            int num_cols, double sp_rows, double sp_cols) {
        Inst obj;
        obj.lib_name = oa::oaString(lib_name.c_str());
        obj.cell_name = oa::oaString(cell_name.c_str());
        obj.view_name = oa::oaString(view_name.c_str());
        obj.inst_name = oa::oaString(inst_name.c_str());
        obj.loc[0] = xc;
        obj.loc[1] = yc;
        obj.orient = oa::oaOrient(oa::oaString(orient.c_str()));
        obj.num_rows = num_rows;
        obj.num_cols = num_cols;
        obj.sp_rows = sp_rows;
        obj.sp_cols = sp_cols;

        // build OA param array
        for(IntIter it = int_params.begin(); it != int_params.end(); it++) {
            oa::oaString key(it->first.c_str());
            obj.params.append(oa::oaParam(key, it->second));
        }
        for(DoubleIter it = double_params.begin(); it != double_params.end(); it++) {
            oa::oaString key(it->first.c_str());
            obj.params.append(oa::oaParam(key, it->second));
        }
        for(StrIter it = str_params.begin(); it != str_params.end(); it++) {
            oa::oaString key(it->first.c_str());
            obj.params.append(oa::oaParam(key, oa::oaString(it->second.c_str())));
        }
        
        inst_list.push_back(obj);
    }
    

    
    void OALayout::add_rect(const std::string & lay_name, const std::string & purp_name,
                            double xl, double yb, double xr, double yt,
                            unsigned int nx, unsigned int ny,
                            double spx, double spy) {
        Rect r;
        r.layer = lay_name;
        r.purpose = purp_name;
        r.bbox[0] = xl;
        r.bbox[1] = yb;
        r.bbox[2] = xr;
        r.bbox[3] = yt;        
        r.nx = nx;
        r.ny = ny;
        r.spx = spx;
        r.spy = spy;

        rect_list.push_back(r);
    }

    void OALayout::add_via(const std::string & via_name, double xc, double yc,
                           const std::string & orient, unsigned int num_rows,
                           unsigned int num_cols, double sp_rows, double sp_cols,
                           double enc1_xl, double enc1_yb, double enc1_xr, double enc1_yt,
                           double enc2_xl, double enc2_yb, double enc2_xr, double enc2_yt,
                           double cut_width, double cut_height,
                           unsigned int nx, unsigned int ny,
                           double spx, double spy) {
        Via v;
        v.via_id = oa::oaString(via_name.c_str());
        v.orient = oa::oaOrient(oa::oaString(orient.c_str()));
        v.loc[0] = xc;
        v.loc[1] = yc;        
        v.num_rows = num_rows;
        v.num_cols = num_cols;
        v.spacing[0] = sp_cols;
        v.spacing[1] = sp_rows;
        v.enc1[0] = (enc1_xr + enc1_xl) / 2.0;
        v.enc1[1] = (enc1_yt + enc1_yb) / 2.0;
        v.off1[0] = (enc1_xr - enc1_xl) / 2.0;
        v.off1[1] = (enc1_yt - enc1_yb) / 2.0;
        v.enc2[0] = (enc2_xr + enc2_xl) / 2.0;
        v.enc2[1] = (enc2_yt + enc2_yb) / 2.0;
        v.off2[0] = (enc2_xr - enc2_xl) / 2.0;
        v.off2[1] = (enc2_yt - enc2_yb) / 2.0;
        v.cut_width = cut_width;
        v.cut_height = cut_height;
        v.nx = nx;
        v.ny = ny;
        v.spx = spx;
        v.spy = spy;

        via_list.push_back(v);
    }

    void OALayout::add_pin(const std::string & net_name, const std::string & pin_name,
                           const std::string & label, const std::string & lay_name,
                           const std::string & purp_name, double xl, double yb,
                           double xr, double yt) {
        Pin p;
        p.layer = lay_name;
        p.purpose = purp_name;
        p.bbox[0] = xl;
        p.bbox[1] = yb;
        p.bbox[2] = xr;
        p.bbox[3] = yt;        
        p.term_name = oa::oaString(net_name.c_str());
        p.pin_name = oa::oaString(pin_name.c_str());
        p.label = oa::oaString(label.c_str());

        pin_list.push_back(p);
    }

    void OALayoutLibrary::open_library(const std::string & lib_path, const std::string & library) {
        // initialize OA.
        oaDesignInit( oacAPIMajorRevNumber, oacAPIMinorRevNumber, oacDataModelRevNumber);

        // open library definition
        oa::oaString lib_def_path(lib_path.c_str());
        oa::oaLibDefList::openLibs(lib_def_path);
        
        // open library
        lib_name = oa::oaScalarName(ns, library.c_str());
        lib_ptr = oa::oaLib::find(lib_name);
        if (lib_ptr == NULL) {
            throw std::invalid_argument("Cannot find library " + library);
        } else if (!lib_ptr->isValid()) {
            throw std::invalid_argument("Invalid library: " + library);
        }
        
        // open technology file
        tech_ptr = oa::oaTech::find(lib_ptr);
        if (tech_ptr == NULL) {
            // opened tech not found, attempt to open
            if (!oa::oaTech::exists(lib_ptr)) {
                throw std::runtime_error("Cannot find technology for library: " + library);            
            } else {
                tech_ptr = oa::oaTech::open(lib_ptr, 'r');
                if (tech_ptr == NULL) {
                    throw std::runtime_error("Cannot open technology for library: " + library);
                }
            }
        }

        // get database unit
        dbu_per_uu = tech_ptr->getDBUPerUU(oa::oaViewType::get(oa::oacMaskLayout));
        
        // fill layer/purpose map
        oa::oaString temp;
        oa::oaIter<oa::oaLayer> layers(tech_ptr->getLayers());
        while (oa::oaLayer *layer = layers.getNext()) {
            layer->getName(temp);
            std::string name = static_cast<std::string>(temp);
            oa::oaLayerNum id = layer->getNumber();
            lay_map[name] = id;
        }
        oa::oaIter<oa::oaPurpose> purposes(tech_ptr->getPurposes());
        while (oa::oaPurpose *purp = purposes.getNext()) {
            purp->getName(temp);
            std::string name = static_cast<std::string>(temp);
            oa::oaLayerNum id = purp->getNumber();
            purp_map[name] = id;
        }

        is_open = true;
        
    }    

    void OALayoutLibrary::add_purpose(const std::string & purp_name, unsigned int purp_num) {
        purp_map[purp_name] = (oa::oaPurposeNum)purp_num;
    }
    
    
    void OALayoutLibrary::add_layer(const std::string & lay_name, unsigned int lay_num) {
        lay_map[lay_name] = (oa::oaLayerNum)lay_num;
    }
    
    void OALayoutLibrary::close() {
        if (is_open) {            
            tech_ptr->close();
            lib_ptr->close();

            is_open = false;
        }
        
    }

    void OALayoutLibrary::create_layout(const std::string & cell, const std::string & view,
                                        const OALayout & layout) {
        // do nothing if no library is opened
        if (!is_open) {
            return;
        }
        
        // open design and top block
        oa::oaScalarName cell_name(ns, cell.c_str());
        oa::oaScalarName view_name(ns, view.c_str());
        oa::oaDesign * dsn_ptr = oa::oaDesign::open(lib_name, cell_name, view_name,
                                                    oa::oaViewType::get(oa::oacMaskLayout), 'w');
        oa::oaBlock * blk_ptr = oa::oaBlock::create(dsn_ptr);

        // create geometries
        for( InstIter it = layout.inst_list.begin(); it != layout.inst_list.end(); it++) {
            create_inst(blk_ptr, *it);
        }
        for( RectIter it = layout.rect_list.begin(); it != layout.rect_list.end(); it++) {
            create_rect(blk_ptr, *it);
        }
        for( ViaIter it = layout.via_list.begin(); it != layout.via_list.end(); it++) {
            create_via(blk_ptr, *it);
        }
        for( PinIter it = layout.pin_list.begin(); it != layout.pin_list.end(); it++) {
            create_pin(blk_ptr, *it);
        }        

        // save and close
        dsn_ptr->save();
        dsn_ptr->close();
    }

    oa::oaCoord OALayoutLibrary::double_to_oa(double val) {
        return (oa::oaCoord) round(val * dbu_per_uu);
    }

    void OALayoutLibrary::array_figure(oa::oaFig * fig_ptr, unsigned int nx, unsigned int ny,
                                       double spx, double spy) {
        if (nx > 1 || ny > 1) {
            oa::oaCoord spx_oa = double_to_oa(spx);
            oa::oaCoord spy_oa = double_to_oa(spy);
            for (unsigned int j = 1; j < ny; j++) {
                fig_ptr->copy(oa::oaTransform(0, j * spy_oa));
            }
            for (unsigned int i = 1; i < nx; i++) {
                oa::oaCoord offx = i * spx_oa;
                for (unsigned int j = 0; j < ny; j++) {
                    fig_ptr->copy(oa::oaTransform(offx, j * spy_oa));
                }
            }
        }
    }

    void OALayoutLibrary::create_inst(oa::oaBlock * blk_ptr, const Inst & inst) {
        oa::oaScalarName lib_name(ns, inst.lib_name);
        oa::oaScalarName cell_name(ns, inst.cell_name);
        oa::oaScalarName view_name(ns, inst.view_name);
        oa::oaScalarName inst_name(ns, inst.inst_name);

        oa::oaTransform xfm((oa::oaOffset)double_to_oa(inst.loc[0]),
                            (oa::oaOffset)double_to_oa(inst.loc[1]),
                            inst.orient);

        oa::oaOffset dx = (oa::oaOffset)double_to_oa(inst.sp_cols);
        oa::oaOffset dy = (oa::oaOffset)double_to_oa(inst.sp_rows);
        const oa::oaParamArray * params_ptr = &inst.params;
        if (params_ptr->getNumElements() == 0) {
            // disable parameters if empty
            params_ptr = NULL;
        }
        if (inst.num_rows > 1 || inst.num_cols > 1) {         
            oa::oaArrayInst::create(blk_ptr, lib_name, cell_name, view_name, inst_name,
                                    xfm, dx, dy, inst.num_rows, inst.num_cols, params_ptr);
        } else {
            oa::oaScalarInst::create(blk_ptr, lib_name, cell_name, view_name, inst_name,
                                     xfm, params_ptr);            
        }
    }
    
    void OALayoutLibrary::create_via(oa::oaBlock * blk_ptr, const Via & inst) {
        oa::oaStdViaDef * vdef = static_cast<oa::oaStdViaDef *>(oa::oaViaDef::find(tech_ptr, inst.via_id));
        if (vdef == NULL) {
            std::cout << "create_via: unknown via " << inst.via_id << ", skipping." << std::endl;
            return;
        }
        
        oa::oaTransform xfm((oa::oaOffset)double_to_oa(inst.loc[0]),
                            (oa::oaOffset)double_to_oa(inst.loc[1]),
                            inst.orient);

        oa::oaViaParam params;
        params.setCutRows(inst.num_rows);
        params.setCutColumns(inst.num_cols);
        params.setCutSpacing(oa::oaVector((oa::oaOffset)double_to_oa(inst.spacing[0]),
                                          (oa::oaOffset)double_to_oa(inst.spacing[1])));        
        params.setLayer1Enc(oa::oaVector((oa::oaOffset)double_to_oa(inst.enc1[0]),
                                         (oa::oaOffset)double_to_oa(inst.enc1[1])));
        params.setLayer1Offset(oa::oaVector((oa::oaOffset)double_to_oa(inst.off1[0]),
                                            (oa::oaOffset)double_to_oa(inst.off1[1])));
        params.setLayer2Enc(oa::oaVector((oa::oaOffset)double_to_oa(inst.enc2[0]),
                                         (oa::oaOffset)double_to_oa(inst.enc2[1])));
        params.setLayer2Offset(oa::oaVector((oa::oaOffset)double_to_oa(inst.off2[0]),
                                            (oa::oaOffset)double_to_oa(inst.off2[1])));
        if (inst.cut_width > 0) {
            params.setCutWidth((oa::oaDist)double_to_oa(inst.cut_width));            
        }
        if (inst.cut_height > 0) {
            params.setCutHeight((oa::oaDist)double_to_oa(inst.cut_height));            
        }
        
        oa::oaFig * fig = static_cast<oa::oaFig *>(oa::oaStdVia::create(blk_ptr, vdef, xfm, &params));
        array_figure(fig, inst.nx, inst.ny, inst.spx, inst.spy);
    }
    
    void OALayoutLibrary::create_rect(oa::oaBlock * blk_ptr, const Rect & inst) {
        LayerIter lay_iter = lay_map.find(inst.layer);
        if (lay_iter == lay_map.end()) {
            std::cout << "create_rect: unknown layer " << inst.layer << ", skipping." << std::endl;
            return;
        }
        oa::oaLayerNum layer = lay_iter->second;

        PurposeIter purp_iter = purp_map.find(inst.purpose);
        if (purp_iter == purp_map.end()) {
            std::cout << "create_rect: unknown purpose " << inst.purpose << ", skipping." << std::endl;
            return;
        }
        oa::oaPurposeNum purpose = purp_iter->second;

        oa::oaBox box(double_to_oa(inst.bbox[0]), double_to_oa(inst.bbox[1]),
                      double_to_oa(inst.bbox[2]), double_to_oa(inst.bbox[3]));
        oa::oaRect * r = oa::oaRect::create(blk_ptr, layer, purpose, box);
        array_figure(static_cast<oa::oaFig *>(r), inst.nx, inst.ny, inst.spx, inst.spy);
    }
    
    void OALayoutLibrary::create_pin(oa::oaBlock * blk_ptr, const Pin & inst) {
        // draw pin rectangle
        LayerIter lay_iter = lay_map.find(inst.layer);
        if (lay_iter == lay_map.end()) {
            std::cout << "create_rect: unknown layer " << inst.layer << ", skipping." << std::endl;
            return;
        }
        oa::oaLayerNum layer = lay_iter->second;

        PurposeIter purp_iter = purp_map.find(inst.purpose);
        if (purp_iter == purp_map.end()) {
            std::cout << "create_rect: unknown purpose " << inst.purpose << ", skipping." << std::endl;
            return;
        }
        oa::oaPurposeNum purpose = purp_iter->second;

        oa::oaBox box(double_to_oa(inst.bbox[0]), double_to_oa(inst.bbox[1]),
                      double_to_oa(inst.bbox[2]), double_to_oa(inst.bbox[3]));
        oa::oaRect * r = oa::oaRect::create(blk_ptr, layer, purpose, box);

        // get terminal
        oa::oaName term_name(ns, inst.term_name);
        oa::oaTerm * term = oa::oaTerm::find(blk_ptr, term_name);
	if (term == NULL) {
            // get net
            oa::oaNet * net = oa::oaNet::find(blk_ptr, term_name);
            if (net == NULL) {
                // create net
                net = oa::oaNet::create(blk_ptr, term_name);
            }
            // create terminal
            term = oa::oaTerm::create(net, term_name);
	}

        // create pin and add rectangle to pin.
        oa::oaPin * pin = oa::oaPin::create(term, inst.pin_name, pin_dir);
	r->addToPin(pin);

        // get label location and orientation
        oa::oaPoint op;
        box.getCenter(op);
        oa::oaOrient lorient("R0");
        oa::oaDist lheight = (oa::oaDist)box.getHeight();
        if (box.getHeight() > box.getWidth()) {
            lorient = oa::oaOrient("R90");
            lheight = (oa::oaDist)box.getWidth();
        }

        // create label
        oa::oaText::create(blk_ptr, layer, purpose, inst.label,
                           op, oa::oacCenterCenterTextAlign, lorient,
                           oa::oacRomanFont, lheight);
    }
    
}
