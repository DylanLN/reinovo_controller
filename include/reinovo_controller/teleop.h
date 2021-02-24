#ifndef TELEOP_H
#define TELEOP_H
#include "ros/ros.h"
#include <ros/network.h>

#include<geometry_msgs/Twist.h>

namespace reinovo_controller {

using namespace std;

class Teleop
{
public:
    Teleop(std::map<std::string,std::string> remappings, string name);
    ~Teleop();

    //前后左右
    void vx_pub(float data);
    void vy_pub(float data);
    void vth_pub(float data);
    void v_stop();
private:
    ros::Publisher vel_pub;
};

}

#endif //FILE_H
