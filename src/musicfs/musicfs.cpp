#include <fuse.h>
#include "wrap.hpp"
#include <iostream>
using namespace std;

struct fuse_operations musicfs_oper;

int main(int argc, char * argv[]){

    if(argc < 3){
        cout << "bad arguments\n";
    }
    set_rootdir(realpath(argv[1], NULL));
    int fuse_stat = fuse_main(argc, argv, &musicfs_oper);
    cout << "fuse_main returned " << fuse_stat << "\n";
    return fuse_stat;

}
