/*
    model.hpp - main data structure for insitu program state
*/

#ifndef model_h

#include <string>
#include <map>

#include "mode.hpp"

namespace insitu_common {

class Model
{
private:
    std::string filename;
    std::map<std::string, insitu_common::Mode> modes;

public:
    Model(std::string _filename);
    ~Model();

    void addMode(insitu_common::Mode newmode);
    void rmMode(std::string modename);
};

}


#endif
