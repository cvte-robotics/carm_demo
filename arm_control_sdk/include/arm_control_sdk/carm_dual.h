#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "arm_control_sdk/data_type_def.h"

namespace carm {
class CArmKernelImpl;
class CArmDualBot {
public:
    /**
     * @brief Construct a new CArmDualBot object
     *
     * @param server_ip
     * @param port
     * @param timeout 连接超时，单位秒
     */
    CArmDualBot(const std::string& server_ip = "10.42.0.101",
                int port = 8090,
                double timeout = 1,
                int left_index = 0,
                int right_index = 1);
    ~CArmDualBot();
    /**
     * @brief 连接carm controller
     *
     * @param server_ip
     * @param port
     * @param timeout 连接超时，单位秒
     * @return int 1: 连接成功，<1:连接失败
     */
    int connect(const std::string& server_ip = "10.42.0.101", int port = 8090, double timeout = 1);
    /**
     * @brief disconnect with carm
     * @return int 1: 断连成功，-1: ；断连失败
     */
    int disconnect();
    /**
     * @brief 判断是否连接carm controller
     *
     * @return true 已连接
     * @return false 未连接
     */
    bool is_connected();

    /*******************基础函数******************* */
    /**
     * @brief 组合指令，控制器复位
     * clean_carm_error();
     * set_servo_enable();
     * set_control_mode(0);
     * @return int 1: 复位成功，-1: 复位失败
     */
    int set_ready();

    /**
     * @brief Set the servo enable object
     *
     * @param enable 设置伺服上使能(true) or 下使能(false)
     * @return int 非阻塞，1: 指令发送成功，<1: 指令发送失败
     */
    int set_servo_enable(bool enable);

    /**
     * @brief 设置控制器控制（fsm）模式
     *
     * @param mode
     * 0-IDLE空闲模式
     * 1-点位控制模式
     * 2-MIT控制模式
     * 3-关节拖动模式
     * 4-PF力位混合模式
     * @return int 非阻塞，1: 指令发送成功，<1: 指令发送失败
     */
    int set_control_mode(int mode);

    /**
     * @brief 获取控制器软件版本
     *
     * @return std::string
     */
    std::string get_version();
    /**
     * @brief 获取主要配置参数，包括关节限位、关节最大速度、加速度、加加速度等
     *
     * @return ArmConfig
     */
    ArmConfig get_left_config();
    ArmConfig get_right_config();
    /**
     * @brief 获取机械臂状态
     *
     * @return ArmStatus
     */
    ArmStatus get_left_status();
    ArmStatus get_right_status();
    /**
     * @brief 获取实际的关节角度
     *
     * @return std::vector<double>
     */
    std::vector<double> get_left_joint_pos();
    std::vector<double> get_right_joint_pos();
    /**
     * @brief 获取实际的关节角速度
     *
     * @return std::vector<double>
     */
    std::vector<double> get_left_joint_vel();
    std::vector<double> get_right_joint_vel();
    /**
     * @brief 获取实际的关节力矩
     *
     * @return std::vector<double>
     */
    std::vector<double> get_left_joint_tau();
    std::vector<double> get_right_joint_tau();
    /**
     * @brief 获取规划的关节角度
     *
     * @return std::vector<double>
     */
    std::vector<double> get_left_plan_joint_pos();
    std::vector<double> get_right_plan_joint_pos();
    /**
     * @brief 获取规划的关节角速度
     *
     * @return std::vector<double>
     */
    std::vector<double> get_left_plan_joint_vel();
    std::vector<double> get_right_plan_joint_vel();
    /**
     * @brief 获取规划的关节力矩
     *
     * @return std::vector<double>
     */
    std::vector<double> get_left_plan_joint_tau();
    std::vector<double> get_right_plan_joint_tau();
    /**
     * @brief 获取控制法兰相对基座的位姿
     *
     * @return std::array<double, 7>: 姿态由四元数描述（x,y,z,x,y,z,w）
     */
    std::array<double, 7> get_left_plan_cart_pose();
    std::array<double, 7> get_right_plan_cart_pose();
    /**
     * @brief 获取实际法兰相对基座的位姿
     *
     * @return std::array<double, 7>: 姿态由四元数描述（x,y,z,x,y,z,w）
     */
    std::array<double, 7> get_left_cart_pose();
    std::array<double, 7> get_right_cart_pose();
    /**
     * @brief 关节进行重力补偿后受到的其他力矩
     *
     * @return std::vector<double>
     */
    std::vector<double> get_left_joint_external_tau();
    std::vector<double> get_right_joint_external_tau();
    /**
     * @brief 获取末端力控的力矩，暂时未开放
     *
     * @return std::vector<double>: 姿态由四元数描述（x,y,z,rx,ry,rz）
     */
    std::vector<double> get_left_cart_external_force();
    std::vector<double> get_right_cart_external_force();
    /**
     * @brief 注册实时更新的带时间戳的关节信息
     *
     * @param {time, joints_pos, joints_vel, joints_tau}
     */
    void register_left_joint_cbk(
            std::function<
                    void(double, std::vector<double>, std::vector<double>, std::vector<double>)>
                    cbk);
    void release_left_joint_cbk();
    void register_right_joint_cbk(
            std::function<
                    void(double, std::vector<double>, std::vector<double>, std::vector<double>)>
                    cbk);
    void release_right_joint_cbk();
    /**
     * @brief 注册实时更新的带时间戳的末端信息
     *
     * @param {time, pose} 姿态由四元数描述（x,y,z,rx,ry,rz）
     */
    void register_left_pose_cbk(std::function<void(double, std::array<double, 7>)> cbk);
    void release_left_pose_cbk();
    void register_right_pose_cbk(std::function<void(double, std::array<double, 7>)> cbk);
    void release_right_pose_cbk();
    /**
     * @brief 注册实时更新的带时间戳的规划末端信息
     *
     * @param {time, pose} 姿态由四元数描述（x,y,z,rx,ry,rz）
     */
    void register_left_plan_pose_cbk(std::function<void(double, std::array<double, 7>)> cbk);
    void release_left_plan_pose_cbk();
    void register_right_plan_pose_cbk(std::function<void(double, std::array<double, 7>)> cbk);
    void release_right_plan_pose_cbk();

