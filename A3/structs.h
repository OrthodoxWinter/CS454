#ifndef __BINDER_H__
#define __BINDER_H__ 1

#include <vector>
#include <string>
#include "helpers.h"

struct server_location {
	std::string name;
	unsigned short port;
};

struct function_info {
	std::string name;
	std::vector<int> argTypes;
};

bool operator ==(const function_info &l, const function_info &r) {
    if(l.name == r.name) {
        if (l.argTypes.size() != r.argTypes) {
        	return false;
        }

        for (unsigned int i = 0; i < l.argTypes.size(); i++) {
        	int lArgType = l.argTypes[i];
        	int rArgType = r.argTypes[i];
        
        	unsigned int lArraySize = lArgType & 0x0000FFFF;
            unsigned int rArraySize = rArgType & 0x0000FFFF;

            if (((lArgType & 0xFFFF0000) != (rArgType & 0xFFFF0000)) || (lArraySize == 0 && rArraySize != 0) || (lArraySize != 0 && rArraySize == 0)) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

function_info toFunctionInfo(std::string name, int *argTypes) {
    unsigned int argTypesLength = getArgTypesLength(argTypes);
    function_info newFunction;
    newFunction.name = name;
    for (unsigned int i = 0; i < argTypesLength; i++) {
        newFunction.argTypes.push_back(argTypes[i]);
    }
    return newFunction;
}

#endif