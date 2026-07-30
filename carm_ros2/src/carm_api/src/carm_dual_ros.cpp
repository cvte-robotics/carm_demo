#include "carm_api/carm_dual_ros.h"

#include <functional>
#include <utility>

namespace {
void splitTrackingJointMessage(const sensor_msgs::msg::JointState& msg,
                               std::vector<double>& joints,
                               double& eeff) {
    joints.clear();
    eeff = -1.0;

    if (!msg.name.empty() && msg.name.size() == msg.position.size()) {
        for (size_t i = 0; i < msg.name.size(); ++i) {
            if (msg.name[i].find("gripper") != std::string::npos ||
                msg.name[i].find("hand") != std::string::npos ||
                msg.name[i].find("eeff") != std::string::npos) {
                eeff = msg.position[i];
            } else {
                joints.push_back(msg.position[i]);
            }
        }
        return;
    }

    if (msg.position.size() > 6) {
        joints.assign(msg.position.begin(), msg.position.begin() + 6);
        eeff = msg.position[6];
    } else {
        joints = msg.position;
    }
}
}  // namespace

DualArmControlNode::DualArmControlNode() : Node("carm_dual_sdk") {
    RCLCPP_INFO(this->get_logger(), "DualArmControlNode started.");

    carm_ = std::make_unique<carm::CArmDualBot>(carm_ip_);

    connect_sub_ = this->create_subscription<std_msgs::msg::String>(
            "connect", 10, std::bind(&DualArmControlNode::connect, this, std::placeholders::_1));
    ready_sub_ = this->create_subscription<std_msgs::msg::Bool>(
            "ready", 10, std::bind(&DualArmControlNode::setReady, this, std::placeholders::_1));
    emergency_stop_sub_ = this->create_subscription<std_msgs::msg::Bool>(
            "emergency_stop",
            10,
            std::bind(&DualArmControlNode::stop, this, std::placeholders::_1));

    left_move_joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "left_move_joint",
            10,
            std::bind(&DualArmControlNode::moveLeftJoint, this, std::placeholders::_1));
    right_move_joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "right_move_joint",
            10,
            std::bind(&DualArmControlNode::moveRightJoint, this, std::placeholders::_1));
    left_move_pose_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
            "left_move_pose",
            10,
            std::bind(&DualArmControlNode::moveLeftPose, this, std::placeholders::_1));
    right_move_pose_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
            "right_move_pose",
            10,
            std::bind(&DualArmControlNode::moveRightPose, this, std::placeholders::_1));
    left_move_line_joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "left_move_line_joint",
            10,
            std::bind(&DualArmControlNode::moveLeftLineJoint, this, std::placeholders::_1));
    right_move_line_joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "right_move_line_joint",
            10,
            std::bind(&DualArmControlNode::moveRightLineJoint, this, std::placeholders::_1));
    left_move_line_pose_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
            "left_move_line_pose",
            10,
            std::bind(&DualArmControlNode::moveLeftLinePose, this, std::placeholders::_1));
    right_move_line_pose_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
            "right_move_line_pose",
            10,
            std::bind(&DualArmControlNode::moveRightLinePose, this, std::placeholders::_1));
    left_move_tracking_joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "left_move_tracking_joint",
            10,
            std::bind(&DualArmControlNode::moveLeftTrackingJoint, this, std::placeholders::_1));
    right_move_tracking_joint_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "right_move_tracking_joint",
            10,
            std::bind(&DualArmControlNode::moveRightTrackingJoint, this, std::placeholders::_1));
    left_move_tracking_pose_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
            "left_move_tracking_pose",
            10,
            std::bind(&DualArmControlNode::moveLeftTrackingPose, this, std::placeholders::_1));
    right_move_tracking_pose_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
            "right_move_tracking_pose",
            10,
            std::bind(&DualArmControlNode::moveRightTrackingPose, this, std::placeholders::_1));

    set_speed_level_sub_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
            "set_speed_level",
            10,
            std::bind(&DualArmControlNode::setSpeedLevel, this, std::placeholders::_1));
    set_servo_enable_sub_ = this->create_subscription<std_msgs::msg::Bool>(
            "set_servo_enable",
            10,
            std::bind(&DualArmControlNode::setServoEnable, this, std::placeholders::_1));
    set_collision_config_sub_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
            "set_collision_config",
            10,
            std::bind(&DualArmControlNode::setCollisionConfig, this, std::placeholders::_1));
    left_set_gripper_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "left_set_gripper",
            10,
            std::bind(&DualArmControlNode::setLeftEndEffector, this, std::placeholders::_1));
    right_set_gripper_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "right_set_gripper",
            10,
            std::bind(&DualArmControlNode::setRightEndEffector, this, std::placeholders::_1));
    set_control_mode_sub_ = this->create_subscription<std_msgs::msg::Int8>(
            "set_control_mode",
            10,
            std::bind(&DualArmControlNode::setControlMode, this, std::placeholders::_1));

    left_real_joint_state_pub_ =
            this->create_publisher<sensor_msgs::msg::JointState>("left_real_joint_state", 10);
    right_real_joint_state_pub_ =
            this->create_publisher<sensor_msgs::msg::JointState>("right_real_joint_state", 10);
    left_flange_cart_state_pub_ =
            this->create_publisher<geometry_msgs::msg::PoseStamped>("left_flange_cart_state", 10);
    right_flange_cart_state_pub_ =
            this->create_publisher<geometry_msgs::msg::PoseStamped>("right_flange_cart_state", 10);
    left_arm_state_pub_ =
            this->create_publisher<std_msgs::msg::Int16MultiArray>("left_arm_state", 10);
    right_arm_state_pub_ =
            this->create_publisher<std_msgs::msg::Int16MultiArray>("right_arm_state", 10);
    task_completion_pub_ = this->create_publisher<std_msgs::msg::String>("task_completion", 10);
    error_pub_ = this->create_publisher<std_msgs::msg::String>("carm_error", 10);

    RCLCPP_INFO(this->get_logger(), "Waiting for connection...");
    rclcpp::sleep_for(std::chrono::seconds(1));
    RCLCPP_INFO(this->get_logger(), "Connection established, enabling dual arm...");
    carm_->set_ready();

    carm_->register_left_joint_cbk(std::bind(&DualArmControlNode::leftJointPublisher,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2,
                                             std::placeholders::_3,
                                             std::placeholders::_4));
    carm_->register_right_joint_cbk(std::bind(&DualArmControlNode::rightJointPublisher,
                                              this,
                                              std::placeholders::_1,
                                              std::placeholders::_2,
                                              std::placeholders::_3,
                                              std::placeholders::_4));
    carm_->register_left_pose_cbk(std::bind(&DualArmControlNode::leftPosePublisher,
                                            this,
                                            std::placeholders::_1,
                                            std::placeholders::_2));
    carm_->register_right_pose_cbk(std::bind(&DualArmControlNode::rightPosePublisher,
                                             this,
                                             std::placeholders::_1,
                                             std::placeholders::_2));
    carm_->register_error_cbk("error",
                              std::bind(&DualArmControlNode::errorPublisher,
                                        this,
                                        std::placeholders::_1,
                                        std::placeholders::_2));
    carm_->register_completion_cbk(
            "task_completion",
            std::bind(&DualArmControlNode::taskCompletionPublisher, this, std::placeholders::_1));
}

