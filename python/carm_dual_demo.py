import time
from carm import carm_py

global carm_

def connect(data) -> int:
    ret = 0
    if data == "":
        # 断连
        ret = carm_.disconnect()
    else:
        # 连接data = "10.42.0.101",连接输入ip
        ret = carm_.connect(data)
    print(f"connect_carm, ret = {ret}")
    return ret

def is_connect() -> int:
    # 检查连接状态
    return carm_.is_connected()

def set_ready() -> int:
    # 清除错误，并初始化机械臂
    ret = carm_.set_ready()
    print(f"set_ready, ret = {ret}")
    return ret

def set_servo_enable(flag) -> int:
    # true上使能，false下使能
    ret = carm_.set_servo_enable(flag)
    print(f"set_servo_enable, ret = {ret}")
    return ret

def set_control_mode(mode) -> int:
    # 0-idle 空闲模式 1-position 点位控制模式, 2-MIT 力矩模式， 3-drag 拖动模式，4-PF 力位混合模式
    ret = carm_.set_control_mode(mode)
    print(f"set_control_mode, ret = {ret}")
    return ret

def get_version() -> str:
    # 获取版本信息
    version = carm_.get_version()
    print(f"get_version, version = {version}")
    return version

def get_config():
    # 读取配置
    config = carm_.get_left_config()
    print("dof: ", config.dof)
    print("limit_upper: ", config.limit_upper)
    print("limit_lower: ", config.limit_lower)
    print("joint_vel: ", config.joint_vel)
    print("joint_acc: ", config.joint_acc)
    print("joint_dec: ", config.joint_dec)
    print("joint_jerk: ", config.joint_jerk)

    config = carm_.get_right_config()
    print("dof: ", config.dof)
    print("limit_upper: ", config.limit_upper)
    print("limit_lower: ", config.limit_lower)
    print("joint_vel: ", config.joint_vel)
    print("joint_acc: ", config.joint_acc)
    print("joint_dec: ", config.joint_dec)
    print("joint_jerk: ", config.joint_jerk)

def get_status():
    # 获取状态
    arm_status = carm_.get_left_status()
    print("arm_index: ", arm_status.arm_index)
    print("arm_name: ", arm_status.arm_name)
    print("arm_is_connected: ", arm_status.arm_is_connected)
    print("arm_dof: ", arm_status.arm_dof)
    print("servo_status: ", arm_status.servo_status)
    print("state: ", arm_status.state)
    print("speed_percentage: ", arm_status.speed_percentage)
    print("on_debug_mode: ", arm_status.on_debug_mode)

    arm_status = carm_.get_right_status()
    print("arm_index: ", arm_status.arm_index)
    print("arm_name: ", arm_status.arm_name)
    print("arm_is_connected: ", arm_status.arm_is_connected)
    print("arm_dof: ", arm_status.arm_dof)
    print("servo_status: ", arm_status.servo_status)
    print("state: ", arm_status.state)
    print("speed_percentage: ", arm_status.speed_percentage)
    print("on_debug_mode: ", arm_status.on_debug_mode)

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
    vel_r = carm_.get_right_joint_tau()
    print(f"get_right_joint_tau, vel_r = {vel_r}")
    return vel_l + vel_r

def get_joint_tau() -> List[float]:
    # 获取关节力矩
    tau_l = carm_.get_left_joint_tau()
    print(f"get_left_joint_tau, tau_l = {tau_l}")
    tau_r = carm_.get_right_joint_tau()
    print(f"get_right_joint_tau, tau_r = {tau_r}")
    return tau_l + tau_r

def get_cart_pose() -> List[float]:
    # 获取机械臂末端笛卡尔位姿
    pose_l = carm_.get_left_cart_pose()
    print(f"get_left_cart_pose, pose_l = {pose_l}")
    pose_r = carm_.get_right_cart_pose()
    print(f"get_right_cart_pose, pose_r = {pose_r}")
    return pose_l + pose_r

