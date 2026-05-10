#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <cmath>
#include <array>

#include <to_vector.h>
using namespace cv;
#include <iostream>

/// Making the image only black and white.
void white_and_black(Mat &c, const int contrast) {
    for (int i = 0; i < c.rows - 1; i++) {
        for (int j = 0; j < c.cols - 1; j++) {
            if (c.at<Vec3b>(i, j)[0] > contrast || c.at<Vec3b>(i, j)[1] > contrast || c.at<Vec3b>(i, j)[2] > contrast) {
                c.at<Vec3b>(i, j) = { 255, 255, 255 };
            }
            else {
                c.at<Vec3b>(i, j) = { 0, 0, 0 };
            }

        }
    }
}


/// This function find countours of an image.
/// Writes contours from the first cv::Mat into the second one.
Mat neib(Mat &c) {
    Mat out;
    resize(c, out, Size(), 1, 1);
    out.setTo(Scalar(255, 255, 255));
    
    for (int y = 1; y < c.rows - 2; y++) {
        for (int x = 1; x < c.cols - 2; x++) {
            if (c.at<Vec3b>(y, x) != c.at<Vec3b>(y - 1, x - 1) || c.at<Vec3b>(y, x) != c.at<Vec3b>(y - 1, x) || 
                c.at<Vec3b>(y, x) != c.at<Vec3b>(y - 1, x + 1) || c.at<Vec3b>(y, x) != c.at<Vec3b>(y, x - 1) ||
                c.at<Vec3b>(y, x) != c.at<Vec3b>(y, x + 1) || c.at<Vec3b>(y, x) != c.at<Vec3b>(y + 1, x - 1) || 
                c.at<Vec3b>(y, x) != c.at<Vec3b>(y + 1, x) || c.at<Vec3b>(y, x) != c.at<Vec3b>(y + 1, x + 1)) {

                    out.at<Vec3b>(y, x) = { c.at<Vec3b>(y, x)[0], c.at<Vec3b>(y, x)[1], c.at<Vec3b>(y, x)[2] };
            }
        }
    }
    return out;
}


/// Extracting coordinates from contour.
std::vector<Point> line(Mat& nc, int y, int x, unsigned char picture, unsigned char condition) {
    std::vector<Point> dots;
    int run = 0;
    nc.at<Vec3b>(y, x) = { picture, picture, picture };

    dots.push_back({x, y});

    short count = 0;
    int y1 = y;
    int x1 = x;
    for (int now = 1; now > 0;) {

        if (y > 0 && y < nc.rows && x > 0 && x < nc.cols) {

            count = 0;

            if (nc.at<Vec3b>(y - 1, x)[1] == condition) {
                count += 1;
                y1 = y - 1;
                x1 = x;
            }
            else if (nc.at<Vec3b>(y, x - 1)[1] == condition) {
                count += 1;
                y1 = y;
                x1 = x - 1;
            }
            else if (nc.at<Vec3b>(y, x + 1)[1] == condition) {
                count += 1;
                y1 = y;
                x1 = x + 1;
            }
            else if (nc.at<Vec3b>(y + 1, x)[1] == condition) {
                count += 1;
                y1 = y + 1;
                x1 = x;
            }


            if (count == 0) {
                if (now > 2) {
                    if (dots[0].x - dots[now - 1].x <= 1 && dots[now - 1].y - dots[0].y <= 1 &&
                        dots[0].x - dots[now - 1].x <= 1 && dots[now - 1].y - dots[0].y <= 1) {
                            dots.push_back({dots[0].x, dots[0].y});
                    }
                }
                return dots;
            }

            else if (count == 1) {
                nc.at<Vec3b>(y, x) = { picture,picture,picture };
                dots.push_back({x1, y1});
                now++;
                run = 0;

                y = y1;
                x = x1;
            }

            else if (count == 2) {
                
                if (run > 4) {
                    nc.at<Vec3b>(y, x) = { picture,picture,picture };
                }
                else {
                    run += 1;
                }

                dots.push_back({x1, y1});
                now++;

                y = y1;
                x = x1;
            }

            else if (count > 2) {
                
                dots.push_back({x1, y1});
                now++;
                
                y = y1;
                x = x1;
            }
        }
        else {
            return dots;
        }
    }
    return dots;
}

// svg part
std::string start(int width, int height) {
    return "<svg width=\"" + std::to_string(width) + "\" height=\"" + std::to_string(height) + "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
};

Rectangle::Rectangle(std::string w, std::string h) {
    width = w;
    height = h;
    x = 0;
    y = 0;
}
Rectangle::Rectangle(int a, int b, int c, int d) {
    x = a;
    y = b;
    width = std::to_string(c);
    height = std::to_string(d);
}

