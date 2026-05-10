#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <unordered_set>
#include <optional>

#include <to_vector.h>
using namespace cv;

using SvgInfo = std::vector<
    std::tuple<
        Point,
        Point,
        char,
        std::optional<std::tuple<char, float, float>>
    >
>;

Mat visual_contour(std::vector<Point>& dots, Size size) {
    Mat result(size, CV_8UC1, Scalar(255));
    
    for (int i = 0; i < dots.size(); i++) {
        
        result.at<uchar>(dots[i]) = 0;
    }
    return result;
}

/// (Point, Point) -> char (L, M, A), std::option<(rx, ry)> 
SvgInfo read_svg(std::string &svg_path) {
    std::ifstream svg_file(svg_path);
    
    std::string line;
    SvgInfo result = {};
    int x, y;
    while (std::getline(svg_file, line)) {
        if (!line.empty()) {
            if (line[0] == 'd' && line.length() > 3) {
                line = line.substr(3);
            }
            if (line[0] == 'M') {
                size_t pos = 2;

                x = std::stoi(line.substr(pos), &pos); 
                y = std::stoi(line.substr(pos + 2)); 
            }
            if (line[0] == 'L') {
                size_t pos = 2;

                int line_x = std::stoi(line.substr(pos), &pos);
                int line_y = std::stoi(line.substr(pos + 2));
                
                result.push_back({ {x, y}, {line_x, line_y}, 'L', std::nullopt });
                
                x = line_x;
                y = line_y;
            }
            if (line[0] == 'A') {
                const char* s = line.c_str() + 1;
                
                char* p;

                float rx = std::strtof(s, &p);
                float ry = std::strtof(p, &p);

                int _rot   = std::strtol(p, &p, 10);
                int _large = std::strtol(p, &p, 10);
                int sweep = std::strtol(p, &p, 10);

                int end_x = std::strtol(p, &p, 10);
                int end_y = std::strtol(p, &p, 10);
                
                result.push_back({ {x, y}, {end_x, end_y}, 'A', {{sweep, rx, ry}} });
                x = end_x;
                y = end_y;
            }
        }
    }
    return result;
}

