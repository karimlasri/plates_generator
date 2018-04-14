#ifndef GENERATE_H_
#define GENERATE_H_

#include <opencv2/core.hpp>

#include "generate.cpp"

using namespace cv;

void put_generated_number(Mat&, const Point2i*, const char*, int, float, int);

int Split(vector<string>& vecteur, string chaine, char separateur);

void generate_image_with_mask(string src_path, string dest_folder);

#endif
