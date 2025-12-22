#include "carm_api/carm_ros.hpp"

#include "arm_control_sdk/carm_cobot.h"
#include "arm_control_sdk/data_type_def.h"

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
    std::array<double, 7> cart = {msg->position.x,
                                  msg->position.y,
                                  msg->position.z,
                                  msg->orientation.x,
                                  msg->orientation.y,
                                  msg->orientation.z,
                                  msg->orientation.w};
    carm_->move_pose(cart, -1, false);
    ROS_INFO("move_pose");
}

void ArmControlNode::moveLineJoint(const sensor_msgs::JointStateConstPtr& msg) {
    carm_->move_line_joint(msg->position, false);
    ROS_INFO("move_line_joint");
}

void ArmControlNode::moveLinePose(const geometry_msgs::PoseConstPtr& msg) {
    std::array<double, 7> cart = {msg->position.x,
                                  msg->position.y,
                                  msg->position.z,
                                  msg->orientation.x,
                                  msg->orientation.y,
                                  msg->orientation.z,
                                  msg->orientation.w};
    carm_->move_line_pose(cart, false);
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
    std::array<double, 7> cart = {msg->position.x,
                                  msg->position.y,
                                  msg->position.z,
                                  msg->orientation.x,
                                  msg->orientation.y,
                                  msg->orientation.z,
                                  msg->orientation.w};
    carm_->track_pose(cart);
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
    if (!msg->position.empty() && !msg->effort.empty()) {
        int ret = carm_->set_gripper(msg->position[0], msg->effort[0]);
        ROS_INFO("set_gripper, ret = %d", ret);
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

int main(int argc, char** argv) {
    ros::init(argc, argv, "arm_control_sdk");
    ArmControlNode node;
    ros::spin();
    return 0;
}