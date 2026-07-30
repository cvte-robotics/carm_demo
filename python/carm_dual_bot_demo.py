#!/usr/bin/env python3
"""
carm_py_dual_demo.py —— 人形双臂 SDK 完整示例（基于 CArmDualBot）。

对齐 carm.py 中 CArmDualBot 的全部接口，覆盖连接、控制、状态查询、
运动指令、夹爪/末端执行器、灵巧手、轨迹运动、运动学、示教、回调等。
"""

import time
from typing import List

from carm import CArmDualBot

# ==================== 全局实例 ====================
carm_: CArmDualBot = None


# ==================== 连接 / 断开 ====================

def connect(data: str) -> int:
    ret = 0
    if data == "":
        ret = carm_.disconnect()
    else:
        ret = carm_.connect(data)
    print(f"connect_carm, ret = {ret}")
    return ret

def is_connect() -> bool:
    return carm_.is_connected()

# ==================== 基础操作 ====================

def set_ready() -> int:
    ret = carm_.set_ready()
    print(f"set_ready, ret = {ret}")
    return ret

def set_servo_enable(flag: bool) -> int:
    ret = carm_.set_servo_enable(flag)
    print(f"set_servo_enable, ret = {ret}")
    return ret

def set_control_mode(mode: int) -> int:
    ret = carm_.set_control_mode(mode)
    print(f"set_control_mode, ret = {ret}")
    return ret

def emergency_stop() -> int:
    ret = carm_.emergency_stop()
    print("emergency_stop called.")
    return ret

def get_version() -> str:
    version = carm_.get_version()
    print(f"get_version, version = {version}")
    return version

def get_config():
    config_l = carm_.get_left_config()
    print("[Left] dof: ", config_l.get("dof"))
    print("[Left] limit_upper: ", config_l.get("limit_upper"))
    print("[Left] limit_lower: ", config_l.get("limit_lower"))
    print("[Left] joint_vel: ", config_l.get("joint_vel"))
    print("[Left] joint_acc: ", config_l.get("joint_acc"))
    print("[Left] joint_dec: ", config_l.get("joint_dec"))
    print("[Left] joint_jerk: ", config_l.get("joint_jerk"))

    config_r = carm_.get_right_config()
    print("[Right] dof: ", config_r.get("dof"))
    print("[Right] limit_upper: ", config_r.get("limit_upper"))
    print("[Right] limit_lower: ", config_r.get("limit_lower"))
    print("[Right] joint_vel: ", config_r.get("joint_vel"))
    print("[Right] joint_acc: ", config_r.get("joint_acc"))
    print("[Right] joint_dec: ", config_r.get("joint_dec"))
    print("[Right] joint_jerk: ", config_r.get("joint_jerk"))

def get_status():
    arm_status_l = carm_.get_left_status()
    print("[Left] arm_index: ", arm_status_l.get("arm_index"))
    print("[Left] arm_name: ", arm_status_l.get("arm_name"))
    print("[Left] arm_is_connected: ", arm_status_l.get("arm_is_connected"))
    print("[Left] arm_dof: ", arm_status_l.get("arm_dof"))
    print("[Left] servo_status: ", arm_status_l.get("servo_status"))
    print("[Left] state: ", arm_status_l.get("state"))
    print("[Left] speed_percentage: ", arm_status_l.get("speed_percentage"))
    print("[Left] on_debug_mode: ", arm_status_l.get("on_debug_mode"))

    arm_status_r = carm_.get_right_status()
    print("[Right] arm_index: ", arm_status_r.get("arm_index"))
    print("[Right] arm_name: ", arm_status_r.get("arm_name"))
    print("[Right] arm_is_connected: ", arm_status_r.get("arm_is_connected"))
    print("[Right] arm_dof: ", arm_status_r.get("arm_dof"))
    print("[Right] servo_status: ", arm_status_r.get("servo_status"))
    print("[Right] state: ", arm_status_r.get("state"))
    print("[Right] speed_percentage: ", arm_status_r.get("speed_percentage"))
    print("[Right] on_debug_mode: ", arm_status_r.get("on_debug_mode"))

# ==================== 状态获取 ====================

