#include "bagoa.hpp"


int main(int argc, char *argv[])
{
    
    try{

        double enc1[] = {0.1, 0.2, 0.3, 0.4};
        double enc2[] = {0.4, 0.3, 0.2, 0.1};

        // create layout
        baglayout::Layout layout;
        layout.add_rect(argv[1], "drawing", 0.0, 0.0, 0.2, 0.1, 3, 2, 0.3, 0.6);
        layout.add_via(argv[2], 0.25, 0.25, "R0", 2, 3, 0.1, 0.15,
                       enc1[0], enc1[3], enc1[1], enc1[2],
                       enc2[0], enc2[3], enc2[1], enc2[2],
                       -1, -1, 4, 5, 0.4, 0.5);
        layout.add_pin("foo", "foo1", "foo:", "M1", "pin", 1.0, 1.1, 1.2, 1.2);

        // open library and draw layout
        bagoa::OALayoutLibrary lib;
        lib.open_library("./cds.lib", "AAAFOO");
        lib.add_purpose("pin", 251);
        lib.create_layout("testoa", "layout", layout);
        lib.close();
    } catch (oa::oaCompatibilityError &ex) {
        throw std::runtime_error("OA Compatibility Error: " + static_cast<std::string>(ex.getMsg()));
    } catch (oa::oaDMError &ex) {
        throw std::runtime_error("OA DM Error: " + static_cast<std::string>(ex.getMsg()));
    } catch (oa::oaError &ex) {
        throw std::runtime_error("OA Error: " + static_cast<std::string>(ex.getMsg()));
    }
    
    return 0;
  
}
