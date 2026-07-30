#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/int16_multi_array.hpp>
#include <std_msgs/msg/int8.hpp>
#include <std_msgs/msg/string.hpp>

#include "arm_control_sdk/carm_dual.h"
#include "arm_control_sdk/data_type_def.h"

class DualArmControlNode : public rclcpp::Node {
public:
    DualArmControlNode();
    ~DualArmControlNode();

private:
    void connect(const std_msgs::msg::String::SharedPtr msg);
    void setReady(const std_msgs::msg::Bool::SharedPtr msg);
    void stop(const std_msgs::msg::Bool::SharedPtr msg);

    void moveLeftJoint(const sensor_msgs::msg::JointState::SharedPtr msg);
    void moveRightJoint(const sensor_msgs::msg::JointState::SharedPtr msg);
    void moveLeftPose(const geometry_msgs::msg::Pose::SharedPtr msg);
    void moveRightPose(const geometry_msgs::msg::Pose::SharedPtr msg);
    void moveLeftLineJoint(const sensor_msgs::msg::JointState::SharedPtr msg);
    void moveRightLineJoint(const sensor_msgs::msg::JointState::SharedPtr msg);
    void moveLeftLinePose(const geometry_msgs::msg::Pose::SharedPtr msg);
    void moveRightLinePose(const geometry_msgs::msg::Pose::SharedPtr msg);
    void moveLeftTrackingJoint(const sensor_msgs::msg::JointState::SharedPtr msg);
    void moveRightTrackingJoint(const sensor_msgs::msg::JointState::SharedPtr msg);
    void moveLeftTrackingPose(const geometry_msgs::msg::Pose::SharedPtr msg);
    void moveRightTrackingPose(const geometry_msgs::msg::Pose::SharedPtr msg);

    void setSpeedLevel(const std_msgs::msg::Int16MultiArray::SharedPtr msg);
    void setControlMode(const std_msgs::msg::Int8::SharedPtr msg);
    void setServoEnable(const std_msgs::msg::Bool::SharedPtr msg);
    void setCollisionConfig(const std_msgs::msg::Int16MultiArray::SharedPtr msg);
    void setLeftEndEffector(const sensor_msgs::msg::JointState::SharedPtr msg);
    void setRightEndEffector(const sensor_msgs::msg::JointState::SharedPtr msg);

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

    std::array<double, 7> poseToArray(const geometry_msgs::msg::Pose& msg) const;
    void publishJointState(rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher,
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
    void publishPoseAndState(
            rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pose_publisher,
            rclcpp::Publisher<std_msgs::msg::Int16MultiArray>::SharedPtr state_publisher,
            const std::string& frame_id,
            double t,
            const std::array<double, 7>& p,
            const carm::ArmStatus& arm_status);

    std::unique_ptr<carm::CArmDualBot> carm_;
    std::string carm_ip_ = "10.42.0.101";

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr connect_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr ready_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr emergency_stop_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr left_move_joint_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr right_move_joint_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr left_move_pose_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr right_move_pose_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr left_move_line_joint_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr right_move_line_joint_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr left_move_line_pose_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr right_move_line_pose_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr left_move_tracking_joint_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr right_move_tracking_joint_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr left_move_tracking_pose_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr right_move_tracking_pose_sub_;
    rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr set_speed_level_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr set_servo_enable_sub_;
    rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr set_collision_config_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr left_set_gripper_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr right_set_gripper_sub_;
    rclcpp::Subscription<std_msgs::msg::Int8>::SharedPtr set_control_mode_sub_;

    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr left_real_joint_state_pub_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr right_real_joint_state_pub_;
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr left_flange_cart_state_pub_;
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr right_flange_cart_state_pub_;
    rclcpp::Publisher<std_msgs::msg::Int16MultiArray>::SharedPtr left_arm_state_pub_;
    rclcpp::Publisher<std_msgs::msg::Int16MultiArray>::SharedPtr right_arm_state_pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr task_completion_pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr error_pub_;
};
