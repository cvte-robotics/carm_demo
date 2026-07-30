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
    ArmControlNode();
    ~ArmControlNode();

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
    std::array<double, 7> poseToArray(const geometry_msgs::Pose& msg) const;

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