DualArmControlNode::~DualArmControlNode() {
    carm_->release_left_joint_cbk();
    carm_->release_right_joint_cbk();
    carm_->release_left_pose_cbk();
    carm_->release_right_pose_cbk();
    carm_->release_error_cbk("error");
    carm_->release_completion_cbk("task_completion");
}

void DualArmControlNode::connect(const std_msgs::msg::String::SharedPtr msg) {
    const int ret = msg->data.empty() ? carm_->disconnect() : carm_->connect(msg->data);
    RCLCPP_INFO(this->get_logger(), "connect, ret = %d", ret);
}

void DualArmControlNode::setReady(const std_msgs::msg::Bool::SharedPtr msg) {
    (void) msg;
    const int ret = carm_->set_ready();
    RCLCPP_INFO(this->get_logger(), "set_ready, ret = %d", ret);
}

void DualArmControlNode::stop(const std_msgs::msg::Bool::SharedPtr msg) {
    (void) msg;
    carm_->emergency_stop();
}

void DualArmControlNode::moveLeftJoint(const sensor_msgs::msg::JointState::SharedPtr msg) {
    carm_->move_left_joint(msg->position, -1, false);
    RCLCPP_INFO(this->get_logger(), "move_left_joint");
}

void DualArmControlNode::moveRightJoint(const sensor_msgs::msg::JointState::SharedPtr msg) {
    carm_->move_right_joint(msg->position, -1, false);
    RCLCPP_INFO(this->get_logger(), "move_right_joint");
}