def get_left_joint_pos() -> List[float]:
    pos = carm_.get_left_joint_pos()
    print(f"get_left_joint_pos, pos = {pos}")
    return pos

def get_right_joint_pos() -> List[float]:
    pos = carm_.get_right_joint_pos()
    print(f"get_right_joint_pos, pos = {pos}")
    return pos

# ==================== 末端执行器（通用 eeff） ====================

def get_left_eeff_state() -> int:
    state = carm_.get_left_eeff_state()
    print(f"get_left_eeff_state, state = {state}")
    return state

def get_right_eeff_state() -> int:
    state = carm_.get_right_eeff_state()
    print(f"get_right_eeff_state, state = {state}")
    return state

def get_left_eeff_pos() -> List[float]:
    pos = carm_.get_left_eeff_pos()
    print(f"get_left_eeff_pos, pos = {pos}")
    return pos

def get_right_eeff_pos() -> List[float]:
    pos = carm_.get_right_eeff_pos()
    print(f"get_right_eeff_pos, pos = {pos}")
    return pos

def set_left_eeff(pos: list, vel: list, tau: list) -> int:
    ret = carm_.set_left_eeff(pos, vel, tau)
    print(f"set_left_eeff, ret = {ret}")
    return ret

def set_right_eeff(pos: list, vel: list, tau: list) -> int:
    ret = carm_.set_right_eeff(pos, vel, tau)
    print(f"set_right_eeff, ret = {ret}")
    return ret

# ==================== 运动控制 ====================

def track_left_joint(targets: List[float], eeff_pos: float = -1.0) -> int:
    ret = carm_.track_left_joint(targets, eeff_pos)
    print(f"track_left_joint, ret = {ret}")
    return ret

def track_right_joint(targets: List[float], eeff_pos: float = -1.0) -> int:
    ret = carm_.track_right_joint(targets, eeff_pos)
    print(f"track_right_joint, ret = {ret}")
    return ret

def track_left_pose(targets: List[float], eeff_pos: float = -1.0) -> int:
    ret = carm_.track_left_pose(targets, eeff_pos)
    print(f"track_left_pose, ret = {ret}")
    return ret

def track_right_pose(targets: List[float], eeff_pos: float = -1.0) -> int:
    ret = carm_.track_right_pose(targets, eeff_pos)
    print(f"track_right_pose, ret = {ret}")
    return ret

def move_left_joint(targets: List[float], desire_time: float = -1.0,
                    is_sync: bool = True) -> int:
    ret = carm_.move_left_joint(targets, desire_time, is_sync)
    print(f"move_left_joint, ret = {ret}")
    return ret

def move_right_joint(targets: List[float], desire_time: float = -1.0,
                     is_sync: bool = True) -> int:
    ret = carm_.move_right_joint(targets, desire_time, is_sync)
    print(f"move_right_joint, ret = {ret}")
    return ret

def move_left_pose(targets: List[float], desire_time: float = -1.0,
                   is_sync: bool = True) -> int:
    ret = carm_.move_left_pose(targets, desire_time, is_sync)
    print(f"move_left_pose, ret = {ret}")
    return ret

def move_right_pose(targets: List[float], desire_time: float = -1.0,
                    is_sync: bool = True) -> int:
    ret = carm_.move_right_pose(targets, desire_time, is_sync)
    print(f"move_right_pose, ret = {ret}")
    return ret

def move_left_joint_traj(target_pos: List[List[float]], eeff_pos: list = None,
                         stamps: list = None, is_sync: bool = True) -> int:
    ret = carm_.move_left_joint_traj(target_pos, eeff_pos, stamps, is_sync)
    print(f"move_left_joint_traj, ret = {ret}")
    return ret

def move_right_joint_traj(target_pos: List[List[float]], eeff_pos: list = None,
                          stamps: list = None, is_sync: bool = True) -> int:
    ret = carm_.move_right_joint_traj(target_pos, eeff_pos, stamps, is_sync)
    print(f"move_right_joint_traj, ret = {ret}")
    return ret

