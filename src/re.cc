#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include "yaml-cpp/yaml.h"
#include "re.h"
#include "fs.h"
#include "lib.h"

using namespace std;

namespace dygre {

    Re::Re() {
        const string DYGRE_DIR = "/var/lib/dygre/";
        const string DYGRE_RUNTIMES_DIR = "/var/lib/dygre/runtimes/";
        
        /**
         * use uuid as runtime env id
         **/
        uuid_t uuid;
        char str[36];
        uuid_generate(uuid);
        uuid_unparse(uuid, str);
        string uuidstr(str);
        id = uuidstr;

        runtimeDir = DYGRE_RUNTIMES_DIR + id + "/";
        lowerDir = runtimeDir + "lower/";
        upperDir = runtimeDir + "upper/";
        workDir = runtimeDir + "work/";
        mountDir = runtimeDir + "mount/";
        libDir = lowerDir + "lib/";
        lib64Dir = lowerDir + "lib64/";
        binDir = lowerDir + "bin/";
    }

    void Re::AddLib(class Lib *lib) {
        libs.push_back(lib);
    }

    void Re::AddBin(class Bin *bin) {
        bins.push_back(bin);
    }

    void Re::AddFile(string src, string dst) {
        files[src] = dst;
    }

    void Re::SetWorkdir(string wd) {
        workdir = wd;
    }

    int Re::MakeFs() {
        int result;

        result = mkdir(runtimeDir.c_str(), 0755);
        if (result != 0) {
            cout << "create dir error." << endl;
            return -1;
        }

        result = mkdir(lowerDir.c_str(), 0755);
        if (result != 0) {
            cout << "create dir error." << endl;
            return -1;
        }

        result = mkdir(upperDir.c_str(), 0755);
        if (result != 0) {
            cout << "create dir error." << endl;
            return -1;
        }
        
        result = mkdir(workDir.c_str(), 0755);
        if (result != 0) {
            cout << "create dir error." << endl;
            return -1;
        }

        result = mkdir(mountDir.c_str(), 0755);
        if (result != 0) {
            cout << "create dir error." << endl;
            return -1;
        }
        
        result = mkdir(libDir.c_str(), 0755);
        if (result != 0) {
            cout << "create dir error." << endl;
            return -1;
        }

        result = mkdir(lib64Dir.c_str(), 0755);
        if (result != 0) {
            cout << "create dir error." << endl;
            return -1;
        }

        result = mkdir(binDir.c_str(), 0755);
        if (result != 0) {
            cout << "create dir error." << endl;
            return -1;
        }

        for (vector<class Lib*>::const_iterator it1 = libs.begin(); it1 != libs.end(); it1++) {
            string oldName = "/var/lib/dygre/libs/" + (*it1)->name + "/" + (*it1)->name + ".so." + (*it1)->version;
            string newName = libDir + (*it1)->name + ".so." + (*it1)->version;
            result = link(oldName.c_str(), newName.c_str());
            if (result != 0) {
                cout << "create link error." << endl;
                return -1;
            }
        }

        link("/lib/x86_64-linux-gnu/ld-2.19.so", (lib64Dir + "ld-linux-x86-64.so.2").c_str());
        
        for (vector<class Bin*>::const_iterator it = bins.begin(); it != bins.end(); it++) {
            string oldName = "/var/lib/dygre/bins/" + (*it)->name + "/" + (*it)->version + "/" + (*it)->name;
            string newName = binDir + (*it)->name;
            result = link(oldName.c_str(), newName.c_str());
            if (result != 0) {
                cout << "create bin link error." << endl;
                return -1;
            }

            YAML::Node config = YAML::LoadFile(("/var/lib/dygre/bins/" + (*it)->name + "/" + (*it)->version + "/" + "dygre.yaml").c_str());
            YAML::Node binlibs = config["libs"];
            for (YAML::const_iterator itx = binlibs.begin(); itx != binlibs.end(); ++itx) {
                dygre::Lib *lib = new dygre::Lib(itx->first.as<std::string>(), itx->second.as<std::string>());
                (*it)->AddLib(lib);
                oldName = "/var/lib/dygre/libs/" + (itx)->first.as<string>() + "/" + itx->first.as<string>() + ".so." + itx->second.as<string>();
                newName = libDir + itx->first.as<string>() + ".so." + itx->second.as<string>();

                fstream _file;
                _file.open(newName, ios::in);
                if (!_file) {
                    result = link(oldName.c_str(), newName.c_str());
                    if (result != 0) {
                        cout << "create link error." << endl;
                        return -1;
                    }
                }
            }

               
        }


        map<string, string>::iterator ite;
        for (ite = files.begin(); ite != files.end(); ite++) {
            ifstream src(ite->first, std::ios::binary);
            if (!src) {
                cout<< "srcerr"<<endl;
            }
            ofstream dst(lowerDir + ite->second, std::ios::binary);
            if (!dst) {
                cout <<"esterr"<<endl;
            }
            dst << src.rdbuf();
        }

        
        //chmod((binDir + "date").c_str(), S_IRWXU);
        
        string args = "lowerdir=" + lowerDir + ",upperdir=" + upperDir + ",workdir=" + workDir;

        result = mount(NULL, mountDir.c_str(), "overlay", 0, args.c_str());
        if (result != 0) {
            cout << "mount error." << endl;
            return -1;
        }

        /**
         * mount src folder to src
         * using mount bind
         **/
        mkdir((mountDir + "src").c_str(), 0755);
        mount(".", (mountDir + "/src").c_str(), "bind", MS_BIND, NULL);


        return 0;
    }

