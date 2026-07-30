import rclpy
import argparse
from rclpy.node import Node
import time
import threading
import tkinter as tk
from tkinter import ttk
from typing import List

from carm import CArmSingleCol

from std_msgs.msg import String, Bool, Int16MultiArray, Int8, MultiArrayDimension, MultiArrayLayout
from geometry_msgs.msg import Pose, PoseStamped
from sensor_msgs.msg import JointState

class ArmControlNode(Node):
    def __init__(self, ip_addr="10.42.0.101", arm_index=0, gui=False):
        super().__init__('arm_control_sdk')
        self.get_logger().info("ArmControlNode started.")
        self.ip_addr = ip_addr
        self.arm_index = arm_index

        # 初始化 CARM API (将在UI连接时真正实例化)
        self.carm_ = None
        self.gui_enabled = gui

        # -------------------- 订阅器与发布器配置 --------------------
        self.sub_configs = {
            "connect": (String, self.connect_cb),
            "ready": (Bool, self.ready_cb),
            "emergency_stop": (Bool, self.stop_cb),
            "move_joint": (JointState, self.move_joint_cb),
            "move_pose": (Pose, self.move_pose_cb),
            "move_line_joint": (JointState, self.move_line_joint_cb),
            "move_line_pose": (Pose, self.move_line_pose_cb),
            "move_tracking_joint": (JointState, self.move_tracking_joint_cb),
            "move_tracking_pose": (Pose, self.move_tracking_pose_cb),
            "set_speed_level": (Int16MultiArray, self.set_speed_level_cb),
            "set_servo_enable": (Bool, self.set_servo_enable_cb),
            "set_collision_config": (Int16MultiArray, self.set_collision_config_cb),
            "set_eeff": (JointState, self.set_eeff_cb),
            "set_control_mode": (Int8, self.set_control_mode_cb),
        }
        self.pub_configs = {
            "real_joint_state": JointState,
            "flange_cart_state": PoseStamped,
            "arm_state": Int16MultiArray,
            "task_completion": String,
            "carm_error": String,
        }
        if not self.gui_enabled:
            del self.sub_configs["connect"]
        self.subs_ = {}
        self.pubs_ = {}
        self.real_joint_state_pub_ = None
        self.flange_cart_state_pub_ = None
        self.arm_state_pub_ = None
        self.task_completion_pub_ = None
        self.error_pub_ = None
        
        # 初始化所有话题
        self.update_topics({k: k for k in list(self.sub_configs.keys()) + list(self.pub_configs.keys())})

        self.get_logger().info("Node initialized. Waiting for connection command...")
        if not self.gui_enabled:
            self.connect_device(ip_addr, arm_index)

    def connect_device(self, ip_addr, arm_index=0):
        if self.carm_ is not None:
            self.carm_.disconnect()
        self.carm_ = CArmSingleCol(ip_addr, port=8090, timeout=1.0, arm_index=arm_index)
        self.ip_addr = ip_addr
        self.arm_index = arm_index
        self.carm_.register_error_cbk("CARM-ROS", self.error_cb)
        self.carm_.register_completion_cbk("CARM-ROS", self.task_completion_cb)
        self.carm_.register_joint_cbk("CARM-ROS", self.joint_cb)
        self.carm_.register_pose_cbk("CARM-ROS", self.pose_cb)
        if not self.gui_enabled:
            self.carm_.set_ready()

    def update_topics(self, topic_names):
        """重启并应用新的话题名称"""
        # 清除旧的话题
        for sub in self.subs_.values():
            self.destroy_subscription(sub)
        for pub in self.pubs_.values():
            self.destroy_publisher(pub)
        self.subs_.clear()
        self.pubs_.clear()
        
        # 创建新的订阅器
        for key, (msg_type, cb) in self.sub_configs.items():
            topic_name = topic_names.get(key, key)
            self.subs_[key] = self.create_subscription(msg_type, topic_name, cb, 10)
            
        # 创建新的发布器
        for key, msg_type in self.pub_configs.items():
            topic_name = topic_names.get(key, key)
            self.pubs_[key] = self.create_publisher(msg_type, topic_name, 10)
            
        # 更新内部直接调用的发布器引用
        self.real_joint_state_pub_ = self.pubs_["real_joint_state"]
        self.flange_cart_state_pub_ = self.pubs_["flange_cart_state"]
        self.arm_state_pub_ = self.pubs_["arm_state"]
        self.task_completion_pub_ = self.pubs_["task_completion"]
        self.error_pub_ = self.pubs_["carm_error"]
        self.get_logger().info("Topics have been updated.")

    # -------------------- Subscriber Callbacks --------------------
    def connect_cb(self, msg: String):
        if self.carm_ is None: return
        if not msg.data:
            self.carm_.disconnect()
            self.get_logger().info("connect: disconnect")
        else:
            self.carm_.connect(msg.data)
            self.get_logger().info(f"connect: {msg.data}")

    def ready_cb(self, msg: Bool):
        if self.carm_ is None: return
        self.carm_.set_ready()
        self.get_logger().info("set_ready")

    def stop_cb(self, msg: Bool):
        if self.carm_ is None: return
        self.carm_.emergency_stop()
        self.get_logger().info("emergency_stop")

    def move_joint_cb(self, msg: JointState):
        if self.carm_ is None: return
        self.carm_.move_joint(list(msg.position), desire_time=-1, is_sync=False)
        self.get_logger().info("move_joint")

    def move_pose_cb(self, msg: Pose):
        if self.carm_ is None: return
        cart = [msg.position.x, msg.position.y, msg.position.z,
                msg.orientation.x, msg.orientation.y, msg.orientation.z, msg.orientation.w]
        self.carm_.move_pose(cart, desire_time=-1, is_sync=False)
        self.get_logger().info("move_pose")

    def move_line_joint_cb(self, msg: JointState):
        if self.carm_ is None: return
        self.carm_.move_line_joint(list(msg.position), is_sync=False)
        self.get_logger().info("move_line_joint")

    def move_line_pose_cb(self, msg: Pose):
        if self.carm_ is None: return
        cart = [msg.position.x, msg.position.y, msg.position.z,
                msg.orientation.x, msg.orientation.y, msg.orientation.z, msg.orientation.w]
        self.carm_.move_line_pose(cart, is_sync=False)
        self.get_logger().info("move_line_pose")

    def move_tracking_joint_cb(self, msg: JointState):
        if self.carm_ is None: return
        joint_positions = []
        eeff_positions = []
        for i, name in enumerate(msg.name):
            if "joint" in name:
                joint_positions.append(msg.position[i])
            elif "gripper" in name or "eeff" in name:
                eeff_positions.append(msg.position[i])

        if eeff_positions:
            self.carm_.track_joint(joint_positions, eeff_pos=eeff_positions[0])
        else:
            self.carm_.track_joint(joint_positions)

    def move_tracking_pose_cb(self, msg: Pose):
        if self.carm_ is None: return
        cart = [msg.position.x, msg.position.y, msg.position.z,
                msg.orientation.x, msg.orientation.y, msg.orientation.z, msg.orientation.w]
        self.carm_.track_pose(cart)

    def set_speed_level_cb(self, msg: Int16MultiArray):
        if self.carm_ is None: return
        if len(msg.data) >= 2:
            ret = self.carm_.set_speed_level(msg.data[0], msg.data[1])
            self.get_logger().info(f"set_speed_level, ret = {ret}")

    def set_servo_enable_cb(self, msg: Bool):
        if self.carm_ is None: return
        ret = self.carm_.set_servo_enable(msg.data)
        self.get_logger().info(f"set_servo_enable, ret = {ret}")
        
    def set_collision_config_cb(self, msg: Int16MultiArray):
        if self.carm_ is None: return
        if len(msg.data) >= 2:
            ret = self.carm_.set_collision_config(bool(msg.data[0]), msg.data[1])
            self.get_logger().info(f"set_collision_config, ret = {ret}")

    def set_eeff_cb(self, msg: JointState):
        if self.carm_ is None: return
        if msg.position:
            vel = list(msg.velocity) if msg.velocity else []
            tau = list(msg.effort) if msg.effort else []
            ret = self.carm_.set_eeff(list(msg.position), vel, tau)
            self.get_logger().info(f"set_eeff, ret = {ret}")

    def set_control_mode_cb(self, msg: Int8):
        if self.carm_ is None: return
        ret = self.carm_.set_control_mode(msg.data)
        self.get_logger().info(f"set_control_mode, ret = {ret}")

    # -------------------- Carm Callbacks & Publishers --------------------
    def joint_cb(self, t: float, p: list, v: list, a: list):
        if self.carm_ is None: return
        now = self.get_clock().now()

        joint_msg = JointState()
        joint_msg.header.stamp = now.to_msg()
        joint_msg.header.frame_id = "base_link"

        eeff_state = self.carm_.get_eeff_state()
        eeff_pos = self.carm_.get_eeff_pos()
        eeff_tau = self.carm_.get_eeff_tau()

        if eeff_state >= 0 and eeff_pos:
            joint_msg.name = ["joint1", "joint2", "joint3", "joint4", "joint5", "joint6", "eeff"]
            joint_msg.position = [float(x) for x in p] + [float(eeff_pos[0])]
            joint_msg.velocity = [float(x) for x in v] + [0.0]
            joint_msg.effort = [float(x) for x in a] + ([float(eeff_tau[0])] if eeff_tau else [0.0])
        else:
            joint_msg.name = ["joint1", "joint2", "joint3", "joint4", "joint5", "joint6"]
            joint_msg.position = [float(x) for x in p]
            joint_msg.velocity = [float(x) for x in v]
            joint_msg.effort = [float(x) for x in a]

        self.real_joint_state_pub_.publish(joint_msg)

    def pose_cb(self, t: float, pose: list):
        if self.carm_ is None: return
        now = self.get_clock().now()

        # Publish flange_cart_state
        cart = pose
        if cart and len(cart) >= 7:
            pose_msg = PoseStamped()
            pose_msg.header.stamp = now.to_msg()
            pose_msg.header.frame_id = "base_link"
            pose_msg.pose.position.x = float(cart[0])
            pose_msg.pose.position.y = float(cart[1])
            pose_msg.pose.position.z = float(cart[2])
            pose_msg.pose.orientation.x = float(cart[3])
            pose_msg.pose.orientation.y = float(cart[4])
            pose_msg.pose.orientation.z = float(cart[5])
            pose_msg.pose.orientation.w = float(cart[6])
            self.flange_cart_state_pub_.publish(pose_msg)

        # Publish arm_state
        arm_status = self.carm_.get_status()
        state_msg = Int16MultiArray()
        fsm_str = arm_status.get("fsm_state", "")
        fsm_map = {"ERROR": -1, "IDLE": 0, "POSITION": 1, "MIT": 2, "CURRENT": 3, "PF": 4, "TELEOPERATION": 5, "IMPEDANCE": 6}
        fsm_val = fsm_map.get(fsm_str, 0) if isinstance(fsm_str, str) else 0

        variable_names = [
            "arm_index", "arm_is_connected", "arm_dof", "servo_status",
            "state", "fsm_state", "speed_percentage", "on_debug_mode"
        ]

        state_msg.data = [
            int(arm_status.get("arm_index", 0)),
            1 if self.carm_.is_connected() else 0,
            int(arm_status.get("arm_dof", 0)),
            int(arm_status.get("servo_status", 0)),
            int(arm_status.get("state", 0)),
            fsm_val,
            int(arm_status.get("speed_percentage", 0) * 100),
            1 if arm_status.get("on_debug_mode", False) else 0
        ]

        dim = MultiArrayDimension()
        dim.label = ",".join(variable_names)
        dim.size = len(variable_names)
        dim.stride = len(variable_names)

        layout = MultiArrayLayout()
        layout.dim = [dim]
        layout.data_offset = 0
        state_msg.layout = layout

        self.arm_state_pub_.publish(state_msg)

    def task_completion_cb(self, task_key: str):
        msg = String()
        msg.data = str(task_key)
        self.task_completion_pub_.publish(msg)

    def error_cb(self, err_code: int, err_msg: str):
        msg = String()
        msg.data = f"[{err_code}] {err_msg}"
        self.error_pub_.publish(msg)