def move_left_pose_traj(target_pos: List[List[float]], eeff_pos: list = None,
                        stamps: list = None, is_sync: bool = True) -> int:
    ret = carm_.move_left_pose_traj(target_pos, eeff_pos, stamps, is_sync)
    print(f"move_left_pose_traj, ret = {ret}")
    return ret

def move_right_pose_traj(target_pos: List[List[float]], eeff_pos: list = None,
                         stamps: list = None, is_sync: bool = True) -> int:
    ret = carm_.move_right_pose_traj(target_pos, eeff_pos, stamps, is_sync)
    print(f"move_right_pose_traj, ret = {ret}")
    return ret

def move_left_flow_pose(target_pos: List[float], line_theta_weight: float = 0.5,
                        accuracy: float = 0.0001, is_sync: bool = True) -> int:
    ret = carm_.move_left_flow_pose(target_pos, line_theta_weight, accuracy, is_sync)
    print(f"move_left_flow_pose, ret = {ret}")
    return ret

def move_right_flow_pose(target_pos: List[float], line_theta_weight: float = 0.5,
                         accuracy: float = 0.0001, is_sync: bool = True) -> int:
    ret = carm_.move_right_flow_pose(target_pos, line_theta_weight, accuracy, is_sync)
    print(f"move_right_flow_pose, ret = {ret}")
    return ret

# ==================== 设置与配置 ====================

def set_speed_level(level: float, response_level: int = 20) -> int:
    ret = carm_.set_speed_level(level, response_level)
    print(f"set_speed_level, ret = {ret}")
    return ret

def set_left_tool_index(index: int) -> int:
    ret = carm_.set_left_tool_index(index)
    print(f"set_left_tool_index, ret = {ret}")
    return ret

def set_right_tool_index(index: int) -> int:
    ret = carm_.set_right_tool_index(index)
    print(f"set_right_tool_index, ret = {ret}")
    return ret

def get_left_tool_index() -> int:
    return carm_.get_left_tool_index()

def get_right_tool_index() -> int:
    return carm_.get_right_tool_index()

def get_left_tool_coordinate(index: int) -> list:
    return carm_.get_left_tool_coordinate(index)

def get_right_tool_coordinate(index: int) -> list:
    return carm_.get_right_tool_coordinate(index)

def set_collision_config(enable_flag: bool = True, sensitivity_level: int = 0) -> int:
    ret = carm_.set_collision_config(enable_flag, sensitivity_level)
    print(f"set_collision_config, ret = {ret}")
    return ret

# ==================== 示教相关 ====================

def trajectory_teach_left(off_on: bool, name: str) -> int:
    ret = carm_.trajectory_teach_left(off_on, name)
    print(f"trajectory_teach_left, ret = {ret}")
    return ret

def trajectory_teach_right(off_on: bool, name: str) -> int:
    ret = carm_.trajectory_teach_right(off_on, name)
    print(f"trajectory_teach_right, ret = {ret}")
    return ret

def trajectory_recorder_left(name: str, is_sync: bool = True) -> int:
    ret = carm_.trajectory_recorder_left(name, is_sync)
    print(f"trajectory_recorder_left, ret = {ret}")
    return ret

def trajectory_recorder_right(name: str, is_sync: bool = True) -> int:
    ret = carm_.trajectory_recorder_right(name, is_sync)
    print(f"trajectory_recorder_right, ret = {ret}")
    return ret

def check_teach():
    left_list: list = []
    right_list: list = []
    ret = carm_.check_teach(left_list, right_list)
    print(f"check_teach, ret = {ret}")
    print("left_traj_list =", left_list)
    print("right_traj_list =", right_list)
    return ret

# ==================== 运动学 ====================

def inverse_kine_left(tool_index: int, quat_pose: List[float],
                      ref_joint: List[float]) -> List[float]:
    jnt_value: List[float] = []
    ret = carm_.inverse_kine_left(tool_index, quat_pose, ref_joint, jnt_value)
    print(f"inverse_kine_left, ret = {ret}, jnt_value = {jnt_value}")
    return jnt_value

def inverse_kine_right(tool_index: int, quat_pose: List[float],
                       ref_joint: List[float]) -> List[float]:
    jnt_value: List[float] = []
    ret = carm_.inverse_kine_right(tool_index, quat_pose, ref_joint, jnt_value)
    print(f"inverse_kine_right, ret = {ret}, jnt_value = {jnt_value}")
    return jnt_value