def get_gripper_state() -> int:
    # 获取末端状态
    state = carm_.get_left_gripper_state()
    print(f"get_left_gripper_state, state = {state}")
    state = carm_.get_right_gripper_state()
    print(f"get_right_gripper_state, state = {state}")
    return state

def get_gripper_pos() -> float:
    # 获取末端位置
    pos = carm_.get_left_gripper_pos()
    print(f"get_left_gripper_pos, pos_l = {pos}")
    pos = carm_.get_right_gripper_pos()
    print(f"get_right_gripper_pos, pos_r = {pos}")
    return pos

def get_gripper_vel() -> float:
    # 获取末端速度
    vel = carm_.get_left_gripper_vel()
    print(f"get_left_gripper_vel, vel = {vel}")
    vel = carm_.get_right_gripper_vel()
    print(f"get_right_gripper_vel, vel = {vel}")
    return vel

def get_gripper_tau() -> float:
    # 获取末端力
    tau = carm_.get_left_gripper_tau()
    print(f"get_left_gripper_tau, tau = {tau}")
    tau = carm_.get_right_gripper_tau()
    print(f"get_right_gripper_tau, tau = {tau}")
    return tau

def track_joint(targets : List[float], gripper_pos=-1) -> int:
    # 跟踪关节
    ret = carm_.track_left_joint(targets, gripper_pos)
    print(f"track_left_joint, ret = {ret}")
    ret = carm_.track_right_joint(targets, gripper_pos)
    print(f"track_right_joint, ret = {ret}")
    return ret

def track_pose(targets : List[float], gripper_pos=-1) -> int:
    # 跟踪笛卡尔
    ret = carm_.track_left_pose(targets, gripper_pos)
    print(f"track_left_pose, ret = {ret}")
    ret = carm_.track_right_pose(targets, gripper_pos)
    print(f"track_right_pose, ret = {ret}")
    return ret

# is_sync代表是否阻塞等待任务完成
# desire_time 时间为负则以设定的速度到达通过set_speed_level调整
def move_joint(targets : List[float], desire_time=-1, is_sync=True) -> int:
    # 点位关节运动
    ret = carm_.move_left_joint(targets, desire_time, is_sync)
    print(f"move_left_joint, ret = {ret}")
    ret = carm_.move_right_joint(targets, desire_time, is_sync)
    print(f"move_right_joint, ret = {ret}")
    return ret

def move_pose(targets : List[float], desire_time=-1, is_sync=True) -> int:
    # 点位末端运动
    ret = carm_.move_left_pose(targets, desire_time, is_sync)
    print(f"move_left_pose, ret = {ret}")
    ret = carm_.move_right_pose(targets, desire_time, is_sync)
    print(f"move_right_pose, ret = {ret}")
    return ret

def move_line_joint(targets : List[float], is_sync=True) -> int:
    # 关节空间线性运动
    ret = carm_.move_left_line_joint(targets, is_sync)
    print(f"move_left_line_joint, ret = {ret}")
    ret = carm_.move_right_line_joint(targets, is_sync)
    print(f"move_right_line_joint, ret = {ret}")
    return ret

def move_line_pose(targets : List[float], is_sync=True) -> int:
    # 笛卡尔空间线性轨迹
    ret = carm_.move_left_line_pose(targets, is_sync)
    print(f"move_left_line_pose, ret = {ret}")
    ret = carm_.move_right_line_pose(targets, is_sync)
    print(f"move_right_line_pose, ret = {ret}")
    return ret

# gripper_pos可为空[]
# stamps可为空[]
def move_joint_traj(target_pos : List[float], gripper_pos : List[float], stamps : List[float], is_sync=True) -> int:
    # 关节轨迹运动
    ret = carm_.move_left_joint_traj(target_pos, gripper_pos, stamps, is_sync)
    print(f"move_left_joint_traj, ret = {ret}")
    ret = carm_.move_right_joint_traj(target_pos, gripper_pos, stamps, is_sync)
    print(f"move_right_joint_traj, ret = {ret}")
    return ret