std::string Rectangle::write(int r, int g, int b) {
    return "<rect x=\"" + std::to_string(x) + "\" y=\"" + std::to_string(y) + "\" width=\"" + width + "\" height=\"" + 
        height + " fill=\"rgb(" + std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b) + ")\"/>\n";
}
std::string Rectangle::write_no_xy(int r, int g, int b) {
    return "<rect width=\"" + width + "\" height=\"" + height + 
        "\" fill=\"rgb(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ")\"/>\n";
}


std::string end() {
    return "</svg>";
}


void Path::new_angles() {
    angles.clear();
    for (int i = 0; i + 1 < dots.size(); i++)
        angles.push_back(dots[i + 1] - dots[i]);
}

void Path::two_dots_straight() {
    std::vector<Point> dots_new = {};
    int now = 0;

    if (dots.size() < 1) {
        return;
    }

    dots_new.push_back(dots[0]);
    now++;

    for (int i = 1; i < dots.size() - 1; i++) {

        if(dots[i].y == dots_new[now - 1].y || dots[i].x == dots_new[now - 1].x){
            ;
        }

        else if (dots[i - 1].y != dots[i + 1].y && dots[i - 1].x != dots[i + 1].x) {
            dots_new.push_back(dots[i]);
            now++;
        }
        else {
            dots_new.push_back(dots[i - 1]);
            now++;
        }

    }

    dots_new.push_back(dots[dots.size() - 1]);

    dots = dots_new;
}

void Path::two_dots_line() {
    new_angles();
    std::vector<int> to_delete = {};
    for (int i = 1; i < angles.size(); i++) {
        if (angles[i].y == angles[i - 1].y && angles[i].x == angles[i - 1].x) {
            to_delete.push_back(i);
        }
    }
    for (int i = to_delete.size() - 1; i >= 0; i--) {
        dots.erase(dots.begin() + to_delete[i], dots.begin() + to_delete[i] + 1);
    }
}

void Path::line_filter() {
    new_angles();
    std::vector<int> to_delete = {};
    // no neighbours forward
    for (int i = 1; i + 1 < angles.size(); i++) {
        if (abs(angles[i - 1].y) + abs(angles[i - 1].x) == 1){
            if (angles[i - 1].y == 0) {
                if (dots[i].x == dots[i + 1].x && angles[i - 1].x * angles[i + 1].x >= 0) {
                    to_delete.push_back(i);
                }
            }
            else if (angles[i - 1].x == 0) {
                if (dots[i].y == dots[i + 1].y && angles[i - 1].y * angles[i + 1].y >= 0) {
                    to_delete.push_back(i);
                }
            }
        }
    }
    for (int i = to_delete.size() - 1; i >= 0; i--) {
        dots.erase(dots.begin() + to_delete[i], dots.begin() + to_delete[i] + 1);
    }
    
    to_delete.clear();
    new_angles();
    
    // no neighbours backwards
    for (int i = (int) angles.size() - 1; i >= 1; i--) {
        if (abs(angles[i].x) + abs(angles[i].y) == 1) {
            if (angles[i].y == 0) {
                if (angles[i - 1].x * angles[i].x >= 0) {
                    to_delete.push_back(i);
                }
            }
            else if (angles[i].x == 0) {
                if (angles[i - 1].y * angles[i].y >= 0) {
                    to_delete.push_back(i);
                }
            }
        }
    }
    for (int i = 0; i + 1 < to_delete.size(); i++) {
        dots.erase(dots.begin() + to_delete[i], dots.begin() + to_delete[i] + 1);
    }
    
}

void Path::no_random_dots(int intense) {
    new_angles();
    for (int i = 2; i + 2 < angles.size(); i += 2) { // to allow angles[i - 2] and angles[i] 

        if ((angles[i - 1].y * angles[i - 1].y + angles[i - 1].x * angles[i - 1].x <= intense)
            && (angles[i].y * angles[i].y + angles[i].x * angles[i].x <= intense)) {

            angles.erase(angles.begin() + i, angles.begin() + i + 2);
            dots.erase(dots.begin() + i, dots.begin() + i + 2);
        }

    }
}

void Path::simple() {
    two_dots_line();
    line_filter();
    two_dots_line();
    no_random_dots(14);
    no_random_dots(14);
    two_dots_line();
}

std::string Path::L() {
    if (dots.size() >= 2) {
        std::string cont = "";

        cont = "M " + std::to_string(dots[0].x) + " " + std::to_string(dots[0].y) + "\n";

        for (int i = 1; i < dots.size(); i++) {
                cont += "L " + std::to_string(dots[i].x) + " " + std::to_string(dots[i].y) + "\n";
        }

        return "<path fill-opacity=\"0\" stroke=\"rgb(" + rgb[0] + ", " + rgb[1] + ", " + rgb[2] + ")\"\nd=\""
            + cont + "\"\n/>\n";
    }
    else {
        return "";
    }
}


