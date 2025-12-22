#include <geometry_msgs/Point.h>
#include <geometry_msgs/PoseStamped.h>
#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Int16MultiArray.h>
#include <std_msgs/Int8.h>
#include <std_msgs/String.h>
#include <tf/transform_datatypes.h>

#include "arm_control_sdk/carm_cobot.h"
#include "arm_control_sdk/data_type_def.h"

class ArmControlNode {
public:
    ArmControlNode() {
        ros::NodeHandle nh;
        ROS_INFO("ArmControlNode started.");

        // Initialize CARM API
        carm_ = std::make_unique<carm::CArmSingleCol>(carm_ip);

        // Base commands
        connect_sub_ = nh.subscribe("connect", 10, &ArmControlNode::connect, this);
        ready_sub_ = nh.subscribe("ready", 10, &ArmControlNode::setReady, this);
        emergency_stop_sub_ = nh.subscribe("emergency_stop", 10, &ArmControlNode::stop, this);

        // Movement commands
        move_joint_sub_ = nh.subscribe("move_joint", 10, &ArmControlNode::moveJoint, this);
        move_pose_sub_ = nh.subscribe("move_pose", 10, &ArmControlNode::movePose, this);
        move_line_joint_sub_ =
                nh.subscribe("move_line_joint", 10, &ArmControlNode::moveLineJoint, this);
        move_line_pose_sub_ =
                nh.subscribe("move_line_pose", 10, &ArmControlNode::moveLinePose, this);
        move_tracking_pose_sub_ =
                nh.subscribe("move_tracking_pose", 10, &ArmControlNode::moveTrackingPose, this);
        move_tracking_joint_sub_ =
                nh.subscribe("move_tracking_joint", 10, &ArmControlNode::moveTrackingJoint, this);

        // Configuration commands
        set_speed_level_sub_ =
                nh.subscribe("set_speed_level", 10, &ArmControlNode::setSpeedLevel, this);
        set_servo_enable_sub_ =
                nh.subscribe("set_servo_enable", 10, &ArmControlNode::setServoEnable, this);
        set_collision_config_sub_ =
                nh.subscribe("set_collision_config", 10, &ArmControlNode::setCollisionConfig, this);
        set_gripper_sub_ = nh.subscribe("set_gripper", 10, &ArmControlNode::setEndEffector, this);
        set_control_mode_sub_ =
                nh.subscribe("set_control_mode", 10, &ArmControlNode::setControlMode, this);

        // Publishers
        real_joint_state_pub_ = nh.advertise<sensor_msgs::JointState>("real_joint_state", 10);
        flange_cart_state_pub_ = nh.advertise<geometry_msgs::PoseStamped>("flange_cart_state", 10);
        arm_state_pub_ = nh.advertise<std_msgs::Int16MultiArray>("arm_state", 10);
        task_completion_pub_ = nh.advertise<std_msgs::String>("task_completion", 10);
        error_pub_ = nh.advertise<std_msgs::String>("carm_error", 10);

        ROS_INFO("Waiting for connection...");
        ros::Duration(1.0).sleep();
        ROS_INFO("Connection established, enabling arm...");
        carm_->set_ready();
        ROS_INFO("Starting to publish arm state topics...");
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
    void connect(const std_msgs::StringConstPtr& msg);

    void setReady(const std_msgs::BoolConstPtr& msg);

    void stop(const std_msgs::BoolConstPtr& msg);

    void moveJoint(const sensor_msgs::JointStateConstPtr& msg);
    void movePose(const geometry_msgs::PoseConstPtr& msg);
    void moveLineJoint(const sensor_msgs::JointStateConstPtr& msg);
    void moveLinePose(const geometry_msgs::PoseConstPtr& msg);
    void moveTrackingJoint(const sensor_msgs::JointStateConstPtr& msg);
    void moveTrackingPose(const geometry_msgs::PoseConstPtr& msg);
    void setSpeedLevel(const std_msgs::Int16MultiArrayConstPtr& msg);
    void setControlMode(const std_msgs::Int8ConstPtr& msg);
    void setServoEnable(const std_msgs::BoolConstPtr& msg);
    void setCollisionConfig(const std_msgs::Int16MultiArrayConstPtr& msg);
    void setEndEffector(const sensor_msgs::JointStateConstPtr& msg);
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
    ros::Subscriber connect_sub_;
    ros::Subscriber ready_sub_;
    ros::Subscriber emergency_stop_sub_;
    ros::Subscriber move_joint_sub_;
    ros::Subscriber move_pose_sub_;
    ros::Subscriber move_line_joint_sub_;
    ros::Subscriber move_line_pose_sub_;
    ros::Subscriber move_tracking_pose_sub_;
    ros::Subscriber move_tracking_joint_sub_;
    ros::Subscriber set_speed_level_sub_;
    ros::Subscriber set_servo_enable_sub_;
    ros::Subscriber set_collision_config_sub_;
    ros::Subscriber set_gripper_sub_;
    ros::Subscriber set_control_mode_sub_;

    // Publishers
    ros::Publisher real_joint_state_pub_;
    ros::Publisher flange_cart_state_pub_;
    ros::Publisher arm_state_pub_;
    ros::Publisher task_completion_pub_;
    ros::Publisher error_pub_;
};