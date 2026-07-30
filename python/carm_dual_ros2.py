import rclpy
import argparse
from rclpy.node import Node
import time
import threading
import tkinter as tk
from tkinter import ttk
from typing import List

from carm import CArmDualBot

from std_msgs.msg import String, Bool, Int16MultiArray, Int8, MultiArrayDimension, MultiArrayLayout
from geometry_msgs.msg import Pose, PoseStamped
from sensor_msgs.msg import JointState

class DualArmControlNode(Node):
    def __init__(self, ip_addr="10.42.0.101", gui=False):
        super().__init__('carm_dual_sdk')
        self.get_logger().info("DualArmControlNode started.")
        self.ip_addr = ip_addr

        # 初始化 CARM API (将在UI连接时真正实例化)
        self.carm_ = None
        self.gui_enabled = gui

        # -------------------- 订阅器与发布器配置 --------------------
        self.sub_configs = {
            "connect": (String, self.connect_cb),
            "ready": (Bool, self.ready_cb),
            "emergency_stop": (Bool, self.stop_cb),
            "left_move_joint": (JointState, self.move_left_joint_cb),
            "right_move_joint": (JointState, self.move_right_joint_cb),
            "left_move_pose": (Pose, self.move_left_pose_cb),
            "right_move_pose": (Pose, self.move_right_pose_cb),
            "left_move_tracking_joint": (JointState, self.move_left_tracking_joint_cb),
            "right_move_tracking_joint": (JointState, self.move_right_tracking_joint_cb),
            "left_move_tracking_pose": (Pose, self.move_left_tracking_pose_cb),
            "right_move_tracking_pose": (Pose, self.move_right_tracking_pose_cb),
            "set_speed_level": (Int16MultiArray, self.set_speed_level_cb),
            "set_servo_enable": (Bool, self.set_servo_enable_cb),
            "set_collision_config": (Int16MultiArray, self.set_collision_config_cb),
            "left_set_eeff": (JointState, self.set_left_eeff_cb),
            "right_set_eeff": (JointState, self.set_right_eeff_cb),
            "set_control_mode": (Int8, self.set_control_mode_cb),
        }
        self.pub_configs = {
            "left_real_joint_state": JointState,
            "right_real_joint_state": JointState,
            "left_flange_cart_state": PoseStamped,
            "right_flange_cart_state": PoseStamped,
            "left_arm_state": Int16MultiArray,
            "right_arm_state": Int16MultiArray,
            "task_completion": String,
            "carm_error": String,
        }
        if not self.gui_enabled:
            del self.sub_configs["connect"]
        self.subs_ = {}
        self.pubs_ = {}

        # 初始化所有话题
        self.update_topics({k: k for k in list(self.sub_configs.keys()) + list(self.pub_configs.keys())})

        self.get_logger().info("DualArmNode initialized. Waiting for connection command...")
        if not self.gui_enabled:
            self.connect_device(ip_addr)

    def connect_device(self, ip_addr):
        if self.carm_ is not None:
            self.carm_.disconnect()
        self.carm_ = CArmDualBot(ip_addr, port=8090, timeout=1.0, left_index=0, right_index=1)
        self.ip_addr = ip_addr
        self.carm_.register_error_cbk("CARM-DUAL-ROS", self.error_cb)
        self.carm_.register_completion_cbk("CARM-DUAL-ROS", self.task_completion_cb)
        self.carm_.register_left_joint_cbk("CARM-DUAL-ROS", self.left_joint_cb)
        self.carm_.register_right_joint_cbk("CARM-DUAL-ROS", self.right_joint_cb)
        self.carm_.register_left_pose_cbk("CARM-DUAL-ROS", self.left_pose_cb)
        self.carm_.register_right_pose_cbk("CARM-DUAL-ROS", self.right_pose_cb)
        if not self.gui_enabled:
            self.carm_.set_ready()

    def update_topics(self, topic_names):
        """重启并应用新的话题名称"""
        for sub in self.subs_.values():
            self.destroy_subscription(sub)
        for pub in self.pubs_.values():
            self.destroy_publisher(pub)
        self.subs_.clear()
        self.pubs_.clear()

        for key, (msg_type, cb) in self.sub_configs.items():
            topic_name = topic_names.get(key, key)
            self.subs_[key] = self.create_subscription(msg_type, topic_name, cb, 10)

        for key, msg_type in self.pub_configs.items():
            topic_name = topic_names.get(key, key)
            self.pubs_[key] = self.create_publisher(msg_type, topic_name, 10)

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

    def move_left_joint_cb(self, msg: JointState):
        if self.carm_ is None: return
        self.carm_.move_left_joint(list(msg.position), desire_time=-1, is_sync=False)
        self.get_logger().info("move_left_joint")

    def move_right_joint_cb(self, msg: JointState):
        if self.carm_ is None: return
        self.carm_.move_right_joint(list(msg.position), desire_time=-1, is_sync=False)
        self.get_logger().info("move_right_joint")

    def move_left_pose_cb(self, msg: Pose):
        if self.carm_ is None: return
        cart = [msg.position.x, msg.position.y, msg.position.z,
                msg.orientation.x, msg.orientation.y, msg.orientation.z, msg.orientation.w]
        self.carm_.move_left_pose(cart, desire_time=-1, is_sync=False)
        self.get_logger().info("move_left_pose")

    def move_right_pose_cb(self, msg: Pose):
        if self.carm_ is None: return
        cart = [msg.position.x, msg.position.y, msg.position.z,
                msg.orientation.x, msg.orientation.y, msg.orientation.z, msg.orientation.w]
        self.carm_.move_right_pose(cart, desire_time=-1, is_sync=False)
        self.get_logger().info("move_right_pose")

    def move_left_tracking_joint_cb(self, msg: JointState):
        if self.carm_ is None: return
        joint_positions = []
        eeff_positions = []
        for i, name in enumerate(msg.name):
            if "joint" in name:
                joint_positions.append(msg.position[i])
            elif "gripper" in name or "eeff" in name:
                eeff_positions.append(msg.position[i])
        if eeff_positions:
            self.carm_.track_left_joint(joint_positions, eeff_pos=eeff_positions[0])
        else:
            self.carm_.track_left_joint(joint_positions)

    def move_right_tracking_joint_cb(self, msg: JointState):
        if self.carm_ is None: return
        joint_positions = []
        eeff_positions = []
        for i, name in enumerate(msg.name):
            if "joint" in name:
                joint_positions.append(msg.position[i])
            elif "gripper" in name or "eeff" in name:
                eeff_positions.append(msg.position[i])
        if eeff_positions:
            self.carm_.track_right_joint(joint_positions, eeff_pos=eeff_positions[0])
        else:
            self.carm_.track_right_joint(joint_positions)

    def move_left_tracking_pose_cb(self, msg: Pose):
        if self.carm_ is None: return
        cart = [msg.position.x, msg.position.y, msg.position.z,
                msg.orientation.x, msg.orientation.y, msg.orientation.z, msg.orientation.w]
        self.carm_.track_left_pose(cart)

    def move_right_tracking_pose_cb(self, msg: Pose):
        if self.carm_ is None: return
        cart = [msg.position.x, msg.position.y, msg.position.z,
                msg.orientation.x, msg.orientation.y, msg.orientation.z, msg.orientation.w]
        self.carm_.track_right_pose(cart)

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

    def set_left_eeff_cb(self, msg: JointState):
        if self.carm_ is None: return
        if msg.position:
            vel = list(msg.velocity) if msg.velocity else []
            tau = list(msg.effort) if msg.effort else []
            ret = self.carm_.set_left_eeff(list(msg.position), vel, tau)
            self.get_logger().info(f"set_left_eeff, ret = {ret}")

    def set_right_eeff_cb(self, msg: JointState):
        if self.carm_ is None: return
        if msg.position:
            vel = list(msg.velocity) if msg.velocity else []
            tau = list(msg.effort) if msg.effort else []
            ret = self.carm_.set_right_eeff(list(msg.position), vel, tau)
            self.get_logger().info(f"set_right_eeff, ret = {ret}")

    def set_control_mode_cb(self, msg: Int8):
        if self.carm_ is None: return
        ret = self.carm_.set_control_mode(msg.data)
        self.get_logger().info(f"set_control_mode, ret = {ret}")

    # -------------------- Carm Callbacks & Publishers --------------------
    def _make_joint_msg(self, t: float, p: list, v: list, a: list,
                        frame_id: str, arm_prefix: str,
                        eeff_state: int, eeff_pos: list, eeff_vel: list, eeff_tau: list):
        now = self.get_clock().now()
        joint_msg = JointState()
        joint_msg.header.stamp = now.to_msg()
        joint_msg.header.frame_id = frame_id

        for i in range(len(p)):
            joint_msg.name.append(f"{arm_prefix}joint{i + 1}")
        joint_msg.position = [float(x) for x in p]
        joint_msg.velocity = [float(x) for x in v]
        joint_msg.effort = [float(x) for x in a]

        if eeff_state >= 0 and eeff_pos:
            joint_msg.name.append(f"{arm_prefix}eeff")
            joint_msg.position.append(float(eeff_pos[0]))
            joint_msg.velocity.append(float(eeff_vel[0]) if eeff_vel else 0.0)
            joint_msg.effort.append(float(eeff_tau[0]) if eeff_tau else 0.0)

        return joint_msg

    def _publish_pose(self, pub_key: str, t: float, pose: list, frame_id: str):
        now = self.get_clock().now()
        if pose and len(pose) >= 7:
            pose_msg = PoseStamped()
            pose_msg.header.stamp = now.to_msg()
            pose_msg.header.frame_id = frame_id
            pose_msg.pose.position.x = float(pose[0])
            pose_msg.pose.position.y = float(pose[1])
            pose_msg.pose.position.z = float(pose[2])
            pose_msg.pose.orientation.x = float(pose[3])
            pose_msg.pose.orientation.y = float(pose[4])
            pose_msg.pose.orientation.z = float(pose[5])
            pose_msg.pose.orientation.w = float(pose[6])
            self.pubs_[pub_key].publish(pose_msg)

    def _publish_arm_state(self, pub_key: str, arm_status: dict):
        state_msg = Int16MultiArray()
        fsm_str = arm_status.get("fsm_state", "")
        fsm_map = {"ERROR": -1, "IDLE": 0, "POSITION": 1, "MIT": 2, "CURRENT": 3,
                   "PF": 4, "TELEOPERATION": 5, "IMPEDANCE": 6}
        fsm_val = fsm_map.get(fsm_str, 0) if isinstance(fsm_str, str) else 0

        variable_names = [
            "arm_index", "arm_is_connected", "arm_dof", "servo_status",
            "state", "fsm_state", "speed_percentage", "on_debug_mode"
        ]
        state_msg.data = [
            int(arm_status.get("arm_index", 0)),
            1 if arm_status.get("arm_is_connected", False) else 0,
            int(arm_status.get("arm_dof", 0)),
            int(arm_status.get("servo_status", 0)),
            int(arm_status.get("state", 0)),
            fsm_val,
            int(arm_status.get("speed_percentage", 0)),
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
        self.pubs_[pub_key].publish(state_msg)

    def left_joint_cb(self, t: float, p: list, v: list, a: list):
        if self.carm_ is None: return
        eeff_state = self.carm_.get_left_eeff_state()
        eeff_pos = self.carm_.get_left_eeff_pos()
        eeff_vel = self.carm_.get_left_eeff_vel()
        eeff_tau = self.carm_.get_left_eeff_tau()
        joint_msg = self._make_joint_msg(
                t, p, v, a, "left_base_link", "left_",
                eeff_state, eeff_pos, eeff_vel, eeff_tau)
        self.pubs_["left_real_joint_state"].publish(joint_msg)

    def right_joint_cb(self, t: float, p: list, v: list, a: list):
        if self.carm_ is None: return
        eeff_state = self.carm_.get_right_eeff_state()
        eeff_pos = self.carm_.get_right_eeff_pos()
        eeff_vel = self.carm_.get_right_eeff_vel()
        eeff_tau = self.carm_.get_right_eeff_tau()
        joint_msg = self._make_joint_msg(
                t, p, v, a, "right_base_link", "right_",
                eeff_state, eeff_pos, eeff_vel, eeff_tau)
        self.pubs_["right_real_joint_state"].publish(joint_msg)

    def left_pose_cb(self, t: float, pose: list):
        if self.carm_ is None: return
        self._publish_pose("left_flange_cart_state", t, pose, "left_base_link")
        self._publish_arm_state("left_arm_state", self.carm_.get_left_status())

    def right_pose_cb(self, t: float, pose: list):
        if self.carm_ is None: return
        self._publish_pose("right_flange_cart_state", t, pose, "right_base_link")
        self._publish_arm_state("right_arm_state", self.carm_.get_right_status())

    def task_completion_cb(self, task_key: str):
        msg = String()
        msg.data = str(task_key)
        self.pubs_["task_completion"].publish(msg)

    def error_cb(self, err_code: int, err_msg: str):
        msg = String()
        msg.data = f"[{err_code}] {err_msg}"
        self.pubs_["carm_error"].publish(msg)


class DualCarMGUI:
    def __init__(self, node):
        self.node = node
        self.root = tk.Tk()
        self.root.title("CARM Dual ROS 2 操作界面")
        self.root.geometry("850x600")

        # IP 连接区
        ip_frame = ttk.LabelFrame(self.root, text="设备连接")
        ip_frame.pack(fill="x", padx=10, pady=5)

        ttk.Label(ip_frame, text="IP地址:").grid(row=0, column=0, padx=5, pady=5)
        self.ip_entry = ttk.Entry(ip_frame, width=15)
        self.ip_entry.insert(0, self.node.ip_addr)
        self.ip_entry.grid(row=0, column=1, padx=5, pady=5)

        ttk.Button(ip_frame, text="连接", command=self.connect).grid(row=0, column=4, padx=5, pady=5)
        ttk.Button(ip_frame, text="断开", command=self.disconnect).grid(row=0, column=5, padx=5, pady=5)

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
                lambda e: canvas.configure(scrollregion=canvas.bbox("all")))
        canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)

        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")

        sub_inner = ttk.LabelFrame(self.scrollable_frame, text="订阅话题 (Subscribers)")
        sub_inner.grid(row=0, column=0, padx=10, pady=5, sticky="n")

        pub_inner = ttk.LabelFrame(self.scrollable_frame, text="发布话题 (Publishers)")
        pub_inner.grid(row=0, column=1, padx=10, pady=5, sticky="n")

        self.topic_entries = {}

        for i, key in enumerate(self.node.sub_configs.keys()):
            ttk.Label(sub_inner, text=f"{key}:").grid(row=i, column=0, padx=5, pady=2, sticky="e")
            entry = ttk.Entry(sub_inner, width=25)
            entry.insert(0, key)
            entry.grid(row=i, column=1, padx=5, pady=2, sticky="w")
            self.topic_entries[key] = entry

        for i, key in enumerate(self.node.pub_configs.keys()):
            ttk.Label(pub_inner, text=f"{key}:").grid(row=i, column=0, padx=5, pady=2, sticky="e")
            entry = ttk.Entry(pub_inner, width=25)
            entry.insert(0, key)
            entry.grid(row=i, column=1, padx=5, pady=2, sticky="w")
            self.topic_entries[key] = entry

        ttk.Button(self.root, text="重启并更新话题", command=self.restart_topics).pack(pady=10)

    def connect(self):
        ip = self.ip_entry.get()
        self.node.get_logger().info(f"UI trying to connect: {ip} ...")

        self.node.connect_device(ip)

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
        self.root.after(500, self.check_status)


def main(args=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("--gui", action="store_true", help="start the Tkinter control interface")
    parser.add_argument("--ip", default="10.42.0.101", help="initial robot IP address")
    parsed, ros_args = parser.parse_known_args(args)
    rclpy.init(args=ros_args)
    node = DualArmControlNode(parsed.ip, gui=parsed.gui)
    if parsed.gui:
        spin_thread = threading.Thread(target=rclpy.spin, args=(node,), daemon=True)
        spin_thread.start()
        gui = DualCarMGUI(node)
        gui.root.mainloop()
    else:
        rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
