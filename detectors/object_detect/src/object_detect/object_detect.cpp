#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cv_bridge/cv_bridge.h>
#include <limits>
#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <common/libcommon.h>
#include <std_msgs/Int8.h>
#include "object_detect/object_detect.h"

constexpr const char *CONF_OBJ_WINDOW = "ConfigObjectDetect";

enum OBJ_STRATEGY
{
    ON_RAW_BINARY = 0,
    ON_KMEAN,
    LEN
};

ObjectDetect::ObjectDetect()
    : pBackSub{cv::createBackgroundSubtractorMOG2() }
    , _nh{"object_detect"}
    , _serverConfig{_nh}
    , _it{_nh}
{
    std::string transport_hint = _nh.param<std::string>("transport_hint", "compressed");
    std::string objectTopic;
    ROS_ASSERT(ros::param::get("/object_topic", objectTopic));

    _serverConfig.setCallback(boost::bind(&ObjectDetect::configCallback, this, _1, _2));
    _objPub = _nh.advertise<std_msgs::Int8>(objectTopic, 1);

    std::string depth_topic, lane_seg_topic;
    ROS_ASSERT(ros::param::get("/depth_topic", depth_topic));
    ROS_ASSERT(ros::param::get("/lane_segmentation_topic", lane_seg_topic));
    _depthSub = _it.subscribe(depth_topic, 1, &ObjectDetect::updateDepthCallback, this, image_transport::TransportHints{transport_hint});
    _binarySub = _it.subscribe(lane_seg_topic, 1, &ObjectDetect::updateBinaryCallback, this, image_transport::TransportHints{transport_hint});

    // cv::namedWindow(CONF_OBJ_WINDOW, cv::WINDOW_GUI_NORMAL);
    // cv::createTrackbar("clusterCount", CONF_OBJ_WINDOW, &kCluster, 10);

    // cv::createTrackbar("strategy", CONF_OBJ_WINDOW, &strategy, OBJ_STRATEGY::LEN - 1);

    // cv::createTrackbar("objectROI_x", CONF_OBJ_WINDOW, &offsetROI_x, 320);
    // cv::createTrackbar("objectROI_y", CONF_OBJ_WINDOW, &offsetROI_y, 240);
    // // cv::createTrackbar("objectROI_w", CONF_OBJ_WINDOW, &objectROIRect.width, 320);
    // cv::createTrackbar("objectROI_h", CONF_OBJ_WINDOW, &objectROIRect.height, 240);
    // cv::createTrackbar("objectROI_offsetTop", CONF_OBJ_WINDOW, &objectROI_offsetTop, 100);

    // cv::createTrackbar("Detect threshold", CONF_OBJ_WINDOW, &detectThreshold, 100); // percents
    // cv::createTrackbar("DiffToKnowLeftRight", CONF_OBJ_WINDOW, &diffDirectPercent, 100); // percents

    // cv::createTrackbar("DepthThresholdMin", CONF_OBJ_WINDOW, &depthThresholdMin, 255); // percents
    // cv::createTrackbar("DepthThresholdMax", CONF_OBJ_WINDOW, &depthThresholdMax, 255); // percents

    // cv::createTrackbar("min H", CONF_OBJ_WINDOW, &configObjectmin[0], 255);
    // cv::createTrackbar("max H", CONF_OBJ_WINDOW, &configObjectmax[0], 255);
    // cv::createTrackbar("min S", CONF_OBJ_WINDOW, &configObjectmin[1], 255);
    // cv::createTrackbar("max S", CONF_OBJ_WINDOW, &configObjectmax[1], 255);
    // cv::createTrackbar("min V", CONF_OBJ_WINDOW, &configObjectmin[2], 255);
    // cv::createTrackbar("max V", CONF_OBJ_WINDOW, &configObjectmax[2], 255);
}

ObjectDetect::~ObjectDetect()
{
}

void ObjectDetect::configCallback(object_detect::ObjectDetectConfig &config, uint32_t level)
{
    kCluster = config.cluster_count;
    offsetROI_x = config.offsetROI_x;
    offsetROI_y = config.offsetROI_y;
    objectROIRect.height = config.objectROI_h;
    objectROI_offsetTop = config.objectROI_offsetTop;
    diffDirectPercent = config.diff_to_know_left_right;
    depthThresholdMin = config.depth_threshold_min;
    depthThresholdMax = config.depth_threshold_max;
    strategy = config.strategy;
}

