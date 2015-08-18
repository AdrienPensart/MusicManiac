#include "wrap.hpp"
#include "MusicFileSystem.hpp"

#include <fuse.h>
#include <iostream>
using namespace std;

int main(int argc, char * argv[]){
    int i;
    for(i = 1; i < argc && (argv[i][0] == '-'); i++) {
        if(i == argc) {
            return (-1);
        }
    }
    MusicFileSystem::instance().setRootDir(argv[1]);

    for(; i < argc; i++) {
        argv[i] = argv[i+1];
    }
    argc--;

    int fuse_stat = fuse_main(argc, argv, &musicfs_oper, NULL);
    cout << "fuse_main returned " << fuse_stat << "\n";
    return fuse_stat;
}