def forward_kine_left(tool_index: int, jnt_value: List[float]) -> List[float]:
    quat_pose: List[float] = []
    ret = carm_.forward_kine_left(tool_index, jnt_value, quat_pose)
    print(f"forward_kine_left, ret = {ret}, quat_pose = {quat_pose}")
    return quat_pose

def forward_kine_right(tool_index: int, jnt_value: List[float]) -> List[float]:
    quat_pose: List[float] = []
    ret = carm_.forward_kine_right(tool_index, jnt_value, quat_pose)
    print(f"forward_kine_right, ret = {ret}, quat_pose = {quat_pose}")
    return quat_pose

# ==================== 回调 ====================

def joint_publisher(t: float, p: List[float], v: List[float], a: List[float]):
    print(f"time: {t}, joint_pos: {p}, joint_vel: {v}, joint_tau: {a}")

def pose_publisher(t: float, p: List[float]):
    print(f"time: {t}, cart_pose(xyz-xyzw): {p}")

def plan_publisher(t: float, p: List[float], v: List[float], a: List[float]):
    print(f"time: {t}, plan_joint_pos: {p}")

def plan_pose_publisher(t: float, p: List[float]):
    print(f"time: {t}, plan_cart_pose(xyz-xyzw): {p}")

def external_publisher(t: float, tau: List[float], force: List[float]):
    print(f"time: {t}, joints_tau: {tau}, cart_external_force: {force}")

def on_carm_error(err_code: int, err_msg: str):
    if err_code == 3001:
        print("receive a warning, msg = ", err_msg)
    else:
        print("receive an error, code = ", err_code, "msg = ", err_msg)

def on_task_completion(task_key: str):
    print("task_completion: ", task_key)

def release_callbacks():
    carm_.release_left_joint_cbk("joint_l")
    carm_.release_left_pose_cbk("pose_l")
    carm_.release_left_plan_joint_cbk("plan_joint_l")
    carm_.release_left_plan_pose_cbk("plan_pose_l")
    carm_.release_left_external_force_cbk("ext_force_l")

    carm_.release_right_joint_cbk("joint_r")
    carm_.release_right_pose_cbk("pose_r")
    carm_.release_right_plan_joint_cbk("plan_joint_r")
    carm_.release_right_plan_pose_cbk("plan_pose_r")
    carm_.release_right_external_force_cbk("ext_force_r")

    carm_.release_error_cbk("onCarmError")
    carm_.release_completion_cbk("task_completion")


# ==================== main ====================

if __name__ == '__main__':
    # carm_ = CArmDualBot("127.0.0.1")
    carm_ = CArmDualBot("10.42.0.101")
    time.sleep(1)

    print("开始广播机器状态话题")
    carm_.register_error_cbk("onCarmError", on_carm_error)
    carm_.register_completion_cbk("task_completion", on_task_completion)

    carm_.register_left_joint_cbk("joint_l", joint_publisher)
    carm_.register_right_joint_cbk("joint_r", joint_publisher)
    carm_.register_left_pose_cbk("pose_l", pose_publisher)
    carm_.register_right_pose_cbk("pose_r", pose_publisher)
    carm_.register_left_plan_joint_cbk("plan_joint_l", plan_publisher)
    carm_.register_right_plan_joint_cbk("plan_joint_r", plan_publisher)
    carm_.register_left_plan_pose_cbk("plan_pose_l", plan_pose_publisher)
    carm_.register_right_plan_pose_cbk("plan_pose_r", plan_pose_publisher)
    carm_.register_left_external_force_cbk("ext_force_l", external_publisher)
    carm_.register_right_external_force_cbk("ext_force_r", external_publisher)

    print("链接完毕，使能机械臂")
    carm_.set_ready()
    time.sleep(1)

    joint_ = [0, 0, 0, 0, 0, 0, 0]
    print("移动到安全位置")
    carm_.move_left_joint(joint_)
    carm_.move_right_joint(joint_)
    time.sleep(1)