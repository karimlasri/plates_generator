#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <libgen.h>
#include <math.h>
#include <chrono>
#include <random>

#include "generate.hpp"
#include "plate.hpp"

#define PLATE_NUMBER_SIZE 7

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Usage: ./generate_random_image_with_mask <image> \n";
        return EXIT_FAILURE;
    }
    string imgpath = string(argv[1]);
    std::vector<string> vect;
    int nb = Split(vect, imgpath, '/');
    string src_name = vect[vect.size()-1];
    nb = Split(vect, src_name, '.');
    src_name = vect[0];


    Mat original = imread(imgpath, IMREAD_COLOR);
    Mat img = imread(imgpath, IMREAD_COLOR);
    Point2f corners[4];
    cout << to_string(img.size().width) << " " << to_string(img.size().height) << "\n";
    // char original_label[PLATE_NUMBER_SIZE];
    srand(time(NULL));
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(0.0, 10.0);
    //int pan_angle = rand() % 90 - 45;
    int pan_angle = int(distribution(generator));
    float angle_ratio = 1.5;
    int orig_size_ratio = 5;
    int orig_width = (img.size().width) / 10 + (rand() % (img.size().width) / 15);
    int orig_height = orig_width / orig_size_ratio;
    int target_width = orig_width * (1 - sin(2 * M_PI * pan_angle / angle_ratio / 360));
    int offset = orig_height * sin(2 * M_PI * pan_angle / 360);
    cv::Point2f bl, tr, br;
    if (pan_angle > 0)
    {
        tr = cv::Point2f(target_width, offset);
        br = cv::Point2f(target_width, orig_height * (1 + sin(2 * M_PI * pan_angle / angle_ratio / 360)) + offset);
        bl = cv::Point2f(0, orig_height * (1 - sin(2 * M_PI * pan_angle / angle_ratio / 360)));
    }
    else if (pan_angle < 0)
    {
        tr = cv::Point2f(target_width, -offset);
        br = cv::Point2f(target_width, orig_height * (1 - sin(2 * M_PI * pan_angle / angle_ratio / 360)) - offset);
        bl = cv::Point2f(0, orig_height * (1 + sin(2 * M_PI * pan_angle / angle_ratio / 360)));
    }

    cv::Point2f tl = cv::Point2f(rand() % (img.size().width - target_width), offset + (rand() % int(img.size().height - orig_height * (1 + sin(pan_angle / angle_ratio) - 2 * offset))));
    corners[0] = tl;
    corners[1] = cv::Point2f(tl.x + tr.x, tl.y + tr.y);
    corners[2] = cv::Point2f(tl.x + br.x, tl.y + br.y);
    corners[3] = cv::Point2f(tl.x + bl.x, tl.y + bl.y);

    cv::Point2i contours[4];
    contours[0] = cv::Point2i(corners[0]);
    contours[1] = cv::Point2i(corners[1]);
    contours[2] = cv::Point2i(corners[2]);
    contours[3] = cv::Point2i(corners[3]);

    char number[PLATE_NUMBER_SIZE];
    generate_random_number(number);
    put_generated_number(img, contours, number);


    string imgout = "generated/" + src_name + "_fake.jpg";
    if (argc >= 3)
    {
        string imgout = "generated/" + string(argv[2]) + ".jpg";
    }
    imwrite(imgout, img);
    cout << "faked saved to " << imgout << "\n";

    // Saving mask
    Point2f srcTri[4];
    srcTri[0] = Point2f(0, 0);
    srcTri[1] = Point2f(599, 0);
    srcTri[2] = Point2f(599, 119);
    srcTri[3] = Point2f(0, 119);

    Mat mask(img.size().height, img.size().width, CV_8UC3, Scalar(0, 0, 0));
    Mat white(120, 600, CV_8UC3, Scalar(255, 255, 255));

    Mat warp_mat = cv::getPerspectiveTransform(srcTri, corners);
    string maskout = "labeled/" + src_name + "_mask.jpg";
    cout << "mask saved to " << maskout << "\n";
    // Mat transformed(mask.size().height, mask.size().width, CV_8UC3);

    warpPerspective(white, mask, warp_mat, mask.size(), CV_INTER_AREA + CV_WARP_FILL_OUTLIERS); //, BORDER_CONSTANT, Scalar(255, 255, 255));
    imwrite(maskout, mask);

    // string labelout = "labels/" + number_to_string(number) + ".txt";
    string labelout = "labels/" + src_name + "_label.txt";
    std::ofstream labelfile(labelout);
    cout << "label saved to " << labelout << "\n";

    // Contour of plate
    labelfile << to_string(contours[0].x) << "-" << to_string(contours[0].y) << "-" << to_string(contours[1].x) << "-" << to_string(contours[1].y) << "-";
    labelfile << to_string(contours[2].x) << "-" << to_string(contours[2].y) << "-" << to_string(contours[3].x) << "-" << to_string(contours[3].y) << "\n";

    labelfile << number_to_string(number) << "\n";

    // Top-left corner of bounding rect
    labelfile << to_string(min(contours[0].x, contours[3].x)) << "," << to_string(min(contours[0].y, contours[1].y)) << "\n";
    // Bottom-right corner of bounding rect
    labelfile << to_string(max(contours[1].x, contours[2].x)) << "," << to_string(max(contours[2].y, contours[3].y)) << "\n";

    labelfile.close();
    //    if (original_label[0]) {
    //        string labeled_path = "labeled/" + number_to_string(original_label)+ ".jpg";
    //        imwrite(labeled_path, original);
    //        cout << "label saved to " << labeled_path << "\n";
    //    }

    return EXIT_SUCCESS;
}
