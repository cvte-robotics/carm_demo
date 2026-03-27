import time
from typing import List

from carm_py import carm_py

global carm_

def connect(data: str) -> int:
    ret = 0
    if data == "":
        # 断连
        ret = carm_.disconnect()
    else:
        # 连接data = "10.42.0.101",连接输入ip
        ret = carm_.connect(data)
    print(f"connect_carm, ret = {ret}")
    return ret

def is_connect() -> bool:
    # 检查连接状态
    return carm_.is_connected()

# ==================== 基础操作 ====================

def set_ready() -> int:
    # 清除错误，并初始化机械臂
    ret = carm_.set_ready()
    print(f"set_ready, ret = {ret}")
    return ret

def set_servo_enable(flag: bool) -> int:
    # true上使能，false下使能
    ret = carm_.set_servo_enable(flag)
    print(f"set_servo_enable, ret = {ret}")
    return ret

def set_control_mode(mode: int) -> int:
    # 0-idle 空闲模式 1-position 点位控制模式, 2-MIT 力矩模式， 3-drag 拖动模式，4-PF 力位混合模式
    ret = carm_.set_control_mode(mode)
    print(f"set_control_mode, ret = {ret}")
    return ret

def emergency_stop() -> int:
    # 紧急急停，急停后调用set_ready恢复
    ret = carm_.emergency_stop()
    print("emergency_stop called.")
    return ret

def get_version() -> str:
    # 获取版本信息
    version = carm_.get_version()
    print(f"get_version, version = {version}")
    return version

def get_config():
    # 读取配置
    config_l = carm_.get_left_config()
    print("[Left] dof: ", config_l.dof)
    print("[Left] limit_upper: ", config_l.limit_upper)
    print("[Left] limit_lower: ", config_l.limit_lower)
    print("[Left] joint_vel: ", config_l.joint_vel)
    print("[Left] joint_acc: ", config_l.joint_acc)
    print("[Left] joint_dec: ", config_l.joint_dec)
    print("[Left] joint_jerk: ", config_l.joint_jerk)

    config_r = carm_.get_right_config()
    print("[Right] dof: ", config_r.dof)
    print("[Right] limit_upper: ", config_r.limit_upper)
    print("[Right] limit_lower: ", config_r.limit_lower)
    print("[Right] joint_vel: ", config_r.joint_vel)
    print("[Right] joint_acc: ", config_r.joint_acc)
    print("[Right] joint_dec: ", config_r.joint_dec)
    print("[Right] joint_jerk: ", config_r.joint_jerk)

def get_status():
    # 获取状态
    arm_status_l = carm_.get_left_status()
    print("[Left] arm_index: ", arm_status_l.arm_index)
    print("[Left] arm_name: ", arm_status_l.arm_name)
    print("[Left] arm_is_connected: ", arm_status_l.arm_is_connected)
    print("[Left] arm_dof: ", arm_status_l.arm_dof)
    print("[Left] servo_status: ", arm_status_l.servo_status)
    print("[Left] state: ", arm_status_l.state)
    print("[Left] speed_percentage: ", arm_status_l.speed_percentage)
    print("[Left] on_debug_mode: ", arm_status_l.on_debug_mode)

    arm_status_r = carm_.get_right_status()
    print("[Right] arm_index: ", arm_status_r.arm_index)
    print("[Right] arm_name: ", arm_status_r.arm_name)
    print("[Right] arm_is_connected: ", arm_status_r.arm_is_connected)
    print("[Right] arm_dof: ", arm_status_r.arm_dof)
    print("[Right] servo_status: ", arm_status_r.servo_status)
    print("[Right] state: ", arm_status_r.state)
    print("[Right] speed_percentage: ", arm_status_r.speed_percentage)
    print("[Right] on_debug_mode: ", arm_status_r.on_debug_mode)


# ==================== 状态获取相关 ====================

def get_joint_pos() -> List[float]:
    # 获取关节角度
    pos_l = carm_.get_left_joint_pos()
    print(f"get_left_joint_pos, pos_l = {pos_l}")
    pos_r = carm_.get_right_joint_pos()
    print(f"get_right_joint_pos, pos_r = {pos_r}")
    return pos_l + pos_r

