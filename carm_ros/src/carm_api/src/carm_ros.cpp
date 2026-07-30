#include "carm_api/carm_ros.hpp"

#include "arm_control_sdk/carm_cobot.h"
#include "arm_control_sdk/data_type_def.h"

ArmControlNode::ArmControlNode() {
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
    move_line_pose_sub_ = nh.subscribe("move_line_pose", 10, &ArmControlNode::moveLinePose, this);
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
    carm_->register_pose_cbk(std::bind(
            &ArmControlNode::posePublisher, this, std::placeholders::_1, std::placeholders::_2));
    carm_->register_error_cbk("error",
                              std::bind(&ArmControlNode::errorPublisher,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2));
    carm_->register_completion_cbk(
            "task_completion",
            std::bind(&ArmControlNode::taskCompletionPublisher, this, std::placeholders::_1));
}

ArmControlNode::~ArmControlNode() {
    carm_->release_error_cbk("error");
    carm_->release_completion_cbk("task_completion");
}

void ArmControlNode::connect(const std_msgs::StringConstPtr& msg) {
    int ret = 0;
    if (msg->data.empty()) {
        ret = carm_->disconnect();
    } else {
        ret = carm_->connect(msg->data);
    }
    ROS_INFO("connect, ret = %d", ret);
}

void ArmControlNode::setReady(const std_msgs::BoolConstPtr& msg) {
    int ret = carm_->set_ready();
    ROS_INFO("set_ready, ret = %d", ret);
}

void ArmControlNode::stop(const std_msgs::BoolConstPtr& msg) { carm_->emergency_stop(); }

void ArmControlNode::moveJoint(const sensor_msgs::JointStateConstPtr& msg) {
    carm_->move_joint(msg->position, -1, false);
    ROS_INFO("move_joint");
}

void ArmControlNode::movePose(const geometry_msgs::PoseConstPtr& msg) {
    carm_->move_pose(poseToArray(*msg), -1, false);
    ROS_INFO("move_pose");
}

void ArmControlNode::moveLineJoint(const sensor_msgs::JointStateConstPtr& msg) {
    carm_->move_line_joint(msg->position, false);
    ROS_INFO("move_line_joint");
}

void ArmControlNode::moveLinePose(const geometry_msgs::PoseConstPtr& msg) {
    carm_->move_line_pose(poseToArray(*msg), false);
    ROS_INFO("move_line_pose");
}

void ArmControlNode::moveTrackingJoint(const sensor_msgs::JointStateConstPtr& msg) {
    std::vector<double> joint_positions;
    std::vector<double> gripper_positions;

    for (size_t i = 0; i < msg->name.size(); ++i) {
        if (msg->name[i].find("joint") != std::string::npos) {
            joint_positions.push_back(msg->position[i]);
        } else if (msg->name[i].find("gripper") != std::string::npos) {
            gripper_positions.push_back(msg->position[i]);
        }
    }

    if (!gripper_positions.empty()) {
        carm_->track_joint(joint_positions, gripper_positions[0]);
    } else {
        carm_->track_joint(joint_positions);
    }
}

void ArmControlNode::moveTrackingPose(const geometry_msgs::PoseConstPtr& msg) {
    carm_->track_pose(poseToArray(*msg));
}

void ArmControlNode::setSpeedLevel(const std_msgs::Int16MultiArrayConstPtr& msg) {
    if (msg->data.size() >= 2) {
        int ret = carm_->set_speed_level(msg->data[0], msg->data[1]);
        ROS_INFO("set_speed_level, ret = %d", ret);
    }
}

void ArmControlNode::setControlMode(const std_msgs::Int8ConstPtr& msg) {
    int ret = carm_->set_control_mode(msg->data);
    ROS_INFO("set_control_mode, ret = %d", ret);
}

void ArmControlNode::setServoEnable(const std_msgs::BoolConstPtr& msg) {
    int ret = carm_->set_servo_enable(msg->data);
    ROS_INFO("set_servo_enable, ret = %d", ret);
}

void ArmControlNode::setCollisionConfig(const std_msgs::Int16MultiArrayConstPtr& msg) {
    if (msg->data.size() >= 2) {
        int ret = carm_->set_collision_config(msg->data[0], msg->data[1]);
        ROS_INFO("set_collision_config, ret = %d", ret);
    }
}