    /**
     * @brief 注册实时更新的带时间戳的关节控制指令信息
     *
     * @param {time, cmd_pos, cmd_vel, cmd_tau}
     */
    void register_left_plan_joint_cbk(
            std::function<
                    void(double, std::vector<double>, std::vector<double>, std::vector<double>)>
                    cbk);
    void release_left_plan_joint_cbk();
    void register_right_plan_joint_cbk(
            std::function<
                    void(double, std::vector<double>, std::vector<double>, std::vector<double>)>
                    cbk);
    void release_right_plan_joint_cbk();

    /**
     * @brief 注册实时更新的带时间戳的外力矩信息
     *
     * @param {time, joints_tau, cart_external_force}
     */
    void register_left_external_force_cbk(
            std::function<void(double, std::vector<double>, std::vector<double>)> cbk);
    void release_left_external_force_cbk();
    void register_right_external_force_cbk(
            std::function<void(double, std::vector<double>, std::vector<double>)> cbk);
    void release_right_external_force_cbk();
    /**
     * @brief 获取夹抓状态
     *
     * @return int: -1: 未连接， 0: 未使能, 1: 正常状态, >1: 对应伺服错误
     */
    int get_left_gripper_state();
    int get_right_gripper_state();
    /**
     * @brief 获取夹抓状位置
     *
     * @return double: 夹抓两指间隔
     */
    double get_left_gripper_pos();
    double get_right_gripper_pos();
    /**
     * @brief 获取夹抓状速度
     *
     * @return double: 夹抓两指的运动速度
     */
    double get_left_gripper_vel();
    double get_right_gripper_vel();
    /**
     * @brief 获取夹抓状力矩
     *
     * @return double: 夹抓两指的扭矩
     */
    double get_left_gripper_tau();
    double get_right_gripper_tau();
    /**
     * @brief 获取夹抓状规划位置
     *
     * @return double: 夹抓两指间隔
     */
    double get_left_plan_gripper_pos();
    double get_right_plan_gripper_pos();
    /**
     * @brief 获取夹抓状规划力矩
     *
     * @return double: 夹抓两指的扭矩
     */
    double get_left_plan_gripper_tau();
    double get_right_plan_gripper_tau();

    /**
     * @brief 获取灵巧手状态
     *
     * @return int: -1: 未连接， 0: 未使能, 1: 正常状态, >1: 对应伺服错误
     */
    int get_left_hand_state();
    int get_right_hand_state();
    /**
     * @brief 获取灵巧手位置
     *
     * @return std::vector<double>: 灵巧手根部关节间隔
     */
    std::vector<double> get_left_hand_pos();
    std::vector<double> get_right_hand_pos();
    /**
     * @brief 获取灵巧手速度
     *
     * @return std::vector<double>: 灵巧手根部关节的运动速度
     */
    std::vector<double> get_left_hand_vel();
    std::vector<double> get_right_hand_vel();
    /**
     * @brief 获取灵巧手力矩
     *
     * @return std::vector<double>: 灵巧手根部关节的扭矩
     */
    std::vector<double> get_left_hand_tau();
    std::vector<double> get_right_hand_tau();
    /**
     * @brief 获取灵巧手规划位置
     *
     * @return std::vector<double>: 灵巧手根部关节间隔
     */
    std::vector<double> get_left_plan_hand_pos();
    std::vector<double> get_right_plan_hand_pos();
    /**
     * @brief 获取灵巧手规划速度
     *
     * @return std::vector<double>: 灵巧手根部关节的目标速度
     */
    std::vector<double> get_left_plan_hand_vel();
    std::vector<double> get_right_plan_hand_vel();
    /**
     * @brief 获取灵巧手规划力矩
     *
     * @return std::vector<double>: 灵巧手根部关节的扭矩
     */
    std::vector<double> get_left_plan_hand_tau();
    std::vector<double> get_right_plan_hand_tau();

