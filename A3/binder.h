struct server_location {
	string name;
	unsigned short port;
}

struct function_info {
	string name;
	list<int> argTypes;
}

bool operator ==(const function_info &l, const function_info &r) {
    if(l.name == r.name) {
        int i = 0;
        int lArgType = l.argTypes[i];
        int rArgType = r.argTypes[i];

        while(lArgType!= 0 && rArgType != 0) {
            unsigned int lArraySize = lArgType & 0x0000FFFF;
            unsigned int rArraySize = rArgType & 0x0000FFFF;

            if (((lArgType & 0xFFFF0000) != (rArgType & 0xFFFF0000)) || (lArraySize == 0 && rArraySize != 0) || (lArraySize != 0 && rArraySize == 0)) {
                return false;
            }
            i++;
            lArgType = l.argTypes[i];
            rArgType = r.argTypes[i];
        }

        return lArgType == 0 && rArgType == 0;
    } else {
        return false;
    }
}