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
        cerr << "Usage: ./generate_random_image_with_mask <image> \n";
        return EXIT_FAILURE;
    }
    string imgpath = string(argv[1]);

    Mat original = imread(imgpath, IMREAD_COLOR);
    Mat img = imread(imgpath, IMREAD_COLOR);

    Point2i corners[4];
    char original_label[PLATE_NUMBER_SIZE];

    int pan_angle = rand() % 90 - 45;
    int angle_ratio = 10
    int orig_size_ratio = 5;
    int orig_width = (img.size().width)/10 + (rand() % (img.size().width)/40)
    int orig_height = orig_width / orig_size_ratio
    int target_width = orig_width*(1-sin(pan_angle/angle_ratio))
    int offset = orig_height*sin(2*pan_angle)
    if pan_angle > 0:
        bl = cv::Point2i(0, orig_height*(1-sin(pan_angle/angle_ratio))
        tr = cv::Point2i(target_width, offset)
        br = cv::Point2i(target_width, orig_height*(1+sin(pan_angle/angle_ratio)+offset)
    elif pan_angle < 0:
        bl = cv::Point2i(0, orig_height*(1+sin(pan_angle/angle_ratio))
        tr = cv::Point2i(target_width, -offset)
        br = cv::Point2i(target_width, orig_height*(1-sin(pan_angle/angle_ratio)-offset)

    tl = cv::Point2i(rand() % (img.size().width - target_width), offset + rand() % (img.size().height - orig_height*(1+sin(pan_angle/angle_ratio) - 2*offset)
    corners[0] = tl
    corners[1] = cv::Point2i(tl.x+tr.x, tl.y+tr.y)
    corners[2] = cv::Point2i(tl.x+br.x, tl.y+br.y)
    corners[3] = cv::Point2i(tl.x+bl.x, tl.y+bl.y)

    char number[PLATE_NUMBER_SIZE];
    generate_random_number(number);
    put_generated_number(img, corners, number);
    string imgout = "generated/" + number_to_string(number) + ".jpg";
    if (argc >= 3) {
        string imgout = "generated/" + string(argv[2]) +".jpg";
    }
    imwrite(imgout, img);
    cout << "faked saved to " << imgout << "\n";

    // Saving mask
    Point2f srcTri[4];
    srcTri[0] = Point2f(0, 0);
    srcTri[1] = Point2f(599, 0);
    srcTri[2] = Point2f(599, 119);
    srcTri[3] = Point2f(0, 119);

    Mat mask(img.size().height, img.size().width, CV_8UC3, Scalar(0,0,0));
    Mat white(600, 120, CV_8UC3, Scalar(255, 255, 255));

    Mat warp_mat = getPerspectiveTransform(srcTri, corners);
    string maskout = "generated/" + number_to_string(number) + "_mask.jpg";
    imwrite(imgout, img);
    // Mat transformed(mask.size().height, mask.size().width, CV_8UC3);

    warpPerspective(white, mask, warp_mat, transformed.size(), CV_INTER_AREA+CV_WARP_FILL_OUTLIERS);
    ofstream labelfile;
    string labelout = "labels/" + number_to_string(number) + ".txt";
        if (argc >= 3) {
        string labelout = "labels/" + number_to_string(number) + ".txt";
    }
    labelfile.open(labelout);
    labelfile << number_to_string(number) << "\n"
    // Top-left corner of bounding rect
    labelfile << number_to_string(min(contours[0].x, contours[3].x)) << "," << number_to_string(min(contours[0].y, contours[1].y)) << "\n"
    // Bottom-right corner of bounding rect
    labelfile << number_to_string(max(contours[1].x, contours[2].x)) << "," << number_to_string(max(contours[2].y, contours[3].y)) << "\n"
    labelfile.close();
//    if (original_label[0]) {
//        string labeled_path = "labeled/" + number_to_string(original_label)+ ".jpg";
//        imwrite(labeled_path, original);
//        cout << "label saved to " << labeled_path << "\n";
//    }

    return EXIT_SUCCESS;
}