    /*******************运动函数******************* */
    /**
     * @brief 跟随运动，周期性发送目标关节位置
     *
     * @param target 关节位置
     * @param gripper_pos 夹抓两指间隔
     * @return int 非阻塞，1: 指令发送成功，<1: 指令发送失败
     */
    int track_left_joint(const std::vector<double>& targets, const double gripper_pos = -1);
    int track_right_joint(const std::vector<double>& targets, const double gripper_pos = -1);
    /**
     * @brief 跟随运动，周期性发送目标位姿，法兰相对基座
     *
     * @param targets 目标位姿，法兰相对基座
     * @param gripper_pos 夹抓两指间隔
     * @return int 非阻塞，1: 指令发送成功，<1: 指令发送失败
     */
    int track_left_pose(const std::array<double, 7>& targets, const double gripper_pos = -1);
    int track_right_pose(const std::array<double, 7>& targets, const double gripper_pos = -1);

    /**
     * @brief 关节到点运动
     *
     * @param target_pos 关节目标位置
     * @param desire_time 目标到达时间，默认-1表示没有时间要求
     * @param is_sync 是否同步，true: 接口阻塞至任务完成 false: 接口非阻塞
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int move_left_joint(const std::vector<double>& target_pos,
                        double desire_time = -1,
                        bool is_sync = true);
    int move_right_joint(const std::vector<double>& target_pos,
                         double desire_time = -1,
                         bool is_sync = true);
    /**
     * @brief 关节到点运动
     *
     * @param target_pos 目标位姿，法兰相对基座
     * @param desire_time 目标到达时间，默认-1表示没有时间要求
     * @param is_sync 是否同步，true: 接口阻塞至任务完成 false: 接口非阻塞
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int move_left_pose(const std::array<double, 7>& target_pos,
                       double desire_time = -1,
                       bool is_sync = true);
    int move_right_pose(const std::array<double, 7>& target_pos,
                        double desire_time = -1,
                        bool is_sync = true);

    /**
     * @brief 直线到点运动
     *
     * @param target_pos 关节目标位置
     * @param is_sync 是否同步，true: 接口阻塞至任务完成 false: 接口非阻塞
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int move_left_line_joint(const std::vector<double>& target_pos, bool is_sync = true);
    int move_right_line_joint(const std::vector<double>& target_pos, bool is_sync = true);
    /**
     * @brief 直线到点运动
     *
     * @param target_pos 目标位姿，法兰相对基座
     * @param is_sync 是否同步，true: 接口阻塞至任务完成 false: 接口非阻塞
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int move_left_line_pose(const std::array<double, 7>& target_pos, bool is_sync = true);
    int move_right_line_pose(const std::array<double, 7>& target_pos, bool is_sync = true);

    /**
     * @brief PT运动
     *
     * @param target_pos 目标关节位置
     * @param gripper_pos 目标夹抓位置，默认为空表示没有夹抓控制要求
     * @param stamps 时间戳，默认为空表示没有时间要求
     * @param is_sync 是否同步，true: 接口阻塞至任务完成 false: 接口非阻塞
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int move_left_joint_traj(const std::vector<std::vector<double>>& target_pos,
                             const std::vector<double> gripper_pos = {},
                             std::vector<double> stamps = {},
                             bool is_sync = true);
    int move_right_joint_traj(const std::vector<std::vector<double>>& target_pos,
                              const std::vector<double> gripper_pos = {},
                              std::vector<double> stamps = {},
                              bool is_sync = true);
    /**
     * @brief PT运动
     *
     * @param target_pos 目标位姿，法兰相对基座
     * @param gripper_pos 目标夹抓位置，默认为空表示没有夹抓控制要求
     * @param stamps 时间戳, 默认为空表示没有时间要求
     * @param is_sync 是否同步，true: 接口阻塞至任务完成 false: 接口非阻塞
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int move_left_pose_traj(const std::vector<std::array<double, 7>>& target_pos,
                            const std::vector<double> gripper_pos = {},
                            std::vector<double> stamps = {},
                            bool is_sync = true);
    int move_right_pose_traj(const std::vector<std::array<double, 7>>& target_pos,
                             const std::vector<double> gripper_pos = {},
                             std::vector<double> stamps = {},
                             bool is_sync = true);

    /**
     * @brief 急停，恢复需要调set_ready接口
     *
     * @return int
     */
    int emergency_stop();

