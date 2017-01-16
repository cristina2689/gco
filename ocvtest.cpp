#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <algorithm>
#include <numeric>

#define A image.at<uchar>(i-1, j-1)
#define B image.at<uchar>(i-1, j+1)
#define C image.at<uchar>(i+1, j-1)
#define D image.at<uchar>(i+1, j+1)

using namespace cv;
using namespace std;

const int metaDataWidth = 20;
const int metaDataHeight = 20;

class DigitalZoom {
    Mat *image;

    int T1 = 1, T2 = 2; // threshold for edge detection
public:
    DigitalZoom(Mat oImage) {
        image = new Mat(2*oImage.rows + metaDataWidth, 2*oImage.cols + metaDataHeight, oImage.type());
        image->setTo(Scalar(255, 255, 255)); // white image
    }
    void locallyAdaptiveZoomingAlgorithm(Mat oImage) {
        Mat image(2 * oImage.rows + metaDataWidth, 2 * oImage.cols + metaDataHeight, oImage.type());
        image.setTo(Scalar(255,255,255));
        CV_Assert(2 * oImage.rows + metaDataWidth == image.rows);
        CV_Assert(2 * oImage.cols + metaDataHeight == image.cols);

        for (int i = 0; i < oImage.rows; i++) {
            for (int j = 0; j < oImage.cols; j++) {
                int ucolor = oImage.at<uchar>(i,j);

                int nLine = 2 * i, nCol = 2 * j;
                CV_Assert(nLine < image.rows);
                CV_Assert(nCol < image.cols);
                image.at<uchar>(nLine,nCol) = ucolor;
            }
        }

        //namedWindow("Display window", WINDOW_AUTOSIZE );// Create a window for display.
        //imshow("Display window", image);
#if 0
        vector<int> compressionparams;
        compressionparams.push_back(CV_IMWRITE_JPEG_QUALITY);
        imwrite("ScaledImage.jpg", *image, compressionparams);
        imshow("Scaled Image", *image);
#endif
       // Edge recognition TODO
        cout << image.rows << " " << image.cols << endl;

        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                // check margins
                if (i > 0 && j > 0 && i < image.rows-1 && j < image.cols -1) {
                    // Type A pixels - check range of pixels
                    if (i % 2 == 1 && j % 2 == 1) {
                        vector<uchar> nPixels = {image.at<uchar>(i-1, j-1),
                                                image.at<uchar>(i+1, j-1),
                                                image.at<uchar>(i-1, j+1),
                                                image.at<uchar>(i+1, j-1)
                                                };
                        if (range(nPixels) < T1) {
                            uchar averageVal = std::accumulate(nPixels.begin(), nPixels.end(), 0)/nPixels.size();
                            image.at<uchar>(i,j) = averageVal;
                        }
                    }
                    // Is SW-NE edge in image
                    else if (std::abs(A - D) > T2 && std::abs(A - D) > std::abs(B - C)) {
                        image.at<uchar>(i,j) = (B + C)/2;
                    }
                    // Is NW-SE edge in image
                    else if (std::abs(B - C) > T2 && std::abs(B - C) > std::abs(A - D)) {
                        image.at<uchar>(i,j) = (A + B)/2;
                    }
                    // Is NS edge in image, then leave the X pixel undefined
                    else if (std::abs(A - D) > T1 && std::abs(B - C) > std::abs(A - D) && (A-D)*(B-C) > 0) {
                        // H1 pixel
                        image.at<uchar>(i - 1, j) = (A+B)/2;
                        // H2 pixel
                        image.at<uchar>(i + 1,j) = (C+D)/2;
                    }
                    // Is EW edge in image, then leave the X pixel undefined
                    else if (std::abs(A - D) > T1 && std::abs(B - C) > T1 && (A-D)*(B-C) < 0) {
                        // H1 pixel
                        image.at<uchar>(i, j - 1) = (A+C)/2;
                        // H2 pixel
                        image.at<uchar>(i, j + 1) = (B+D)/2;
                    }
                // *
                // P and * P *
                // *

                if (image.at<uchar>(i,j) == 255 && ((i %2 == 1 && j % 2 == 0) || (i%2 == 0 && j % 2 == 1))) {
                    uchar a = image.at<uchar>(i-1, j);
                    uchar b = image.at<uchar>(i+1, j);
                    uchar x1 = image.at<uchar>(i, j -1);
                    uchar x2 = image.at<uchar>(i, j + 1);

                    if (std::abs(a - b) > T2 && std::abs(a-b) > std::abs(x1 - x2)) {
                            image.at<uchar>(i,j) = (a + b)/2;
                    } else if (std::abs(x2 - x1) > T2 && std::abs(x2 -x1) > std::abs(a - b)) {
                            image.at<uchar>(i,j) = (x1 + x2)/2;
                    } else {
                        image.at<uchar>(i,j) = (a + b)/2;
                    }
                }

            }
            }
        }
#if 0
        for (int i = 1; i < image.rows - 1; i++) {
            for (int j = 1; j < image.cols - 1; j++) {
                if (image.at<uchar>(i,j) == 255 && (i % 2 == 0 || j % 2 == 0)) {
                    vector<uchar> pixels;
                    if (A != 255)
                        pixels.push_back(A);
                    if (B != 255)
                        pixels.push_back(B);
                    if (C != 255)
                        pixels.push_back(C);
                    if (D != 255)
                        pixels.push_back(D);
                    if (pixels.size() > 0)
                        image.at<uchar>(i,j) = accumulate(pixels.begin(), pixels.end(), 0)/pixels.size();
                    //image.at<uchar>(i,j) = 0;
                }

            }
        }
#endif
        namedWindow("Display window", WINDOW_AUTOSIZE );// Create a window for display.
        imshow("Display window", image);

        vector<int> compressionparams;
        compressionparams.push_back(CV_IMWRITE_JPEG_QUALITY);
        imwrite("stupidpuppy3.jpg", image, compressionparams);
    }
    // highest diff between 2 pixels
    int range(const vector<uchar> &pixels) {
        auto res = minmax_element(pixels.begin(), pixels.end());
        return (*res.first - *res.second);
    }

    void printZoomedImage() {
        Size s = image->size();
        cout << s.height << " " << s.width << endl;
        namedWindow("Display window", WINDOW_AUTOSIZE );// Create a window for display.
        imshow("Display window", *image);
    }
    ~DigitalZoom () {
        delete image;
    }
};



int main( int argc, char** argv )
{
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    Mat image, imgOut;
    image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);   // Read the file

    if(! image.data )                               // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    resize(image, imgOut, Size(), 2, 2, INTER_LANCZOS4);
    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow("Display window", imgOut);                // Show our image inside it.
    //------------------
    // My algorithm here
    DigitalZoom dz(image);
    dz.locallyAdaptiveZoomingAlgorithm(image);
    //dz.printZoomedImage();

#if 0
    vector<int> compressionparams;
    compressionparams.push_back(CV_IMWRITE_JPEG_QUALITY);
    imwrite("ScaledStrawberry.jpg", imgOut, compressionparams);
    imshow("Scaled Image", imgOut);
#endif
    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