void ArmControlNode::setEndEffector(const sensor_msgs::JointStateConstPtr& msg) {
    if (!msg->position.empty()) {
        int ret = 0;
        if (msg->position.size() == 1 && msg->velocity.empty()) {
            const double tau = msg->effort.empty() ? 10.0 : msg->effort[0];
            ret = carm_->set_gripper(msg->position[0], tau);
            ROS_INFO("set_gripper, ret = %d", ret);
        } else {
            ret = carm_->set_eeff(msg->position, msg->velocity, msg->effort);
            ROS_INFO("set_eeff, ret = %d", ret);
        }
    }
}

void ArmControlNode::jointPublisher(double t,
                                    std::vector<double> p,
                                    std::vector<double> v,
                                    std::vector<double> a) {
    sensor_msgs::JointState real_joint_msg;

    real_joint_msg.header.stamp.fromSec(t);
    real_joint_msg.header.frame_id = "base_link";
    if (carm_->get_gripper_state() >= 0) {
        real_joint_msg.name = {
                "joint1", "joint2", "joint3", "joint4", "joint5", "joint6", "gripper"};
        real_joint_msg.position = p;
        real_joint_msg.position.push_back(carm_->get_gripper_pos());
        real_joint_msg.velocity = v;
        real_joint_msg.velocity.push_back(carm_->get_gripper_vel());
        real_joint_msg.effort = a;
        real_joint_msg.effort.push_back(carm_->get_gripper_tau());
    } else {
        real_joint_msg.name = {"joint1", "joint2", "joint3", "joint4", "joint5", "joint6"};
        real_joint_msg.position = p;
        real_joint_msg.velocity = v;
        real_joint_msg.effort = a;
    }

    real_joint_state_pub_.publish(real_joint_msg);
}

void ArmControlNode::posePublisher(double t, std::array<double, 7> p) {
    if (p.size() < 7) return;

    // Publish flange_cart_state
    geometry_msgs::PoseStamped flange_msg;

    flange_msg.header.stamp.fromSec(t);
    flange_msg.header.frame_id = "base_link";
    flange_msg.pose.position.x = p[0];
    flange_msg.pose.position.y = p[1];
    flange_msg.pose.position.z = p[2];
    flange_msg.pose.orientation.x = p[3];
    flange_msg.pose.orientation.y = p[4];
    flange_msg.pose.orientation.z = p[5];
    flange_msg.pose.orientation.w = p[6];
    flange_cart_state_pub_.publish(flange_msg);

    // Publish arm_state
    auto arm_status = carm_->get_status();

    std::vector<std::string> variable_names = {"arm_index",
                                               "arm_is_connected",
                                               "arm_dof",
                                               "servo_status",
                                               "state",
                                               "fsm_state",
                                               "speed_percentage",
                                               "on_debug_mode"};

    std_msgs::Int16MultiArray array_msg;
    array_msg.data = {static_cast<int16_t>(arm_status.arm_index),
                      static_cast<int16_t>(arm_status.arm_is_connected),
                      static_cast<int16_t>(arm_status.arm_dof),
                      static_cast<int16_t>(arm_status.servo_status),
                      static_cast<int16_t>(arm_status.state),
                      static_cast<int16_t>(arm_status.fsm_state),
                      static_cast<int16_t>(arm_status.speed_percentage),
                      static_cast<int16_t>(arm_status.on_debug_mode)};

    // Set layout
    std_msgs::MultiArrayDimension dim;
    dim.label = "";
    for (const auto& name : variable_names) {
        if (!dim.label.empty()) dim.label += ",";
        dim.label += name;
    }
    dim.size = variable_names.size();
    dim.stride = variable_names.size();

    array_msg.layout.dim.push_back(dim);
    array_msg.layout.data_offset = 0;

    arm_state_pub_.publish(array_msg);
}

void ArmControlNode::taskCompletionPublisher(const std::string task_key) {
    std_msgs::String msg;
    msg.data = task_key;
    task_completion_pub_.publish(msg);
}

void ArmControlNode::errorPublisher(int code, const std::string error_msg) {
    std_msgs::String msg;
    msg.data = error_msg;
    error_pub_.publish(msg);
}

std::array<double, 7> ArmControlNode::poseToArray(const geometry_msgs::Pose& msg) const {
    return {msg.position.x,
            msg.position.y,
            msg.position.z,
            msg.orientation.x,
            msg.orientation.y,
            msg.orientation.z,
            msg.orientation.w};
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "arm_control_sdk");
    ArmControlNode node;
    ros::spin();
    return 0;
}
