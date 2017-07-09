#ifndef __BINDER_H__
#define __BINDER_H__ 1

#include <vector>
#include <string>
#include "helpers.h"

using namespace std;

struct server_location {
	string name;
	unsigned short port;
};

struct function_info {
	string name;
	vector<int> argTypes;
};

bool operator <(const server_location &l, const server_location &r) {
    int order = l.name.compare(r.name);
    if (order == 0) {
        return l.port < r.port;
    } else {
        return order < 0;
    }
}

bool operator ==(const function_info &l, const function_info &r) {
    if (l.name == r.name) {
        if (l.argTypes.size() != r.argTypes.size()) {
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

function_info toFunctionInfo(string name, int *argTypes) {
    unsigned int argTypesLength = getArgTypesLength(argTypes);
    function_info newFunction;
    newFunction.name = name;
    for (unsigned int i = 0; i < argTypesLength; i++) {
        newFunction.argTypes.push_back(argTypes[i]);
    }
    return newFunction;
}

#endif