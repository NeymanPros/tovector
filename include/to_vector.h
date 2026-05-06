#ifndef TO_VECTOR
#define TO_VECTOR

#include <opencv2/core.hpp>

#include <array>

using namespace cv;

/// Making the image only black and white.
void white_and_black(Mat &c, const int contrast);


/// This function find countours of an image.
/// Writes contours from the first cv::Mat into the second one.
Mat neib(Mat &c);


/// Extracting lines from contour.
std::vector<Point> line(Mat& nc, const int i, const int j, unsigned char picture, unsigned char condition);

/// Starting part of svg code.
std::string start(int width, int height);


/// This class creates the white background for an image.
class Rectangle {
private:
    std::string width = "0";
    std::string height = "0";
    int x = 0;
    int y = 0;
public:
    Rectangle() {};
    
    Rectangle(int a, int b) {
        x = a;
        y = b;
    }
    Rectangle(std::string w, std::string h);
    Rectangle(int a, int b, int c, int d);
    ~Rectangle() = default;

    std::string write(int r, int g, int b);
    std::string write_no_xy(int r, int g, int b);
};

/// Ending part of svg file.
std::string end();

/// Interprets vector of dots into svg code.
class Path {
private:
    std::vector<Point> dots;
    std::array <std::string, 3> rgb = { "0", "0", "0" };
    std::vector<Point> angles;
public:
    Path() {};

    Path(const std::vector<Point>& new_dots) {
        if (new_dots.size() > 1) {
            dots = new_dots;
        }
    };

    ~Path() {
        dots.clear();
        angles.clear();
    };

    /// Finds the shift from some point to the next one in a path::dotII vector.
    void new_angles();

    /// Leaves only the first and the last dots in diagonal lines.
    void two_dots_line();
    
    /// Transforms reduntant dot pair into single dot.
    void line_filter();
    void two_dots_straight();

    /// Tries to remove unwanted spikes from dotII.
    void no_random_dots(int intense);

    /// Use optimal from series of tests set of path::two_dots_line(), path::no_random_dots() and path::two_dots_diagonal() 
    void simple();

    /// Makes a part of an image fully consist of lines.
    std::string L();

    /// Places circles where each dots point.
    std::string points_only();

    /// Creates a part of an image with a chance of adding ellipse curves.
    std::string create_path();
};

#endif // TO_VECTOR