def move_pose_traj(target_pos : List[float], gripper_pos : List[float], stamps : List[float], is_sync=True) -> int:
    # 末端轨迹运动
    ret = carm_.move_left_pose_traj(target_pos, gripper_pos, stamps, is_sync)
    print(f"move_left_pose_traj, ret = {ret}")
    ret = carm_.move_right_pose_traj(target_pos, gripper_pos, stamps, is_sync)
    print(f"move_right_pose_traj, ret = {ret}")
    return ret

def emergency_stop() -> int:
    # 紧急急停，急停后调用set_ready恢复
    ret = carm_.emergency_stop()
    print("emergency_stop called.")
    return ret

def set_gripper(pos : float, tau=10) -> int:
    # 控制末端
    # 夹抓间隔(0-0.08m) 夹抓扭矩(0-20N)
    ret = carm_.set_left_gripper(pos, tau)
    print(f"set_left_gripper, ret = {ret}")
    ret = carm_.set_right_gripper(pos, tau)
    print(f"set_right_gripper, ret = {ret}")
    return ret

def set_speed_level(level : float, response_level=20) -> int:
    # 设置速度等级(0~10)与响应等级
    ret = carm_.set_speed_level(level, response_level)
    print(f"set_speed_level, ret = {ret}")
    return ret

def set_tool_index(index : int) -> int:
    # 选择工具
    ret = carm_.set_left_tool_index(index)
    print(f"set_left_tool_index, ret = {ret}")
    ret = carm_.set_right_tool_index(index)
    print(f"set_right_tool_index, ret = {ret}")
    return ret

def get_tool_index() -> List[int]:
    # 获取当前工具 index
    return [carm_.get_left_tool_index(), carm_.get_right_tool_index()]

def get_tool_coordinate(index) -> List[int]:
    # 获取某工具的坐标系参数
    return [carm_.get_left_tool_coordinate(index), carm_.get_right_tool_coordinate(index)]

def set_collision_config(enable_flag=True, sensitivity_level=0) -> int:
    # 碰撞配置打开还是关闭，以及灵敏度
    ret = carm_.set_collision_config(enable_flag, sensitivity_level)
    print(f"set_collision_config, ret = {ret}")
    return ret

def trajectory_teach(off_on : bool, name : str) -> int:
    # 开启或者停止并保存录制路径
    ret = carm_.trajectory_teach(off_on, name)
    print(f"trajectory_teach, ret = {ret}")
    return ret

def trajectory_recorder(name : str) -> int:
    # 播放对应名字的路径
    ret = carm_.trajectory_recorder(name)
    print(f"trajectory_recorder, ret = {ret}")
    return ret

def check_teach():
    # 只显示 如下标准格式的路径：20251215111017.name.json
    ret, name_list = carm_.check_teach()
    return ret, name_list

def inverse_kine_array(tool_index : int, quat_pose_list : List[List[float]], ref_joint_list : List[List[float]]) -> int:
    """
    工具序号tool_index。
    quat_pose_list: N个姿态，每个为长度7 array。
    ref_joint_list: N个初始关节值，每个为长度N vector。
    返回 (ret, [关节解1,关节解2,...])
    """
    ret, joints = carm_.inverse_kine_left_array(tool_index, quat_pose_list, ref_joint_list)
    print(f"inverse_kine_left_array, ret = {ret}")
    print("joint_values =", joints)

    ret, joints = carm_.inverse_kine_right_array(tool_index, quat_pose_list, ref_joint_list)
    print(f"inverse_kine_right_array, ret = {ret}")
    print("joint_values =", joints)
    return ret

def forward_kine_array(tool_index : int, jnt_value_list : List[List[float]]) -> int:
    """
    工具序号tool_index。
    jnt_value_list: N组关节值。
    返回(ret, [quat_pose1, quat_pose2,...])
    """
    ret, poses = carm_.forward_kine_left_array(tool_index, jnt_value_list)
    print(f"forward_kine_left_array, ret = {ret}")
    print("quat_poses =", poses)

    ret, poses = carm_.forward_kine_right_array(tool_index, jnt_value_list)
    print(f"forward_kine_right_array, ret = {ret}")
    print("quat_poses =", poses)
    return ret

