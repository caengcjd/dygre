#include <string>
#include <vector>
#include <unistd.h>
#include "lib.h"
#include "bin.h"

using namespace std;

namespace dygre {

    class Re {
    private:
        string id;
        vector<class Lib*> libs;
        vector<class Bin*> bins;
        map<string, string> files;

        /**
         * chdir to workdir in re on start
         **/
        string workdir;

        /**
         * runtime dirs
         **/
        string runtimeDir;
        string lowerDir;
        string upperDir;
        string workDir;
        string mountDir;
        string libDir;
        string lib64Dir;
        string binDir;

        char childStack[8192];
        pid_t childPid;

        int MakeFs();
        

    public:
        /**
         * cmd to execute
         **/
        string cmd;

        Re();
        void Install();
        void Start();
        void AddLib(class Lib *lib);
        void AddBin(class Bin *bin);
        void AddFile(string, string);
        static int Child(void *data);
        void SetWorkdir(string);
        void ParseConfig(string);
    };

}
