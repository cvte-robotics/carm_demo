#include "carm_api/carm_ros.hpp"

#include "arm_control_sdk/carm_cobot.h"
#include "arm_control_sdk/data_type_def.h"

ArmControlNode::ArmControlNode() : Node("arm_control_sdk") {
    RCLCPP_INFO(this->get_logger(), "ArmControlNode started.");

    // Initialize CARM API
    carm_ = std::make_unique<carm::CArmSingleCol>(carm_ip);

    // Base commands
    connect_sub_ = this->create_subscription<std_msgs::msg::String>(
            "connect", 10, std::bind(&ArmControlNode::connect, this, std::placeholders::_1));
    ready_sub_ = this->create_subscription<std_msgs::msg::Bool>(
            "ready", 10, std::bind(&ArmControlNode::setReady, this, std::placeholders::_1));
    emergency_stop_sub_ = this->create_subscription<std_msgs::msg::Bool>(
            "emergency_stop", 10, std::bind(&ArmControlNode::stop, this, std::placeholders::_1));

    // Movement commands
    move_joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "move_joint", 10, std::bind(&ArmControlNode::moveJoint, this, std::placeholders::_1));
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

void ArmControlNode::connect(const std_msgs::msg::String::SharedPtr msg) {
    int ret = 0;
    if (msg->data.empty()) {
        ret = carm_->disconnect();
    } else {
        ret = carm_->connect(msg->data);
    }
    RCLCPP_INFO(this->get_logger(), "connect, ret = %d", ret);
}

void ArmControlNode::setReady(const std_msgs::msg::Bool::SharedPtr msg) {
    int ret = carm_->set_ready();
    RCLCPP_INFO(this->get_logger(), "set_ready, ret = %d", ret);
}

void ArmControlNode::stop(const std_msgs::msg::Bool::SharedPtr msg) { carm_->emergency_stop(); }

void ArmControlNode::moveJoint(const sensor_msgs::msg::JointState::SharedPtr msg) {
    carm_->move_joint(msg->position, -1, false);
    RCLCPP_INFO(this->get_logger(), "move_joint");
}

void ArmControlNode::movePose(const geometry_msgs::msg::Pose::SharedPtr msg) {
    carm_->move_pose(poseToArray(*msg), -1, false);
    RCLCPP_INFO(this->get_logger(), "move_pose");
}

void ArmControlNode::moveLineJoint(const sensor_msgs::msg::JointState::SharedPtr msg) {
    carm_->move_line_joint(msg->position, false);
    RCLCPP_INFO(this->get_logger(), "move_line_joint");
}

void ArmControlNode::moveLinePose(const geometry_msgs::msg::Pose::SharedPtr msg) {
    carm_->move_line_pose(poseToArray(*msg), false);
    RCLCPP_INFO(this->get_logger(), "move_line_pose");
}