def get_joint_vel() -> List[float]:
    # 获取关节速度
    vel_l = carm_.get_left_joint_vel()
    print(f"get_left_joint_vel, vel_l = {vel_l}")
    vel_r = carm_.get_right_joint_vel() # 固定之前 get_right_joint_tau 的 typo
    print(f"get_right_joint_vel, vel_r = {vel_r}")
    return vel_l + vel_r

def get_joint_tau() -> List[float]:
    # 获取关节力矩
    tau_l = carm_.get_left_joint_tau()
    print(f"get_left_joint_tau, tau_l = {tau_l}")
    tau_r = carm_.get_right_joint_tau()
    print(f"get_right_joint_tau, tau_r = {tau_r}")
    return tau_l + tau_r

def get_plan_joint_pos() -> List[float]:
    # 获取规划关节角度 (新增)
    pos_l = carm_.get_left_plan_joint_pos()
    print(f"get_left_plan_joint_pos, pos_l = {pos_l}")
    pos_r = carm_.get_right_plan_joint_pos()
    print(f"get_right_plan_joint_pos, pos_r = {pos_r}")
    return pos_l + pos_r

def get_plan_joint_vel() -> List[float]:
    # 获取规划关节速度 (新增)
    vel_l = carm_.get_left_plan_joint_vel()
    print(f"get_left_plan_joint_vel, vel_l = {vel_l}")
    vel_r = carm_.get_right_plan_joint_vel()
    print(f"get_right_plan_joint_vel, vel_r = {vel_r}")
    return vel_l + vel_r

def get_plan_joint_tau() -> List[float]:
    # 获取规划关节力矩 (新增)
    tau_l = carm_.get_left_plan_joint_tau()
    print(f"get_left_plan_joint_tau, tau_l = {tau_l}")
    tau_r = carm_.get_right_plan_joint_tau()
    print(f"get_right_plan_joint_tau, tau_r = {tau_r}")
    return tau_l + tau_r

def get_cart_pose() -> List[float]:
    # 获取机械臂末端笛卡尔实际位姿
    pose_l = carm_.get_left_cart_pose()
    print(f"get_left_cart_pose, pose_l = {pose_l}")
    pose_r = carm_.get_right_cart_pose()
    print(f"get_right_cart_pose, pose_r = {pose_r}")
    return pose_l + pose_r

def get_plan_cart_pose() -> List[float]:
    # 获取机械臂末端笛卡尔规划位姿 (新增)
    pose_l = carm_.get_left_plan_cart_pose()
    print(f"get_left_plan_cart_pose, pose_l = {pose_l}")
    pose_r = carm_.get_right_plan_cart_pose()
    print(f"get_right_plan_cart_pose, pose_r = {pose_r}")
    return pose_l + pose_r

def get_joint_external_tau() -> List[float]:
    # 获取关节重力补偿后外部力矩 (新增)
    tau_l = carm_.get_left_joint_external_tau()
    print(f"get_left_joint_external_tau, tau_l = {tau_l}")
    tau_r = carm_.get_right_joint_external_tau()
    print(f"get_right_joint_external_tau, tau_r = {tau_r}")
    return tau_l + tau_r

def get_cart_external_force() -> List[float]:
    # 获取末端外部力矩 (新增)
    force_l = carm_.get_left_cart_external_force()
    print(f"get_left_cart_external_force, force_l = {force_l}")
    force_r = carm_.get_right_cart_external_force()
    print(f"get_right_cart_external_force, force_r = {force_r}")
    return force_l + force_r


# ==================== 夹爪相关 ====================

def get_gripper_state() -> List[int]:
    # 获取末端状态
    state_l = carm_.get_left_gripper_state()
    print(f"get_left_gripper_state, state = {state_l}")
    state_r = carm_.get_right_gripper_state()
    print(f"get_right_gripper_state, state = {state_r}")
    return [state_l, state_r]

def get_gripper_pos() -> List[float]:
    # 获取末端位置
    pos_l = carm_.get_left_gripper_pos()
    print(f"get_left_gripper_pos, pos_l = {pos_l}")
    pos_r = carm_.get_right_gripper_pos()
    print(f"get_right_gripper_pos, pos_r = {pos_r}")
    return [pos_l, pos_r]

def get_gripper_vel() -> List[float]:
    # 获取末端速度
    vel_l = carm_.get_left_gripper_vel()
    print(f"get_left_gripper_vel, vel_l = {vel_l}")
    vel_r = carm_.get_right_gripper_vel()
    print(f"get_right_gripper_vel, vel_r = {vel_r}")
    return [vel_l, vel_r]

