#!/usr/bin/env python3
"""
carm_py_demo.py —— 六轴单臂 SDK 完整示例（基于 CArmSingleCol）。

对齐 carm.py 中 CArmSingleCol 的全部接口，覆盖连接、控制、状态查询、
运动指令、夹爪/末端执行器、轨迹运动、运动学、示教、回调等。
"""

import time
from typing import List

from carm import CArmSingleCol

# ==================== 全局实例 ====================
carm_: CArmSingleCol = None


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
    config = carm_.get_config()
    print("dof: ", config.get("dof"))
    print("limit_upper: ", config.get("limit_upper"))
    print("limit_lower: ", config.get("limit_lower"))
    print("joint_vel: ", config.get("joint_vel"))
    print("joint_acc: ", config.get("joint_acc"))
    print("joint_dec: ", config.get("joint_dec"))
    print("joint_jerk: ", config.get("joint_jerk"))

def get_status():
    arm_status = carm_.get_status()
    print("arm_index: ", arm_status.get("arm_index"))
    print("arm_name: ", arm_status.get("arm_name"))
    print("arm_is_connected: ", arm_status.get("arm_is_connected"))
    print("arm_dof: ", arm_status.get("arm_dof"))
    print("servo_status: ", arm_status.get("servo_status"))
    print("state: ", arm_status.get("state"))
    print("speed_percentage: ", arm_status.get("speed_percentage"))
    print("on_debug_mode: ", arm_status.get("on_debug_mode"))

# ==================== 状态获取 ====================

def get_joint_pos() -> List[float]:
    pos = carm_.get_joint_pos()
    print(f"get_joint_pos, pos = {pos}")
    return pos

def get_joint_vel() -> List[float]:
    vel = carm_.get_joint_vel()
    print(f"get_joint_vel, vel = {vel}")
    return vel

def get_joint_tau() -> List[float]:
    tau = carm_.get_joint_tau()
    print(f"get_joint_tau, tau = {tau}")
    return tau

def get_plan_joint_pos() -> List[float]:
    pos = carm_.get_plan_joint_pos()
    print(f"get_plan_joint_pos, pos = {pos}")
    return pos

def get_plan_joint_vel() -> List[float]:
    vel = carm_.get_plan_joint_vel()
    print(f"get_plan_joint_vel, vel = {vel}")
    return vel

def get_plan_joint_tau() -> List[float]:
    tau = carm_.get_plan_joint_tau()
    print(f"get_plan_joint_tau, tau = {tau}")
    return tau

def get_cart_pose() -> List[float]:
    pose = carm_.get_cart_pose()
    print(f"get_cart_pose, pose(x,y,z,Qx,Qy,Qz,Qw) = {pose}")
    return pose

def get_plan_cart_pose() -> List[float]:
    pose = carm_.get_plan_cart_pose()
    print(f"get_plan_cart_pose, pose(x,y,z,Qx,Qy,Qz,Qw) = {pose}")
    return pose

def get_joint_external_tau() -> List[float]:
    tau = carm_.get_joint_external_tau()
    print(f"get_joint_external_tau, tau = {tau}")
    return tau

def get_cart_external_force() -> List[float]:
    force = carm_.get_cart_external_force()
    print(f"get_cart_external_force, force(fx,fy,fz,tx,ty,tz) = {force}")
    return force

# ==================== 末端执行器（通用 eeff） ====================

def get_eeff_state() -> int:
    state = carm_.get_eeff_state()
    print(f"get_eeff_state, state = {state}")
    return state

def get_eeff_pos() -> List[float]:
    pos = carm_.get_eeff_pos()
    print(f"get_eeff_pos, pos = {pos}")
    return pos

def get_eeff_vel() -> List[float]:
    vel = carm_.get_eeff_vel()
    print(f"get_eeff_vel, vel = {vel}")
    return vel

def get_eeff_tau() -> List[float]:
    tau = carm_.get_eeff_tau()
    print(f"get_eeff_tau, tau = {tau}")
    return tau

def get_plan_eeff_pos() -> List[float]:
    pos = carm_.get_plan_eeff_pos()
    print(f"get_plan_eeff_pos, pos = {pos}")
    return pos

