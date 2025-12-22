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
    ArmControlNode() : Node("arm_control_sdk") {
        RCLCPP_INFO(this->get_logger(), "ArmControlNode started.");

        // Initialize CARM API
        carm_ = std::make_unique<carm::CArmSingleCol>(carm_ip);

        // Base commands
        connect_sub_ = this->create_subscription<std_msgs::msg::String>(
                "connect", 10, std::bind(&ArmControlNode::connect, this, std::placeholders::_1));
        ready_sub_ = this->create_subscription<std_msgs::msg::Bool>(
                "ready", 10, std::bind(&ArmControlNode::setReady, this, std::placeholders::_1));
        emergency_stop_sub_ = this->create_subscription<std_msgs::msg::Bool>(
                "emergency_stop",
                10,
                std::bind(&ArmControlNode::stop, this, std::placeholders::_1));

        // Movement commands
        move_joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
                "move_joint",
                10,
                std::bind(&ArmControlNode::moveJoint, this, std::placeholders::_1));
        move_pose_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
                "move_pose", 10, std::bind(&ArmControlNode::movePose, this, std::placeholders::_1));
        move_line_joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
                "move_line_joint",
                10,
                std::bind(&ArmControlNode::moveLineJoint, this, std::placeholders::_1));
        move_line_pose_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
                "move_line_pose",
                10,
                std::bind(&ArmControlNode::moveLinePose, this, std::placeholders::_1));
        move_tracking_pose_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
                "move_tracking_pose",
                10,
                std::bind(&ArmControlNode::moveTrackingPose, this, std::placeholders::_1));
        move_tracking_joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
                "move_tracking_joint",
                10,
                std::bind(&ArmControlNode::moveTrackingJoint, this, std::placeholders::_1));

        // Configuration commands
        set_speed_level_sub_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
                "set_speed_level",
                10,
                std::bind(&ArmControlNode::setSpeedLevel, this, std::placeholders::_1));
        set_servo_enable_sub_ = this->create_subscription<std_msgs::msg::Bool>(
                "set_servo_enable",
                10,
                std::bind(&ArmControlNode::setServoEnable, this, std::placeholders::_1));
        set_collision_config_sub_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
                "set_collision_config",
                10,
                std::bind(&ArmControlNode::setCollisionConfig, this, std::placeholders::_1));
        set_gripper_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
                "set_gripper",
                10,
                std::bind(&ArmControlNode::setEndEffector, this, std::placeholders::_1));
        set_control_mode_sub_ = this->create_subscription<std_msgs::msg::Int8>(
                "set_control_mode",
                10,
                std::bind(&ArmControlNode::setControlMode, this, std::placeholders::_1));

        // Publishers
        real_joint_state_pub_ =
                this->create_publisher<sensor_msgs::msg::JointState>("real_joint_state", 10);
        flange_cart_state_pub_ =
                this->create_publisher<geometry_msgs::msg::PoseStamped>("flange_cart_state", 10);
        arm_state_pub_ = this->create_publisher<std_msgs::msg::Int16MultiArray>("arm_state", 10);
        task_completion_pub_ = this->create_publisher<std_msgs::msg::String>("task_completion", 10);
        error_pub_ = this->create_publisher<std_msgs::msg::String>("carm_error", 10);

        RCLCPP_INFO(this->get_logger(), "Waiting for connection...");
        rclcpp::sleep_for(std::chrono::seconds(1));
        RCLCPP_INFO(this->get_logger(), "Connection established, enabling arm...");
        carm_->set_ready();
        RCLCPP_INFO(this->get_logger(), "Starting to publish arm state topics...");
        // Assuming the CARM API has similar callback registration methods
        carm_->register_joint_cbk(std::bind(&ArmControlNode::jointPublisher,
                                            this,
                                            std::placeholders::_1,
                                            std::placeholders::_2,
                                            std::placeholders::_3,
                                            std::placeholders::_4));
        carm_->register_pose_cbk(std::bind(&ArmControlNode::posePublisher,
                                           this,
                                           std::placeholders::_1,
                                           std::placeholders::_2));

        carm_->register_error_cbk("error",
                                  std::bind(&ArmControlNode::errorPublisher,
                                            this,
                                            std::placeholders::_1,
                                            std::placeholders::_2));
        carm_->register_completion_cbk(
                "task_completion",
                std::bind(&ArmControlNode::taskCompletionPublisher, this, std::placeholders::_1));
    }

    ~ArmControlNode() {
        carm_->release_error_cbk("error");
        carm_->release_completion_cbk("task_completion");
    }

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