def get_gripper_tau() -> List[float]:
    # 获取末端力
    tau_l = carm_.get_left_gripper_tau()
    print(f"get_left_gripper_tau, tau_l = {tau_l}")
    tau_r = carm_.get_right_gripper_tau()
    print(f"get_right_gripper_tau, tau_r = {tau_r}")
    return [tau_l, tau_r]

def get_plan_gripper_pos() -> List[float]:
    # 获取规划夹爪位置 (新增)
    pos_l = carm_.get_left_plan_gripper_pos()
    print(f"get_left_plan_gripper_pos, pos_l = {pos_l}")
    pos_r = carm_.get_right_plan_gripper_pos()
    print(f"get_right_plan_gripper_pos, pos_r = {pos_r}")
    return [pos_l, pos_r]

def get_plan_gripper_tau() -> List[float]:
    # 获取规划夹爪力 (新增)
    tau_l = carm_.get_left_plan_gripper_tau()
    print(f"get_left_plan_gripper_tau, tau_l = {tau_l}")
    tau_r = carm_.get_right_plan_gripper_tau()
    print(f"get_right_plan_gripper_tau, tau_r = {tau_r}")
    return [tau_l, tau_r]

def set_gripper(pos : float, tau=10) -> int:
    # 控制末端
    # 夹抓间隔(0-0.08m) 夹抓扭矩(0-20N)
    ret_l = carm_.set_left_gripper(pos, tau)
    print(f"set_left_gripper, ret = {ret_l}")
    ret_r = carm_.set_right_gripper(pos, tau)
    print(f"set_right_gripper, ret = {ret_r}")
    return ret_l and ret_r

# ==================== 灵巧手相关 (新增) ====================

def get_hand_state() -> List[int]:
    # 获取灵巧手状态 (新增)
    state_l = carm_.get_left_hand_state()
    print(f"get_left_hand_state, state_l = {state_l}")
    state_r = carm_.get_right_hand_state()
    print(f"get_right_hand_state, state_r = {state_r}")
    return [state_l, state_r]

def get_hand_pos() -> List[float]:
    # 获取灵巧手位置 (新增)
    pos_l = carm_.get_left_hand_pos()
    print(f"get_left_hand_pos, pos_l = {pos_l}")
    pos_r = carm_.get_right_hand_pos()
    print(f"get_right_hand_pos, pos_r = {pos_r}")
    return pos_l + pos_r

def get_hand_vel() -> List[float]:
    # 获取灵巧手速度 (新增)
    vel_l = carm_.get_left_hand_vel()
    print(f"get_left_hand_vel, vel_l = {vel_l}")
    vel_r = carm_.get_right_hand_vel()
    print(f"get_right_hand_vel, vel_r = {vel_r}")
    return vel_l + vel_r

def get_hand_tau() -> List[float]:
    # 获取灵巧手力矩 (新增)
    tau_l = carm_.get_left_hand_tau()
    print(f"get_left_hand_tau, tau_l = {tau_l}")
    tau_r = carm_.get_right_hand_tau()
    print(f"get_right_hand_tau, tau_r = {tau_r}")
    return tau_l + tau_r

def get_plan_hand_pos() -> List[float]:
    # 获取规划灵巧手位置 (新增)
    pos_l = carm_.get_left_plan_hand_pos()
    print(f"get_left_plan_hand_pos, pos_l = {pos_l}")
    pos_r = carm_.get_right_plan_hand_pos()
    print(f"get_right_plan_hand_pos, pos_r = {pos_r}")
    return pos_l + pos_r

def get_plan_hand_vel() -> List[float]:
    # 获取规划灵巧手速度 (新增)
    vel_l = carm_.get_left_plan_hand_vel()
    print(f"get_left_plan_hand_vel, vel_l = {vel_l}")
    vel_r = carm_.get_right_plan_hand_vel()
    print(f"get_right_plan_hand_vel, vel_r = {vel_r}")
    return vel_l + vel_r

def get_plan_hand_tau() -> List[float]:
    # 获取规划灵巧手力矩 (新增)
    tau_l = carm_.get_left_plan_hand_tau()
    print(f"get_left_plan_hand_tau, tau_l = {tau_l}")
    tau_r = carm_.get_right_plan_hand_tau()
    print(f"get_right_plan_hand_tau, tau_r = {tau_r}")
    return tau_l + tau_r

