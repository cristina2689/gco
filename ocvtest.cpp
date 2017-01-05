#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

const int metaDataWidth = 20;
const int metaDataHeight = 20;

class DigitalZoom {
    Mat *image;

    float T1, T2; // threshold for edge detection
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
                Vec3b color = oImage.at<Vec3b>(Point(i,j));
                int nLine = 2 * i, nCol = 2 * j;
                CV_Assert(nLine < image.rows);
                CV_Assert(nCol < image.cols);
                image.at<Vec3b>(Point(nLine, nCol)) = color;
            }
        }
        namedWindow("Display window", WINDOW_AUTOSIZE );// Create a window for display.
        imshow("Display window", image);
#if 0
        vector<int> compressionparams;
        compressionparams.push_back(CV_IMWRITE_JPEG_QUALITY);
        imwrite("ScaledImage.jpg", *image, compressionparams);
        imshow("Scaled Image", *image);
#endif
       // Edge recognition TODO
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
#if 0
    resize(image, imgOut, Size(), 2, 2);
    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow("Display window", image );                // Show our image inside it.
#endif
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
