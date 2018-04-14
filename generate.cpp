#include <iostream>
#include <math.h>
#include <limits>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>

#include "plate.hpp"

using namespace std;
using namespace cv;


void put_generated_number(Mat &img, const Point2i corners[4], const char *number, int font = 2, float font_scale = 2.5, int thickness = 6) {
    if (thickness == -1) thickness = rand() % 7 + 4;
    int width = 600, height = 120;

    Mat plate(height, width, CV_8UC3, Scalar::all(255));

    // Adding side rectangles
    float side_rect_rh = 0.02, side_rect_rw_0 = 0.005, side_rect_rw_1 = 0.09;
    for (int i = round(height*side_rect_rh); i < round(height*(1-side_rect_rh)); i++) {
        for (int j = round(width*side_rect_rw_0); j < round(width*side_rect_rw_1); j++) {
            plate.at<Vec3b>(Point(j, i)) = Vec3b(255, 0, 0);
        }
    }

    // Left blue side country
    char plate_country[2] = {(char)(rand() % 26 + 'A'), '\0'};
    putText(plate, String(plate_country), Point(round(width*0.03), round(height*0.8)), 4, 1, Scalar::all(255), 2);

    // Adding stars
    Point2i center(round(width*(side_rect_rw_0 + side_rect_rw_1/2.0)), round(height*0.3));
    float radius = (float)width*(side_rect_rw_1 - side_rect_rw_0)/4.0;
    int stars_radius = round(width*0.004);
    for (int i = 0; i < 12; i++) {
        int star_y = round(center.y + radius * sin(i*2*M_PI/12));
        int star_x = round(center.x + radius * cos(i*2*M_PI/12));
        circle(plate, Point2i(star_x, star_y), stars_radius, Scalar(0, 255, 255), -1);
    }

    // Right rectangle
    int right_rect = 1;
    if (right_rect) {
        for (int i = round(height*side_rect_rh); i < round(height*(1-side_rect_rh)); i++) {
            for (int j = round(width*(1-side_rect_rw_1)); j < round(width*(1-side_rect_rw_0)); j++) {
                plate.at<Vec3b>(Point(j, i)) = Vec3b(255, 0, 0);
            }
        }
        char plate_state[3] = {(char)(rand() % 10 + '0'), (char)(rand() % 10 + '0'), '\0'};
        putText(plate, String(plate_state), Point2i(round(width*0.92), round(height*0.8)), 4, 1, Scalar::all(255), 2);
    }

    // Add plate number
    float text_rh = 0.1, text_rw = 0.1;
    string str_number = number_to_string(number);
    string str_3_number = str_number.substr(0, 2)+" "+str_number.substr(2, 3)+" "+str_number.substr(5);
    Size text_size = getTextSize(String(str_3_number), font, font_scale, thickness, NULL);
    Point2i text_org(0, text_size.height);
    Mat plate_number(text_size.height+thickness, text_size.width+thickness, CV_8UC3, Scalar::all(255));
    putText(plate_number, String(str_3_number), text_org, font, font_scale, Scalar(0, 0, 0), thickness);
    const int seperator_width = 12, seperator_thick = 6;
    line(
        plate_number,
        Point2f((float)(plate_number.cols)/4.0-(float)seperator_width/2.0+11, (float)(plate_number.rows)/2.0),
        Point2f((float)(plate_number.cols)/4.0+(float)seperator_width/2.0+11, (float)(plate_number.rows)/2.0),
        Scalar::all(0), thickness=seperator_thick);
    line(
        plate_number,
        Point2f((float)(plate_number.cols)*3.0/4.0-(float)seperator_width/2.0-15, (float)(plate_number.rows)/2.0),
        Point2f((float)(plate_number.cols)*3.0/4.0+(float)seperator_width/2.0-15, (float)(plate_number.rows)/2.0),
        Scalar::all(0), thickness=seperator_thick);
    Mat region;
    if (right_rect) {
        resize(plate_number, plate_number, Size(round((float)width*(1-2*text_rh)), round((float)height*(1.0-2.0*text_rw))), 0, 0, INTER_NEAREST);
        region = plate.rowRange(round(height * text_rh), round(height * (1 - text_rh))).colRange(round(width * (text_rw)), round(width * (1 - text_rw)));
    } else {
        resize(plate_number, plate_number, Size(round((float)width*(1-text_rh)), round((float)height*(1.0-2.0*text_rh))), 0, 0, INTER_NEAREST);
        region = plate.rowRange(round(height * text_rh), round(height * (1 - text_rh))).colRange(round(width * (text_rw)), width);
    }
    plate_number.copyTo(region);

    // Set your 3 points to calculate the  Affine Transform
    Point2f srcTri[4], dstTri[4];
    srcTri[0] = Point2f(0, 0);
    srcTri[1] = Point2f(plate.cols - 1, 0);
    srcTri[2] = Point2f(plate.cols - 1, plate.rows - 1);
    srcTri[3] = Point2f(0, plate.rows - 1);

    dstTri[0] = Point2f(corners[0].x, corners[0].y);
    dstTri[1] = Point2f(corners[1].x, corners[1].y);
    dstTri[2] = Point2f(corners[2].x, corners[2].y);
    dstTri[3] = Point2f(corners[3].x, corners[3].y);

    Mat warp_mat = getPerspectiveTransform(srcTri, dstTri);
    warpPerspective(plate, img, warp_mat, img.size(), 1, BORDER_TRANSPARENT);
}


