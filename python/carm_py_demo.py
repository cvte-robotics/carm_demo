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
    # 清除错误，并初始化机械臂 (控制器复位)
    ret = carm_.set_ready()
    print(f"set_ready, ret = {ret}")
    return ret

def set_servo_enable(flag: bool) -> int:
    # true上使能，false下使能
    ret = carm_.set_servo_enable(flag)
    print(f"set_servo_enable, ret = {ret}")
    return ret

def set_control_mode(mode: int) -> int:
    # 0-idle 空闲模式 1-position 点位控制模式, 2-MIT 力矩模式，3-drag 拖动模式，4-PF 力位混合模式
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
    config = carm_.get_config()
    print("dof: ", config.dof)
    print("limit_upper: ", config.limit_upper)
    print("limit_lower: ", config.limit_lower)
    print("joint_vel: ", config.joint_vel)
    print("joint_acc: ", config.joint_acc)
    print("joint_dec: ", config.joint_dec)
    print("joint_jerk: ", config.joint_jerk)

def get_status():
    # 获取状态
    arm_status = carm_.get_status()
    print("arm_index: ", arm_status.arm_index)
    print("arm_name: ", arm_status.arm_name)
    print("arm_is_connected: ", arm_status.arm_is_connected)
    print("arm_dof: ", arm_status.arm_dof)
    print("servo_status: ", arm_status.servo_status)
    print("state: ", arm_status.state)
    print("speed_percentage: ", arm_status.speed_percentage)
    print("on_debug_mode: ", arm_status.on_debug_mode)

# ==================== 状态获取相关 ====================

def get_joint_pos() -> List[float]:
    # 获取实际关节角度
    pos = carm_.get_joint_pos()
    print(f"get_joint_pos, pos = {pos}")
    return pos

def get_joint_vel() -> List[float]:
    # 获取实际关节速度
    vel = carm_.get_joint_vel()
    print(f"get_joint_vel, vel = {vel}")
    return vel

def get_joint_tau() -> List[float]:
    # 获取实际关节力矩
    tau = carm_.get_joint_tau()
    print(f"get_joint_tau, tau = {tau}")
    return tau

def get_plan_joint_pos() -> List[float]:
    # 获取控制指令规划的关节角度
    pos = carm_.get_plan_joint_pos()
    print(f"get_plan_joint_pos, pos = {pos}")
    return pos

def get_plan_joint_vel() -> List[float]:
    # 获取控制指令规划的关节速度
    vel = carm_.get_plan_joint_vel()
    print(f"get_plan_joint_vel, vel = {vel}")
    return vel

def get_plan_joint_tau() -> List[float]:
    # 获取控制指令规划的关节力矩
    tau = carm_.get_plan_joint_tau()
    print(f"get_plan_joint_tau, tau = {tau}")
    return tau

def get_cart_pose() -> List[float]:
    # 获取机械臂末端(法兰)实际笛卡尔位姿(x, y, z, qx, qy, qz, qw)
    pose = carm_.get_cart_pose()
    print(f"get_cart_pose, pose(x,y,z,Qx,Qy,Qz,Qw) = {pose}")
    return pose

def get_plan_cart_pose() -> List[float]:
    # 获取控制指令规划法兰相对基座的位姿 (新增)
    pose = carm_.get_plan_cart_pose()
    print(f"get_plan_cart_pose, pose(x,y,z,Qx,Qy,Qz,Qw) = {pose}")
    return pose

def get_joint_external_tau() -> List[float]:
    # 获取关节进行重力补偿后受到的其他力矩
    tau = carm_.get_joint_external_tau()
    print(f"get_joint_external_tau, tau = {tau}")
    return tau

def get_cart_external_force() -> List[float]:
    # 获取末端力控的力矩 (fx, fy, fz, tx, ty, tz)
    force = carm_.get_cart_external_force()
    print(f"get_cart_external_force, force(fx,fy,fz,tx,ty,tz) = {force}")
    return force

# ==================== 夹爪相关 ====================

def get_gripper_state() -> int:
    # 获取末端状态（-1: 未连接, 0: 未使能, 1: 正常状态）
    state = carm_.get_gripper_state()
    print(f"get_gripper_state, state = {state}")
    return state

def get_gripper_pos() -> float:
    # 获取末端位置 (夹爪两指间隔)
    pos = carm_.get_gripper_pos()
    print(f"get_gripper_pos, pos = {pos}")
    return pos

def get_gripper_vel() -> float:
    # 获取末端速度
    vel = carm_.get_gripper_vel()
    print(f"get_gripper_vel, vel = {vel}")
    return vel

def get_gripper_tau() -> float:
    # 获取末端力 (夹爪两指的扭矩)
    tau = carm_.get_gripper_tau()
    print(f"get_gripper_tau, tau = {tau}")
    return tau