void DualArmControlNode::moveLeftPose(const geometry_msgs::msg::Pose::SharedPtr msg) {
    carm_->move_left_pose(poseToArray(*msg), -1, false);
    RCLCPP_INFO(this->get_logger(), "move_left_pose");
}

void DualArmControlNode::moveRightPose(const geometry_msgs::msg::Pose::SharedPtr msg) {
    carm_->move_right_pose(poseToArray(*msg), -1, false);
    RCLCPP_INFO(this->get_logger(), "move_right_pose");
}

void DualArmControlNode::moveLeftLineJoint(const sensor_msgs::msg::JointState::SharedPtr msg) {
    (void) msg;
    RCLCPP_WARN(this->get_logger(), "move_left_line_joint is not exposed by CArmDualBot SDK");
}

void DualArmControlNode::moveRightLineJoint(const sensor_msgs::msg::JointState::SharedPtr msg) {
    (void) msg;
    RCLCPP_WARN(this->get_logger(), "move_right_line_joint is not exposed by CArmDualBot SDK");
}

void DualArmControlNode::moveLeftLinePose(const geometry_msgs::msg::Pose::SharedPtr msg) {
    (void) msg;
    RCLCPP_WARN(this->get_logger(), "move_left_line_pose is not exposed by CArmDualBot SDK");
}

void DualArmControlNode::moveRightLinePose(const geometry_msgs::msg::Pose::SharedPtr msg) {
    (void) msg;
    RCLCPP_WARN(this->get_logger(), "move_right_line_pose is not exposed by CArmDualBot SDK");
}

void DualArmControlNode::moveLeftTrackingJoint(const sensor_msgs::msg::JointState::SharedPtr msg) {
    std::vector<double> joints;
    double eeff = -1.0;
    splitTrackingJointMessage(*msg, joints, eeff);
    carm_->track_left_joint(joints, eeff);
}

void DualArmControlNode::moveRightTrackingJoint(const sensor_msgs::msg::JointState::SharedPtr msg) {
    std::vector<double> joints;
    double eeff = -1.0;
    splitTrackingJointMessage(*msg, joints, eeff);
    carm_->track_right_joint(joints, eeff);
}

void DualArmControlNode::moveLeftTrackingPose(const geometry_msgs::msg::Pose::SharedPtr msg) {
    carm_->track_left_pose(poseToArray(*msg));
}

void DualArmControlNode::moveRightTrackingPose(const geometry_msgs::msg::Pose::SharedPtr msg) {
    carm_->track_right_pose(poseToArray(*msg));
}

void DualArmControlNode::setSpeedLevel(const std_msgs::msg::Int16MultiArray::SharedPtr msg) {
    if (msg->data.size() >= 2) {
        const int ret = carm_->set_speed_level(msg->data[0], msg->data[1]);
        RCLCPP_INFO(this->get_logger(), "set_speed_level, ret = %d", ret);
    }
}

