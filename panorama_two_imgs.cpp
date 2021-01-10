#include <stdio.h>

#include <opencv2/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;
/**
 * @brief IF DEBUG flag is true display the intermidiate results
 *
 */
#define DEBUG false

/**
 * @brief Display the image
 *
 * @param img: input opencv image
 * @param title: title string of the window
 */
void display_image(const Mat &img, string title = "Display_Window") {
  namedWindow(title, WINDOW_AUTOSIZE);
  imshow(title, img);
}

/**
 * @brief read the input images from the command line arguments; assuming only
 * two input images
 *
 * @param inputs : output array of Mat
 * @param argv   : command line arguments
 */

void read_input_images(vector<Mat> &inputs, char **argv) {
  inputs[0] = imread(argv[1], IMREAD_COLOR);
  inputs[1] = imread(argv[2], IMREAD_COLOR);
  if (!inputs[0].data || !inputs[1].data) {
    cout << "Check the input images \n";
    exit(0);
  }
  if (DEBUG) {
    cout << "Displaying input images\n";
    display_image(inputs[0], "im-1");
    waitKey(0);
    display_image(inputs[1], "im-2");
    waitKey(0);
  }
}

/**
 * @brief detect features in the input images
 *
 * @param inputs : input images
 * @param keypoints   : a vector of keypoints vectors
 * @param keypoints_descriptors : a vector of Matrix describing kpts
 */

void detect_features(vector<Mat> &inputs, vector<vector<KeyPoint>> &keypoints,
                     vector<Mat> &keypoints_descriptors) {
  vector<Mat> grays(2);
  vector<Mat> outputs(2);
  Ptr<Feature2D> detector = ORB::create();
  cvtColor(inputs[0], grays[0], CV_BGR2GRAY, 1);
  cvtColor(inputs[1], grays[1], CV_BGR2GRAY, 1);
  detector->detectAndCompute(grays[0], noArray(), keypoints[0],
                             keypoints_descriptors[0], false);
  detector->detectAndCompute(grays[1], noArray(), keypoints[1],
                             keypoints_descriptors[1], false);
  drawKeypoints(inputs[0], keypoints[0], outputs[0], Scalar::all(-1),
                DrawMatchesFlags::DEFAULT);
  drawKeypoints(inputs[1], keypoints[1], outputs[1], Scalar::all(-1),
                DrawMatchesFlags::DEFAULT);
  cout << "Displaying input images\n";
  // display_image(outputs[0], "outim-1");
  // display_image(outputs[1], "outim-2");
}
/**
 * @brief store good matches on a vector
 *
 * @param inputs : input images
 * @param keypoints_descriptors : a vector of Matrix describing kpts
 * @param matches
 */
void get_good_matches(vector<Mat> &keypoints_descriptors,
                      vector<DMatch> &matches) {
  BFMatcher matcher(NORM_HAMMING);

  matcher.match(keypoints_descriptors[0], keypoints_descriptors[1], matches);

  std::sort(matches.begin(), matches.end());
  auto it = matches.end();
  while (it != matches.begin() && !matches.empty() && (*it).distance > 0) {
    //cout << (*it).distance << " " << matches[0].distance * 4 << endl;
    if ((*it).distance < 4) {
      it = matches.erase(it);
    } else
      it--;
  }
}

int main(int argc, char **argv) {
  if (argc < 3) {
    cout << " Usage: ./panorama_two_imgs [required] input_image_01 [required] "
            "input_image_02 [optional] output_image \n";
    return -1;
  }
  // declare required variables
  vector<Mat> inputs(2);
  vector<vector<KeyPoint>> keypoints(inputs.size());
  vector<Mat> keypoints_descriptors(inputs.size());
  vector<DMatch> matches;
  Mat H;
  vector<char> match_mask;
  Mat result_stitched;
  Mat output_matches;
  // read the input images
  read_input_images(inputs, argv);

  // [TODO] write a function to detect corners
  // member function of ORB class to detect the feature points
  // and compute their descriptors in both images. Display these feature points
  // on the images
  detect_features(inputs, keypoints, keypoints_descriptors);

  // [TODO] write a function to get matches
  // write a function to visualize matches
  get_good_matches(keypoints_descriptors, matches);
  drawMatches(inputs[0], keypoints[0], inputs[1], keypoints[1], matches,
              output_matches);
  display_image(output_matches, "Output matches");

  // [TODO] write a function to get homography
  // write a function to visualize mosaic
  vector<Point2f> keypts1, keypts2;
  for (int i = 0; i < matches.size(); i++) {
    keypts1.push_back(keypoints[0][matches[i].queryIdx].pt);
    keypts2.push_back(keypoints[1][matches[i].trainIdx].pt);
  }
  H = findHomography(keypts1, keypts2, RANSAC, 4, match_mask);
  warpPerspective(inputs[1], result_stitched, H.inv(),
                  inputs[1].size() + inputs[0].size());

  inputs[0].copyTo(result_stitched(Rect(0, 0, inputs[0].cols, inputs[0].rows)));
  display_image(result_stitched, "Output stitched");
  // write the output image
  if (argc == 4) {
    imwrite(argv[3], result_stitched);
  }
  waitKey(0);

  return 0;
}
