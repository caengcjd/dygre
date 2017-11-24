#include <string>
#include "bin.h"

using namespace std;

namespace dygre {
    Bin::Bin(string name, string version) {
        this->name = name;
        this->version = version;
    }

    void Bin::AddLib(class Lib *lib) {
        this->libs.push_back(lib);
    }

    vector<class Lib*> Bin::GetLibs() {
        return libs;
    }
}
