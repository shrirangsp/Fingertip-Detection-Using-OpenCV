/*Project Name: Fingerprint detection using OpenCV*/


#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc.hpp>
using namespace std;
using namespace cv;

int findMaxContourArea(vector<vector<Point> > contours);
void contourCreation(Mat img);

int threshold_value = 220;
int threshold_type = 0; //binary threshold
int const max_BINARY_value = 255;

Mat img, imgSegmented, frame;
Mat drawingContours = Mat::zeros(img.rows, img.cols, CV_8UC1);

int main()
{

    //Start Webcam
    VideoCapture cap = VideoCapture(0);

    int slider = 0;
    int slider_max = 255;

    namedWindow("Control", 1);

    int iLowH = 0;
    int iHighH = 179;

    int iLowS = 0;
    int iHighS = 255;

    int iLowV = 0;
    int iHighV = 255;

    //Trackbars for controlliing Hue Saturation Value
    cvCreateTrackbar("LowH", "Control", &iLowH, 255);
    cvCreateTrackbar("HighH", "Control", &iHighH, 255);

    cvCreateTrackbar("LowS", "Control", &iLowS, 255);
    cvCreateTrackbar("HighS", "Control", &iHighS, 255);

    cvCreateTrackbar("LowV", "Control", &iLowV, 255);
    cvCreateTrackbar("HighV", "Control", &iHighV, 255);

    while(1)
    {
        cap >> frame;
        cap.read(frame);


        Mat imgHSV;

        imshow("Original Frame", frame);
        cvtColor(frame, imgHSV, CV_BGR2HSV);


        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgSegmented);

        //Morphological Operations

        dilate(imgSegmented, imgSegmented, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        erode(imgSegmented, imgSegmented, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        threshold(imgSegmented, img, threshold_value, max_BINARY_value, threshold_type);

        imshow("Thresholded", img);


        contourCreation(img);

        waitKey(25);
    }
}
void contourCreation(Mat img)
{

    std::vector<std::vector<cv::Point> > contours;
    findContours(img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

    Mat drawing = Mat::zeros(img.rows, img.cols, CV_8UC1);

    int maxContourAreaLoc = findMaxContourArea(contours);
    drawContours(drawing, contours, maxContourAreaLoc, Scalar(255, 255, 255));

    std::vector<std::vector<cv::Point> > contoursDrawing;
    drawingContours = drawing.clone();

    int i = drawingContours.rows;
    while (contoursDrawing.size() < 4 && i > 0)
    {
        line(drawingContours, Point(0, i), Point(drawingContours.cols, i), Scalar(0, 0, 0), 5, 8, 0);
        findContours(drawingContours, contoursDrawing, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
        i--;
    }

    if (contoursDrawing.size() == 4)
    {
        vector<RotatedRect> minRect(contoursDrawing.size());
        float scale = 0.34;
        for (size_t i = 0; i < contoursDrawing.size(); i++)
        {
            drawContours(drawingContours, contoursDrawing, i, Scalar(255, 255, 255));
            minRect[i] = minAreaRect(Mat(contoursDrawing[i]));

            Point2f rect_points[4];
            minRect[i].points(rect_points);

            //Reduce rectangle size
            if (minRect[i].size.height > minRect[i].size.width) {
                minRect[i].center = (rect_points[1] + rect_points[2]) / 2 + (rect_points[0] - rect_points[1]) / 6;
                minRect[i].size.height = (float)scale * (minRect[i].size.height);
            }
            else
            {
                minRect[i].center = (rect_points[2] + rect_points[3]) / 2 + (rect_points[0] - rect_points[3]) / 6;
                minRect[i].size.width = (float)scale * (minRect[i].size.width);
            }

            minRect[i].points(rect_points);
            for (int j = 0; j < 4; j++)
            {
                line(drawingContours, rect_points[j], rect_points[(j + 1) % 4], Scalar(255, 0, 0), 1, 8);
                line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(255, 0, 0), 1, 8);
            }
        }
    }
    imshow("DrawingContours", drawingContours);
   imshow("Output", frame);
}
int findMaxContourArea(vector<vector<Point> > contours)
{
    int maxContourAreaLoc = 0;
    double maxContourArea = 0;

    for (size_t i = 0; i < contours.size(); i++)
    {
        if (contourArea(contours[i], false) > maxContourArea)
        {
            maxContourArea = contourArea(contours[i], false);
            maxContourAreaLoc = i;
        }
    }
    return maxContourAreaLoc;
}