def get_plan_eeff_tau() -> List[float]:
    tau = carm_.get_plan_eeff_tau()
    print(f"get_plan_eeff_tau, tau = {tau}")
    return tau

def set_eeff(pos: list, vel: list, tau: list) -> int:
    ret = carm_.set_eeff(pos, vel, tau)
    print(f"set_eeff, ret = {ret}")
    return ret

# ==================== 运动控制 ====================

def track_joint(targets: List[float], eeff_pos: float = -1.0) -> int:
    ret = carm_.track_joint(targets, eeff_pos)
    print(f"track_joint, ret = {ret}")
    return ret

def track_pose(targets: List[float], eeff_pos: float = -1.0) -> int:
    ret = carm_.track_pose(targets, eeff_pos)
    print(f"track_pose, ret = {ret}")
    return ret

def move_joint(targets: List[float], desire_time: float = -1.0, is_sync: bool = True) -> int:
    ret = carm_.move_joint(targets, desire_time, is_sync)
    print(f"move_joint, ret = {ret}")
    return ret

def move_pose(targets: List[float], desire_time: float = -1.0, is_sync: bool = True) -> int:
    ret = carm_.move_pose(targets, desire_time, is_sync)
    print(f"move_pose, ret = {ret}")
    return ret

def move_line_joint(targets: List[float], is_sync: bool = True) -> int:
    ret = carm_.move_line_joint(targets, is_sync)
    print(f"move_line_joint, ret = {ret}")
    return ret

def move_line_pose(targets: List[float], is_sync: bool = True) -> int:
    ret = carm_.move_line_pose(targets, is_sync)
    print(f"move_line_pose, ret = {ret}")
    return ret

def move_joint_traj(target_pos: List[List[float]], eeff_pos: list = None,
                    stamps: list = None, is_sync: bool = True) -> int:
    ret = carm_.move_joint_traj(target_pos, eeff_pos, stamps, is_sync)
    print(f"move_joint_traj, ret = {ret}")
    return ret

def move_pose_traj(target_pos: List[List[float]], eeff_pos: list = None,
                   stamps: list = None, is_sync: bool = True) -> int:
    ret = carm_.move_pose_traj(target_pos, eeff_pos, stamps, is_sync)
    print(f"move_pose_traj, ret = {ret}")
    return ret

def move_flow_pose(target_pos: List[float], line_theta_weight: float = 0.5,
                   accuracy: float = 0.0001, is_sync: bool = True) -> int:
    ret = carm_.move_flow_pose(target_pos, line_theta_weight, accuracy, is_sync)
    print(f"move_flow_pose, ret = {ret}")
    return ret

# ==================== 设置与配置 ====================

def set_speed_level(level: float, response_level: int = 20) -> int:
    ret = carm_.set_speed_level(level, response_level)
    print(f"set_speed_level, ret = {ret}")
    return ret

def set_tool_index(index: int) -> int:
    ret = carm_.set_tool_index(index)
    print(f"set_tool_index, ret = {ret}")
    return ret

def get_tool_index() -> int:
    return carm_.get_tool_index()

def get_tool_coordinate(index: int) -> list:
    return carm_.get_tool_coordinate(index)

def set_collision_config(enable_flag: bool = True, sensitivity_level: int = 0) -> int:
    ret = carm_.set_collision_config(enable_flag, sensitivity_level)
    print(f"set_collision_config, ret = {ret}")
    return ret

# ==================== 示教相关 ====================

def trajectory_teach(off_on: bool, name: str) -> int:
    ret = carm_.trajectory_teach(off_on, name)
    print(f"trajectory_teach, ret = {ret}")
    return ret

def trajectory_recorder(name: str, is_sync: bool = True) -> int:
    ret = carm_.trajectory_recorder(name, is_sync)
    print(f"trajectory_recorder, ret = {ret}")
    return ret

def check_teach() -> list:
    traj_list: list = []
    carm_.check_teach(traj_list)
    print(f"check_teach, traj_list = {traj_list}")
    return traj_list

# ==================== 运动学 ====================

