#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <libgen.h>
#include <dirent.h>

#include "generate.hpp"


using namespace std;
using namespace cv;

//namespace fs = std::experimental::filesystem;

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage: ./create_db <source_folder> <dest_folder> \n";
        return EXIT_FAILURE;
    }
    string source_path = string(argv[1]);
    string dest_path = string(argv[2]);

//    for(auto& p: fs::directory_iterator(source_path)){
//        generate_image_with_mask(p, dest_path);
//    }

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (source_path.c_str())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        std::cout << ent->d_name;
        generate_image_with_mask(ent->d_name, dest_path);
      }
      closedir (dir);
    } else {
      /* could not open directory */
      perror ("");
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
