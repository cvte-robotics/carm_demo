#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Int16MultiArray.h>
#include <std_msgs/Int8.h>
#include <std_msgs/String.h>

#include "arm_control_sdk/carm_dual.h"
#include "arm_control_sdk/data_type_def.h"

class DualArmControlNode {
public:
    DualArmControlNode();
    ~DualArmControlNode();

private:
    void connect(const std_msgs::StringConstPtr& msg);
    void setReady(const std_msgs::BoolConstPtr& msg);
    void stop(const std_msgs::BoolConstPtr& msg);

    void moveLeftJoint(const sensor_msgs::JointStateConstPtr& msg);
    void moveRightJoint(const sensor_msgs::JointStateConstPtr& msg);
    void moveLeftPose(const geometry_msgs::PoseConstPtr& msg);
    void moveRightPose(const geometry_msgs::PoseConstPtr& msg);
    void moveLeftLineJoint(const sensor_msgs::JointStateConstPtr& msg);
    void moveRightLineJoint(const sensor_msgs::JointStateConstPtr& msg);
    void moveLeftLinePose(const geometry_msgs::PoseConstPtr& msg);
    void moveRightLinePose(const geometry_msgs::PoseConstPtr& msg);
    void moveLeftTrackingJoint(const sensor_msgs::JointStateConstPtr& msg);
    void moveRightTrackingJoint(const sensor_msgs::JointStateConstPtr& msg);
    void moveLeftTrackingPose(const geometry_msgs::PoseConstPtr& msg);
    void moveRightTrackingPose(const geometry_msgs::PoseConstPtr& msg);

    void setSpeedLevel(const std_msgs::Int16MultiArrayConstPtr& msg);
    void setControlMode(const std_msgs::Int8ConstPtr& msg);
    void setServoEnable(const std_msgs::BoolConstPtr& msg);
    void setCollisionConfig(const std_msgs::Int16MultiArrayConstPtr& msg);
    void setLeftEndEffector(const sensor_msgs::JointStateConstPtr& msg);
    void setRightEndEffector(const sensor_msgs::JointStateConstPtr& msg);

    void leftJointPublisher(double t,
                            std::vector<double> p,
                            std::vector<double> v,
                            std::vector<double> a);
    void rightJointPublisher(double t,
                             std::vector<double> p,
                             std::vector<double> v,
                             std::vector<double> a);
    void leftPosePublisher(double t, std::array<double, 7> p);
    void rightPosePublisher(double t, std::array<double, 7> p);
    void taskCompletionPublisher(const std::string task_key);
    void errorPublisher(int code, const std::string error_msg);

    std::array<double, 7> poseToArray(const geometry_msgs::Pose& msg) const;
    void publishJointState(ros::Publisher& publisher,
                           const std::string& frame_id,
                           const std::string& arm_prefix,
                           double t,
                           std::vector<double> p,
                           std::vector<double> v,
                           std::vector<double> a,
                           int eeff_state,
                           const std::vector<double>& eeff_pos,
                           const std::vector<double>& eeff_vel,
                           const std::vector<double>& eeff_tau);
    void publishPoseAndState(ros::Publisher& pose_publisher,
                             ros::Publisher& state_publisher,
                             const std::string& frame_id,
                             double t,
                             const std::array<double, 7>& p,
                             const carm::ArmStatus& arm_status);

    std::unique_ptr<carm::CArmDualBot> carm_;
    std::string carm_ip_ = "10.42.0.101";

    ros::Subscriber connect_sub_;
    ros::Subscriber ready_sub_;
    ros::Subscriber emergency_stop_sub_;
    ros::Subscriber left_move_joint_sub_;
    ros::Subscriber right_move_joint_sub_;
    ros::Subscriber left_move_pose_sub_;
    ros::Subscriber right_move_pose_sub_;
    ros::Subscriber left_move_line_joint_sub_;
    ros::Subscriber right_move_line_joint_sub_;
    ros::Subscriber left_move_line_pose_sub_;
    ros::Subscriber right_move_line_pose_sub_;
    ros::Subscriber left_move_tracking_joint_sub_;
    ros::Subscriber right_move_tracking_joint_sub_;
    ros::Subscriber left_move_tracking_pose_sub_;
    ros::Subscriber right_move_tracking_pose_sub_;
    ros::Subscriber set_speed_level_sub_;
    ros::Subscriber set_servo_enable_sub_;
    ros::Subscriber set_collision_config_sub_;
    ros::Subscriber left_set_gripper_sub_;
    ros::Subscriber right_set_gripper_sub_;
    ros::Subscriber set_control_mode_sub_;

    ros::Publisher left_real_joint_state_pub_;
    ros::Publisher right_real_joint_state_pub_;
    ros::Publisher left_flange_cart_state_pub_;
    ros::Publisher right_flange_cart_state_pub_;
    ros::Publisher left_arm_state_pub_;
    ros::Publisher right_arm_state_pub_;
    ros::Publisher task_completion_pub_;
    ros::Publisher error_pub_;
};
