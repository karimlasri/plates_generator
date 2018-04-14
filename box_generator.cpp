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
#include <unistd.h>

#include "generate.hpp"
#include "plate.hpp"

#define PLATE_NUMBER_SIZE 7

using namespace std;
using namespace cv;

unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b; a=a-c; a=a^(c>>13);
    b=b-c; b=b-a; b=b^(a<<8);
    c=c-a; c=c-b; c=c^(b>>13);
    a=a-b; a=a-c; a=a^(c>>13);
    b=b-c; b=b-a; b=b^(a<<16);
    c=c-a; c=c-b; c=c^(b>>5);
    a=a-b; a=a-c, a=a^(c>>3);
    b=b-c, b=b-a; b=b^(a<<10);
    c=c-a; c=c-b; c=c^(b>>15);
    return c;
}

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

    unsigned long seed = mix(clock(), time(NULL), getpid());

    srand(seed);
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0, 20);
    // Angle for pan
    int pan_angle = int(distribution(generator));
    // Angle ratio used for the shift between right and left sides
    int angle_ratio = 10;
    // Ratio between width and height in original image
    int orig_size_ratio = 5;
    // Original centered plate dimensions
    int orig_width = (img.size().width) / 4 + (rand() % (img.size().width) / 12);
    int orig_height = orig_width / orig_size_ratio;
    // Transformed plate dimensions
    int target_width = orig_width * (1 - sin(2 * M_PI * pan_angle / angle_ratio / 360));
    // Shift between right and left angles
    int offset = orig_height * sin(2 * M_PI * pan_angle / 360);
    // Coordinates of the corners of transformed plate relative to top-left angle
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

    // Coordinates of the transformed plate's corners in target image
    corners[0] = tl;
    corners[1] = cv::Point2f(tl.x + tr.x, tl.y + tr.y);
    corners[2] = cv::Point2f(tl.x + br.x, tl.y + br.y);
    corners[3] = cv::Point2f(tl.x + bl.x, tl.y + bl.y);

    // Contour of transformed plate with integer coordinates
    cv::Point2i contours[4];
    contours[0] = cv::Point2i(corners[0]);
    contours[1] = cv::Point2i(corners[1]);
    contours[2] = cv::Point2i(corners[2]);
    contours[3] = cv::Point2i(corners[3]);

    // Generating plate and putting it on target image
    char number[PLATE_NUMBER_SIZE];
    generate_random_number(number);
    put_generated_number(img, contours, number);
    string imgout = "generated/" + number_to_string(number) + ".jpg";
    if (argc >= 3)
    {
        string imgout = "generated/" + string(argv[2]) + ".jpg";
    }
    //std::vector<int> bounding_tl = (min(contours[0].x, contours[3].x), min(contours[0].y, contours[1].y)))
    int tl_x = min(contours[0].x, contours[1].x);
    int tl_y = min(contours[0].y, contours[1].y);
    int br_x = max(contours[0].x, contours[1].x);
    int br_y = max(contours[0].y, contours[1].y);
    cv::Rect roi(tl_x, tl_y, br_x-tl_x, br_y-tl_y);
    cv::Mat crop = img(roi);
    // Bottom-right corner of bounding rect
    imwrite(imgout, crop);
    cout << "box saved to " << imgout << "\n";

    return EXIT_SUCCESS;
}
