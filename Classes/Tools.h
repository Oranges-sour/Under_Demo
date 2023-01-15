#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <string>
using namespace std;

#include "Random.h"

class Tools {
public:
    static string random_string(int len) {
        return random_string(len, *Random::getDefault());
    }

    static string random_string(int len, Random& random) {
        static const char ch[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
                                  '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                                  'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',
                                  'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
        static rand_int r(0, 35);
        string str = "";

        for (int i = 1; i <= len; ++i) {
            str += ch[r(random)];
        }

        return str;
    }
};

#endif