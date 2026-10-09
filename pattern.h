#pragma once

#include <string>

struct NoDayCrossRet {
    bool hit;
    double delta;
    double target;
    std::string cross_conditon;    
};

NoDayCrossRet no_day_cross(bool bull);