def set_hand(pos: List[float], tau: List[float] = [], vel: List[float] = []) -> int:
    # 灵巧手控制 (新增)
    ret_l = carm_.set_left_hand(pos, tau, vel)
    print(f"set_left_hand, ret = {ret_l}")
    ret_r = carm_.set_right_hand(pos, tau, vel)
    print(f"set_right_hand, ret = {ret_r}")
    return ret_l and ret_r

# ==================== 运动控制 ====================

def track_joint(targets : List[float], gripper_pos=-1.0) -> int:
    # 跟踪关节
    ret_l = carm_.track_left_joint(targets, gripper_pos)
    print(f"track_left_joint, ret = {ret_l}")
    ret_r = carm_.track_right_joint(targets, gripper_pos)
    print(f"track_right_joint, ret = {ret_r}")
    return ret_l and ret_r

def track_pose(targets : List[float], gripper_pos=-1.0) -> int:
    # 跟踪笛卡尔
    ret_l = carm_.track_left_pose(targets, gripper_pos)
    print(f"track_left_pose, ret = {ret_l}")
    ret_r = carm_.track_right_pose(targets, gripper_pos)
    print(f"track_right_pose, ret = {ret_r}")
    return ret_l and ret_r

# is_sync代表是否阻塞等待任务完成
# desire_time 时间为负则以设定的速度到达通过set_speed_level调整
def move_joint(targets : List[float], desire_time=-1.0, is_sync=True) -> int:
    # 点位关节运动
    ret_l = carm_.move_left_joint(targets, desire_time, is_sync)
    print(f"move_left_joint, ret = {ret_l}")
    ret_r = carm_.move_right_joint(targets, desire_time, is_sync)
    print(f"move_right_joint, ret = {ret_r}")
    return ret_l and ret_r

def move_pose(targets : List[float], desire_time=-1.0, is_sync=True) -> int:
    # 点位末端运动
    ret_l = carm_.move_left_pose(targets, desire_time, is_sync)
    print(f"move_left_pose, ret = {ret_l}")
    ret_r = carm_.move_right_pose(targets, desire_time, is_sync)
    print(f"move_right_pose, ret = {ret_r}")
    return ret_l and ret_r

def move_line_joint(targets : List[float], is_sync=True) -> int:
    # 关节空间线性运动
    ret_l = carm_.move_left_line_joint(targets, is_sync)
    print(f"move_left_line_joint, ret = {ret_l}")
    ret_r = carm_.move_right_line_joint(targets, is_sync)
    print(f"move_right_line_joint, ret = {ret_r}")
    return ret_l and ret_r

def move_line_pose(targets : List[float], is_sync=True) -> int:
    # 笛卡尔空间线性轨迹
    ret_l = carm_.move_left_line_pose(targets, is_sync)
    print(f"move_left_line_pose, ret = {ret_l}")
    ret_r = carm_.move_right_line_pose(targets, is_sync)
    print(f"move_right_line_pose, ret = {ret_r}")
    return ret_l and ret_r

# gripper_pos可为空[]
# stamps可为空[]
def move_joint_traj(target_pos : List[List[float]], gripper_pos : List[float], stamps : List[float], is_sync=True) -> int:
    # 关节轨迹运动
    ret_l = carm_.move_left_joint_traj(target_pos, gripper_pos, stamps, is_sync)
    print(f"move_left_joint_traj, ret = {ret_l}")
    ret_r = carm_.move_right_joint_traj(target_pos, gripper_pos, stamps, is_sync)
    print(f"move_right_joint_traj, ret = {ret_r}")
    return ret_l and ret_r

def move_pose_traj(target_pos : List[List[float]], gripper_pos : List[float], stamps : List[float], is_sync=True) -> int:
    # 末端轨迹运动
    ret_l = carm_.move_left_pose_traj(target_pos, gripper_pos, stamps, is_sync)
    print(f"move_left_pose_traj, ret = {ret_l}")
    ret_r = carm_.move_right_pose_traj(target_pos, gripper_pos, stamps, is_sync)
    print(f"move_right_pose_traj, ret = {ret_r}")
    return ret_l and ret_r

