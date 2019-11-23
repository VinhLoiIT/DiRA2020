#include "detectlane.h"
#include "laneline.h"

DetectLane::DetectLane()
: rgb{}
, depth{}
, leftLane{nullptr}
, rightLane{nullptr}
, debug{debug}
{
    
    // setUseOptimized(true);
    // setNumThreads(4);
 
    // cvCreateTrackbar("Hough", "Threshold", &hough_lowerbound, max_houghThreshold);
    //cvCreateTrackbar("Hough_minLinlength", "Threshold", &minLinlength, 150);
    cvCreateTrackbar("MinShadow H", "Threshold", &minLaneInShadow[0], 255);
    cvCreateTrackbar("MinShadow S", "Threshold", &minLaneInShadow[1], 255);
    cvCreateTrackbar("MinShadow V", "Threshold", &minLaneInShadow[2], 255);
    cvCreateTrackbar("MaxShadow H", "Threshold", &maxLaneInShadow[0], 255);
    cvCreateTrackbar("MaxShadow S", "Threshold", &maxLaneInShadow[1], 255);
    cvCreateTrackbar("MaxShadow V", "Threshold", &maxLaneInShadow[2], 255);
    cvCreateTrackbar( "Min Threshold:", "Threshold", &lowThreshold, 15);

    leftLane = new LeftLane();
    rightLane = new RightLane();
}

DetectLane::~DetectLane(){
    delete leftLane;
    delete rightLane;
} 

void DetectLane::updateDepth(const Mat& src)
{
    this->depth = src.clone();
}

void DetectLane::updateRGB(const Mat& src)
{
    this->rgb = src.clone();
}


void DetectLane::processDepth(){
    if (this->depth.empty())
    {
        return;
    }
    imshow("depth", this->depth);


    Mat samples(this->depth.rows * this->depth.cols, 3, CV_32F);
    for( int y = 0; y < this->depth.rows; y++ )
        for( int x = 0; x < this->depth.cols; x++ )
        for( int z = 0; z < 3; z++)
            samples.at<float>(y + x*this->depth.rows, z) = this->depth.at<Vec3b>(y,x)[z];

    int clusterCount = lowThreshold;
    Mat labels;
    int attempts = 5;
    Mat centers;
    kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers );


    Mat new_image( this->depth.size(), this->depth.type() );
    for( int y = 0; y < this->depth.rows; y++ )
        for( int x = 0; x < this->depth.cols; x++ )
        { 
        int cluster_idx = labels.at<int>(y + x*this->depth.rows,0);
        new_image.at<Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0);
        new_image.at<Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1);
        new_image.at<Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2);
        }

    Mat birdviewdepth = birdviewTransformation(new_image);
    imshow("birdviewdepth",birdviewdepth);
    imshow( "clustered image", new_image );
 

}

Point DetectLane::calculateError() {
    if (this->rgb.empty())
    {
        return Point{birdheight / 2, birdwidth};
    }
    imshow("rgb", this->rgb);
    

    Mat binary = preprocess(this->rgb);

    Mat shadowMask = shadow(this->rgb);
    bitwise_or(binary, shadowMask, binary);
    imshow("binary", binary);
    
    Mat birdview = birdviewTransformation(binary);
    Mat morphBirdview = morphological(birdview);
    imshow("birdview", birdview);

    cv::Mat birdviewColor;
    cv::cvtColor(birdview, birdviewColor, cv::COLOR_GRAY2BGR);

    leftLane->update(birdview);
    rightLane->update(birdview);

    // return {};

    // leftLane->show(birdviewColor);
    // rightLane->show(birdviewColor);

    // auto leftParams = leftLane->getLineParams();
    // auto rightParams = rightLane->getLineParams();
    
    // for (int y = 0; y < birdviewColor.rows; y++)
    // {
    //     float xLeft = getXByY(*leftParams, y);
    //     float xRight = getXByY(*rightParams, y);
    //     int xMid = static_cast<int>((xLeft + xRight) / 2);
    //     cv::circle(birdviewColor, Point{xMid, y}, 5, cv::Scalar{0,0,255}, -1);
    // }

    // cv::imshow("Lanes", birdviewColor);
    // cv::waitKey(0);
    
    int turn = detectSigns(this->rgb);

    Point currentCarPosition = Hough(ROI(binary), this->rgb);

    currentCarPosition.x += turn * 120;

    return currentCarPosition;
}

