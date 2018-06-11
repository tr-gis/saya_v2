/**     Copyright (c) 2012-2014 ASIMOV Robotics. All rights reserved. ////
*
*
//------------------------------------------------------------------------------*/

#include "hardware_interface/joint_state_interface.h"
#include "hardware_interface/joint_command_interface.h"
#include "hardware_interface/robot_hw.h"
#include "ros/ros.h"
#include "sensor_msgs/JointState.h"
#include <string>
#include <cmath>
#include <std_msgs/Float64.h>
#include <dynamixel_msgs/JointState.h>
#include <ros/callback_queue.h>

#ifndef HDF_BASE_HDF_HW_H
#define HDF_BASE_HDF_HW_H

namespace hdf_base
{
	/**
	Class representing hardware,allows for ros_control to modify internal state via joint interfaces
	*/
	class HdfHW : public hardware_interface::RobotHW
  	{
  		public:
    		HdfHW(ros::NodeHandle nh, ros::NodeHandle private_nh);

    		void readFromHardware();

    		void writeToHardware();
                
                void resetTravelOffset();

      		private:

    		void registerControlInterfaces();

      		void jointStateCallback(const dynamixel_msgs::JointStateConstPtr& dyn_joint_state);

      		double scaleValue(double input,double value_min,double value_max,double dxl_value_min,double dxl_value_max);

    		ros::NodeHandle nh_, private_nh_;
      		std::map<std::string, ros::Publisher> joint_cmd_pubs_;
      		std::map<std::string, ros::Subscriber> joint_state_subs_;

      		int dynamixel_position_offset[2];
      		int current_dynamixel_position[2];

      		std::map<std::string, dynamixel_msgs::JointStateConstPtr> received_joint_states_;

      		boost::shared_ptr<ros::AsyncSpinner> subscriber_spinner_;
      		ros::CallbackQueue subscriber_queue_;

    	 	// ROS Control interfaces
    		hardware_interface::JointStateInterface joint_state_interface_;
    		hardware_interface::VelocityJointInterface velocity_joint_interface_;
      		

    	struct Joint
    	{
      		double position;
                double position_offset;
      		double velocity;
      		double effort;
      		double velocity_command;
          	
          	int dxl_position;
          	int dxl_position_offset;
          	int tick;

      		Joint() : position(0), velocity(0), effort(0), velocity_command(0),dxl_position(0),dxl_position_offset(0),tick(0){ }
    	} joints_[2];
  	};
}// namespace hdf_base
#endif  // HDF_BASE_HDF_HW_H
