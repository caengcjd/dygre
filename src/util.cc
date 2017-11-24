#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <yaml-cpp/yaml.h>
#include "util.h"

namespace dygre {
    /**
     * link from different fs may not work
     **/
    void util::Insert(string bin, string version, string reId) {cout << bin << version << reId <<endl;
        string oldName = "/var/lib/dygre/bins/" + bin + "/" + version + "/" + bin; 
        string newName = "/var/lib/dygre/runtimes/" + reId + "/mount/bin/" + bin; cout<<oldName<<endl<<newName<<endl;
/*
        link(oldName.c_str(), newName.c_str());
        */
        ifstream src(oldName.c_str(), std::ios::binary);
        ofstream dst(newName.c_str(), std::ios::binary);

        dst << src.rdbuf();

        chmod(newName.c_str(), S_IRWXU);

        YAML::Node config = YAML::LoadFile(("/var/lib/dygre/bins/" + bin + "/" + version + "/dygre.yaml").c_str());

        YAML::Node binlibs = config["libs"];
            for (YAML::const_iterator itx = binlibs.begin(); itx != binlibs.end(); ++itx) {

                oldName = "/var/lib/dygre/libs/" + (itx)->first.as<string>() + "/" + itx->first.as<string>() + ".so." + itx->second.as<string>();
                newName = "/var/lib/dygre/runtimes/" + reId + "/mount/lib/" + itx->first.as<string>() + ".so." + itx->second.as<string>();

                fstream _file;
                _file.open(newName, ios::in);
                if (!_file) {
                    ifstream src1(oldName.c_str(), std::ios::binary);
                    ofstream dst1(newName.c_str(), std::ios::binary);

                    dst1 << src1.rdbuf();

                    
                }
            }

    }
}