void ObjectDetect::updateDepthCallback(const sensor_msgs::ImageConstPtr &msg)
{
    cv_bridge::CvImagePtr cv_ptr;
    cv::Mat out;
    try
    {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        if (!cv_ptr->image.empty())
        {
            cv::cvtColor(cv_ptr->image, depth, cv::COLOR_BGR2GRAY);
        }
    }
    catch (cv_bridge::Exception &e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

void ObjectDetect::updateBinaryCallback(const sensor_msgs::ImageConstPtr &msg)
{
    cv_bridge::CvImagePtr cv_ptr;
    cv::Mat out;
    try
    {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::MONO8);
        if (!cv_ptr->image.empty())
        {
            binary = cv_ptr->image.clone();
        }
    }
    catch (cv_bridge::Exception &e)
    {
        ROS_ERROR("Could not convert from '%s' to '%s'.", msg->encoding.c_str(), sensor_msgs::image_encodings::MONO8.c_str());
    }
}

// bool ObjectDetect::detect()
// {
//     if (this->depth.empty())
//     {
//         return false;
//     }
//     this->depth = birdviewTransformation(this->depth);

//     cv::rectangle(this->depth, objectROI, cv::Scalar{0, 0, 255}, 2);
//     cv::imshow("DepthObjectROI", this->depth);
//     cv::Mat objectROIImage = this->depth(objectROI);
//     cv::Mat kmeanImage = kmean(objectROIImage, kCluster);

//     // {
//     //     double minVal, maxVal;
//     //     cv::minMaxLoc(kmeanImage, &minVal, &maxVal, NULL, NULL);
//     //     int minValInt = cvRound(minVal), maxValInt = cvRound(maxVal);
//     //     cv::Mat mask = (kmeanImage != maxValInt);
//     //     cv::imshow("Mask", mask);
//     // }

//     cv::Mat hsv;
//     cv::Mat mask;
//     cv::cvtColor(kmeanImage, hsv, cv::COLOR_BGR2HSV);
//     cv::inRange(hsv, cv::Scalar(configObjectmin[0], configObjectmin[1], configObjectmin[2]),
//                     cv::Scalar(configObjectmax[0], configObjectmax[1], configObjectmax[2]), mask);
//     cv::imshow("cropped", mask);

//     cv::Mat fgMask;
//     pBackSub->apply(kmeanImage, fgMask);

//     fgMask &= mask;
//     cv::imshow("foreground", fgMask);

//     float percent = cv::countNonZero(fgMask) * 100.0f/ (fgMask.rows * fgMask.cols);
//     std::cout << "percent :" << percent << std::endl;
//     if ( percent >= detectThreshold)
//     {
//         std::cout << "OBJECTTTTT" << std::endl;
//         return true;
//     }

//     return false;
// }
// new detect

int ObjectDetect::estimateDirect(const cv::Mat &binaryROI)
{
    if (binaryROI.rows < 10 || binaryROI.cols < 20)
    {
        // if ROI is too small
        return 0;
    }
    cv::Rect objectROI_left{0, 0, binaryROI.cols / 2, binaryROI.rows};
    cv::Rect objectROI_right{binaryROI.cols / 2, 0, binaryROI.cols / 2, binaryROI.rows};

    cv::Mat ImageLeft = binaryROI(objectROI_left);
    cv::Mat ImageRight = binaryROI(objectROI_right);

    int countLeft = cv::countNonZero(ImageLeft);
    int countRight = cv::countNonZero(ImageRight);

    float percentLeft = countLeft * 100.0f / (ImageLeft.rows * ImageLeft.cols);     // 0-100%
    float percentRight = countRight * 100.0f / (ImageRight.rows * ImageRight.cols); // 0-100%

    ROS_DEBUG("Percent left = %.2f, right = %.2f, diff = %.2f", percentLeft, percentRight, percentLeft - percentRight);
    if (abs(percentLeft - percentRight) > diffDirectPercent)
    {
        return (percentLeft > percentRight) ? static_cast<int>(LEFT) : static_cast<int>(RIGHT);
    }
    return static_cast<int>(NONE);
}

int ObjectDetect::getDirect(const cv::Rect &objectROI)
{
    int result = 0;
    switch (strategy)
    {
    case 1:
        result = getDirectOnKmean(objectROI);
        break;
    case 2:
        result = getDirectOnKmeanBGSub(objectROI);
        break;

    default:
    case 0:
        result = getDirectOnRawBinary(objectROI);
        break;
    }
    return result;
}

int ObjectDetect::getDirectOnRawBinary(const cv::Rect &objectROI)
{
    cv::Mat depthThresholded;
    depthThresholded = this->depth(objectROI);
    cv::inRange(depthThresholded, cv::Scalar{depthThresholdMin * 1.0}, cv::Scalar{depthThresholdMax * 1.0}, depthThresholded);

    // cv::imshow("DepthThresholded", depthThresholded);
    // cv::waitKey(1);
    return estimateDirect(depthThresholded);
}

int ObjectDetect::getDirectOnKmean(const cv::Rect &objectROI)
{
    // cv::Mat kmeanImage = kmean(this->depth, kCluster);
    // cv::imshow("KMean", kmeanImage);

    // cv::Mat objectROIImage = kmeanImage(objectROI);
    // cv::imshow("KMeanCrop", objectROIImage);

    // return 0;
    cv::Mat objectROIImage = this->depth(objectROI);
    cv::Mat kmeanImage = kmean(objectROIImage, kCluster);

    cv::Mat mask;
    cv::inRange(kmeanImage, cv::Scalar{depthThresholdMin * 1.0}, cv::Scalar{depthThresholdMax * 1.0}, mask);

    // cv::imshow(CONF_OBJ_WINDOW, mask);

    return estimateDirect(mask);
}

int ObjectDetect::getDirectOnKmeanBGSub(const cv::Rect &objectROI)
{
    cv::Mat depthThresholded;
    depthThresholded = this->depth(objectROI);
    cv::inRange(depthThresholded, cv::Scalar{depthThresholdMin * 1.0}, cv::Scalar{depthThresholdMax * 1.0}, depthThresholded);
    // showImage(CONF_OBJ_WINDOW, depthThresholded);
    cv::Mat fgMask;
    pBackSub->apply(this->depth, fgMask);
    int tmp = estimateDirect(depthThresholded);
    if (!tmp)
    {
        fgMask = fgMask(objectROI);
        depthThresholded &= fgMask;
        tmp = estimateDirect(depthThresholded);
    }

    // showImage(_depthThresholdedPublisher, "mono8", depthThresholded);

    return tmp;
}

int ObjectDetect::detectOneFrame()
{
    if (this->binary.empty() || this->depth.empty())
    {
        ROS_ERROR("Binary image empty or depth empty. Make sure subscribing right topic");
        return 0;
    }

    Hough(this->binary);
    ROS_DEBUG("objectROI = %d, %d, %d, %d", objectROIRect.x, objectROIRect.y, objectROIRect.width, objectROIRect.height);

    return getDirect(objectROIRect);
}

void ObjectDetect::Hough(const cv::Mat &binary)
{
    std::vector<cv::Vec4f> lines;
    cv::Mat HoughTransform = cv::Mat::zeros(binary.size(), CV_8UC3);
    cv::Vec2f lineL{0.0f, 0.0f}, lineR{0.0f, 0.0f};
    int cntL = 0, cntR = 0;
    float leftSlope_average = 0.0;
    float rightSlope_average = 0.0;
    float leftYintercept_average = 0.0;
    float rightYintercept_average = 0.0;

    cv::HoughLinesP(binary, lines, 1, CV_PI / 180, votes, minLinlength, maxLineGap);

    for (const cv::Vec4f &line : lines)
    {
        float slope = 0;
        if (abs(line[0] - line[2]) < 0.01f)
        {
            // vertical line, tan = inf
            slope = 0;
            continue;
        }
        else
        {
            slope = (line[1] - line[3]) / (line[0] - line[2]);
        }
        float Yintercept = line[1] - slope * line[0];

        if (slope < 0)
        {
            leftSlope_average += slope;
            leftYintercept_average += Yintercept;
            cntL++;
        }
        else
        {
            rightSlope_average += slope;
            rightYintercept_average += Yintercept;
            cntR++;
        }
    }

    if (cntL > 0)
    {
        leftSlope_average /= cntL;
        leftYintercept_average /= cntL;
        drawLine(leftSlope_average, leftYintercept_average, HoughTransform);
        lineL = cv::Vec2f{leftSlope_average, leftYintercept_average};
    }

    if (cntR > 0)
    {
        rightSlope_average /= cntR;
        rightYintercept_average /= cntR;
        drawLine(rightSlope_average, rightYintercept_average, HoughTransform);
        lineR = cv::Vec2f{rightSlope_average, rightYintercept_average};
    }

    float midY = offsetY;
    float midX = offsetX;

    if (fabs(rightSlope_average) < 0.05f)
        cntR = 0;
    if (fabs(leftSlope_average) < 0.05f)
        cntL = 0;

    if (cntL == 0 && cntR > 0)
    {
        midX = (midY - rightYintercept_average) / rightSlope_average;
    }
    else if (cntL > 0 && cntR == 0)
    {
        midX = (midY - leftYintercept_average) / leftSlope_average;
    }
    else if (cntL > 0 && cntR > 0)
    {
        midX = (((midY - leftYintercept_average) / leftSlope_average) + ((midY - rightYintercept_average) / rightSlope_average)) / 2.0f;
    }

    cv::Mat colorBinary;


    cv::Mat depthThresholded;
    cv::inRange(depth, depthThresholdMin, depthThresholdMax, depthThresholded);
    // cv::imshow("Depth", depthThresholded);
    // cv::waitKey(1);

    cv::cvtColor(depthThresholded, colorBinary, cv::COLOR_GRAY2BGR);
    cv::addWeighted(colorBinary, 0.5, HoughTransform, 1, 1, HoughTransform);

    cv::circle(HoughTransform, cv::Point(midX, midY), 3, cv::Scalar(0, 0, 255), -1);
    cv::circle(HoughTransform, cv::Point(offsetX, offsetY), 3, cv::Scalar(0, 255, 0), -1);

    if ((lineL[0] != 0) && (lineR[0] != 0.0f))
    {
        int left_most_x, right_most_x;
        // ROS_INFO("lineL = [%.2f, %.2f], lineR = [%.2f, %.2f]", lineL[0], lineL[1], lineR[0], lineR[1]);

        left_most_x = (offsetROI_y - lineL[1]) / lineL[0];
        right_most_x = (offsetROI_y - lineR[1]) / lineR[0];

        if (left_most_x > right_most_x)
        {
            std::swap(left_most_x, right_most_x);
        }

        // ROS_INFO("1: left_most_x = %d, right_most_x = %d", left_most_x, right_most_x);

        left_most_x = std::min(std::max(left_most_x - offsetROI_x, 0), binary.cols - 1);
        right_most_x = std::min(std::max(right_most_x + offsetROI_x, 0), binary.cols - 1);

        // ROS_INFO("2: left_most_x = %d, right_most_x = %d", left_most_x, right_most_x);

        if (abs(left_most_x - right_most_x) <= 10)
        {
            return;
        }

        objectROIRect.x = left_most_x;
        objectROIRect.y = std::max(0, offsetROI_y - objectROI_offsetTop);
        objectROIRect.width = std::abs(right_most_x - left_most_x);
        cv::rectangle(HoughTransform, objectROIRect, cv::Scalar{255, 0, 0}, 2);

        cv::imshow("HoughLines", HoughTransform);
        cv::waitKey(1);
        // showImage(_houghPublisher, "bgr8", HoughTransform);
    }
}

void ObjectDetect::drawLine(float slope, float y_intercept, cv::Mat &HoughTransform)
{
    float y0 = 240.0;
    float y1 = 1.0;
    float x0 = (y0 - y_intercept) / slope;
    float x1 = (y1 - y_intercept) / slope;
    cv::line(HoughTransform, cv::Point(x0, y0), cv::Point(x1, y1), cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
}

void ObjectDetect::update()
{
    int object = detectOneFrame();
    ROS_DEBUG("Object = %d", object);

    std_msgs::Int8 msg;
    msg.data = object;
    _objPub.publish(msg);

    // TODO: handle multiple object in one frame
    // geometry_msgs::Polygon bbox;
    // bbox.points.push_back

    // return object;
    // objectHistories.push_back(object);
    // if (objectHistories.size() > maxHistory)
    // {
    //     objectHistories.pop_front();
    // }

    // int cntLeft = std::count(objectHistories.begin(), objectHistories.end(), -1);
    // int cntRight = std::count(objectHistories.begin(), objectHistories.end(), 1);
    // int cntNone = std::count(objectHistories.begin(), objectHistories.end(), 0);

    // int max = std::max(cntLeft, std::max(cntRight, cntNone));
    // if (max == cntLeft)
    //     return -1;
    // else if (max == cntRight)
    //     return 1;
    // else
    //     return 0;
}