#ifndef PLANNING_H
#define PLANNING_H

#include <opencv2/core.hpp>
#include <cds_msgs/lane.h>
#include <cds_msgs/sign.h>
#include <cds_msgs/SetMap.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Int8.h>
#include <ros/ros.h>
#include <dynamic_reconfigure/server.h>
#include <dynamic_reconfigure/client.h>
#include <planning/PlanningConfig.h>
#include <common/libcommon.h>
#include <list>
#include <memory>
#include "car_control/CarControlConfig.h"

enum class TurningState : int {
  NOT_TURNING,
  IS_TURNING,
  DONE,
};

enum class SignState : int {
  NOSIGN,
  LEFT,
  FORWARD,
  RIGHT,
  NO_LEFT,
  NO_RIGHT,
  STOP,
  DONE,
};

enum class AvoidObjectState : int {
  READY,
  ON_AVOIDING,
  DONE,
};

enum class Map : int {
  RED = 0,
  BLUE = 1,
};

class Planning {
 public:
  Planning();
  void planning();

 private:
  void planningWhileTurning(const float& min_speed, const float& max_speed);
  void planningWhileDriveStraight(const float& min_speed,
                                  const float& max_speed);

 public:
  void configCallback(planning::PlanningConfig& config, uint32_t level);
  void configCarControlCallback(car_control::CarControlConfig& config,
                                uint32_t level);

  void laneCallback(const cds_msgs::lane& msg);
  void signCallback(const cds_msgs::sign& msg);
  void objectCallback(const std_msgs::Bool& msg);
  void turnCallback(const std_msgs::Int8& msg);

 private:
  bool setMapService(cds_msgs::SetMap::Request& req,
                     cds_msgs::SetMap::Response& resp);
  void setValuesBasedOnMap(Map map);

  void requestResetLane(int lane);
  bool requestRecover(int lane);
  bool requestIsAbleToTurn(int direction);

 private:
  void onTurnTimeout(const ros::TimerEvent& event);
  void onObjectTimeout(const ros::TimerEvent& event);

 private:
  void publishMessage(const cv::Point& drivePoint, float speed);
  cv::Point driveCloseToLeft();
  cv::Point driveCloseToLeftFromRight();
  cv::Point driveCloseToRight();
  cv::Point driveCloseToRightFromLeft();
  cv::Point driveStraightFromLeft();
  cv::Point driveStraightFromRight();
  cv::Point driveStraight();
  cv::Point turnLeft();
  cv::Point turnRight();

  cv::Point lastDrivePoint;

 private:
  ros::NodeHandle _nh;
  ros::Publisher _controlPub;

  ros::Subscriber _laneSub;
  ros::Subscriber _signSub;
  ros::Subscriber _objSub;
  ros::Subscriber _crossroadSub;

  ros::ServiceServer _setMapSrv;
  ros::ServiceClient _recoverClient, _resetLaneClient;
  dynamic_reconfigure::Server<planning::PlanningConfig> _configServer;
  dynamic_reconfigure::Server<car_control::CarControlConfig>
      _configCarControlServer;
  dynamic_reconfigure::Client<car_control::CarControlConfig> _carConfigClient;
  car_control::CarControlConfig _carConfig;
  ros::Timer _objectTimer, _turnTimer;

  std::shared_ptr<LineParams> leftParams, rightParams;
  std::vector<cv::Rect> objectBoxes;

  TurningState turningState;
  int turningDirect;
  int turningTime = 25;  // 1/10 seconds

  AvoidObjectState objectState;
  int objectDirect;
  int avoidObjectTime = 35;  // 1/10 seconds

  int drivePointY = 240 - 100;
  int clipMinX = 0, clipMaxX = 320;
  int offset = 80;

  int countTurning, delay;   // for turning
  SignState prevSign, sign;  // for signDetect
  int object;                // for signDetect

  int rate;

  int laneToDriveCloseTo;

  int turnSign = 0;
  bool isTurnable = false;
  int lastPriority = 0;

  Map currentMap;
};

#endif
