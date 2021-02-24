#ifndef TELEOP_H
#define TELEOP_H
#include "ros/ros.h"
#include <ros/network.h>

#include<geometry_msgs/Twist.h>

namespace reinovo_controller {

using namespace std;

class DriveControl
{
public:
    DriveControl(std::map<std::string,std::string> remappings);
    ~DriveControl();

private:
    /* data */
};

}

#endif //FILE_H