void DualArmControlNode::setControlMode(const std_msgs::msg::Int8::SharedPtr msg) {
    const int ret = carm_->set_control_mode(msg->data);
    RCLCPP_INFO(this->get_logger(), "set_control_mode, ret = %d", ret);
}

void DualArmControlNode::setServoEnable(const std_msgs::msg::Bool::SharedPtr msg) {
    const int ret = carm_->set_servo_enable(msg->data);
    RCLCPP_INFO(this->get_logger(), "set_servo_enable, ret = %d", ret);
}

void DualArmControlNode::setCollisionConfig(const std_msgs::msg::Int16MultiArray::SharedPtr msg) {
    if (msg->data.size() >= 2) {
        const int ret = carm_->set_collision_config(msg->data[0], msg->data[1]);
        RCLCPP_INFO(this->get_logger(), "set_collision_config, ret = %d", ret);
    }
}

void DualArmControlNode::setLeftEndEffector(const sensor_msgs::msg::JointState::SharedPtr msg) {
    if (!msg->position.empty()) {
        const double tau = msg->effort.empty() ? 10.0 : msg->effort[0];
        const int ret = carm_->set_left_gripper(msg->position[0], tau);
        RCLCPP_INFO(this->get_logger(), "set_left_gripper, ret = %d", ret);
    }
}

void DualArmControlNode::setRightEndEffector(const sensor_msgs::msg::JointState::SharedPtr msg) {
    if (!msg->position.empty()) {
        const double tau = msg->effort.empty() ? 10.0 : msg->effort[0];
        const int ret = carm_->set_right_gripper(msg->position[0], tau);
        RCLCPP_INFO(this->get_logger(), "set_right_gripper, ret = %d", ret);
    }
}

void DualArmControlNode::leftJointPublisher(double t,
                                            std::vector<double> p,
                                            std::vector<double> v,
                                            std::vector<double> a) {
    publishJointState(left_real_joint_state_pub_,
                      "left_base_link",
                      "left_",
                      t,
                      std::move(p),
                      std::move(v),
                      std::move(a),
                      carm_->get_left_gripper_state(),
                      carm_->get_left_eeff_pos(),
                      carm_->get_left_eeff_vel(),
                      carm_->get_left_eeff_tau());
}

void DualArmControlNode::rightJointPublisher(double t,
                                             std::vector<double> p,
                                             std::vector<double> v,
                                             std::vector<double> a) {
    publishJointState(right_real_joint_state_pub_,
                      "right_base_link",
                      "right_",
                      t,
                      std::move(p),
                      std::move(v),
                      std::move(a),
                      carm_->get_right_gripper_state(),
                      carm_->get_right_eeff_pos(),
                      carm_->get_right_eeff_vel(),
                      carm_->get_right_eeff_tau());
}

void DualArmControlNode::leftPosePublisher(double t, std::array<double, 7> p) {
    publishPoseAndState(left_flange_cart_state_pub_,
                        left_arm_state_pub_,
                        "left_base_link",
                        t,
                        p,
                        carm_->get_left_status());
}

void DualArmControlNode::rightPosePublisher(double t, std::array<double, 7> p) {
    publishPoseAndState(right_flange_cart_state_pub_,
                        right_arm_state_pub_,
                        "right_base_link",
                        t,
                        p,
                        carm_->get_right_status());
}

void DualArmControlNode::taskCompletionPublisher(const std::string task_key) {
    auto msg = std_msgs::msg::String();
    msg.data = task_key;
    task_completion_pub_->publish(msg);
}

void DualArmControlNode::errorPublisher(int code, const std::string error_msg) {
    (void) code;
    auto msg = std_msgs::msg::String();
    msg.data = error_msg;
    error_pub_->publish(msg);
}

std::array<double, 7> DualArmControlNode::poseToArray(const geometry_msgs::msg::Pose& msg) const {
    return {msg.position.x,
            msg.position.y,
            msg.position.z,
            msg.orientation.x,
            msg.orientation.y,
            msg.orientation.z,
            msg.orientation.w};
}