void ArmControlNode::moveTrackingJoint(const sensor_msgs::msg::JointState::SharedPtr msg) {
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

void ArmControlNode::moveTrackingPose(const geometry_msgs::msg::Pose::SharedPtr msg) {
    carm_->track_pose(poseToArray(*msg));
}

void ArmControlNode::setSpeedLevel(const std_msgs::msg::Int16MultiArray::SharedPtr msg) {
    if (msg->data.size() >= 2) {
        int ret = carm_->set_speed_level(msg->data[0], msg->data[1]);
        RCLCPP_INFO(this->get_logger(), "set_speed_level, ret = %d", ret);
    }
}

void ArmControlNode::setControlMode(const std_msgs::msg::Int8::SharedPtr msg) {
    int ret = carm_->set_control_mode(msg->data);
    RCLCPP_INFO(this->get_logger(), "set_control_mode, ret = %d", ret);
}

void ArmControlNode::setServoEnable(const std_msgs::msg::Bool::SharedPtr msg) {
    int ret = carm_->set_servo_enable(msg->data);
    RCLCPP_INFO(this->get_logger(), "set_servo_enable, ret = %d", ret);
}

void ArmControlNode::setCollisionConfig(const std_msgs::msg::Int16MultiArray::SharedPtr msg) {
    if (msg->data.size() >= 2) {
        int ret = carm_->set_collision_config(msg->data[0], msg->data[1]);
        RCLCPP_INFO(this->get_logger(), "set_collision_config, ret = %d", ret);
    }
}

void ArmControlNode::setEndEffector(const sensor_msgs::msg::JointState::SharedPtr msg) {
    if (!msg->position.empty()) {
        int ret = 0;
        if (msg->position.size() == 1 && msg->velocity.empty()) {
            const double tau = msg->effort.empty() ? 10.0 : msg->effort[0];
            ret = carm_->set_gripper(msg->position[0], tau);
            RCLCPP_INFO(this->get_logger(), "set_gripper, ret = %d", ret);
        } else {
            ret = carm_->set_eeff(msg->position, msg->velocity, msg->effort);
            RCLCPP_INFO(this->get_logger(), "set_eeff, ret = %d", ret);
        }
    }
}

void ArmControlNode::jointPublisher(double t,
                                    std::vector<double> p,
                                    std::vector<double> v,
                                    std::vector<double> a) {
    auto real_joint_msg = std::make_unique<sensor_msgs::msg::JointState>();
    int64_t secs = static_cast<int64_t>(t);
    uint32_t nsecs = static_cast<uint32_t>((t - secs) * 1e9);

    real_joint_msg->header.stamp.sec = secs;
    real_joint_msg->header.stamp.nanosec = nsecs;
    real_joint_msg->header.frame_id = "base_link";
    if (carm_->get_gripper_state() >= 0) {
        real_joint_msg->name = {
                "joint1", "joint2", "joint3", "joint4", "joint5", "joint6", "gripper"};
        real_joint_msg->position = p;
        real_joint_msg->position.push_back(carm_->get_gripper_pos());
        real_joint_msg->velocity = v;
        real_joint_msg->velocity.push_back(carm_->get_gripper_vel());
        real_joint_msg->effort = a;
        real_joint_msg->effort.push_back(carm_->get_gripper_tau());
    } else {
        real_joint_msg->name = {"joint1", "joint2", "joint3", "joint4", "joint5", "joint6"};
        real_joint_msg->position = p;
        real_joint_msg->velocity = v;
        real_joint_msg->effort = a;
    }

    real_joint_state_pub_->publish(std::move(real_joint_msg));
}

void ArmControlNode::posePublisher(double t, std::array<double, 7> p) {
    // Publish flange_cart_state
    auto flange_msg = std::make_unique<geometry_msgs::msg::PoseStamped>();
    int64_t secs = static_cast<int64_t>(t);
    uint32_t nsecs = static_cast<uint32_t>((t - secs) * 1e9);

    flange_msg->header.stamp.sec = secs;
    flange_msg->header.stamp.nanosec = nsecs;
    flange_msg->header.frame_id = "base_link";
    flange_msg->pose.position.x = p[0];
    flange_msg->pose.position.y = p[1];
    flange_msg->pose.position.z = p[2];
    flange_msg->pose.orientation.x = p[3];
    flange_msg->pose.orientation.y = p[4];
    flange_msg->pose.orientation.z = p[5];
    flange_msg->pose.orientation.w = p[6];
    flange_cart_state_pub_->publish(std::move(flange_msg));

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

    std::vector<int16_t> arm_state_data = {static_cast<int16_t>(arm_status.arm_index),
                                           static_cast<int16_t>(arm_status.arm_is_connected),
                                           static_cast<int16_t>(arm_status.arm_dof),
                                           static_cast<int16_t>(arm_status.servo_status),
                                           static_cast<int16_t>(arm_status.state),
                                           static_cast<int16_t>(arm_status.fsm_state),
                                           static_cast<int16_t>(arm_status.speed_percentage),
                                           static_cast<int16_t>(arm_status.on_debug_mode)};

    auto array_msg = std::make_unique<std_msgs::msg::Int16MultiArray>();
    array_msg->data = arm_state_data;

    // Set layout
    auto dim = std_msgs::msg::MultiArrayDimension();
    dim.label = "";
    for (const auto& name : variable_names) {
        if (!dim.label.empty()) dim.label += ",";
        dim.label += name;
    }
    dim.size = variable_names.size();
    dim.stride = variable_names.size();

    array_msg->layout.dim.push_back(dim);
    array_msg->layout.data_offset = 0;

    arm_state_pub_->publish(std::move(array_msg));
}

void ArmControlNode::taskCompletionPublisher(const std::string task_key) {
    auto msg = std_msgs::msg::String();
    msg.data = task_key;
    task_completion_pub_->publish(msg);
}

void ArmControlNode::errorPublisher(int code, const std::string error_msg) {
    auto msg = std_msgs::msg::String();
    msg.data = error_msg;
    error_pub_->publish(msg);
}

std::array<double, 7> ArmControlNode::poseToArray(const geometry_msgs::msg::Pose& msg) const {
    return {msg.position.x,
            msg.position.y,
            msg.position.z,
            msg.orientation.x,
            msg.orientation.y,
            msg.orientation.z,
            msg.orientation.w};
}

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ArmControlNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
