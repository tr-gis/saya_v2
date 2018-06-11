/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2016, Sayabot Systems Pvt. Ltd.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the CU Boulder nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/


#include <boost/assign/list_of.hpp>
#include <string>
#include <algorithm>

#include "hdf_base/hdf_hw.h"

namespace hdf_base
{

     /**
     * Initialize HDFCbot hardware
     */
     ros::V_string joint_names = boost::assign::list_of("front_left_wheel_joint")
                                                      ("front_right_wheel_joint");

     static int count = 0;
     int dxl_old_pos[2];

     HdfHW::HdfHW(ros::NodeHandle nh, ros::NodeHandle private_nh)  : nh_(nh),
     private_nh_(private_nh)
     {

          registerControlInterfaces();
     }

     /**
     * Register interfaces with the RobotHW interface manager
     */
     void HdfHW::registerControlInterfaces()
     {

          for (unsigned int i = 0; i < joint_names.size(); i++)
          {
               hardware_interface::JointStateHandle joint_state_handle(joint_names[i],&joints_[i].position, &joints_[i].velocity, &joints_[i].effort);
               joint_state_interface_.registerHandle(joint_state_handle);
              
               hardware_interface::JointHandle joint_handle_velocity(joint_state_handle, &joints_[i].velocity_command);
               velocity_joint_interface_.registerHandle(joint_handle_velocity);
               
               joint_cmd_pubs_[joint_names[i]] = nh_.advertise<std_msgs::Float64>("/" + joint_names[i] + "/command", 100);
               ros::Subscriber sub = nh_.subscribe("/" + joint_names[i] + "/state", 1, &HdfHW::jointStateCallback, this);
               joint_state_subs_[joint_names[i]] = sub;
  
               nh_.setCallbackQueue(&subscriber_queue_);
    
          }
          registerInterface(&joint_state_interface_);
          registerInterface(&velocity_joint_interface_);
          
          subscriber_spinner_.reset(new ros::AsyncSpinner(1, &subscriber_queue_));
          subscriber_spinner_->start();
     }

     void HdfHW::resetTravelOffset()
     {
          for (int i = 0; i < 2; i++)
          {
               joints_[i].position_offset = received_joint_states_[joint_names[i]]->current_pos;
           }
     }
     /**
     * Read dynamixel position and velocity values
     */
     void HdfHW::readFromHardware()
     {

          if(received_joint_states_.size()<2)
          {
               return;
          }

          if(count < 10)
          {
               for (int i = 0; i < 2; i++)
               {
                    joints_[i].dxl_position_offset = received_joint_states_[joint_names[i]]->current_pos;
                    dxl_old_pos[i] = 0;
                }
               count++;
          }

        
          for (int i=0; i < 2; i++)
          {
              if(i%2 == 0)
               {
                    joints_[i].velocity = received_joint_states_[joint_names[i]]->velocity;
               }
               else
               {
                    joints_[i].velocity = received_joint_states_[joint_names[i]]->velocity;
               }  
               int dxl_new_pos = scaleValue(received_joint_states_[joint_names[i]]->current_pos,0,6.28318531,0,4095) - joints_[i].dxl_position_offset;
               int delta = dxl_new_pos - dxl_old_pos[i];
               dxl_old_pos[i] = dxl_new_pos;

               if(joints_[i].velocity > 0 && std::abs(delta) > 2048)
               {
                    joints_[i].tick++;
                    joints_[i].dxl_position_offset = 0;
               }
               else if(joints_[i].velocity < 0 && std::abs(delta) > 2048)
               {
                    joints_[i].tick--;
                    joints_[i].dxl_position_offset = 0;
               }
               joints_[i].dxl_position = (joints_[i].tick * 4096) + dxl_new_pos;
               if(i%2 == 0)
               {
                    joints_[i].position = ((6.28/4096) * joints_[i].dxl_position);
                 }                    
                                else
               {
                    joints_[i].position = 4096 - ((6.28/4096) * joints_[i].dxl_position);
               }

          }
     }
     /**
     * Write dynamixel position and velocity values that received from base controller
     */
     void HdfHW::writeToHardware()
     {
        
          for(unsigned int i=0; i<joint_names.size(); i++)
          {
               std_msgs::Float64 msg;
               if(i%2 == 0)
               {
                    msg.data = joints_[i].velocity_command;
               }
               else
               {
                    msg.data = -joints_[i].velocity_command;
               }
               joint_cmd_pubs_[joint_names[i]].publish(msg);
          }
     }
     
     
     void HdfHW::jointStateCallback(const dynamixel_msgs::JointStateConstPtr& dyn_joint_state)
     {
          received_joint_states_[dyn_joint_state->name] = dyn_joint_state;
     }
     
     
     /*
     * Scaling values from roscontrol interms of dynamixel values.
     */
     double HdfHW::scaleValue(double input,double value_min,double value_max,double dxl_value_min,double dxl_value_max)
     {
          try
          {
               double input_range = value_max - value_min;
               double output_range = dxl_value_max - dxl_value_min;
               double scaled_value =(((input - value_min)/input_range) * output_range )+dxl_value_min;
               return scaled_value;
          }
          catch(int e)
          {
               ROS_ERROR("Exception occured while scaling servo values%d\n ",e);
               return -1;
          } 
          return -1;
     }
}
