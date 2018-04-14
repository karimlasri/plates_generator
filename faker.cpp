#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <libgen.h>

#include "plate.hpp"
#include "detect.hpp"
#include "generate.hpp"


using namespace std;
using namespace cv;

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage: ./faker <car image path> \n";
        return EXIT_FAILURE;
    }
    string imgpath = string(argv[1]);
    
    Mat original = imread(imgpath, IMREAD_COLOR);
    Mat img = imread(imgpath, IMREAD_COLOR);
    Point2i corners[4];
    char original_label[PLATE_NUMBER_SIZE];

    if (detect_corners(imgpath, corners, original_label)) {
        char number[PLATE_NUMBER_SIZE];
        generate_random_number(number);
        put_generated_number(img, corners, number);
        string imgout = "generated/"+number_to_string(number)+".jpg";
        if (argc >= 3) {
            string imgout = string(argv[2]);
        }
        imwrite(imgout, img);
        cout << "faked saved to " << imgout << "\n";
        if (original_label[0]) {
            string labeled_path = "labeled/"+
                to_string(corners[0].x)+
                "-"+
                to_string(corners[0].y)+
                "-"+
                to_string(corners[1].x)+
                "-"+
                to_string(corners[1].y)+
                "-"+
                to_string(corners[2].x)+
                "-"+
                to_string(corners[2].y)+
                "-"+
                to_string(corners[3].x)+
                "-"+
                to_string(corners[3].y)+
            ".jpg";
            imwrite(labeled_path, original);
            cout << "label saved to " << labeled_path << "\n";
        }
    }

    return EXIT_SUCCESS;
}