def move_flow_pose(target_pos : List[float], line_theta_weight=0.5, accuracy=0.0001, is_sync=True) -> int:
    # 位姿迭代运动 (新增)
    ret_l = carm_.move_left_flow_pose(target_pos, line_theta_weight, accuracy, is_sync)
    print(f"move_left_flow_pose, ret = {ret_l}")
    ret_r = carm_.move_right_flow_pose(target_pos, line_theta_weight, accuracy, is_sync)
    print(f"move_right_flow_pose, ret = {ret_r}")
    return ret_l and ret_r

# ==================== 设置与配置 ====================

def set_speed_level(level : float, response_level=20) -> int:
    # 设置速度等级(0~10)与响应等级
    ret = carm_.set_speed_level(level, response_level)
    print(f"set_speed_level, ret = {ret}")
    return ret

def set_tool_index(index : int) -> int:
    # 选择工具
    ret_l = carm_.set_left_tool_index(index)
    print(f"set_left_tool_index, ret = {ret_l}")
    ret_r = carm_.set_right_tool_index(index)
    print(f"set_right_tool_index, ret = {ret_r}")
    return ret_l and ret_r

def get_tool_index() -> List[int]:
    # 获取当前工具 index
    return [carm_.get_left_tool_index(), carm_.get_right_tool_index()]

def get_tool_coordinate(index: int) -> List[List[float]]:
    # 获取某工具的坐标系参数
    return [carm_.get_left_tool_coordinate(index), carm_.get_right_tool_coordinate(index)]

def set_collision_config(enable_flag=True, sensitivity_level=0) -> int:
    # 碰撞配置打开还是关闭，以及灵敏度
    ret = carm_.set_collision_config(enable_flag, sensitivity_level)
    print(f"set_collision_config, ret = {ret}")
    return ret

# ==================== 示教相关 ====================

def trajectory_teach(off_on : bool, name : str) -> int:
    # 开启或者停止并保存录制路径
    ret_l = carm_.trajectory_teach_left(off_on, name)
    print(f"trajectory_teach_left, ret = {ret_l}")
    ret_r = carm_.trajectory_teach_right(off_on, name)
    print(f"trajectory_teach_right, ret = {ret_r}")
    return ret_l and ret_r

def trajectory_recorder(name : str, is_sync=True) -> int:
    # 播放对应名字的路径
    ret_l = carm_.trajectory_recorder_left(name, is_sync)
    print(f"trajectory_recorder_left, ret = {ret_l}")
    ret_r = carm_.trajectory_recorder_right(name, is_sync)
    print(f"trajectory_recorder_right, ret = {ret_r}")
    return ret_l and ret_r

def check_teach():
    # 返回: ret, left_traj_list, right_traj_list
    ret, left_list, right_list = carm_.check_teach()
    return ret, left_list, right_list


# ==================== 运动学 ====================

def inverse_kine_array(tool_index : int, quat_pose_list : List[List[float]], ref_joint_list : List[List[float]]) -> int:
    """
    批量逆解
    (这里直接只返回是否成功状态为了简便，日志打印了解)
    """
    ret_l, joints_l = carm_.inverse_kine_left_array(tool_index, quat_pose_list, ref_joint_list)
    print(f"inverse_kine_left_array, ret = {ret_l}")

    ret_r, joints_r = carm_.inverse_kine_right_array(tool_index, quat_pose_list, ref_joint_list)
    print(f"inverse_kine_right_array, ret = {ret_r}")
    return ret_l and ret_r

def forward_kine_array(tool_index : int, jnt_value_list : List[List[float]]) -> int:
    """
    批量正解
    """
    ret_l, poses_l = carm_.forward_kine_left_array(tool_index, jnt_value_list)
    print(f"forward_kine_left_array, ret = {ret_l}")

    ret_r, poses_r = carm_.forward_kine_right_array(tool_index, jnt_value_list)
    print(f"forward_kine_right_array, ret = {ret_r}")
    return ret_l and ret_r

def inverse_kine(tool_index : int, quat_pose : List[float], ref_joint : List[float]) -> int:
    """
    单点逆解
    """
    ret_l, jnt_l = carm_.inverse_kine_left(tool_index, quat_pose, ref_joint)
    print(f"inverse_kine_left, ret = {ret_l}")

    ret_r, jnt_r = carm_.inverse_kine_right(tool_index, quat_pose, ref_joint)
    print(f"inverse_kine_right, ret = {ret_r}")
    return ret_l and ret_r

