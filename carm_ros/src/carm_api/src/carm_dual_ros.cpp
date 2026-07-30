#include "carm_api/carm_dual_ros.h"

#include <functional>
#include <utility>

namespace {
void splitTrackingJointMessage(const sensor_msgs::JointState& msg,
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

DualArmControlNode::DualArmControlNode() {
    ros::NodeHandle nh;
    ROS_INFO("DualArmControlNode started.");

    carm_ = std::make_unique<carm::CArmDualBot>(carm_ip_);

    connect_sub_ = nh.subscribe("connect", 10, &DualArmControlNode::connect, this);
    ready_sub_ = nh.subscribe("ready", 10, &DualArmControlNode::setReady, this);
    emergency_stop_sub_ = nh.subscribe("emergency_stop", 10, &DualArmControlNode::stop, this);

    left_move_joint_sub_ =
            nh.subscribe("left_move_joint", 10, &DualArmControlNode::moveLeftJoint, this);
    right_move_joint_sub_ =
            nh.subscribe("right_move_joint", 10, &DualArmControlNode::moveRightJoint, this);
    left_move_pose_sub_ =
            nh.subscribe("left_move_pose", 10, &DualArmControlNode::moveLeftPose, this);
    right_move_pose_sub_ =
            nh.subscribe("right_move_pose", 10, &DualArmControlNode::moveRightPose, this);
    left_move_line_joint_sub_ =
            nh.subscribe("left_move_line_joint", 10, &DualArmControlNode::moveLeftLineJoint, this);
    right_move_line_joint_sub_ = nh.subscribe(
            "right_move_line_joint", 10, &DualArmControlNode::moveRightLineJoint, this);
    left_move_line_pose_sub_ =
            nh.subscribe("left_move_line_pose", 10, &DualArmControlNode::moveLeftLinePose, this);
    right_move_line_pose_sub_ =
            nh.subscribe("right_move_line_pose", 10, &DualArmControlNode::moveRightLinePose, this);
    left_move_tracking_joint_sub_ = nh.subscribe(
            "left_move_tracking_joint", 10, &DualArmControlNode::moveLeftTrackingJoint, this);
    right_move_tracking_joint_sub_ = nh.subscribe(
            "right_move_tracking_joint", 10, &DualArmControlNode::moveRightTrackingJoint, this);
    left_move_tracking_pose_sub_ = nh.subscribe(
            "left_move_tracking_pose", 10, &DualArmControlNode::moveLeftTrackingPose, this);
    right_move_tracking_pose_sub_ = nh.subscribe(
            "right_move_tracking_pose", 10, &DualArmControlNode::moveRightTrackingPose, this);

    set_speed_level_sub_ =
            nh.subscribe("set_speed_level", 10, &DualArmControlNode::setSpeedLevel, this);
    set_servo_enable_sub_ =
            nh.subscribe("set_servo_enable", 10, &DualArmControlNode::setServoEnable, this);
    set_collision_config_sub_ =
            nh.subscribe("set_collision_config", 10, &DualArmControlNode::setCollisionConfig, this);
    left_set_gripper_sub_ =
            nh.subscribe("left_set_gripper", 10, &DualArmControlNode::setLeftEndEffector, this);
    right_set_gripper_sub_ =
            nh.subscribe("right_set_gripper", 10, &DualArmControlNode::setRightEndEffector, this);
    set_control_mode_sub_ =
            nh.subscribe("set_control_mode", 10, &DualArmControlNode::setControlMode, this);

    left_real_joint_state_pub_ =
            nh.advertise<sensor_msgs::JointState>("left_real_joint_state", 10);
    right_real_joint_state_pub_ =
            nh.advertise<sensor_msgs::JointState>("right_real_joint_state", 10);
    left_flange_cart_state_pub_ =
            nh.advertise<geometry_msgs::PoseStamped>("left_flange_cart_state", 10);
    right_flange_cart_state_pub_ =
            nh.advertise<geometry_msgs::PoseStamped>("right_flange_cart_state", 10);
    left_arm_state_pub_ = nh.advertise<std_msgs::Int16MultiArray>("left_arm_state", 10);
    right_arm_state_pub_ = nh.advertise<std_msgs::Int16MultiArray>("right_arm_state", 10);
    task_completion_pub_ = nh.advertise<std_msgs::String>("task_completion", 10);
    error_pub_ = nh.advertise<std_msgs::String>("carm_error", 10);

    ROS_INFO("Waiting for connection...");
    ros::Duration(1.0).sleep();
    ROS_INFO("Connection established, enabling dual arm...");
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

void DualArmControlNode::connect(const std_msgs::StringConstPtr& msg) {
    const int ret = msg->data.empty() ? carm_->disconnect() : carm_->connect(msg->data);
    ROS_INFO("connect, ret = %d", ret);
}

void DualArmControlNode::setReady(const std_msgs::BoolConstPtr& msg) {
    (void)msg;
    const int ret = carm_->set_ready();
    ROS_INFO("set_ready, ret = %d", ret);
}

void DualArmControlNode::stop(const std_msgs::BoolConstPtr& msg) {
    (void)msg;
    carm_->emergency_stop();
}

void DualArmControlNode::moveLeftJoint(const sensor_msgs::JointStateConstPtr& msg) {
    carm_->move_left_joint(msg->position, -1, false);
    ROS_INFO("move_left_joint");
}

void DualArmControlNode::moveRightJoint(const sensor_msgs::JointStateConstPtr& msg) {
    carm_->move_right_joint(msg->position, -1, false);
    ROS_INFO("move_right_joint");
}

void DualArmControlNode::moveLeftPose(const geometry_msgs::PoseConstPtr& msg) {
    carm_->move_left_pose(poseToArray(*msg), -1, false);
    ROS_INFO("move_left_pose");
}

void DualArmControlNode::moveRightPose(const geometry_msgs::PoseConstPtr& msg) {
    carm_->move_right_pose(poseToArray(*msg), -1, false);
    ROS_INFO("move_right_pose");
}

void DualArmControlNode::moveLeftLineJoint(const sensor_msgs::JointStateConstPtr& msg) {
    (void)msg;
    ROS_WARN("move_left_line_joint is not exposed by CArmDualBot SDK");
}

void DualArmControlNode::moveRightLineJoint(const sensor_msgs::JointStateConstPtr& msg) {
    (void)msg;
    ROS_WARN("move_right_line_joint is not exposed by CArmDualBot SDK");
}

void DualArmControlNode::moveLeftLinePose(const geometry_msgs::PoseConstPtr& msg) {
    (void)msg;
    ROS_WARN("move_left_line_pose is not exposed by CArmDualBot SDK");
}

void DualArmControlNode::moveRightLinePose(const geometry_msgs::PoseConstPtr& msg) {
    (void)msg;
    ROS_WARN("move_right_line_pose is not exposed by CArmDualBot SDK");
}

void DualArmControlNode::moveLeftTrackingJoint(const sensor_msgs::JointStateConstPtr& msg) {
    std::vector<double> joints;
    double eeff = -1.0;
    splitTrackingJointMessage(*msg, joints, eeff);
    carm_->track_left_joint(joints, eeff);
}

void DualArmControlNode::moveRightTrackingJoint(const sensor_msgs::JointStateConstPtr& msg) {
    std::vector<double> joints;
    double eeff = -1.0;
    splitTrackingJointMessage(*msg, joints, eeff);
    carm_->track_right_joint(joints, eeff);
}

void DualArmControlNode::moveLeftTrackingPose(const geometry_msgs::PoseConstPtr& msg) {
    carm_->track_left_pose(poseToArray(*msg));
}

void DualArmControlNode::moveRightTrackingPose(const geometry_msgs::PoseConstPtr& msg) {
    carm_->track_right_pose(poseToArray(*msg));
}

void DualArmControlNode::setSpeedLevel(const std_msgs::Int16MultiArrayConstPtr& msg) {
    if (msg->data.size() >= 2) {
        const int ret = carm_->set_speed_level(msg->data[0], msg->data[1]);
        ROS_INFO("set_speed_level, ret = %d", ret);
    }
}

void DualArmControlNode::setControlMode(const std_msgs::Int8ConstPtr& msg) {
    const int ret = carm_->set_control_mode(msg->data);
    ROS_INFO("set_control_mode, ret = %d", ret);
}

void DualArmControlNode::setServoEnable(const std_msgs::BoolConstPtr& msg) {
    const int ret = carm_->set_servo_enable(msg->data);
    ROS_INFO("set_servo_enable, ret = %d", ret);
}

void DualArmControlNode::setCollisionConfig(const std_msgs::Int16MultiArrayConstPtr& msg) {
    if (msg->data.size() >= 2) {
        const int ret = carm_->set_collision_config(msg->data[0], msg->data[1]);
        ROS_INFO("set_collision_config, ret = %d", ret);
    }
}

void DualArmControlNode::setLeftEndEffector(const sensor_msgs::JointStateConstPtr& msg) {
    if (!msg->position.empty()) {
        const double tau = msg->effort.empty() ? 10.0 : msg->effort[0];
        const int ret = carm_->set_left_gripper(msg->position[0], tau);
        ROS_INFO("set_left_gripper, ret = %d", ret);
    }
}

void DualArmControlNode::setRightEndEffector(const sensor_msgs::JointStateConstPtr& msg) {
    if (!msg->position.empty()) {
        const double tau = msg->effort.empty() ? 10.0 : msg->effort[0];
        const int ret = carm_->set_right_gripper(msg->position[0], tau);
        ROS_INFO("set_right_gripper, ret = %d", ret);
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
    publishPoseAndState(
            left_flange_cart_state_pub_, left_arm_state_pub_, "left_base_link", t, p, carm_->get_left_status());
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
    std_msgs::String msg;
    msg.data = task_key;
    task_completion_pub_.publish(msg);
}

void DualArmControlNode::errorPublisher(int code, const std::string error_msg) {
    (void)code;
    std_msgs::String msg;
    msg.data = error_msg;
    error_pub_.publish(msg);
}

std::array<double, 7> DualArmControlNode::poseToArray(const geometry_msgs::Pose& msg) const {
    return {msg.position.x,
            msg.position.y,
            msg.position.z,
            msg.orientation.x,
            msg.orientation.y,
            msg.orientation.z,
            msg.orientation.w};
}

void DualArmControlNode::publishJointState(ros::Publisher& publisher,
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
    sensor_msgs::JointState real_joint_msg;
    real_joint_msg.header.stamp.fromSec(t);
    real_joint_msg.header.frame_id = frame_id;

    for (size_t i = 0; i < p.size(); ++i) {
        real_joint_msg.name.push_back(arm_prefix + "joint" + std::to_string(i + 1));
    }
    real_joint_msg.position = std::move(p);
    real_joint_msg.velocity = std::move(v);
    real_joint_msg.effort = std::move(a);

    if (eeff_state >= 0 && !eeff_pos.empty()) {
        real_joint_msg.name.push_back(arm_prefix + "gripper");
        real_joint_msg.position.push_back(eeff_pos[0]);
        real_joint_msg.velocity.push_back(eeff_vel.empty() ? 0.0 : eeff_vel[0]);
        real_joint_msg.effort.push_back(eeff_tau.empty() ? 0.0 : eeff_tau[0]);
    }

    publisher.publish(real_joint_msg);
}

void DualArmControlNode::publishPoseAndState(ros::Publisher& pose_publisher,
                                             ros::Publisher& state_publisher,
                                             const std::string& frame_id,
                                             double t,
                                             const std::array<double, 7>& p,
                                             const carm::ArmStatus& arm_status) {
    geometry_msgs::PoseStamped flange_msg;
    flange_msg.header.stamp.fromSec(t);
    flange_msg.header.frame_id = frame_id;
    flange_msg.pose.position.x = p[0];
    flange_msg.pose.position.y = p[1];
    flange_msg.pose.position.z = p[2];
    flange_msg.pose.orientation.x = p[3];
    flange_msg.pose.orientation.y = p[4];
    flange_msg.pose.orientation.z = p[5];
    flange_msg.pose.orientation.w = p[6];
    pose_publisher.publish(flange_msg);

    const std::vector<std::string> variable_names = {"arm_index",
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

    std_msgs::MultiArrayDimension dim;
    for (const auto& name : variable_names) {
        if (!dim.label.empty()) {
            dim.label += ",";
        }
        dim.label += name;
    }
    dim.size = variable_names.size();
    dim.stride = variable_names.size();
    array_msg.layout.dim.push_back(dim);
    array_msg.layout.data_offset = 0;

    state_publisher.publish(array_msg);
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "carm_dual_sdk");
    DualArmControlNode node;
    ros::spin();
    return 0;
}