def inverse_kine(tool_index : int, quat_pose : List[List[float]], ref_joint : List[List[float]]) -> int:
    """
    单个逆解
    tool_index: 工具序号
    quat_pose: 长度7 array
    ref_joint: 关节初值vector
    返回 (ret, jnt_value)
    """
    ret, jnt = carm_.inverse_kine_left(tool_index, quat_pose, ref_joint)
    print(f"inverse_kine_left, ret = {ret}")
    print("joint_value =", jnt)

    ret, jnt = carm_.inverse_kine_right(tool_index, quat_pose, ref_joint)
    print(f"inverse_kine_right, ret = {ret}")
    print("joint_value =", jnt)
    return ret

def forward_kine(tool_index : int, jnt_value : List[List[float]]) -> int:
    """
    单个正解
    tool_index: 工具序号
    jnt_value: 关节值vector
    返回(ret, quat_pose)
    """
    ret, pose = carm_.forward_kine_left(tool_index, jnt_value)
    print(f"forward_kine, ret = {ret}")
    print("quat_pose =", pose)

    ret, pose = carm_.forward_kine_right(tool_index, jnt_value)
    print(f"forward_kine, ret = {ret}")
    print("quat_pose =", pose)
    return ret





############ 回调示例 ###########
# 实际关节位置
def joint_publisher(t : float, p : List[float], v : List[float], a : List[float]):
    if carm_.get_gripper_state() >= 0:
        print("time: ", t)
        print("joint_pos: ", p, "gripper_pos: ", carm_.get_gripper_pos())
        print("joint_vel: ", v, "gripper_vel: ", carm_.get_gripper_vel())
        print("joint_tau: ", a, "gripper_tau: ", carm_.get_gripper_tau())
    else:
        print("time: ", t)
        print("joint_pos: ", p)
        print("joint_vel: ", v)
        print("joint_tau: ", a)

# 规划的关节位置
def plan_publisher(t : float, p : List[float], v : List[float], a : List[float]):
    print("time: ", t)
    print("joint_pos: ", p)
    print("joint_vel: ", v)
    print("joint_tau: ", a)

# 实际法兰位置
def pose_publisher(t : float, p : List[float]):
    # 发布 flange_cart_state
    print("time: ", t, "xyz-xyzw: ", p)

# 外力矩信息（排除重力后）
def external_publisher(t : float, tau : List[float], force : List[float]):
    print("time: ", t)
    print("joints_tau: ", tau)
    print("cart_external_force: ", force)


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
    carm_.release_left_external_force_cbk()
    carm_.release_error_cbk("onCarmError")
    carm_.release_completion_cbk("task_completion")

    carm_.release_right_joint_cbk()
    carm_.release_right_pose_cbk()
    carm_.release_right_plan_joint_cbk()
    carm_.release_right_external_force_cbk()
    carm_.release_error_cbk("onCarmError")
    carm_.release_completion_cbk("task_completion")


if __name__ == '__main__':
    # carm_ = carm_py.CArmDualBot("127.0.0.1")
    carm_ = carm_py.CArmDualBot("10.42.0.101")
    time.sleep(1)

    print("链接完毕，使能机械臂")
    carm_.set_ready()
    time.sleep(1)

    joint_ = [0, 0, 0, 0, 0, 0, 0]
    print("移动到安全位置")
    carm_.move_left_joint(joint_)
    carm_.move_right_joint(joint_)
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
    # 获取规划目标值
    carm_.register_left_plan_joint_cbk(lambda t, p, v, a : plan_publisher(t, p, v, a))
    carm_.register_right_plan_joint_cbk(lambda t, p, v, a : plan_publisher(t, p, v, a))
    # 外力矩信息（排除重力后）
    carm_.register_left_external_force_cbk(lambda t, tau, force : external_publisher(t, tau, force))
    carm_.register_right_external_force_cbk(lambda t, tau, force : external_publisher(t, tau, force))