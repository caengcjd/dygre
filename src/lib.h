#ifndef DYGRE_LIB_H
#define DYGRE_LIB_H
#include <string>

using namespace std;

namespace dygre {
    class Lib {
    public:
        string name;
        string version;

    public:
        Lib(string name, string version);
    };
}
#endif
