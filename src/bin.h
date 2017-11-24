#ifndef DYGRE_BIN_H
#define DYGRE_BIN_H

#include <string>
#include <vector>

using namespace std;

namespace dygre {
    class Bin {
    private:
        vector<class Lib*> libs;
        
    public:
        string name;
        string version;

        Bin(string name, string version);
        void AddLib(class Lib*);
        vector<class Lib*> GetLibs();
    };
}
#endif
