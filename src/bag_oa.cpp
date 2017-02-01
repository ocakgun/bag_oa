#include "bag_oa.h"

banesoace bag_oa {

    const oa::oaNativeNS ns;
    
    oa::oaBoolean LibDefObserver::onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString & msg,
                                                 oa::oaLibDefListWarningTypeEnum type) {
        throw std::runtime_error("OA Error: " + static_cast<std::string>(msg));
        return true;
    }
    
    OAWriter::OAWriter(const std::string & lib_path, const std::string & library, const std::string & cell, const std::string & view) :
    lib_def_obs(1), is_closed(false), dbu_per_uu(0)
    {
        // open library definition
        oa::oaString lib_def_path(lib_path.c_str());
        oa::oaLibDefList::openLibs(lib_def_path);
        
        // open library
        oa::oaScalarName lib_name(ns, library.c_str());
        oa::oaLib * lib_ptr = oa::oaLib::find(lib_name);
        if (lib_ptr == NULL) {
            throw std::invalid_argument("Cannot find library " + library);
        } else if (!lib_ptr->isValid()) {
            throw std::invalid_argument("Invalid library: " + library);
        }
        
        // open technology file
        oa::oaTech * tech_ptr = oa::oaTech::find(lib_ptr);
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
        dbu_per_uu = tech_ptr->getDBUPerUU();
        
        // fill layer/purpose map
        oa::oaString temp;
        oa::oaIter<oa::oaLayer> layers(tech_ptr->getLayers());
        while (oaLayer *layer = layers.getNext()) {
            layer->getName(temp);
            std::string name = static_cast<std::string>(temp);
            oa::oaLayerNum id = layer->getNumber();
            lay_map[name] = id;
        }
        oa::oaIter<oa::oaPurpose> purposes(tech_ptr->getPurposes());
        while (oaLayer *purp = purposes.getNext()) {
            purp->getName(temp);
            std::string name = static_cast<std::string>(temp);
            oa::oaLayerNum id = purp->getNumber();
            purp_map[name] = id;
        }
        
        // open design and top block
        oa::oaScalarName cell_name(ns, cell.c_str());
        oa::oaScalarName view_name(ns, view.c_str());
        dsn_ptr = oa::oaDesign::open(lib_name, cell_name, view_name, oa::oaViewType::get(oa::oacMaskLayout), 'w');
        blk_ptr = oa::oaBlock::create(dsn_ptr);
    }    

    oa::oaCoord OAWriter::double_to_oa(double val) {
        return (oa::oaCoord) round(val * dbu_per_uu);
    }

    void OAWriter::array_figure(oa::oaFig * fig_ptr, unsigned int nx, unsigned int ny,
                                double spx, double spy) {
        if (nx > 1 || ny > 1) {
            oa::oaCoord spx_oa = double_to_oa(spx);
            oa::oaCoord spy_oa = double_to_oa(spy);
            for (unsigned int j = 1; j < ny; j++) {
                fig->copy(oa::oaTransform(0, j * spy_oa));
            }
            for (unsigned int i = 1; i < nx; i++) {
                oa::oaCoord offx = i * spx_oa;
                for (unsigned int j = 0; j < ny; j++) {
                    fig->copy(oa::oaTransform(offx, j * spy_oa));
                }
            }
        }
    }    
    
    bool OAWriter::create_via(const std::string & via_name, double xc, double yc,
                              const std::string & orient, unsigned int num_rows,
                              unsigned int num_cols, double sp_rows, double sp_cols,
                              const double (&enc1)[4], const double (&enc2)[4],
                              double cut_width, double cut_height,
                              unsigned int nx, unsigned int ny,
                              double spx, double spy) {
        // get via definition
        oa::oaString via_id(via_name.c_str());
        oa::oaStdViaDef vdef = static_cast<oa::oaStdViaDef *>(oa::oaViaDef::find(tech_ptr, via_id));
        if (vdef == NULL) {
            std::cout << "create_via: unknown via " << via_name << ", skipping.";
            return false;    
        }

        oa::oaTransform xfm((oa::oaOffset)double_to_oa(xc),
                            (oa::oaOffset)double_to_oa(yc),
                            oa::oaOrient(oa::oaString(orient.c_str())));

        oa::oaViaParam params;
        params.setCutRows(num_rows);
        params.setCutColumns(num_cols);
        params.setCutSpacing(oa::oaVector((oa::oaOffset)double_to_oa(sp_cols),
                                          (oa::oaOffset)double_to_oa(sp_rows)));
        
        double encx = (enc1[0] + enc1[1]) / 2.0;
        double ency = (enc1[2] + enc1[3]) / 2.0;
        double offx = (enc1[1] - enc1[0]) / 2.0;
        double offy = (enc1[2] - enc1[3]) / 2.0;
        params.setLayer1Enc(oa::oaVector((oa::oaOffset)double_to_oa(encx),
                                         (oa::oaOffset)double_to_oa(ency)));
        params.setLayer1Offset(oa::oaVector((oa::oaOffset)double_to_oa(offx),
                                            (oa::oaOffset)double_to_oa(offy)));        
        encx = (enc2[0] + enc2[1]) / 2.0;
        ency = (enc2[2] + enc2[3]) / 2.0;
        offx = (enc2[1] - enc2[0]) / 2.0;
        offy = (enc2[2] - enc2[3]) / 2.0;
        params.setLayer2Enc(oa::oaVector((oa::oaOffset)double_to_oa(encx),
                                         (oa::oaOffset)double_to_oa(ency)));
        params.setLayer2Offset(oa::oaVector((oa::oaOffset)double_to_oa(offx),
                                            (oa::oaOffset)double_to_oa(offy)));

        if (cut_width > 0) {
            params.setCutWidth((oa::oaDist)double_to_oa(cut_width));            
        }
        if (cut_height > 0) {
            params.setCutHeight((oa::oaDist)double_to_oa(cut_height));            
        }
        
        oa::oaFig * v = static_cast<oa::oaFig *>(oa::oaStdVia::create(blk_ptr, vdef, xfm, params));
        array_figure(v, nx, ny, spx, spy);
        return true;
    }
   
    bool OAWriter::create_rect(const std::string & lay_name, const std::string & purp_name,
                               double xl, double yb, double xr, double yt,
                               unsigned int nx, unsigned int ny,
                               double spx, double spy) {
        LayerIter lay_iter = lay_map.find(lay_name);
        if (lay_iter == lay_map.end()) {
            std::cout << "create_rect: unknown layer " << lay_name << ", skipping.";
            return false;
        }
        oa::oaLayerNum layer = lay_iter->second;

        PurposeIter purp_iter = purp_map.find(purp_name);
        if (purp_iter == purp_map.end()) {
            std::cout << "create_rect: unknown purpose " << purp_name << ", skipping.";
            return false;
        }
        oa::oaPurposeNum purpose = purp_iter->second;

        oa::oaBox box(double_to_oa(xl), double_to_oa(yb), double_to_oa(xr), double_to_oa(yt));
        oa::oaRect * r = oa::oaRect::create(blk_ptr, layer, purpose, box);
        array_figure(static_cast<oa::oaFig *>(r), nx, ny, spx, spy);
        return true;
    }

    bool OAWriter::create_pin(const std::string & net_name, const std::string & pin_name,
                              const std::string & label, const std::string & lay_name,
                              const std::string & purp_name, double xl, double yb,
                              double xr, double yt) {
        // draw pin rectangle
        LayerIter lay_iter = lay_map.find(lay_name);
        if (lay_iter == lay_map.end()) {
            std::cout << "create_rect: unknown layer " << lay_name << ", skipping.";
            return false;
        }
        oa::oaLayerNum layer = lay_iter->second;

        PurposeIter purp_iter = purp_map.find(purp_name);
        if (purp_iter == purp_map.end()) {
            std::cout << "create_rect: unknown purpose " << purp_name << ", skipping.";
            return false;
        }
        oa::oaPurposeNum purpose = purp_iter->second;

        oa::oaBox box(double_to_oa(xl), double_to_oa(yb), double_to_oa(xr), double_to_oa(yt));
        oa::oaRect * r = oa::oaRect::create(blk_ptr, layer, purpose, box);

        // get terminal
        oa::oaName term_name(ns, oa::oaString(net_name.c_str()));
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
        oa::oaByte dir = oa::oacTop | oa::oacBottom | oa::oacLeft | oa::oacRight;
        oa::oaString pin_name_oa(pin_name.c_str());
        oa::oaPin * pin = oaPin::create(term, pin_name_oa, dir);
	r->addToPin(pin);

        // get label location and orientation
        oa::oaPoint op;
        box.getcenter(op);
        oa::oaOrient lorient("R0");
        oa::oaDist lheight = (oa::oaDist)box.getHeight();
        if (box.getHeight() > box.getWidth()) {
            lorient = oa::oaOrient("R90");
            lheight = (oa::oaDist)box.getWidth();
        }

        // create label
        oa::oaString label_name(label.c_str());
	oaText *text = oa::oaText::create(blk_ptr, layer, purpose, label_name,
                                          op, oa::oacCenterCenterTextAlign, lorient,
                                          oa::oacRomanFont, lheight);
        return true;
    }
    

    void OAWriter::add_purpose(const std::string & purp_name, unsigned int purp_num) {
        purp_map[purp_name] = (oa::oaPurposeNum)purp_num;
    }
    
    
    void OAWriter::add_layer(const std::string & lay_name, unsigned int lay_num) {
        purp_map[lay_name] = (oa::oaLayerNum)lay_num;
    }
    
    void OAWriter::close() {
        dsn_ptr->save();
        dsn_ptr->close();
        tech_ptr->close();
        lib_ptr->close();
    }
}