Mat DetectLane::shadow(const Mat& src) {
    Mat shadow, hsv;
    cvtColor(src, hsv, CV_BGR2HSV);
    inRange(hsv, Scalar(minLaneInShadow[0], minLaneInShadow[1], minLaneInShadow[2]),
                    Scalar(maxLaneInShadow[1], maxLaneInShadow[1], maxLaneInShadow[2]), shadow);
    return shadow;
}

int DetectLane::detectSigns(const Mat& src) {
    Mat blue;
    int turnFactor = 0;

    cvtColor(src, blue, CV_BGR2HSV);
    inRange(blue, Scalar(minBlue[0], minBlue[1], minBlue[2]), Scalar(maxBlue[0], maxBlue[1], maxBlue[2]), blue);
    imshow("blue", blue);

    Mat canvas = Mat::zeros(src.size(), CV_8UC3);
    vector<vector<Point>> contours;
    findContours(blue, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    if (!contours.empty()) {
        
        vector<vector<Point>> contoursPoly((int)contours.size());
        vector<Rect> boundRect((int)contours.size());

        for (int i = 0; i < (int)contours.size(); ++i) {
            approxPolyDP(contours[i], contoursPoly[i], 3, true);
            boundRect[i] = boundingRect(contoursPoly[i]);    
        }

        int maxArea = 0, bestRect;
        for (int i = 0; i < (int)boundRect.size(); ++i) {
            int boundRect_W = abs(boundRect[i].tl().x - boundRect[i].br().x);
            int boundRect_H = abs(boundRect[i].tl().y - boundRect[i].br().y);
            if (boundRect_W * boundRect_H > maxArea) {
                maxArea = boundRect_W * boundRect_H;
                bestRect = i;
            }
        }

        if (maxArea < 125) goto skip;

        Point topLeft = boundRect[bestRect].tl();
        Point bottomRight = boundRect[bestRect].br();
        rectangle(canvas, topLeft, bottomRight, Scalar(0, 0, 255), 2, 8, 0);

        int rectW = abs(topLeft.x - bottomRight.x) * 4;
        int rectH = abs(topLeft.y - bottomRight.y) * 4;
        Mat zoom(rectW, rectH, CV_8UC1);
        Point2f boundingBox[4], zoomBox[4];
        
        boundingBox[0] = topLeft;
        boundingBox[1] = Point(bottomRight.x, topLeft.y);
        boundingBox[2] = bottomRight;
        boundingBox[3] = Point(topLeft.x, bottomRight.y);

        zoomBox[0] = Point(0, 0);
        zoomBox[1] = Point(rectW, 0);
        zoomBox[2] = Point(rectW, rectH);
        zoomBox[3] = Point(0, rectH);

        Mat M = getPerspectiveTransform(boundingBox, zoomBox);
        warpPerspective(blue, zoom, M, zoom.size(), INTER_LINEAR, BORDER_CONSTANT);

        int cntLeftOnes = 0, cntRightOnes = 0;
        for (int i = 0; i < rectW; ++i)
            for (int j = 0; j < rectH; ++j) 
                if ((int)zoom.at<uchar>(i, j) == 255) {
                    if (i < rectW / 2) ++cntLeftOnes;
                        else ++cntRightOnes;
                }

        cntLeftOnes = cntLeftOnes * cntLeftOnes;
        cntRightOnes = cntRightOnes * cntRightOnes;
        turnFactor = cntLeftOnes > cntRightOnes ? 1 : -1;   
    }

    skip: 
    addWeighted(src, 0.5, canvas, 1, 1, canvas);
    imshow("SignsDetector", canvas);
    return turnFactor;
}

Point DetectLane::Hough(const Mat& img, const Mat& src) {
    vector<Vec4f> lines;
    Mat HoughTransform = Mat::zeros(src.size(), CV_8UC3);
    float cntL = 0.0, cntR = 0.0;
    float leftSlope_average = 0.0;
    float rightSlope_average = 0.0;
    float leftYintercept_average = 0.0;
    float rightYintercept_average = 0.0;

    HoughLinesP(img, lines, 1, CV_PI / 180, votes, minLinlength, maxLineGap);

    for (int i = 0; i < (int)lines.size(); ++i) {
        Vec4f t = lines[i];
        float slope = (t[1] - t[3]) / (t[0] - t[2]);
        float Yintercept = t[1] - slope * t[0];
        
        if (slope < 0.0) { 
            leftSlope_average += slope, leftYintercept_average += Yintercept, cntL += 1.0;
        }
        else {
            rightSlope_average += slope, rightYintercept_average += Yintercept, cntR += 1.0;
        }
    }

    if (cntL > 0) {
        leftSlope_average /= cntL;
        leftYintercept_average /= cntL;
        drawLine(leftSlope_average, leftYintercept_average, HoughTransform);
    }

    if (cntR > 0) {
        rightSlope_average /= cntR;
        rightYintercept_average /= cntR;
        drawLine(rightSlope_average, rightYintercept_average, HoughTransform);
    }

    float midY = offsetY;
    float midX = offsetX;

    if (fabs(rightSlope_average) < 0.05) cntR = 0;
    if (fabs(leftSlope_average) < 0.05) cntL = 0;
    
    if (cntL == 0.0 && cntR > 0) {
        midX = (midY - rightYintercept_average) / rightSlope_average;
    } 
    else if (cntL > 0 && cntR == 0.0) {
        midX = (midY - leftYintercept_average) / leftSlope_average;
    }
    else if (cntL > 0 && cntR > 0) {
        midX = (((midY - leftYintercept_average) / leftSlope_average) + ((midY - rightYintercept_average) / rightSlope_average)) / 2.0;
    }

    addWeighted(src, 0.5, HoughTransform, 1, 1, HoughTransform);
    circle(HoughTransform, Point(midX, midY), 3, Scalar(0, 0, 255), -1);
    circle(HoughTransform, Point(offsetX, offsetY), 3, Scalar(0, 255, 0), -1);
    imshow("HoughLines", HoughTransform);

    return Point(midX, midY);
}

void DetectLane::drawLine(float slope, float y_intercept, Mat &HoughTransform) {
    float y0 = 240.0;
    float y1 = 1.0;
    float x0 = (y0 - y_intercept) / slope;
    float x1 = (y1 - y_intercept) / slope;
    line(HoughTransform, Point(x0, y0), Point(x1, y1), Scalar(0, 255, 0), 2, LINE_AA);
}

Mat DetectLane::ROI(const Mat &src) {
    //Crop region of interest (ROI)
    int W = src.size().width;
    int H = src.size().height;
    Mat RegionOfInterest;
    Mat mask = Mat::zeros(src.size(), CV_8UC1);

    vector<Point> vertices;
    vertices.push_back(Point(0, skyline));
    vertices.push_back(Point(W, skyline));
    vertices.push_back(Point(W, H));
    vertices.push_back(Point(0, H));
    
    fillConvexPoly(mask, vertices, Scalar(255, 255, 255), 8, 0);
    bitwise_and(src, mask, RegionOfInterest);
    
    //imshow("RegionOfInterest", RegionOfInterest);
    return RegionOfInterest;
}

Mat DetectLane::morphological(const Mat& img) {
    Mat dst;

    dilate(img, dst, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    erode(dst, dst, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));

    return dst;
}

Mat DetectLane::preprocess(const Mat& src) { 
    Mat hsv, binary;
    
    cvtColor(src, hsv, COLOR_BGR2HSV);
    inRange(hsv, Scalar(minThreshold[0], minThreshold[1], minThreshold[2]), 
                 Scalar(maxThreshold[0], maxThreshold[1], maxThreshold[2]), binary);

    return binary;
}

Mat DetectLane::birdviewTransformation(const Mat& src) {
    int W = src.size().width;
    int H = src.size().height;

    Point2f srcVertices[4];
    srcVertices[0] = Point(0, skyline);
    srcVertices[1] = Point(W, skyline);
    srcVertices[2] = Point(0, H);
    srcVertices[3] = Point(W, H);
 
    Point2f dstVertices[4];
    dstVertices[0] = Point(0, 0);
    dstVertices[1] = Point(birdwidth, skyline);
    dstVertices[2] = Point(skyline, birdheight);
    dstVertices[3] = Point(birdwidth - skyline, birdheight);

    Mat M = getPerspectiveTransform(srcVertices, dstVertices);
    Mat birdview(birdwidth, birdheight, CV_8UC3);
    warpPerspective(src, birdview, M, birdview.size(), INTER_LINEAR, BORDER_CONSTANT);

    // imshow("birdview", birdview);
    return birdview;
}