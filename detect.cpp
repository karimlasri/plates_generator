#include <iostream>
#include <alpr.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "plate.hpp"

using namespace std;
using namespace alpr;
using namespace cv;


const string countries[2] = {"eu", "us"};

int detect_corners(string imgpath, Point2i *corners, char *original) {
    int found = 0;

    for (string country: countries) {
        Alpr detector(country);
        detector.setDetectRegion(true);
        detector.setTopN(1);

        alpr::AlprResults results = detector.recognize(imgpath);

        cout << imgpath;
        for (AlprPlateResult plate: results.plates) {
            alpr::AlprCoordinate 
                topLeft     = plate.plate_points[0],
                topRight    = plate.plate_points[1],
                bottomRight = plate.plate_points[2],
                bottomLeft  = plate.plate_points[3];

            cout << "\t\t[found " << results.plates.size() << "]";

            corners[0] = cv::Point2i(topLeft.x, topLeft.y);
            corners[1] = cv::Point2i(topRight.x, topRight.y);
            corners[2] = cv::Point2i(bottomRight.x, bottomRight.y);
            corners[3] = cv::Point2i(bottomLeft.x, bottomLeft.y);

            if (!string_to_number(plate.bestPlate.characters, original)) {
                original[0] = '\0';
            } else {
                cout << " " << plate.bestPlate.characters;
            }
            
            found = 1;

            break;
        }
        if (found) break;
    }

    cout << "\n";

    return found;
}
