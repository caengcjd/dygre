#include <iostream>
#include <fstream>
#include <cstring>
#include "yaml-cpp/yaml.h"
#include "re.h"
#include "lib.h"
#include "bin.h"
#include "util.h"

using namespace std;

namespace dygre {

}

int main(int argc, char *argv[])
{
    fstream dygreFile;
    dygreFile.open("dygre.yaml", ios::in);
    if (!dygreFile) {
        cout << "no dygre.yaml found." << endl;
        return 0;
    }
    
    dygre::Re re;
    re.ParseConfig("dygre.yaml");
    
    
    if (strcmp(argv[1], "install") == 0) {
        cout << "install";
        re.Install();
        return 0;
    }

    if (strcmp(argv[1], "start") == 0) {

         re.Start();    
    }

    if (strcmp(argv[1], "insert") == 0) {
        dygre::util::Insert(argv[2], argv[3], argv[4]);
    }
    
    
    return 0;
}
