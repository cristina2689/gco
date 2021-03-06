#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <algorithm>
#include <numeric>

#define A image->at<uchar>(i-1, j-1)
#define B image->at<uchar>(i-1, j+1)
#define C image->at<uchar>(i+1, j-1)
#define D image->at<uchar>(i+1, j+1)

using namespace cv;
using namespace std;

const int metaDataWidth = 20;
const int metaDataHeight = 20;

class DigitalZoom {
    Mat *image;     // current image
    Mat *oImage; // original image

    int T1 = 7, T2 = 20; // threshold for edge detection
public:
    DigitalZoom(Mat *oImage) {
        image = new Mat(2*oImage->rows, 2*oImage->cols, oImage->type());
        image->setTo(Scalar(255, 255, 255)); // white image
        this->oImage = oImage;
    }
    void locallyAdaptiveZoomingAlgorithm() {
        CV_Assert(2 * oImage->rows == image->rows);
        CV_Assert(2 * oImage->cols == image->cols);

        for (int i = 0; i < oImage->rows; i++) {
            for (int j = 0; j < oImage->cols; j++) {
                int ucolor = oImage->at<uchar>(i,j);

                int nLine = 2 * i, nCol = 2 * j;
                CV_Assert(nLine < image->rows);
                CV_Assert(nCol < image->cols);
                image->at<uchar>(nLine,nCol) = ucolor;
            }
        }
#if 0
        vector<int> compressionparams;
        compressionparams.push_back(CV_IMWRITE_JPEG_QUALITY);
        imwrite("ScaledImage.jpg", *image, compressionparams);
        imshow("Scaled Image", *image);
#endif
       // Edge recognition
        cout << image->rows << " " << image->cols << endl;

        for (int i = 0; i < image->rows; i++) {
            for (int j = 0; j < image->cols; j++) {
                // check margins
                if (i > 0 && j > 0 && i < image->rows-1 && j < image->cols -1) {
                    // Type A pixels - check range of pixels
                    if (i % 2 == 1 && j % 2 == 1) {
                        vector<uchar> nPixels = {image->at<uchar>(i-1, j-1),
                                                image->at<uchar>(i+1, j-1),
                                                image->at<uchar>(i-1, j+1),
                                                image->at<uchar>(i+1, j-1)
                                                };
                        if (range(nPixels) < T1) {
                            uchar averageVal = std::accumulate(nPixels.begin(), nPixels.end(), 0)/nPixels.size();
                            image->at<uchar>(i,j) = averageVal;
                        }
                    }
                    // Is SW-NE edge in image
                    else if (std::abs(A - D) > T2 && std::abs(A - D) > std::abs(B - C)) {
                        image->at<uchar>(i,j) = (B + C)/2;
                    }
                    // Is NW-SE edge in image
                    else if (std::abs(B - C) > T2 && std::abs(B - C) > std::abs(A - D)) {
                        image->at<uchar>(i,j) = (A + B)/2;
                    }
                    // Is NS edge in image, then leave the X pixel undefined
                    else if (std::abs(A - D) > T1 && std::abs(B - C) > std::abs(A - D) && (A-D)*(B-C) > 0) {
                        // H1 pixel
                        image->at<uchar>(i - 1, j) = (A+B)/2;
                        // H2 pixel
                        image->at<uchar>(i + 1,j) = (C+D)/2;
                    }
                    // Is EW edge in image, then leave the X pixel undefined
                    else if (std::abs(A - D) > T1 && std::abs(B - C) > T1 && (A-D)*(B-C) < 0) {
                        // H1 pixel
                        image->at<uchar>(i, j - 1) = (A+C)/2;
                        // H2 pixel
                        image->at<uchar>(i, j + 1) = (B+D)/2;
                    }
                // *
                // P and * P *
                // *
                if (image->at<uchar>(i,j) == 255 && ((i %2 == 1 && j % 2 == 0) || (i%2 == 0 && j % 2 == 1))) {
                    uchar a = image->at<uchar>(i-1, j);
                    uchar b = image->at<uchar>(i+1, j);
                    uchar x1 = image->at<uchar>(i, j -1);
                    uchar x2 = image->at<uchar>(i, j + 1);

                    if (std::abs(a - b) > T2 && std::abs(a-b) > std::abs(x1 - x2)) {
                            image->at<uchar>(i,j) = (a + b)/2;
                    } else if (std::abs(x2 - x1) > T2 && std::abs(x2 -x1) > std::abs(a - b)) {
                            image->at<uchar>(i,j) = (x1 + x2)/2;
                    } else {
                        image->at<uchar>(i,j) = (a + b)/2;
                    }
                }

            }
            }
        }
    }
    // highest diff between 2 pixels
    int range(const vector<uchar> &pixels) {
        auto res = minmax_element(pixels.begin(), pixels.end());
        return (*res.first - *res.second);
    }

    void addLanczOS4() {
        Mat imgOut;
        resize(*oImage, imgOut, Size(), 2, 2, INTER_LANCZOS4);
        CV_Assert(imgOut.rows == image->rows);
        CV_Assert(imgOut.cols == image->cols);

        for (int i = 0; i < image->rows; i++) {
            for (int j = 0; j < image->cols; j++) {
                if (image->at<uchar>(i,j) == 255) {
                    image->at<uchar>(i,j) = imgOut.at<uchar>(i,j);
                }
            }
        }
    }

    void printZoomedImage() {
        Mat out;
        Size s = image->size();
        cout << s.height << " " << s.width << endl;
        namedWindow("Display window", WINDOW_AUTOSIZE );// Create a window for display.

        vector<int> compressionparams;
        compressionparams.push_back(CV_IMWRITE_JPEG_QUALITY);
        imwrite("stupidpuppy5.jpg", *image, compressionparams);

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
    //------------------
    // My algorithm here
    DigitalZoom dz(&image);
    dz.addLanczOS4();
    dz.locallyAdaptiveZoomingAlgorithm();
    dz.printZoomedImage();
    //

    // Lanczos
    //Mat imgOut;
    //resize(image, imgOut, Size(), 2, 2, INTER_LANCZOS4);

    // Write Image to file
#if 0
    vector<int> compressionparams;
    compressionparams.push_back(CV_IMWRITE_JPEG_QUALITY);
    imwrite("ScaledStrawberry.jpg", imgOut, compressionparams);
    imshow("Scaled Image", imgOut);
#endif

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
