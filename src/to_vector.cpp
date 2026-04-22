#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <cmath>
#include <array>

#include <to_vector.h>
using namespace cv;

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
void neib(Mat &c, Mat &nc, int i, int j) {
    if ((c.at<Vec3b>(i, j) != c.at<Vec3b>(i - 1, j))     || (c.at<Vec3b>(i, j) != c.at<Vec3b>(i + 1, j)) ||
        (c.at<Vec3b>(i, j) != c.at<Vec3b>(i, j - 1))     || (c.at<Vec3b>(i, j) != c.at<Vec3b>(i, j + 1)) ||
        (c.at<Vec3b>(i, j) != c.at<Vec3b>(i - 1, j - 1)) || (c.at<Vec3b>(i, j) != c.at<Vec3b>(i - 1, j + 1)) ||
        (c.at<Vec3b>(i, j) != c.at<Vec3b>(i + 1, j - 1)) || (c.at<Vec3b>(i, j) != c.at<Vec3b>(i + 1, j + 1))) {

            nc.at<Vec3b>(i, j) = { c.at<Vec3b>(i, j)[0], c.at<Vec3b>(i, j)[1], c.at<Vec3b>(i, j)[2] };
    }
}


/// Extracting lines from contour.
std::vector<int> line(Mat& nc, Mat& c, const int i, const int j, unsigned char picture, unsigned char condition) {
    std::vector<int> dots;
    int a = i;
    int b = j;
    int run = 0;
    nc.at<Vec3b>(i, j) = { picture, picture, picture };

    dots.push_back(i);
    dots.push_back(j);

    short count = 0;
    int a1 = a;
    int b1 = b;
    for (int now = 2; now > 0;) {

        if (a > 0 && a < c.rows && b > 0 && b < c.cols) {

            count = 0;

            if (nc.at<Vec3b>(a - 1, b)[1] == condition) {
                count += 1;
                a1 = a - 1;
                b1 = b;
            }
            if (nc.at<Vec3b>(a, b + 1)[1] == condition) {
                count += 1;
                a1 = a;
                b1 = b + 1;
            }
            if (nc.at<Vec3b>(a, b - 1)[1] == condition) {
                count += 1;
                a1 = a;
                b1 = b - 1;
            }
            if (nc.at<Vec3b>(a + 1, b)[1] == condition) {
                count += 1;
                a1 = a + 1;
                b1 = b;
            }


            if (count == 0) {
                if (dots[0] - dots[now - 2] <= 1 && dots[now - 2] - dots[0] <= 1 &&
                    dots[1] - dots[now - 1] <= 1 && dots[now - 1] - dots[1] <= 1){
                    dots.push_back(dots[0]);
                    dots.push_back(dots[1]);
                }
                now = -10;
            }

            else if (count == 1) {
                nc.at<Vec3b>(a, b) = { picture,picture,picture };
                dots.push_back(a1);
                dots.push_back(b1);
                now += 2;
                run = 0;

                a = a1;
                b = b1;
            }

            else if (count == 2) {
                
                if (run > 4) {
                    nc.at<Vec3b>(a, b) = { picture,picture,picture };
                }
                else {
                    run += 1;
                }

                dots.push_back(a1);
                dots.push_back(b1);
                now += 2;

                a = a1;
                b = b1;
            }

            else if (count > 2) {
                
                dots.push_back(a1);
                dots.push_back(b1);
                now += 2;
                
                a = a1;
                b = b1;
            }
        }
        else {
            now = -10;
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
        "\" fill=\"rgb(" + std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b) + ")\"/>\n";
}


std::string end() {
    return "</svg>";
}


void Path::new_angles() {
    angles.clear();
    
    for (int i = 2; i < dots.size(); i++)
        angles.push_back(dots[i] - dots[i - 2]);
}
    
void Path::two_dots_line() {
    std::vector<int> dots_new = {};
    int now = 0;

    if (dots.size() < 2) {
        return;
    }

    dots_new.push_back(dots[0]);
    dots_new.push_back(dots[1]);
    now += 2;

    for (int i = 2; i < dots.size() - 2; i += 2) {

        if(dots[i] == dots_new[now - 2] || dots[i + 1] == dots_new[now - 1]){
            ;
        }

        else if (dots[i - 2] != dots[i + 2] && dots[i - 1] != dots[i + 3]) {
            dots_new.push_back(dots[i]);
            dots_new.push_back(dots[i + 1]);
            now += 2;
        }
        else {
            dots_new.push_back(dots[i - 2]);
            dots_new.push_back(dots[i - 1]);
            now += 2;
        }

    }

    dots_new.push_back(dots[dots.size() - 2]);
    dots_new.push_back(dots[dots.size() - 1]);

    dots = dots_new;
}

void Path::two_dots_diagonal() {
    new_angles();

    for (int i = 2; i < angles.size() - 2; i += 2) {
     
        if ((angles[i] == angles[i + 2] && angles[i] == angles[i - 2]) && (angles[i + 1] == angles[i - 1] && angles[i + 1] == angles[i + 3])) {

            angles.erase(angles.begin() + i, angles.begin() + i + 2);
            dots.erase(dots.begin() + i, dots.begin() + i + 2);
            i -= 2;

        }

    }

    new_angles();
}

void Path::no_random_dots(int intense) {

    for (int i = 2; i + 4 < angles.size(); i += 2) {

        if ((angles[i] * angles[i] + angles[i + 1] * angles[i + 1] <= intense)
            && (angles[i + 2] * angles[i + 2] + angles[i + 3] * angles[i + 3] <= intense)) {

            angles.erase(angles.begin() + i, angles.begin() + i + 2);
            dots.erase(dots.begin() + i, dots.begin() + i + 2);
        }

    }

}

void Path::simple() {
    this->two_dots_line();
    this->two_dots_diagonal();
    this->no_random_dots(8);
    this->no_random_dots(8);
    this->two_dots_diagonal();
}

std::string Path::L() {
    if (dots.size() > 3) {
        std::string cont = "";

        cont = "M " + std::to_string(dots[1]) + " " + std::to_string(dots[0]) + "\n";

        for (int i = 2; i < dots.size(); i += 2) {
                cont += "L " + std::to_string(dots[i + 1]) + " " + std::to_string(dots[i]) + "\n";
        }

        return "<path fill-opacity=\"0\" stroke=\"rgb(" + rgb[0] + " " + rgb[1] + " " + rgb[2] + ")\"\nd=\""
            + cont + "\"\n/>\n";
    }
    else {
        return "";
    }
}


std::string Path::points_only() {
    std::string cont = "";
    
    for (int i = 0; i < dots.size(); i += 2) {
        cont += "<circle x=" << dots[i] << " y = " << dots[i + 1] << " r=0.5 />\n";
    }
    return cont;
}


std::string Path::create_path() {
    if (dots.size() > 4) {
        std::string cont = "";
        
        auto write_arc = [&](double rx, double ry, char cw, int k_safe, int i_safe, int num) {
            std::string writing = "A " + std::to_string(rx) + ' ' + std::to_string(ry) + " 0 0 " + cw + ' ' + std::to_string(dots[k_safe + 1]) + ' ' + std::to_string(dots[k_safe]) + '\n';
            
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

        cont = "M " + std::to_string(dots[1]) + " " + std::to_string(dots[0]) + "\n";

        for (int i = 2; i < dots.size() - 2; i += 2) {

            line = true;
            if ((angles[i - 1] == -1 || angles[i - 1] == 1) && (angles[i - 1] > 0) == (angles[i + 1] > 0)) {

                if (abs(angles[i - 2]) >= abs(angles[i]) && (angles[i - 2] > 0) == (angles[i] > 0)) {
                    k = i;

                    //[k-2]/[k-1] >= [k]/[k+1]
                    for (; dots.size() - 2 > k && angles[i - 2] * angles[k] > 0 && angles[i - 1] * angles[k + 1] > 0 &&
                        abs(angles[k - 2] * angles[k + 1]) >= abs(angles[k] * angles[k - 1]); k += 2);

                    if (abs(dots[k] - dots[i]) > 6) {
                            if ((num == 1 || num == 4) && (cw == ((angles[i + 1] > 0) != (angles[i] > 0)) + '0') && (angles[i_safe] > 0) == (angles[i] > 0)) {
                                i = i_safe;
                            }
                            else if (num != 0) {
                                cont += write_arc(rx, ry, cw, k_safe, i_safe, num);
                            }
                            num = 1;
                            cw = ((angles[i + 1] > 0) != (angles[i] > 0)) + '0';

                            c = abs(dots[k + 1] - dots[i - 1]);
                            b = abs(dots[i - 2] - dots[k]);
                            n = dots[k + 1] - dots[k - 1];
                            m = dots[k] - dots[k - 2];
                            ctg = abs(c * c / (2 * b * c + m / n * b * b));
                            rx = c + ctg * b;
                            ry = sqrt(b * b + (b * b * b * b * ctg * ctg) / (2 * b * c * ctg + c * c));

                            i_safe = i;
                            k_safe = k;
                            i = k;
                            line = false;
                        
                    } //1+

                }

                else if (abs(angles[i - 2]) <= abs(angles[i]) && (angles[i - 2] > 0) == (angles[i] > 0)) {
                    k = i;
                    for (; dots.size() - 2 > k && angles[i - 2] * angles[k] > 0 && angles[i - 1] * angles[k + 1] > 0 &&
                        abs(angles[k - 2] * angles[k + 1]) <= abs(angles[k] * angles[k - 1]); k += 2);

                    if (abs(dots[k] - dots[i]) > 6) {
                        if ((num == 2 || num == 3) && (cw == ((angles[i + 1] > 0) == (angles[i] > 0)) + '0') && (angles[i_safe] > 0) == (angles[i] > 0)) {
                                i = i_safe;
                            }
                            else if (num != 0) {
                                cont += write_arc(rx, ry, cw, k_safe, i_safe, num);
                            }
                            num = 2;
                            cw = ((angles[i + 1] > 0) == (angles[i] > 0)) + '0';

                            c = abs(dots[k + 1] - dots[i - 1]);
                            b = abs(dots[i - 2] - dots[k]);
                            n = dots[k + 1] - dots[k - 1];
                            m = dots[k] - dots[k - 2];

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

            else if ((angles[i - 2] == -1 || angles[i - 2] == 1) && (angles[i - 2] > 0) == (angles[i] > 0)) {

                if (abs(angles[i - 1]) >= abs(angles[i + 1]) && (angles[i - 1] > 0) == (angles[i + 1] > 0)) {
                    k = i;

                    for (; dots.size() - 2 > k && angles[i - 2] * angles[k] > 0 && angles[i - 1] * angles[k + 1] > 0 &&
                        abs(angles[k - 1] * angles[k]) >= abs(angles[k + 1] * angles[k - 2]); k += 2);

                    if (abs(dots[k + 1] - dots[i + 1]) > 6) {
                        if ((num == 2 || num == 3) && (cw == ((angles[i + 1] > 0) == (angles[i] > 0)) + '0') && (angles[i_safe] > 0) == (angles[i] > 0)) {
                                i = i_safe;
                            }
                            else if (num != 0) {
                                cont += write_arc(rx, ry, cw, k_safe, i_safe, num);
                            }
                            num = 3;
                            cw = ((angles[i + 1] > 0) == (angles[i] > 0)) + '0';

                            b = abs(dots[k + 1] - dots[i - 1]);
                            c = abs(dots[i - 2] - dots[k]);
                            m = dots[k + 1] - dots[k - 1];
                            n = dots[k] - dots[k - 2];

                            ctg = abs(c * c / (2 * b * c + m / n * b * b));
                            ry = c + ctg * b;
                            rx = sqrt(b * b + (b * b * b * b * ctg * ctg) / (2 * b * c * ctg + c * c));

                            i_safe = i;
                            k_safe = k;
                            i = k;
                            line = false;
                    } //3+
                    

                }

                else if (abs(angles[i - 1]) <= abs(angles[i + 1]) && (angles[i - 1] > 0) == (angles[i + 1] > 0)) {
                    k = i;

                    for (; dots.size() - 2 > k && angles[i - 2] * angles[k] > 0 && angles[i - 1] * angles[k + 1] > 0 &&
                        abs(angles[k - 1] * angles[k]) <= abs(angles[k + 1] * angles[k - 2]); k += 2);

                    if (abs(dots[k + 1] - dots[i + 1]) > 6) {
                        if ((num == 1 || num == 4) && (cw == ((angles[i + 1] > 0) != (angles[i] > 0)) + '0') && (angles[i_safe] > 0) == (angles[i] > 0)) {
                                i = i_safe;
                            }
                            else if (num != 0) {
                                cont += write_arc(rx, ry, cw, k_safe, i_safe, num);
                            }
                            num = 4;
                            cw = ((angles[i + 1] > 0) != (angles[i] > 0)) + '0';

                            b = abs(dots[k + 1] - dots[i - 1]);
                            c = abs(dots[i - 2] - dots[k]);
                            m = dots[k + 1] - dots[k - 1];
                            n = dots[k] - dots[k - 2];

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
                cont += "L " + std::to_string(dots[i + 1]) + " " + std::to_string(dots[i]) + "\n";
            }
            else if (i >= dots.size() - 4 && num != 0) {
                cont += write_arc(rx, ry, cw, k_safe, i_safe, num);

            }
        }


        cont += "L " + std::to_string(dots[dots.size() - 1]) + " " + std::to_string(dots[dots.size() - 2]) + "\n";

        return "<path fill-opacity=\"0\" stroke=\"rgb(" + rgb[0] + " " + rgb[1] + " " + rgb[2] + ")\"\nd=\""
            + cont + "\"\n/>\n";
    }

    else if (dots.size() == 4) {

        if (dots[0] == dots[2] && dots[1] == dots[3])
            return "";

        else {
            cont = "M " + std::to_string(dots[1]) + " " + std::to_string(dots[0]) + "\n";
            cont += "L " + std::to_string(dots[3]) + " " + std::to_string(dots[2]) + "\n";

            return out();
        }

    }
    else {
        return "";
    }
}

std::string Path::out() {
    return "<path fill-opacity=\"0\" stroke=\"rgb(" + rgb[0] + " " + rgb[1] + " " + rgb[2] + ")\"\nd=\""
        + cont + "\"\n/>\n";
}