    /**
     * @brief 末端执行器控制，当前仅支持夹抓
     *
     * @param pos 两指间隔，0-80mm
     * @param tau 夹抓夹持力，0~100N
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int set_left_gripper(double pos, double tau = 10);
    int set_right_gripper(double pos, double tau = 10);

    /**
     * @brief 末端执行器控制，当前仅支持夹抓
     *
     * @param pos 两指间隔，0-80mm
     * @param tau 夹抓夹持力，0~100N
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int set_left_hand(const std::vector<double>& pos,
                      const std::vector<double>& tau = {},
                      const std::vector<double>& vel = {});
    int set_right_hand(const std::vector<double>& pos,
                       const std::vector<double>& tau = {},
                       const std::vector<double>& vel = {});

    /*******************设置函数******************* */
    /**
     * @brief 在线改变速度等级（在线降速）
     *
     * @param level 速度等级 0-10分别对应速度百分比0%-100%
     * @param response_level 响应等级
     * 插补周期数，表示在多少个周期内完成速度转变，范围10-100
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int set_speed_level(double level, int response_level = 20);

    /**
     * @brief 设置当前工具号
     *
     * @param index
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int set_left_tool_index(int index);
    int set_right_tool_index(int index);

    /**
     * @brief 获取当前工具号
     *
     * @return int
     */
    int get_left_tool_index();
    int get_right_tool_index();

    /**
     * @brief 获取指定工具的坐标系（工具末端相对法兰的位姿关系）
     *
     * @param index
     * @return std::array<double, 7>
     */
    std::array<double, 7> get_left_tool_coordinate(int index);
    std::array<double, 7> get_right_tool_coordinate(int index);

    /**
     * @brief 启动/关闭碰撞检测，设置碰撞检测等级
     *
     * @param enable_flag 使能，是否开启碰撞检测
     * @param sensitivity_level 灵敏度等级 0-2，0最高
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int set_collision_config(bool enable_flag = true, int sensitivity_level = 0);

    /**
     * @brief 开始示教模式
     *
     * @param task_id
     *      // 0 停止轨迹记录，1 开始示教轨迹，2 复现轨迹 3 查询当前记录
     * @param name 路径的命名
     * @return int
     */
    // 0,1 开始结束示教
    int trajectory_teach(bool off_on, std::string name);
    // 2 复现轨迹
    int trajectory_recorder(std::string name);
    // 3 获取记录
    int check_teach(std::vector<std::string>& traj_list);

    /**
     * @brief 运动学接口
     *
     * @param tool_index
     * @param pose_joint
     * @return int 1: 指令发送成功，<1: 指令发送失败
     */
    int inverse_kine_left_array(int tool_index,
                                const std::vector<std::array<double, 7>>& quat_pose,
                                const std::vector<std::vector<double>>& ref_joint,
                                std::vector<std::vector<double>>& jnt_value);
    int forward_kine_left_array(int tool_index,
                                const std::vector<std::vector<double>>& jnt_value,
                                std::vector<std::array<double, 7>>& quat_pose);
    int inverse_kine_left(int tool_index,
                          const std::array<double, 7>& quat_pose,
                          const std::vector<double>& ref_joint,
                          std::vector<double>& jnt_value);
    int forward_kine_left(int tool_index,
                          const std::vector<double>& jnt_value,
                          std::array<double, 7>& quat_pose);

    int inverse_kine_right_array(int tool_index,
                                 const std::vector<std::array<double, 7>>& quat_pose,
                                 const std::vector<std::vector<double>>& ref_joint,
                                 std::vector<std::vector<double>>& jnt_value);
    int forward_kine_right_array(int tool_index,
                                 const std::vector<std::vector<double>>& jnt_value,
                                 std::vector<std::array<double, 7>>& quat_pose);
    int inverse_kine_right(int tool_index,
                           const std::array<double, 7>& quat_pose,
                           const std::vector<double>& ref_joint,
                           std::vector<double>& jnt_value);
    int forward_kine_right(int tool_index,
                           const std::vector<double>& jnt_value,
                           std::array<double, 7>& quat_pose);

    void register_error_cbk(const std::string& key,
                            std::function<void(int, const std::string)> cbk);
    void release_error_cbk(const std::string& key);

    void register_completion_cbk(const std::string& key,
                                 std::function<void(const std::string)> cbk);
    void release_completion_cbk(const std::string& key);

private:
    CArmKernelImpl* implement_l_;
    CArmKernelImpl* implement_r_;
};
}  // namespace carm