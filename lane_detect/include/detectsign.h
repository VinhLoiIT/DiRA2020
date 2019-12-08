#ifndef DETECT_SIGN_H
#define DETECT_SIGN_H

#include <opencv2/core.hpp>
#include <list>

class DetectSign
{
public:
    DetectSign(const cv::Mat& LEFT_TEMPLATE, const cv::Mat& RIGHT_TEMPLATE);
    void updateRGB(const cv::Mat& rgb);
    void updateDepth(const cv::Mat& depth);
    int detect();

private:
    int detectOneFrame();
    

private:
    cv::Mat rgb;
    cv::Mat depth;

    cv::Mat LEFT_TEMPLATE, RIGHT_TEMPLATE;
    
    int MAX_DIFF = 0;
    const int MAX_FRAME_COUNT = 7;
    std::list<int> recentDetects;
    int canny = 255;
    int votes = 22;
    int frameCount = 0;
    // int minBlue[3] = {100, 90, 35};
    int minBlue[3] = {77, 10, 20};
    int maxBlue[3] = {170, 142, 60};

    int maxPercent = 30;
};



#endif