def get_plan_gripper_pos() -> float:
    # 获取末端规划位置 (新增)
    pos = carm_.get_plan_gripper_pos()
    print(f"get_plan_gripper_pos, pos = {pos}")
    return pos

def get_plan_gripper_tau() -> float:
    # 获取末端规划力矩 (新增)
    tau = carm_.get_plan_gripper_tau()
    print(f"get_plan_gripper_tau, tau = {tau}")
    return tau

def set_gripper(pos : float, tau=10.0) -> int:
    # 控制末端
    # pos夹抓间隔(0-0.08m) tau夹抓扭矩(0-100N)
    ret = carm_.set_gripper(pos, tau)
    print(f"set_gripper, ret = {ret}")
    return ret

# ==================== 运动控制 ====================

def track_joint(targets : List[float], gripper_pos=-1.0) -> int:
    # 跟踪关节 (周期性发送)
    ret = carm_.track_joint(targets, gripper_pos)
    print(f"track_joint, ret = {ret}")
    return ret

def track_pose(targets : List[float], gripper_pos=-1.0) -> int:
    # 跟踪笛卡尔
    ret = carm_.track_pose(targets, gripper_pos)
    print(f"track_pose, ret = {ret}")
    return ret

# is_sync代表是否阻塞等待任务完成
# desire_time 目标到达时间，时间为负则以设定的速度到达，可通过set_speed_level调整
def move_joint(targets : List[float], desire_time=-1.0, is_sync=True) -> int:
    # 点位关节运动
    ret = carm_.move_joint(targets, desire_time, is_sync)
    print(f"move_joint, ret = {ret}")
    return ret

def move_pose(targets : List[float], desire_time=-1.0, is_sync=True) -> int:
    # 关节到点运动，目标位姿
    ret = carm_.move_pose(targets, desire_time, is_sync)
    print(f"move_pose, ret = {ret}")
    return ret

def move_line_joint(targets : List[float], is_sync=True) -> int:
    # 直线到点运动，关节目标
    ret = carm_.move_line_joint(targets, is_sync)
    print(f"move_line_joint, ret = {ret}")
    return ret

def move_line_pose(targets : List[float], is_sync=True) -> int:
    # 直线到点运动，目标位姿
    ret = carm_.move_line_pose(targets, is_sync)
    print(f"move_line_pose, ret = {ret}")
    return ret

# gripper_pos可为空[]
# stamps可为空[]
def move_joint_traj(target_pos : List[List[float]], gripper_pos : List[float], stamps : List[float], is_sync=True) -> int:
    # PT运动（关节序列）
    ret = carm_.move_joint_traj(target_pos, gripper_pos, stamps, is_sync)
    print(f"move_joint_traj, ret = {ret}")
    return ret

def move_pose_traj(target_pos : List[List[float]], gripper_pos : List[float], stamps : List[float], is_sync=True) -> int:
    # PT运动（位姿序列）
    ret = carm_.move_pose_traj(target_pos, gripper_pos, stamps, is_sync)
    print(f"move_pose_traj, ret = {ret}")
    return ret

def move_flow_pose(target_pos : List[float], line_theta_weight=0.5, accuracy=0.0001, is_sync=True) -> int:
    # 位姿迭代运动 (新增)
    ret = carm_.move_flow_pose(target_pos, line_theta_weight, accuracy, is_sync)
    print(f"move_flow_pose, ret = {ret}")
    return ret

# ==================== 设置与配置 ====================

def set_speed_level(level : float, response_level=20) -> int:
    # 在线改变速度等级(0~10分别对应速度百分比0%-100%)与响应等级(插补周期数)
    ret = carm_.set_speed_level(level, response_level)
    print(f"set_speed_level, ret = {ret}")
    return ret

def set_tool_index(index : int) -> int:
    # 选择工具号
    ret = carm_.set_tool_index(index)
    print(f"set_tool_index, ret = {ret}")
    return ret

def get_tool_index() -> int:
    # 获取当前工具号
    return carm_.get_tool_index()

def get_tool_coordinate(index: int) -> List[float]:
    # 获取某工具的坐标系参数（末端相对法兰的位姿关系）
    return carm_.get_tool_coordinate(index)

def set_collision_config(enable_flag=True, sensitivity_level=0) -> int:
    # 启动/关闭碰撞检测，并设置灵敏度(0-2，0最高)
    ret = carm_.set_collision_config(enable_flag, sensitivity_level)
    print(f"set_collision_config, ret = {ret}")
    return ret

# ==================== 示教相关 ====================

def trajectory_teach(off_on : bool, name : str) -> int:
    # 开始/停止示教录制并命名
    ret = carm_.trajectory_teach(off_on, name)
    print(f"trajectory_teach, ret = {ret}")
    return ret