void DualArmControlNode::publishJointState(
        rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher,
        const std::string& frame_id,
        const std::string& arm_prefix,
        double t,
        std::vector<double> p,
        std::vector<double> v,
        std::vector<double> a,
        int eeff_state,
        const std::vector<double>& eeff_pos,
        const std::vector<double>& eeff_vel,
        const std::vector<double>& eeff_tau) {
    auto real_joint_msg = std::make_unique<sensor_msgs::msg::JointState>();
    const int64_t secs = static_cast<int64_t>(t);
    const uint32_t nsecs = static_cast<uint32_t>((t - secs) * 1e9);

    real_joint_msg->header.stamp.sec = secs;
    real_joint_msg->header.stamp.nanosec = nsecs;
    real_joint_msg->header.frame_id = frame_id;

    for (size_t i = 0; i < p.size(); ++i) {
        real_joint_msg->name.push_back(arm_prefix + "joint" + std::to_string(i + 1));
    }
    real_joint_msg->position = std::move(p);
    real_joint_msg->velocity = std::move(v);
    real_joint_msg->effort = std::move(a);

    if (eeff_state >= 0 && !eeff_pos.empty()) {
        real_joint_msg->name.push_back(arm_prefix + "gripper");
        real_joint_msg->position.push_back(eeff_pos[0]);
        real_joint_msg->velocity.push_back(eeff_vel.empty() ? 0.0 : eeff_vel[0]);
        real_joint_msg->effort.push_back(eeff_tau.empty() ? 0.0 : eeff_tau[0]);
    }

    publisher->publish(std::move(real_joint_msg));
}

void DualArmControlNode::publishPoseAndState(
        rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pose_publisher,
        rclcpp::Publisher<std_msgs::msg::Int16MultiArray>::SharedPtr state_publisher,
        const std::string& frame_id,
        double t,
        const std::array<double, 7>& p,
        const carm::ArmStatus& arm_status) {
    auto flange_msg = std::make_unique<geometry_msgs::msg::PoseStamped>();
    const int64_t secs = static_cast<int64_t>(t);
    const uint32_t nsecs = static_cast<uint32_t>((t - secs) * 1e9);

    flange_msg->header.stamp.sec = secs;
    flange_msg->header.stamp.nanosec = nsecs;
    flange_msg->header.frame_id = frame_id;
    flange_msg->pose.position.x = p[0];
    flange_msg->pose.position.y = p[1];
    flange_msg->pose.position.z = p[2];
    flange_msg->pose.orientation.x = p[3];
    flange_msg->pose.orientation.y = p[4];
    flange_msg->pose.orientation.z = p[5];
    flange_msg->pose.orientation.w = p[6];
    pose_publisher->publish(std::move(flange_msg));

    const std::vector<std::string> variable_names = {"arm_index",
                                                     "arm_is_connected",
                                                     "arm_dof",
                                                     "servo_status",
                                                     "state",
                                                     "fsm_state",
                                                     "speed_percentage",
                                                     "on_debug_mode"};

    auto array_msg = std::make_unique<std_msgs::msg::Int16MultiArray>();
    array_msg->data = {static_cast<int16_t>(arm_status.arm_index),
                       static_cast<int16_t>(arm_status.arm_is_connected),
                       static_cast<int16_t>(arm_status.arm_dof),
                       static_cast<int16_t>(arm_status.servo_status),
                       static_cast<int16_t>(arm_status.state),
                       static_cast<int16_t>(arm_status.fsm_state),
                       static_cast<int16_t>(arm_status.speed_percentage),
                       static_cast<int16_t>(arm_status.on_debug_mode)};

    std_msgs::msg::MultiArrayDimension dim;
    for (const auto& name : variable_names) {
        if (!dim.label.empty()) {
            dim.label += ",";
        }
        dim.label += name;
    }
    dim.size = variable_names.size();
    dim.stride = variable_names.size();
    array_msg->layout.dim.push_back(dim);
    array_msg->layout.data_offset = 0;

    state_publisher->publish(std::move(array_msg));
}

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<DualArmControlNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