def inverse_kine_array(tool_index: int, quat_pose_list: List[List[float]],
                       ref_joint_list: List[List[float]]) -> List[float]:
    """批量逆运动学求解"""
    jnt_value: List[float] = []
    ret = carm_.inverse_kine_array(tool_index, quat_pose_list, ref_joint_list, jnt_value)
    print(f"inverse_kine_array, ret = {ret}")
    print("joint_values =", jnt_value)
    return jnt_value

def forward_kine_array(tool_index: int, jnt_value_list: List[List[float]]) -> List[float]:
    """批量正运动学求解"""
    quat_pose: List[float] = []
    ret = carm_.forward_kine_array(tool_index, jnt_value_list, quat_pose)
    print(f"forward_kine_array, ret = {ret}")
    print("quat_poses =", quat_pose)
    return quat_pose

def inverse_kine(tool_index: int, quat_pose: List[float],
                 ref_joint: List[float]) -> List[float]:
    """单点逆解"""
    jnt_value: List[float] = []
    ret = carm_.inverse_kine(tool_index, quat_pose, ref_joint, jnt_value)
    print(f"inverse_kine, ret = {ret}")
    print("joint_value =", jnt_value)
    return jnt_value

def forward_kine(tool_index: int, jnt_value: List[float]) -> List[float]:
    """单点正解"""
    quat_pose: List[float] = []
    ret = carm_.forward_kine(tool_index, jnt_value, quat_pose)
    print(f"forward_kine, ret = {ret}")
    print("quat_pose =", quat_pose)
    return quat_pose

# ==================== 回调 ====================

def joint_publisher(t: float, p: List[float], v: List[float], a: List[float]):
    if carm_.get_eeff_state() >= 0:
        eeff_pos = carm_.get_eeff_pos()
        eeff_vel = carm_.get_eeff_vel()
        eeff_tau = carm_.get_eeff_tau()
        print("time: ", t)
        print("joint_pos: ", p, "eeff_pos: ", eeff_pos)
        print("joint_vel: ", v, "eeff_vel: ", eeff_vel)
        print("joint_tau: ", a, "eeff_tau: ", eeff_tau)
    else:
        print("time: ", t, "joint_pos: ", p)

def plan_publisher(t: float, p: List[float], v: List[float], a: List[float]):
    print("time: ", t, "plan_joint_pos: ", p)

def pose_publisher(t: float, p: List[float]):
    print("time: ", t, "cart_pose(xyz-xyzw): ", p)

def plan_pose_publisher(t: float, p: List[float]):
    print("time: ", t, "plan_cart_pose(xyz-xyzw): ", p)

def external_publisher(t: float, tau: List[float], force: List[float]):
    print("time: ", t, "joints_tau: ", tau, "cart_external_force: ", force)

def on_carm_error(err_code: int, err_msg: str):
    if err_code == 3001:
        print("receive a warning, msg = ", err_msg)
    else:
        print("receive an error, code = ", err_code, "msg = ", err_msg)

def on_task_completion(task_key: str):
    print("task_completion: ", task_key)

def release_callbacks():
    carm_.release_joint_cbk("joint")
    carm_.release_pose_cbk("pose")
    carm_.release_plan_joint_cbk("plan_joint")
    carm_.release_plan_pose_cbk("plan_pose")
    carm_.release_external_force_cbk("ext_force")
    carm_.release_error_cbk("onCarmError")
    carm_.release_completion_cbk("task_completion")


# ==================== main ====================

if __name__ == '__main__':
    # carm_ = CArmSingleCol("127.0.0.1")
    carm_ = CArmSingleCol("10.42.0.101")
    time.sleep(1)

    print("开始广播机器状态话题")
    carm_.register_error_cbk("onCarmError", on_carm_error)
    carm_.register_completion_cbk("task_completion", on_task_completion)

    carm_.register_joint_cbk("joint", joint_publisher)
    carm_.register_pose_cbk("pose", pose_publisher)
    carm_.register_plan_joint_cbk("plan_joint", plan_publisher)
    carm_.register_plan_pose_cbk("plan_pose", plan_pose_publisher)
    carm_.register_external_force_cbk("ext_force", external_publisher)

    print("链接完毕，使能机械臂")
    carm_.set_ready()
    time.sleep(1)

    joint_ = [0, 0, 0, 0, 0, 0]
    print("移动到安全位置")
    carm_.move_joint(joint_)
    time.sleep(1)





