#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>
#include <iostream>
#include <fstream>

#include <to_vector.h>
using namespace cv;


int main(int argc, char *argv[]) {
    std::string name = "";
    if (argc > 1){
        name = argv[1];
    }
    else {
        std::cout << "Type the full path to the image:\n";
        std::cin >> name;
    }
    Mat c = imread(name, IMREAD_COLOR);

    if (c.empty()) {
        std::cout << "The image wasn't found!\n";
        return 0;
    }
    
    int contrast = 128;
    white_and_black(c, contrast);

    Mat nc;
    resize(c, nc, Size(), 1, 1);
    
    nc.setTo(Scalar(255, 255, 255));

    for (int i = 1; i < c.rows - 2; i++) {
        for (int j = 1; j < c.cols - 2; j++) {
            neib(c, nc, i, j);
        }
    }
    
    namedWindow("Contours", WINDOW_KEEPRATIO);
    
    bool line_only = false;
    if (nc.rows > 1080){
        resizeWindow("Contours", (int) ((double) nc.cols * 1080 / nc.rows), 1080);
    }
    
    if ((int) ((double) nc.cols * 1080 / nc.rows) > 1920){
        resizeWindow("Contours", 1920, (int) ((double) nc.cols * 1920 / nc.rows));
    }
    imshow("Contours", nc);
    
    int z = waitKey(20000);
    if (z == 'l') {
        line_only = true;
    }

    
    for (int increment = 64; (z == 'w' || z == 'b') && increment > 1; increment /= 2) {
        c = imread(name, IMREAD_COLOR);
        if (z == 'w') {
            contrast += increment;
            white_and_black(c, contrast);
        }
        else if (z == 'b') {
            contrast -= increment;
            white_and_black(c, contrast);
        }

        for (int i = 1; i < c.rows - 2; i++) {
            for (int j = 1; j < c.cols - 2; j++) {
                neib(c, nc, i, j);
            }
        }

        imshow("Contours", nc);
        
        z = waitKey(20000);
        if (z == 'q') {
            return 0;
        }
    }

    if (z == 'q') {
        return 0;
    }

    std::string result = name;
    for (int i = result.length() - 1; (result[i] != '.' && result[i] != '/' && result[i] != '\\') && i > 0; i--) {
        result.pop_back();
    }
    result += "svg";
    std::ofstream svg(result);
    

    svg << start(c.cols, c.rows);
    Rectangle field("100%", "100%");
    svg << field.write_no_xy(255, 255, 255);

    unsigned char picture = 255;
    unsigned char condition = 0;

    for (int j = 1; j < c.cols - 1; j++) {
        for (int i = 1; i < c.rows - 1; i++) {

            if (nc.at<Vec3b>(i, j)[0] == condition) {
                std::vector<int> dots = line(nc, c, i, j, picture, condition);
                Path p(dots);
                p.simple();
                if (line_only) {
                    svg << p.L();
                }
                else {
                    svg << p.create_path();
                }
            }

        }
    }

    svg << end();
    svg.close();
}