int Split(vector<string>& vecteur, string chaine, char separateur)
{
	vecteur.clear();

	string::size_type stTemp = chaine.find(separateur);

	while(stTemp != string::npos)
	{
		vecteur.push_back(chaine.substr(0, stTemp));
		chaine = chaine.substr(stTemp + 1);
		stTemp = chaine.find(separateur);
	}

	vecteur.push_back(chaine);

	return vecteur.size();
}


void generate_image_with_mask(string src_path, string dest_folder) {

    std::vector<string> vect;
    int nb = Split(vect, src_path, '/');
    string src_name = vect[vect.size()-1];
    nb = Split(vect, src_name, '.');
    src_name = vect[0];

    Mat img = imread(src_path, IMREAD_COLOR);
    Point2f corners[4];
    // char original_label[PLATE_NUMBER_SIZE];
    srand(time(NULL));
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0, 20);
    //int pan_angle = rand() % 90 - 45;
    int pan_angle = int(distribution(generator));
    int angle_ratio = 10;
    int orig_size_ratio = 5;
    int orig_width = (img.size().width) / 4 + (rand() % (img.size().width) / 12);
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
    string imgout = dest_folder + '/' + src_name + "_fake.jpg";
    imwrite(imgout, img);
    cout << "faked saved to " << imgout << "\n";

    // Saving mask
    Point2f srcTri[4];
    srcTri[0] = Point2f(0, 0);
    srcTri[1] = Point2f(599, 0);
    srcTri[2] = Point2f(599, 119);
    srcTri[3] = Point2f(0, 119);

    Mat mask(img.size().height, img.size().width, CV_8UC3, Scalar(0,0,0));
    Mat white(120, 600, CV_8UC3, Scalar(255, 255, 255));

    Mat warp_mat = getPerspectiveTransform(srcTri, corners);
    string maskout = dest_folder + '/' + src_name + "_mask.jpg";
    // Mat transformed(mask.size().height, mask.size().width, CV_8UC3);

    warpPerspective(white, mask, warp_mat, mask.size(), CV_INTER_AREA+CV_WARP_FILL_OUTLIERS);
    imwrite(maskout, img);

    std::ofstream labelfile;
    string labelout = dest_folder + '/' + src_name + "_label.txt";

    labelfile.open(labelout);

    // Contour of plate
    labelfile << to_string(corners[0].x) << "-" << to_string(corners[0].y) << "-" << to_string(corners[1].x) << "-" << to_string(corners[1].y) << "-";
    labelfile << to_string(corners[2].x) << "-" << to_string(corners[2].y) << "-" << to_string(corners[3].x) << "-" << to_string(corners[3].y);

    labelfile << number_to_string(number) << "\n";

    // Top-left corner of bounding rect
    labelfile << to_string(min(corners[0].x, corners[3].x)) << "," << to_string(min(contours[0].y, contours[1].y)) << "\n";
    // Bottom-right corner of bounding rect
    labelfile << to_string(max(corners[1].x, corners[2].x)) << "," << to_string(max(contours[2].y, contours[3].y)) << "\n";

    labelfile.close();
//    if (original_label[0]) {
//        string labeled_path = "labeled/" + number_to_string(original_label)+ ".jpg";
//        imwrite(labeled_path, original);
//        cout << "label saved to " << labeled_path << "\n";
//    }
}
