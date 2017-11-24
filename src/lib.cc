#include <string>
#include "lib.h"

using namespace std;

namespace dygre {
    Lib::Lib(string name, string version) {
        this->name = name;
        this->version = version;
    }
}