    int Re::Child(void *data) {
        class Re *re = (class Re*)data;
        int result;
        string cmd = re->cmd;
        cout << "executing: " << cmd << endl;
        cout << "process pid is: " << getpid() << endl;
        mount("proc", "/proc", "proc", 0, NULL);
        mount("none", "/dev", "tmpfs", MS_NOEXEC | MS_STRICTATIME, NULL);

        
        mkdir((re->mountDir + "pivot").c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        result = syscall(SYS_pivot_root, re->mountDir.c_str(), (re->mountDir + "pivot").c_str());
        if (result != 0) {
            cout << "pivot error" << endl;
        }
        chdir(re->workdir.c_str());

        result = umount2("/pivot", MNT_DETACH);

        rmdir("/pivot");

        /**
         * parse cmd line
         **/
        int argnum = 1, i = 0;
        for (i; cmd.c_str()[i] != '\0'; i++) {
            if (cmd.c_str()[i] == ' ')
                argnum++;
        }

        char split[] = " ";
        
        char arglist[argnum + 1][128];

        char *substr = strtok((char*)(re->cmd.c_str()), split);
        i = 0;
        while (substr != NULL) {
            strcpy(arglist[i], substr);
            i++;
            substr = strtok(NULL, split);
        }
        if (strcmp(arglist[0], "bash") == 0) {
            execlp("/bin/bash", "bash", NULL);
            return 0;
        }
        arglist[argnum][0] = 0;
        char *al[] = {arglist[0], arglist[1], 0};
        execvp(al[0], (char* const*)al);
        return 0;
    }

    void Re::Start() {
        clog << "Starting re: " << id << endl;

        int result;
        result = MakeFs();

        if (result != 0) {
            cout << "Make fs error." << endl;
            return;
        }

        childPid = clone(Child, childStack + 8192, CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWIPC | CLONE_NET | SIGCHLD, this);

        waitpid(childPid, NULL, 0);
        
        //int result = mkdir("/var/lib/dygre/runtimes, mode_t mode); 
    }

    void Re::Install() {

    }

    void Re::ParseConfig(string filename) {

        YAML::Node config = YAML::LoadFile(filename);
        YAML::Node bins = config["bins"];

        for (YAML::const_iterator it = bins.begin(); it != bins.end(); it++) {
            dygre::Bin *bin = new dygre::Bin(it->first.as<string>(), it->second.as<string>());
            AddBin(bin);
        }
    
        if (config["libs"]) {
            YAML::Node libs = config["libs"];
            for (YAML::const_iterator it = libs.begin(); it != libs.end(); ++it) {
                dygre::Lib *lib = new dygre::Lib(it->first.as<std::string>(), it->second.as<std::string>());
                AddLib(lib);
            }
        }

        if (config["files"]) {
            YAML::Node files = config["files"];
            for (YAML::const_iterator it = files.begin(); it != files.end(); ++it) {
                AddFile(it->first.as<string>(), it->second.as<string>());
            }
        }
    

        cmd = config["cmd"].as<string>();
        SetWorkdir(config["workdir"].as<string>());
    }


}
