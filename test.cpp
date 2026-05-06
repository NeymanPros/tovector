#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <to_vector.h>
using namespace cv;

Mat visual_contour(std::vector<Point>& dots, Size size) {
    Mat result(size, CV_8UC1, Scalar(255));
    for (int i = 0; i < dots.size(); i++) {

        result.at<uchar>(dots[i]) = 0;
    }
    return result;
}

bool testing(std::string image, std::string tests) {
    try {
        // Open image
        std::cout << "\n======================\nTesting image: " << image << "\n";
        Mat c = imread(image, IMREAD_COLOR);
        if (c.empty()) {
            std::cout << "The image wasn't found!\n";
            return false;
        }
        std::cout << "Opened!\n\n";
        
        std::string ext = "";
        for (int i = image.length() - 1; (image.back() != '.' && image.back() != '/' && image.back() != '\\') && i > 0; i--) {
            ext = image.back() + ext;
            image.pop_back();
        }
        if (image.back() != '.') {
            std::cout << "Bad image name, add an extention!\n";
            return false;
        }
        else {
            image.pop_back();
            ext = "." + ext;
        }
        std::string image_name = "";
        for (int i = image.length() - 1; image[i] != '/'; i--) {
            image_name = image[i] + image_name;
        }
        
        // White and black
        std::cout << "Test of \"white_and_black\"\n";
        white_and_black(c, 128);
        imwrite(tests + image_name + "1_wb.png", c, {cv::IMWRITE_PNG_COMPRESSION, 0});
        std::cout << "Success!\n\n";
        
        // Neib
        std::cout << "Test of \"neib\"\n";
        Mat nc = neib(c);
        
        imwrite(tests + image_name + "2_cont.png", nc, {cv::IMWRITE_PNG_COMPRESSION, 0});
        std::cout << "Success!\n\n";
        
        // Line
        std::cout << "Test of \"line\"\n";
        unsigned char picture = 255;
        unsigned char condition = 0;
        std::vector<std::vector<Point>> dots_all;
        
        for (int j = 1; j < c.cols - 1; j++) {
            for (int i = 1; i < c.rows - 1; i++) {

                if (nc.at<Vec3b>(i, j)[0] == condition) {
                    std::vector<Point> dots_new = line(nc, i, j, picture, condition);
                    if (dots_new.size() > 2) 
                        dots_all.push_back(dots_new);
                }

            }
        }
        
        for (int i = 0; i < dots_all.size(); i++) {
            Mat vis = visual_contour(dots_all[i], nc.size());
            imwrite(tests + image_name + "_" + std::to_string(i) + "_3_line.png", vis);
        }
        std::cout << "Success!\n\n";
        
        // Path
        std::cout << "Test of \"class Path\"\n";
        for (int i = 0; i < dots_all.size(); i++) {
            Path p(dots_all[i]);
            
            //p.two_dots_line();
            p.two_dots_line();
            std::ofstream svg_first(tests + image_name + "_" + std::to_string(i) + "_4_less.svg");
            svg_first << start(c.cols, c.rows);
            Rectangle field("100%", "100%");
            svg_first << field.write_no_xy(255, 255, 255);
            svg_first << p.points_only();
            svg_first << end();
            svg_first.close();
            
            p.line_filter();
            p.two_dots_line();
            std::ofstream svg_sec(tests + image_name + "_" + std::to_string(i) + "_5_filter.svg");
            svg_sec << start(c.cols, c.rows);
            svg_sec << field.write_no_xy(255, 255, 255);
            svg_sec << p.points_only();
            svg_sec << end();
            svg_sec.close();
            
            p.no_random_dots(14);
            p.no_random_dots(14);
            p.two_dots_line();
            std::ofstream svg_three(tests + image_name + "_" + std::to_string(i) + "_6_rnd.svg");
            svg_three << start(c.cols, c.rows);
            svg_three << field.write_no_xy(255, 255, 255);
            svg_three << p.points_only();
            svg_three<< end();
            svg_three.close();
            
            std::ofstream svg_end(tests + image_name + "_" + std::to_string(i) + "_7_end.svg");
            svg_end << start(c.cols, c.rows);
            svg_end << field.write_no_xy(255, 255, 255);
            svg_end << p.create_path();
            svg_end << end();
            svg_end.close();
        }
        std::cout << "Success!\n\n";
        
        return true;
    }
    catch (...) {
        std::cout << "Unsuccessful, exit" << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    std::string tests = argv[0];
    for (int i = tests.length() - 1; tests[i] != '/' && tests[i] != '\\'; i--) {
        tests.pop_back();
    }
    tests += "../test_images/";
    if (!std::filesystem::exists(tests)) {
        std::filesystem::create_directory(tests);
    }
    
    bool tests_res[2] = {false, false};
    tests_res[0] = testing("../tests/octagon.png", tests);
    tests_res[1] = testing("../tests/small.png", tests);
    std::cout << "\n======================\nFirst test is " << tests_res[0] << std::endl;
    std::cout << "Second test is " << tests_res[1] << std::endl;
}

