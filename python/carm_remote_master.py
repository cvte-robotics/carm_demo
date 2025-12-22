import rclpy
from rclpy.node import Node
import time
import argparse

from carm import carm_py

from std_msgs.msg import String, Bool, Int16MultiArray, MultiArrayLayout, MultiArrayDimension
from geometry_msgs.msg import Point, Pose, PoseArray
from sensor_msgs.msg import JointState
from example_interfaces.srv import AddTwoInts

class ArmControlNode(Node):

    def __init__(self, model):
        super().__init__('arm_remote_master')
        print("ArmRemoteControlNode started.")
        # 假设: self.carm_ 是你机械臂SDK的实例, 需要你实际注入
        # self.carm_ = carm_py.CArmSingleCol("127.0.0.1", 8090, 1)
        self.carm_ = carm_py.CArmSingleCol("10.42.0.101", 8090, 1)
        # 广播
        self.real_joint_state_ = self.create_publisher(
            JointState, "real_joint_state_remote", 10
        )
        self.flange_cart_state_ = self.create_publisher(
            Pose, "flange_cart_state_remote", 10
        )
        self.arm_state_ = self.create_publisher(
            Int16MultiArray, "arm_state_remote", 10
        )

        self.tracking_joint_state_ = self.create_publisher(
            JointState, "move_tracking_joint", 10
        )

        self.tracking_pose_state_ = self.create_publisher(
            Pose, "move_tracking_pose", 10
        )

        print("等待连接>>>>>>>>>>>>>")
        time.sleep(1)
        print("链接完毕，使能机械臂")
        self.carm_.set_ready()
        time.sleep(1)
        joint_ = [0.0, 0.0, 0.0, 0, 0, 0]
        print("移动到安全位置")
        self.carm_.move_joint(joint_)
        time.sleep(3)
        
        #主机端
        print("设置为示教模式")
        self.carm_.set_control_mode(3)
        print("开始广播机器状态话题")
        self.carm_.register_joint_cbk(lambda t, p, v, a:self.data_publisher())
        time.sleep(3)
        status = self.carm_.get_status()
        if  status.fsm_state == 3:
            if model == 0:
                print("开始广播关节跟踪话题: 夹爪：", self.carm_.get_gripper_state())
                self.carm_.register_pose_cbk(lambda t, p:self.tracking_joint_publisher())
            elif model == 1:
                print("开始广播笛卡尔跟踪话题")
                self.carm_.register_pose_cbk(lambda t, p:self.tracking_pose_publisher())
            else:
                print("默认开始广播关节跟踪话题: 夹爪：", self.carm_.get_gripper_state())
                self.carm_.register_pose_cbk(lambda t, p:self.tracking_joint_publisher())
        else:
            print("设置模式失败，请重启程序")


    def tracking_joint_publisher(self):
        if self.carm_.get_gripper_state() >= 0:
            # 发布 real_joint_state
            real_joint_msg = JointState()
            real_joint_msg.name = ["joint1", "joint2", "joint3", "joint4", "joint5", "joint6",  "gripper"]            # list of string
            real_joint_msg.position = self.carm_.get_joint_pos()    # list of float
            real_joint_msg.position.append(self.carm_.get_gripper_pos())
            real_joint_msg.velocity = self.carm_.get_joint_vel()    # list of float
            real_joint_msg.velocity.append(self.carm_.get_gripper_vel())
            real_joint_msg.effort = self.carm_.get_joint_tau()      # list of float
            real_joint_msg.effort.append(self.carm_.get_gripper_tau())
            self.tracking_joint_state_.publish(real_joint_msg)
        else:
            # 发布 real_joint_state
            real_joint_msg = JointState()
            real_joint_msg.name = ["joint1", "joint2", "joint3", "joint4", "joint5", "joint6"]            # list of string
            real_joint_msg.position = self.carm_.get_joint_pos()    # list of float
            real_joint_msg.velocity = self.carm_.get_joint_vel()    # list of float
            real_joint_msg.effort = self.carm_.get_joint_tau()      # list of float
            self.tracking_joint_state_.publish(real_joint_msg)

    def tracking_pose_publisher(self):
        cart = self.carm_.get_cart_pose()
        cart_msg = Pose()
        cart_msg.position.x = cart[0]
        cart_msg.position.y = cart[1]
        cart_msg.position.z = cart[2]
        cart_msg.orientation.x = cart[3]
        cart_msg.orientation.y = cart[4]
        cart_msg.orientation.z = cart[5]
        cart_msg.orientation.w = cart[6]
        self.tracking_pose_state_.publish(cart_msg)

    def data_publisher(self):
        if self.carm_.get_gripper_state() >= 0:
            # 发布 real_joint_state
            real_joint_msg = JointState()
            real_joint_msg.name = ["joint1", "joint2", "joint3", "joint4", "joint5", "joint6", "gripper"]            # list of string
            real_joint_msg.position = self.carm_.get_joint_pos()    # list of float
            real_joint_msg.position.append(self.carm_.get_gripper_pos())
            real_joint_msg.velocity = self.carm_.get_joint_vel()    # list of float
            real_joint_msg.velocity.append(self.carm_.get_gripper_vel())
            real_joint_msg.effort = self.carm_.get_joint_tau()      # list of float
            real_joint_msg.effort.append(self.carm_.get_gripper_tau())
            self.real_joint_state_.publish(real_joint_msg)

        else:
            # 发布 real_joint_state
            real_joint_msg = JointState()
            real_joint_msg.name = ["joint1", "joint2", "joint3", "joint4", "joint5", "joint6"]            # list of string
            real_joint_msg.position = self.carm_.get_joint_pos()    # list of float
            real_joint_msg.velocity = self.carm_.get_joint_vel()    # list of float
            real_joint_msg.effort = self.carm_.get_joint_tau()      # list of float
            self.real_joint_state_.publish(real_joint_msg)


        # 发布 flange_cart_state
        cart = self.carm_.get_cart_pose()
        flange_msg = Pose()
        flange_msg.position.x = cart[0]
        flange_msg.position.y = cart[1]
        flange_msg.position.z = cart[2]
        flange_msg.orientation.x = cart[3]
        flange_msg.orientation.y = cart[4]
        flange_msg.orientation.z = cart[5]
        flange_msg.orientation.w = cart[6]
        self.flange_cart_state_.publish(flange_msg)

        # 发布 arm_state
        # 获取各变量
        arm_status = self.carm_.get_status()

        # 变量名和顺序
        variable_names = [
            'arm_index',
            'arm_is_connected',
            'arm_dof',
            'servo_status',
            'state',
            'fsm_state',
            'speed_percentage',
            'on_debug_mode'
        ]
        
        arm_state_data = [
            arm_status.arm_index,
            int(arm_status.arm_is_connected),
            arm_status.arm_dof,
            int(arm_status.servo_status),
            arm_status.state,
            arm_status.fsm_state,
            int(arm_status.speed_percentage),
            int(arm_status.on_debug_mode)
        ]

        # 构造多维数组的layout
        array_msg = Int16MultiArray()
        array_msg.data = arm_state_data

        # 设置 layout
        dim = MultiArrayDimension()
        dim.label = ','.join(variable_names)  # 使用逗号分隔所有变量名
        dim.size = len(variable_names)
        dim.stride = len(variable_names)
        layout = MultiArrayLayout()
        layout.dim = [dim]
        layout.data_offset = 0
        
        array_msg.layout = layout

        self.arm_state_.publish(array_msg)

def main(model = 0, args=None):
    rclpy.init(args=args)
    print("model:", model)
    node = ArmControlNode(model)
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--model', type=int, default=0, help='model name (default: 0)')
    args, ros_args = parser.parse_known_args()
    main(args.model, args=ros_args)