class CarMGUI:
    def __init__(self, node):
        self.node = node
        self.root = tk.Tk()
        self.root.title("CARM ROS 2 操作界面")
        self.root.geometry("850x550")
        
        # IP 连接区
        ip_frame = ttk.LabelFrame(self.root, text="设备连接")
        ip_frame.pack(fill="x", padx=10, pady=5)
        
        ttk.Label(ip_frame, text="IP地址:").grid(row=0, column=0, padx=5, pady=5)
        self.ip_entry = ttk.Entry(ip_frame, width=15)
        self.ip_entry.insert(0, self.node.ip_addr)
        self.ip_entry.grid(row=0, column=1, padx=5, pady=5)
        
        ttk.Button(ip_frame, text="连接", command=self.connect).grid(row=0, column=4, padx=5, pady=5)
        ttk.Button(ip_frame, text="断开", command=self.disconnect).grid(row=0, column=5, padx=5, pady=5)
        
        # 状态指示灯
        self.status_light = tk.Canvas(ip_frame, width=20, height=20, highlightthickness=0)
        self.status_light_circle = self.status_light.create_oval(2, 2, 18, 18, fill="gray")
        self.status_light.grid(row=0, column=6, padx=10, pady=5)
        
        self.check_status()
        
        # 话题配置区
        topics_frame = ttk.LabelFrame(self.root, text="ROS 2 话题配置 (修改后点击下方重启)")
        topics_frame.pack(fill="both", expand=True, padx=10, pady=5)
        
        canvas = tk.Canvas(topics_frame)
        scrollbar = ttk.Scrollbar(topics_frame, orient="vertical", command=canvas.yview)
        self.scrollable_frame = ttk.Frame(canvas)
        
        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
        )
        canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)
        
        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")
        
        # 分开两端：Sub 列表和 Pub 列表
        sub_inner = ttk.LabelFrame(self.scrollable_frame, text="订阅话题 (Subscribers)")
        sub_inner.grid(row=0, column=0, padx=10, pady=5, sticky="n")
        
        pub_inner = ttk.LabelFrame(self.scrollable_frame, text="发布话题 (Publishers)")
        pub_inner.grid(row=0, column=1, padx=10, pady=5, sticky="n")
        
        self.topic_entries = {}
        
        # 渲染订阅列表
        for i, key in enumerate(self.node.sub_configs.keys()):
            ttk.Label(sub_inner, text=f"{key}:").grid(row=i, column=0, padx=5, pady=2, sticky="e")
            entry = ttk.Entry(sub_inner, width=25)
            entry.insert(0, key)
            entry.grid(row=i, column=1, padx=5, pady=2, sticky="w")
            self.topic_entries[key] = entry
            
        # 渲染发布列表
        for i, key in enumerate(self.node.pub_configs.keys()):
            ttk.Label(pub_inner, text=f"{key}:").grid(row=i, column=0, padx=5, pady=2, sticky="e")
            entry = ttk.Entry(pub_inner, width=25)
            entry.insert(0, key)
            entry.grid(row=i, column=1, padx=5, pady=2, sticky="w")
            self.topic_entries[key] = entry
            
        ttk.Button(self.root, text="重启并更新话题", command=self.restart_topics).pack(pady=10)
        
    def connect(self):
        ip = self.ip_entry.get()
        arm_idx = self.node.arm_index
        self.node.get_logger().info(f"UI trying to connect: {ip} (arm_index: {arm_idx}) ...")
        
        self.node.connect_device(ip, arm_idx)
        
        # connect 内自带有 1秒的 wait(timeout)，因此 UI 线程最多只会停顿 1 秒左右
        self.node.get_logger().info(f"UI connect finished: {ip}")
        
    def disconnect(self):
        self.node.get_logger().info("UI trying to disconnect ...")
        if self.node.carm_ is not None:
            self.node.carm_.disconnect()
        self.node.get_logger().info("UI disconnect finished")
        
    def restart_topics(self):
        new_topics = {k: v.get().strip() for k, v in self.topic_entries.items() if v.get().strip()}
        self.node.update_topics(new_topics)

    def check_status(self):
        connected = False
        if self.node.carm_ is not None:
            connected = self.node.carm_.is_connected()
        
        color = "green" if connected else "gray"
        self.status_light.itemconfig(self.status_light_circle, fill=color)
        
        # 每 500 毫秒检测一次连接状态
        self.root.after(500, self.check_status)


def main(args=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--gui", action="store_true", help="start the Tkinter control interface")
    parser.add_argument("--ip", default="10.42.0.101", help="initial robot IP address")
    parser.add_argument("--arm_index", type=int, default=0, help="single-arm index")
    parsed, ros_args = parser.parse_known_args(args)
    rclpy.init(args=ros_args)
    node = ArmControlNode(parsed.ip, arm_index=parsed.arm_index, gui=parsed.gui)
    if parsed.gui:
        spin_thread = threading.Thread(target=rclpy.spin, args=(node,), daemon=True)
        spin_thread.start()
        gui = CarMGUI(node)
        gui.root.mainloop()
    else:
        rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