def trajectory_recorder(name : str, is_sync=True) -> int:
    # 播放对应名字的示教路径 (支持同步参数更新)
    ret = carm_.trajectory_recorder(name, is_sync)
    print(f"trajectory_recorder, ret = {ret}")
    return ret

def check_teach():
    # 获取已记录的示教轨迹列表
    ret, name_list = carm_.check_teach()
    return ret, name_list

# ==================== 运动学 ====================

def inverse_kine_array(tool_index : int, quat_pose_list : List[List[float]], ref_joint_list : List[List[float]]):
    """
    批量逆运动学求解
    tool_index: 工具号。
    quat_pose_list: 目标位姿列表。
    ref_joint_list: 初始参考关节值。
    返回 (ret, [关节解1, 关节解2...])
    """
    ret, joints = carm_.inverse_kine_array(tool_index, quat_pose_list, ref_joint_list)
    print(f"inverse_kine_array, ret = {ret}")
    print("joint_values =", joints)
    return ret, joints

def forward_kine_array(tool_index : int, jnt_value_list : List[List[float]]):
    """
    批量正运动学求解
    tool_index: 工具号。
    jnt_value_list: N组关节角度列表。
    返回(ret, [quat_pose1, quat_pose2...])
    """
    ret, poses = carm_.forward_kine_array(tool_index, jnt_value_list)
    print(f"forward_kine_array, ret = {ret}")
    print("quat_poses =", poses)
    return ret, poses

def inverse_kine(tool_index : int, quat_pose : List[float], ref_joint : List[float]):
    """
    单点逆解
    tool_index: 工具号
    quat_pose: 期望位姿 长度7 array
    ref_joint: 关节初值vector
    返回 (ret, jnt_value)
    """
    ret, jnt = carm_.inverse_kine(tool_index, quat_pose, ref_joint)
    print(f"inverse_kine, ret = {ret}")
    print("joint_value =", jnt)
    return ret, jnt

def forward_kine(tool_index : int, jnt_value : List[float]):
    """
    单点正解
    tool_index: 工具号
    jnt_value: 关节值vector
    返回(ret, quat_pose)
    """
    ret, pose = carm_.forward_kine(tool_index, jnt_value)
    print(f"forward_kine, ret = {ret}")
    print("quat_pose =", pose)
    return ret, pose

# ==================== 回调示例 ====================

# 实际关节位置
def joint_publisher(t : float, p : List[float], v : List[float], a : List[float]):
    if carm_.get_gripper_state() >= 0:
        print("time: ", t)
        print("joint_pos: ", p, "gripper_pos: ", carm_.get_gripper_pos())
        print("joint_vel: ", v, "gripper_vel: ", carm_.get_gripper_vel())
        print("joint_tau: ", a, "gripper_tau: ", carm_.get_gripper_tau())
    else:
        print("time: ", t, "joint_pos: ", p)

# 规划的关节位置
def plan_publisher(t : float, p : List[float], v : List[float], a : List[float]):
    print("time: ", t, "plan_joint_pos: ", p)

# 实际法兰位姿
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
    if err_code == 3001:
        print("receive a warning, msg = ", err_msg)
    else:
        print("receive an error, code = ", err_code, "msg = ", err_msg)

def task_completion(task_key : str):
    print("task_completion: ", task_key)

def release_callbacks():
    carm_.release_joint_cbk()
    carm_.release_pose_cbk()
    carm_.release_plan_joint_cbk()
    carm_.release_plan_pose_cbk() # 新增
    carm_.release_external_force_cbk()
    carm_.release_error_cbk("onCarmError")
    carm_.release_completion_cbk("task_completion")


if __name__ == '__main__':
    # carm_ = carm_py.CArmSingleCol("127.0.0.1")
    carm_ = carm_py.CArmSingleCol("10.42.0.101")
    time.sleep(1)

    print("开始广播机器状态话题")
    carm_.register_error_cbk("onCarmError", lambda err_code, err_msg : onCarmError(err_code, err_msg))
    carm_.register_completion_cbk("task_completion", lambda key : task_completion(key))
    
    # 获取并打印状态回调
    carm_.register_joint_cbk(lambda t, p, v, a : joint_publisher(t, p, v, a))
    carm_.register_pose_cbk(lambda t, p : pose_publisher(t, p))
    carm_.register_plan_joint_cbk(lambda t, p, v, a : plan_publisher(t, p, v, a))
    carm_.register_plan_pose_cbk(lambda t, p : plan_pose_publisher(t, p))
    carm_.register_external_force_cbk(lambda t, tau, force : external_publisher(t, tau, force))

    print("链接完毕，使能机械臂")
    carm_.set_ready()
    time.sleep(1)

    joint_ = [0, 0, 0, 0, 0, 0]
    print("移动到安全位置")
    carm_.move_joint(joint_)
    time.sleep(1)





