import time
import sys
import os
from typing import List
from carm import Carm

global carm_
carm_ = None  # type: Carm

def connect(data: str) -> bool:
    if not carm_: return False
    return carm_.connect()

def is_connect() -> bool:
    if not carm_: return False
    return carm_.is_connected()

def set_ready() -> bool:
    ret = carm_.set_ready()
    print(f"set_ready, ret = {ret}")
    return ret

def set_servo_enable(flag: bool):
    ret = carm_.set_servo_enable(flag)
    print(f"set_servo_enable, ret = {ret}")
    return ret

def set_control_mode(mode: int):
    ret = carm_.set_control_mode(mode)
    print(f"set_control_mode, ret = {ret}")
    return ret

def get_version():
    version = carm_.version
    print(f"get_version, version = {version}")
    return version

def get_config():
    limit = carm_.limit
    if limit:
        print("limit_upper: ", limit.get("limit_upper"))
        print("limit_lower: ", limit.get("limit_lower"))
        print("joint_vel: ", limit.get("joint_vel"))
        print("joint_acc: ", limit.get("joint_acc"))
        print("joint_dec: ", limit.get("joint_dec"))
        print("joint_jerk: ", limit.get("joint_jerk"))
    return limit

def get_joint_pos() -> List[float]:
    pos = carm_.joint_pos
    print(f"get_joint_pos, pos = {pos}")
    return pos

def get_joint_vel() -> List[float]:
    vel = carm_.joint_vel
    print(f"get_joint_vel, vel = {vel}")
    return vel

def get_joint_tau() -> List[float]:
    tau = carm_.joint_tau
    print(f"get_joint_tau, tau = {tau}")
    return tau

def get_plan_joint_pos() -> List[float]:
    pos = carm_.plan_joint_pos
    print(f"get_plan_joint_pos, pos = {pos}")
    return pos

def get_plan_joint_vel() -> List[float]:
    vel = carm_.plan_joint_vel
    print(f"get_plan_joint_vel, vel = {vel}")
    return vel

def get_plan_joint_tau() -> List[float]:
    tau = carm_.plan_joint_tau
    print(f"get_plan_joint_tau, tau = {tau}")
    return tau

def get_cart_pose() -> List[float]:
    pose = carm_.cart_pose
    print(f"get_cart_pose, pose(x,y,z,Qx,Qy,Qz,Qw) = {pose}")
    return pose

def get_joint_external_tau() -> List[float]:
    tau = carm_.joint_external_tau
    print(f"get_joint_external_tau, tau = {tau}")
    return tau

def get_cart_external_force() -> List[float]:
    force = carm_.cart_external_force
    print(f"get_cart_external_force, force(x,y,z,Rx,Ry,Rz) = {force}")
    return force

def get_gripper_state() -> int:
    state = carm_.gripper_state
    print(f"get_gripper_state, state = {state}")
    return state

def get_gripper_pos() -> float:
    pos = carm_.gripper_pos
    print(f"get_gripper_pos, pos = {pos}")
    return pos

def get_gripper_tau() -> float:
    tau = carm_.gripper_tau
    print(f"get_gripper_tau, tau = {tau}")
    return tau

def track_joint(targets : List[float], gripper_pos=None):
    ret = carm_.track_joint(targets, end_effector=gripper_pos)
    print(f"track_joint, ret = {ret}")
    return ret

def track_pose(targets : List[float], gripper_pos=None):
    ret = carm_.track_pose(targets, end_effector=gripper_pos)
    print(f"track_pose, ret = {ret}")
    return ret

def move_joint(targets : List[float], desire_time=-1, is_sync=True):
    ret = carm_.move_joint(targets, tm=desire_time, is_sync=is_sync)
    print(f"move_joint, ret = {ret}")
    return ret

def move_pose(targets : List[float], desire_time=-1, is_sync=True):
    ret = carm_.move_pose(targets, tm=desire_time, is_sync=is_sync)
    print(f"move_pose, ret = {ret}")
    return ret

def move_line_joint(targets : List[float], is_sync=True):
    ret = carm_.move_line_joint(targets, is_sync=is_sync)
    print(f"move_line_joint, ret = {ret}")
    return ret

def move_line_pose(targets : List[float], is_sync=True):
    ret = carm_.move_line_pose(targets, is_sync=is_sync)
    print(f"move_line_pose, ret = {ret}")
    return ret

def emergency_stop():
    ret = carm_.stop(3)
    print("emergency_stop called.")
    return ret

def set_gripper(pos : float, tau=10.0):
    ret = carm_.set_gripper(pos, tau)
    print(f"set_gripper, ret = {ret}")
    return ret

def set_speed_level(level : float, response_level=20):
    ret = carm_.set_speed_level(level, response_level)
    print(f"set_speed_level, ret = {ret}")
    return ret

def set_tool_index(index : int):
    ret = carm_.set_tool_index(index)
    print(f"set_tool_index, ret = {ret}")
    return ret

def get_tool_index() -> int:
    return carm_.tool_index

def get_tool_coordinate(index) -> dict:
    return carm_.get_tool_coordinate(index)

def set_collision_config(enable_flag=True, sensitivity_level=0):
    ret = carm_.set_collision_config(enable_flag, sensitivity_level)
    print(f"set_collision_config, ret = {ret}")
    return ret

def trajectory_teach(off_on : bool, name : str):
    ret = carm_.trajectory_teach(off_on, name)
    print(f"trajectory_teach, ret = {ret}")
    return ret

def trajectory_recorder(name : str):
    ret = carm_.trajectory_recorder(name)
    print(f"trajectory_recorder, ret = {ret}")
    return ret

def check_teach():
    name_list = carm_.check_teach()
    return True, name_list

def inverse_kine(tool_index: int, quat_pose: List[float], ref_joint: List[float]):
    res = carm_.inverse_kine(quat_pose, ref_joint, tool=tool_index)
    print(f"inverse_kine, res = {res}")
    # 提取关节数据 (假设响应具有 "data"->"joint1"等 )
    try:
        jnt = res["data"]["joint1"]
        return True, jnt
    except:
        return False, []

def forward_kine(tool_index: int, jnt_value: List[float]):
    pose = carm_.forward_kine(jnt_value, tool=tool_index)
    print(f"forward_kine, pose = {pose}")
    if pose: return True, pose
    return False, []

def onCarmError(msg):
    print("receive a error, msg = ", msg)

def task_completion(task_key : str):
    print("task_completion: ", task_key)


if __name__ == '__main__':
    print("Init connection...")
    carm_ = Carm(addr="10.42.0.101")
    time.sleep(1)
    print("注册回调")
    carm_.on_error(onCarmError)
    carm_.on_task_finish(task_completion)
    print("链接完毕，使能机械臂")
    set_ready()
    time.sleep(1)

    joint_ = [0, 0, 0, 0, 0, 0]
    print("移动到安全位置")
    carm_.move_joint(joint_)

    # 加入你的逻辑代码，例如：..................

    time.sleep(1)
    print("断开连接")
    carm_.disconnect()

