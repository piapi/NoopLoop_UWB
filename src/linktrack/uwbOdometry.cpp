#include <ros/ros.h>
#include <serial/serial.h>

#include <unordered_map>

#include "nlink_unpack/nlink_utils.h"
#include "protocol_extracter/nprotocol_extracter.h"
#include "init.h"

#include <nlink_parser/LinktrackAnchorframe0.h>
#include <nlink_parser/LinktrackNodeframe0.h>
#include <nlink_parser/LinktrackNodeframe1.h>
#include <nlink_parser/LinktrackNodeframe2.h>
#include <nlink_parser/LinktrackNodeframe3.h>
#include <nlink_parser/LinktrackNodeframe4.h>
#include <nlink_parser/LinktrackNodeframe5.h>
#include <nlink_parser/LinktrackNodeframe6.h>
#include <nlink_parser/LinktrackTagframe0.h>
#include <std_msgs/String.h>

#include "nutils.h"
#include "protocols.h"
#include <sensor_msgs/Imu.h>
#include <nav_msgs/Odometry.h>
ros::Publisher pub_imu;
ros::Publisher pub_Odom;
void msg_callBack(const nlink_parser::LinktrackTagframe0::ConstPtr &msg)
{
    //发布imu
    sensor_msgs::Imu imu_msg;
    imu_msg.header.stamp = ros::Time::now();
    imu_msg.header.frame_id = "uwb";
    imu_msg.angular_velocity.x = msg->imu_gyro_3d[0];
    imu_msg.angular_velocity.y = msg->imu_gyro_3d[1];
    imu_msg.angular_velocity.z = msg->imu_gyro_3d[2];
    imu_msg.linear_acceleration.x = msg->imu_acc_3d[0];
    imu_msg.linear_acceleration.y = msg->imu_acc_3d[1];
    imu_msg.linear_acceleration.z = msg->imu_acc_3d[2];
    imu_msg.orientation.w = msg->quaternion[0];
    imu_msg.orientation.x = msg->quaternion[1];
    imu_msg.orientation.y = msg->quaternion[2];
    imu_msg.orientation.z = msg->quaternion[3];
    pub_imu.publish(imu_msg);
    //发布odom
    nav_msgs::Odometry Odom;
    Odom.header.stamp = imu_msg.header.stamp ;
    Odom.header.frame_id = "map";
    Odom.child_frame_id = "uwb";
    Odom.pose.pose.position.x = msg->pos_3d[0];
    Odom.pose.pose.position.y = msg->pos_3d[1];
    Odom.pose.pose.position.z = msg->pos_3d[2];
    Odom.pose.pose.orientation = imu_msg.orientation;
    Odom.twist.twist.angular.x = msg->imu_gyro_3d[0];
    Odom.twist.twist.angular.y = msg->imu_gyro_3d[1];
    Odom.twist.twist.angular.z = msg->imu_gyro_3d[2];
    Odom.twist.twist.linear.x = msg->vel_3d[0];
    Odom.twist.twist.linear.y = msg->vel_3d[1];
    Odom.twist.twist.linear.z = msg->vel_3d[2];
    //将距离保存在这里
    for (int i = 0; i < 8; i++)
    {
        Odom.pose.covariance[i] = msg->dis_arr[i];
    }
    pub_Odom.publish(Odom);
}
int main(int argc, char **argv)
{
    ros::init(argc, argv, "UWBPublisher");
    ROS_INFO("\033[1;32m----> UWB Publisher.\033[0m");
    ros::NodeHandle nh;
    pub_imu = nh.advertise<sensor_msgs::Imu>("/uwb/imu", 100);
    pub_Odom = nh.advertise<nav_msgs::Odometry>("/uwb/odom", 100);
    ros::Subscriber sub = nh.subscribe<nlink_parser::LinktrackTagframe0>("/nlink_linktrack_tagframe0", 100, msg_callBack);
    ros::spin();
}