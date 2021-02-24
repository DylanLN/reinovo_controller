#include <ros/ros.h>
#include <ros/network.h>
#include "../include/reinovo_controller/teleop.h"

namespace reinovo_controller {

using namespace std;

Teleop::Teleop(std::map<std::string,std::string> remappings, string name)
{
    ros::init(remappings, "teleop");
	if ( ! ros::master::check() ) {
        cout << "ros master is no connect!" << endl;
    }
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
    ros::NodeHandle nh_;

    vel_pub = nh_.advertise<geometry_msgs::Twist>(name, 100);    //发布速度信息
}

Teleop::~Teleop()
{
    cout << "~teleop" << endl;
    vel_pub.shutdown();
    if(ros::isStarted()) {
      ros::shutdown(); // explicitly needed since we use ros::start();
      ros::waitForShutdown();
    }
}

void Teleop::vx_pub(float data)
{
    geometry_msgs::Twist msg;
    msg.linear.x = data;
    vel_pub.publish(msg);
}

void Teleop::vy_pub(float data)
{
    geometry_msgs::Twist msg;
    msg.linear.y = data;
    vel_pub.publish(msg);
}

void Teleop::vth_pub(float data)
{
    geometry_msgs::Twist msg;
    msg.angular.z = data;
    vel_pub.publish(msg);
}

void Teleop::v_stop()
{
    geometry_msgs::Twist msg;
    vel_pub.publish(msg);
}

}
