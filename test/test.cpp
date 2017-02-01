#include "bagoa.hpp"


int main(int argc, char *argv[])
{
    
    try{

        double enc1[] = {0.1, 0.2, 0.3, 0.4};
        double enc2[] = {0.4, 0.3, 0.2, 0.1};

        std::cout << "1" << std::endl;        
        bagoa::OAWriter writer;
        writer.open("./cds.lib", "AAAFOO", "testoa", "layout");
        std::cout << "2" << std::endl;        
        writer.add_purpose("pin", 251);
        std::cout << "3" << std::endl;
        
        writer.create_rect(argv[1], "drawing", 0.0, 0.0, 0.2, 0.1, 3, 2, 0.3, 0.6);
        std::cout << "4" << std::endl;
        writer.create_via(argv[2], 0.25, 0.25, "R0", 2, 3, 0.1, 0.15,
                          enc1[0], enc1[3], enc1[1], enc1[2],
                          enc2[0], enc2[3], enc2[1], enc2[2],
                          -1, -1, 4, 5, 0.4, 0.5);
        std::cout << "5" << std::endl;
        writer.create_pin("foo", "foo1", "foo:", "M1", "pin", 1.0, 1.1, 1.2, 1.2);
        std::cout << "6" << std::endl;
        writer.close();
        std::cout << "7" << std::endl;
    } catch (oa::oaCompatibilityError &ex) {
        throw std::runtime_error("OA Compatibility Error: " + static_cast<std::string>(ex.getMsg()));
    } catch (oa::oaError &ex) {
        throw std::runtime_error("OA Error: " + static_cast<std::string>(ex.getMsg()));
    }
    
    return 0;
  
}
