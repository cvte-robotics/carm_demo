#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>

#include <example_interfaces/srv/add_two_ints.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/int16_multi_array.hpp>
#include <std_msgs/msg/int8.hpp>
#include <std_msgs/msg/string.hpp>

#include "arm_control_sdk/carm_cobot.h"
#include "arm_control_sdk/data_type_def.h"

class ArmControlNode : public rclcpp::Node {
public:
    ArmControlNode();
    ~ArmControlNode();

private:
    // Callback functions
    void connect(const std_msgs::msg::String::SharedPtr msg);
    void setReady(const std_msgs::msg::Bool::SharedPtr msg);
    void stop(const std_msgs::msg::Bool::SharedPtr msg);

    void moveJoint(const sensor_msgs::msg::JointState::SharedPtr msg);

    void movePose(const geometry_msgs::msg::Pose::SharedPtr msg);
    void moveLineJoint(const sensor_msgs::msg::JointState::SharedPtr msg);
    void moveLinePose(const geometry_msgs::msg::Pose::SharedPtr msg);

    void moveTrackingJoint(const sensor_msgs::msg::JointState::SharedPtr msg);

    void moveTrackingPose(const geometry_msgs::msg::Pose::SharedPtr msg);
    void setSpeedLevel(const std_msgs::msg::Int16MultiArray::SharedPtr msg);
    void setControlMode(const std_msgs::msg::Int8::SharedPtr msg);
    void setServoEnable(const std_msgs::msg::Bool::SharedPtr msg);

    void setCollisionConfig(const std_msgs::msg::Int16MultiArray::SharedPtr msg);
    void setEndEffector(const sensor_msgs::msg::JointState::SharedPtr msg);
    void jointPublisher(double t,
                        std::vector<double> p,
                        std::vector<double> v,
                        std::vector<double> a);
    void posePublisher(double t, std::array<double, 7> p);
    void taskCompletionPublisher(const std::string task_key);
    void errorPublisher(int code, const std::string error_msg);
    std::array<double, 7> poseToArray(const geometry_msgs::msg::Pose& msg) const;

    // Member variables
    std::unique_ptr<carm::CArmSingleCol> carm_;
    std::string carm_ip = "10.42.0.101";

    // Subscribers
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr connect_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr ready_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr emergency_stop_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr move_joint_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr move_pose_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr move_line_joint_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr move_line_pose_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr move_tracking_pose_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr move_tracking_joint_sub_;
    rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr set_speed_level_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr set_servo_enable_sub_;
    rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr set_collision_config_sub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr set_gripper_sub_;
    rclcpp::Subscription<std_msgs::msg::Int8>::SharedPtr set_control_mode_sub_;

    // Publishers
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr real_joint_state_pub_;
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr flange_cart_state_pub_;
    rclcpp::Publisher<std_msgs::msg::Int16MultiArray>::SharedPtr arm_state_pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr task_completion_pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr error_pub_;
};