float count_accuracy(SvgInfo &test, SvgInfo &perfect) {
    std::unordered_set<Point> perfect_dots = {}, test_dots = {};
    int line_count = 0, arc_count = 0;
    int dot_hit = 0, dot_miss = 0;
    float line_hit = 0, arc_hit = 0;
    
    for (auto& elem: test) {
        test_dots.insert(std::get<0>(elem));
        test_dots.insert(std::get<1>(elem));
    }
    
    for (auto& elem: perfect) {
        float obj_hit = 0;
        Point start = std::get<0>(elem), end = std::get<1>(elem);
        char obj = std::get<2>(elem);
        
        test_dots.insert(start);
        test_dots.insert(end);
        
        for (auto& test_elem: test) {
            
            if (obj == std::get<2>(test_elem)) {
                
                if (obj == 'L') {
                    if (std::get<0>(test_elem) == start && std::get<1>(test_elem) == end) {
                        obj_hit = 20;
                    }
                    else if (std::get<0>(test_elem) == end && std::get<1>(test_elem) == start) {
                        obj_hit = 20;
                    }
                }
                
                if (obj == 'A') {
                    if (std::get<0>(test_elem) == start || std::get<1>(test_elem) == end) {
                        auto params = std::get<3>(elem).value;
                        auto params_test = std::get<3>(test_elem).value;
                        obj_hit = 10;
                        if (std::get<0>(params) == std::get<0>(params_test)) {
                            obj_hit += 4;
                        }
                        float rx = std::get<1>(params) / std::get<1>(params); 
                        float ry = std::get<2>(params) / std::get<2>(params);
                        if (rx > 0.5 && rx < 2)
                            obj_hit += 3;
                        if (ry > 0.5 && ry < 2)
                            obj_hit += 3;
                    }
                    else if (std::get<0>(test_elem) == end || std::get<1>(test_elem) == start) {
                        auto params = std::get<3>(elem).value;
                        auto params_test = std::get<3>(test_elem).value;
                        obj_hit = 10;
                        if (std::get<0>(params) != std::get<0>(params_test)) {
                            obj_hit += 4;
                        }
                        float rx = std::get<1>(params) / std::get<1>(params); 
                        float ry = std::get<2>(params) / std::get<2>(params);
                        if (rx > 0.5 && rx < 2)
                            obj_hit += 3;
                        if (ry > 0.5 && ry < 2)
                            obj_hit += 3;
                    }
                }
            }
        
            if (obj == 'L') {
                line_count++;
                line_hit += obj_hit;
            }
            if (obj == 'A') {
                arc_count++;
                line_hit += obj_hit;
            }
        }
    }
    
    int dots_hit = 0;
    for (auto& dot: perfect_dots) {
        if (test_dots.contains(dot) )
            dots_hit++;
    }
    if (perfect_dots.size() == 0)
        return 100;
    float dots = (float) dot_hit * 60.f / (float) perfect_dots.size();
    
    float missed = (float) (test_dots.size() - dots_hit) / (float) perfect_dots.size();
    missed = std::pow(missed, 0.4);
    if (missed > 60) 
        missed = 60;
        
    float lines;
    if (line_count != 0) 
        lines = line_hit / (float) line_count;
    else 
        lines = 20;
        
    float arcs;
    if (arc_count != 0) 
        arcs = arc_hit / (float) arc_count;
    else 
        arcs = 20;
        
    return dots - missed + lines + arcs;
}
//std::cout << std::get<0>(elem).x << " "  << std::get<0>(elem).y;// << " " << std::get<1>(perfect[0]) << " " << std::get<2>(perfect[0]) << std::endl;

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
        std::string svg_perfect = image + "_perfect.svg";
        
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
            imwrite(tests + image_name + "_" + std::to_string(i + 1) + "_3_line.png", vis);
        }
        std::cout << "Success!\n\n";
        
        // Path
        std::cout << "Test of \"class Path\"\n";
        SvgInfo final = {};
        for (int i = 0; i < dots_all.size(); i++) {
            Path p(dots_all[i]);
            
            p.two_dots_line();
            std::ofstream svg_first(tests + image_name + "_" + std::to_string(i + 1) + "_4_less.svg");
            svg_first << start(c.cols, c.rows);
            Rectangle field("100%", "100%");
            svg_first << field.write_no_xy(255, 255, 255);
            svg_first << p.points_only();
            svg_first << end();
            svg_first.close();
            
            p.line_filter();
            p.two_dots_line();
            std::ofstream svg_sec(tests + image_name + "_" + std::to_string(i + 1) + "_5_filter.svg");
            svg_sec << start(c.cols, c.rows);
            svg_sec << field.write_no_xy(255, 255, 255);
            svg_sec << p.points_only();
            svg_sec << end();
            svg_sec.close();
            
            p.no_random_dots(14);
            p.no_random_dots(14);
            p.two_dots_line();
            std::ofstream svg_three(tests + image_name + "_" + std::to_string(i + 1) + "_6_rnd.svg");
            svg_three << start(c.cols, c.rows);
            svg_three << field.write_no_xy(255, 255, 255);
            svg_three << p.points_only();
            svg_three<< end();
            svg_three.close();
            
            std::string end_path = tests + image_name + "_" + std::to_string(i + 1) + "_7_end.svg";
            std::ofstream svg_end(end_path);
            svg_end << start(c.cols, c.rows);
            svg_end << field.write_no_xy(255, 255, 255);
            std::string vector = p.create_path();
            svg_end << vector;
            svg_end << end();
            svg_end.close();
            
            SvgInfo final_current = read_svg(end_path);
            final.insert(final.end(), final_current.begin(), final_current.end());
        }
        SvgInfo perfect = read_svg(svg_perfect);
        float accuracy = count_accuracy(final, perfect);
        std::cout << "Accuracy is " << std::to_string(accuracy) << "/100\n\n";
        std::cout << "Test finished!\n\n=================================\n";
        
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
}

