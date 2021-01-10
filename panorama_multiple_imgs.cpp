#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/stitching.hpp"

using namespace cv;
using namespace std;

int main(int argc, char **argv) {
vector<Mat> inputs;
Mat ouput;

string arg = argv[1];
vector<String> filename;

string dir_name = arg + "/*";
glob(dir_name, filename, false);

vector<Mat> images;
size_t nbr = filename.size();
for (size_t i = 0; i < nbr; i++) inputs.push_back(imread(filename[i]));

Stitcher stitcher = Stitcher::createDefault();
Stitcher::Status status = stitcher.stitch(inputs, ouput);

if (status != Stitcher::OK) {
cout << "can't stitch images, error code = " << int(status) << "\n";
} else {
cout << "success!\n";
imshow("Output", ouput);
imwrite("../Output.png", ouput);
}

return 0;
}


if test