std::string Path::points_only() {
    std::string cont = "";
    
    for (int i = 0; i < dots.size(); i++) {
        cont += "<circle cx=\"" + std::to_string(dots[i].x) + "\" cy=\"" + std::to_string(dots[i].y) + "\" r=\"1\"/>\n";
    }
    return cont;
}


std::string Path::create_path() {
    std::string cont = "";
    auto out = [&](std::string& cont) {
        return "<path fill-opacity=\"0\" stroke=\"rgb(" + rgb[0] + ", " + rgb[1] + ", " + rgb[2] + ")\"\nd=\""
            + cont + "\"\n/>\n";
    };
    if (dots.size() > 2) {
        
        auto write_arc = [&](double rx, double ry, char cw, int k_safe, int i_safe, int num) {
            std::string writing = "";
            
            writing += "A " + std::to_string(rx) + ' ' + std::to_string(ry) + " 0 0 " + cw + ' ' + std::to_string(dots[k_safe].x) + ' ' + std::to_string(dots[k_safe].y) + '\n';
            return writing;
        };

        new_angles();
        char cw = '0';
        bool line;
        double ctg, rx = 0, ry = 0;
        double b, c, m, n;
        int k = 0;
        int k_safe = 0;
        int num = 0;
        int i_safe = -1;

        cont = "M " + std::to_string(dots[0].x) + " " + std::to_string(dots[0].y) + "\n";

        for (int i = 1; i < dots.size() - 1; i++) {

            line = true;
            if (angles[i - 1].x >= -2 && angles[i - 1].x <= 2 && (angles[i - 1].x > 0) == (angles[i].x > 0)) {

                if (abs(angles[i - 1].y) >= abs(angles[i].y) && (angles[i - 1].y > 0) == (angles[i].y > 0)) {
                    k = i + 1;

                    // y1/x1 <= y0/x0
                    for (; dots.size() - 1 > k && angles[i - 1].y * angles[k - 1].y > 0 && angles[i - 1].x * angles[k - 1].x > 0 &&
                        abs(angles[k - 2].y * angles[k - 1].x) >= abs(angles[k - 1].y * angles[k - 2].x); k++);

                    if (k >= i + 3 && abs(dots[k].y - dots[i].y) > 6) {
                        if ((num == 1 || num == 4) && (cw == ((angles[i].x > 0) != (angles[i].y > 0)) + '0') && (angles[i_safe - 1].y > 0) == (angles[i].y > 0)) {
                            i = i_safe;
                        }
                        else if (num != 0) {
                            cont += write_arc(rx, ry, cw, k_safe, i_safe, num);
                        }
                        num = 1;
                        cw = ((angles[i].x > 0) != (angles[i].y > 0)) + '0';

                        b = abs(dots[k].y - dots[i - 1].y);
                        c = abs(dots[k].x - dots[i - 1].x);
                        n = dots[k].x - dots[k - 1].x;
                        m = dots[k].y - dots[k - 1].y;
                        ctg = abs(c * c / (2 * b * c + m / n * b * b));
                        rx = c + ctg * b;
                        ry = sqrt(b * b + (b * b * b * b * ctg * ctg) / (2 * b * c * ctg + c * c));

                        i_safe = i;
                        k_safe = k;
                        i = k;
                        line = false;
                        
                    } //1+

                }

                else if (abs(angles[i - 1].y) <= abs(angles[i].y) && (angles[i - 1].y > 0) == (angles[i].y > 0)) {
                    k = i + 1;
                    
                    // y0/x0 <= y1/x1
                    for (; dots.size() - 1 > k && angles[i - 1].y * angles[k - 1].y > 0 && angles[i - 1].x * angles[k - 1].x > 0 &&
                        abs(angles[k - 2].y * angles[k - 1].x) <= abs(angles[k - 1].y * angles[k - 2].x); k++);

                    if (k >= i + 3 && abs(dots[k].x - dots[i].x) > 6) {
                        if ((num == 2 || num == 3) && (cw == ((angles[i].x > 0) == (angles[i].y > 0)) + '0') && (angles[i_safe - 1].y > 0) == (angles[i].y > 0)) {
                            i = i_safe;
                        }
                        else if (num != 0) {
                            cont += write_arc(rx, ry, cw, k_safe, i_safe, num);
                        }
                        num = 2;
                        cw = ((angles[i].x > 0) == (angles[i].y > 0)) + '0';

                        c = abs(dots[k].x - dots[i - 1].x);
                        b = abs(dots[k].y - dots[i - 1].y);
                        n = dots[k].x - dots[k - 1].x;
                        m = dots[k].y - dots[k - 1].y;

                        ctg = abs(c * c / (2 * b * c + m / n * b * b));
                        rx = c + ctg * b;
                        ry = sqrt(b * b + (b * b * b * b * ctg * ctg) / (2 * b * c * ctg + c * c));

                        k_safe = k;
                        i_safe = i;
                        i = k;
                        line = false;
                    
                    } //2+
                }
            }

            else if (angles[i - 1].y >= -2 && angles[i - 1].y <= 2 && (angles[i - 1].y > 0) == (angles[i].y > 0)) {

                if (abs(angles[i - 1].x) >= abs(angles[i].x) && (angles[i - 1].x > 0) == (angles[i].x > 0)) {
                    k = i + 1;

                    // x0/y0 >= x1/y1
                    for (; dots.size() - 1 > k && angles[i - 1].y * angles[k - 1].x > 0 && angles[i - 1].x * angles[k - 1].x > 0 &&
                        abs(angles[k - 2].x * angles[k - 1].y) >= abs(angles[k - 1].x * angles[k - 2].y); k++);

                    if (k >= i + 2 && abs(dots[k].x - dots[i].x) > 6) {
                        if ((num == 2 || num == 3) && (cw == ((angles[i].x > 0) == (angles[i].y > 0)) + '0') && (angles[i_safe - 1].y > 0) == (angles[i].y > 0)) {
                            i = i_safe;
                        }
                        else if (num != 0) {
                            cont += write_arc(rx, ry, cw, k_safe, i_safe, num);
                        }
                        num = 3;
                        cw = ((angles[i].x > 0) == (angles[i].y > 0)) + '0';

                        b = abs(dots[i - 1].x - dots[k].x);
                        c = abs(dots[i - 1].y - dots[k].y);
                        m = dots[k].x - dots[k - 1].x;
                        n = dots[k].y - dots[k - 1].y;

                        ctg = abs(c * c / (2 * b * c + m / n * b * b));
                        ry = c + ctg * b;
                        rx = sqrt(b * b + (b * b * b * b * ctg * ctg) / (2 * b * c * ctg + c * c));

                        i_safe = i;
                        k_safe = k;
                        i = k;
                        line = false;
                    } //3+
                    

                }

                else if (abs(angles[i - 1].x) <= abs(angles[i].x) && (angles[i - 1].x > 0) == (angles[i].x > 0)) {
                    k = i + 1;
                    
                    // x0/y0 <= x1/y1
                    for (; dots.size() - 1 > k && angles[i - 1].y * angles[k - 1].y > 0 && angles[i - 1].x * angles[k - 1].x > 0 &&
                        abs(angles[k - 2].x * angles[k - 1].y) <= abs(angles[k - 1].x * angles[k - 2].y); k++);

                    if (k >= i + 2 && abs(dots[k].x - dots[i].x) > 6) {
                        if ((num == 1 || num == 4) && (cw == ((angles[i].x > 0) != (angles[i].y > 0)) + '0') && (angles[i_safe - 1].y > 0) == (angles[i].y > 0)) {
                            i = i_safe;
                        }
                        else if (num != 0) {
                            cont += write_arc(rx, ry, cw, k_safe, i_safe, num);
                        }
                        num = 4;
                        cw = ((angles[i].x > 0) != (angles[i].y > 0)) + '0';

                        b = abs(dots[i - 1].x - dots[k].x);
                        c = abs(dots[i - 1].y - dots[k].y);
                        m = dots[k].x - dots[k - 1].x;
                        n = dots[k].y - dots[k - 1].y;

                        ctg = abs(c * c / (2 * b * c + m / n * b * b));
                        ry = c + ctg * b;
                        rx = sqrt(b * b + (b * b * b * b * ctg * ctg) / (2 * b * c * ctg + c * c));

                        i_safe = i;
                        k_safe = k;
                        i = k;
                        line = false;
                        
                    } //4+

                }
            }

            if (line) {
                if (num != 0) {
                    cont += write_arc(rx, ry, cw, k_safe, i_safe, num);
                }

                num = 0;
                cont += "L " + std::to_string(dots[i].x) + " " + std::to_string(dots[i].y) + "\n";
            }
            else if (i >= dots.size() - 2 && num != 0) {
                cont += write_arc(rx, ry, cw, k_safe, i_safe, num);

            }
        }


        cont += "L " + std::to_string(dots[dots.size() - 1].x) + " " + std::to_string(dots[dots.size() - 1].y) + "\n";

        return out(cont);
    }

    else if (dots.size() == 4) {

        if (dots[0] == dots[2])
            return "";

        else {
            cont = "M " + std::to_string(dots[0].x) + " " + std::to_string(dots[0].y) + "\n";
            cont += "L " + std::to_string(dots[1].x) + " " + std::to_string(dots[1].y) + "\n";

            return out(cont);
        }

    }
    else {
        return "";
    }
}