def forward_kine(tool_index : int, jnt_value : List[float]) -> int:
    """
    单点正解
    """
    ret_l, pose_l = carm_.forward_kine_left(tool_index, jnt_value)
    print(f"forward_kine_left, ret = {ret_l}")

    ret_r, pose_r = carm_.forward_kine_right(tool_index, jnt_value)
    print(f"forward_kine_right, ret = {ret_r}")
    return ret_l and ret_r

# ==================== 回调示例 ====================

# 实际关节位置
def joint_publisher(t : float, p : List[float], v : List[float], a : List[float]):
    print("time: ", t, "joint_pos: ", p, "joint_vel:", v, "joint_tau:", a)

# 规划的关节位置
def plan_publisher(t : float, p : List[float], v : List[float], a : List[float]):
    print("time: ", t, "plan_joint_pos: ", p)

# 实际法兰位置
def pose_publisher(t : float, p : List[float]):
    print("time: ", t, "cart_pose(xyz-xyzw): ", p)

# 规划法兰位姿 (新增)
def plan_pose_publisher(t : float, p : List[float]):
    print("time: ", t, "plan_cart_pose(xyz-xyzw): ", p)

# 外力矩信息（排除重力后）
def external_publisher(t : float, tau : List[float], force : List[float]):
    print("time: ", t, "joints_tau: ", tau, "cart_external_force: ", force)


def onCarmError(err_code : int, err_msg : str):
    error_flag = True
    if (err_code == 3001):
        print("recieve a warnning, msg = ", err_msg)
    else:
        print("recieve a error, code = ", err_code, "msg = ", err_msg)

def task_completion(task_key : str):
    print("task_completion: ", task_key)

def release_callbacks():
    carm_.release_left_joint_cbk()
    carm_.release_left_pose_cbk()
    carm_.release_left_plan_joint_cbk()
    carm_.release_left_plan_pose_cbk() # 新增
    carm_.release_left_external_force_cbk()

    carm_.release_right_joint_cbk()
    carm_.release_right_pose_cbk()
    carm_.release_right_plan_joint_cbk()
    carm_.release_right_plan_pose_cbk() # 新增
    carm_.release_right_external_force_cbk()

    carm_.release_error_cbk("onCarmError")
    carm_.release_completion_cbk("task_completion")


if __name__ == '__main__':
    # carm_ = carm_py.CArmDualBot("127.0.0.1")
    carm_ = carm_py.CArmDualBot("10.42.0.101")
    time.sleep(1)

    print("开始广播机器状态话题")
    carm_.register_error_cbk("onCarmError", lambda err_code, err_msg : onCarmError(err_code, err_msg))
    carm_.register_completion_cbk("task_completion", lambda key : task_completion(key))
    
    # 获取关节
    carm_.register_left_joint_cbk(lambda t, p, v, a : joint_publisher(t, p, v, a))
    carm_.register_right_joint_cbk(lambda t, p, v, a : joint_publisher(t, p, v, a))
    
    # 获取姿态
    carm_.register_left_pose_cbk(lambda t, p : pose_publisher(t, p))
    carm_.register_right_pose_cbk(lambda t, p : pose_publisher(t, p))
    
    # 获取规划目标值 (关节)
    carm_.register_left_plan_joint_cbk(lambda t, p, v, a : plan_publisher(t, p, v, a))
    carm_.register_right_plan_joint_cbk(lambda t, p, v, a : plan_publisher(t, p, v, a))

    # 获取规划目标值 (笛卡尔位姿) (新增)
    carm_.register_left_plan_pose_cbk(lambda t, p : plan_pose_publisher(t, p))
    carm_.register_right_plan_pose_cbk(lambda t, p : plan_pose_publisher(t, p))
    
    # 外力矩信息（排除重力后）
    carm_.register_left_external_force_cbk(lambda t, tau, force : external_publisher(t, tau, force))
    carm_.register_right_external_force_cbk(lambda t, tau, force : external_publisher(t, tau, force))

    print("链接完毕，使能机械臂")
    carm_.set_ready()
    time.sleep(1)

    joint_ = [0, 0, 0, 0, 0, 0, 0]
    print("移动到安全位置")
    carm_.move_left_joint(joint_)
    carm_.move_right_joint(joint_)
    time.sleep(1)