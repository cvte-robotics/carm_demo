/**
 * @file test_sdk.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-04-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "arm_control_sdk/carm_cobot.h"
#include "arm_control_sdk/data_type_def.h"

/**************************需要设置的参数*************************** */
// 请确认臂的IP
std::string carm_IP = "10.42.0.101";
/***************************************************************** */

std::shared_ptr<carm::CArmSingleCol> carm_{nullptr};

bool error_flag = false;
volatile sig_atomic_t ctrl_c_flag = 0;

void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n捕获到 Ctrl+C, 准备退出..." << std::endl;
        ctrl_c_flag = 1;
    }
}

/***********************************************************
 * 基础状态操作
 *
 ***********************************************************/

/**
 * @brief 连接carm, 并且连接外部机械臂
 *
 */
void connect() {
    int ret = carm_->connect(carm_IP);
    printf("connect_carm, ret = %d\n", ret);
}

/**
 * @brief 紧急停止机械臂
 *
 */
void emergency() {
    carm_->emergency_stop();
    printf("SIG_EMERGENCY_STOP\n");
}

/**
 * @brief carm 运行出错时的回调函数
 *
 * @param err_code
 * @param err_msg
 */
void onCarmError(int err_code, const std::string err_msg) {
    error_flag = true;
    if (err_code == 3001)
        printf("recieve a warnning, msg = %s\n", err_msg.c_str());
    else
        printf("recieve a error, code = %d, msg = %s\n", err_code, err_msg.c_str());
}

/**
 * @brief 回归安全点
 *
 */
void moveToHome() {
    std::vector<double> joint = {0, 0.0, 0.0, 0, 0, 0};
    carm_->move_joint(joint);
    printf("moveToHome finished\n");
}

/**
 * @brief 设置碰撞检测等级
 *
 */
void setCollisionCheck() {
    bool flag = false;
    int sensitivity_level;
    printf("请出入是否使能碰撞检测： 0 关闭 1 使能\n");
    std::cin >> flag;
    if (flag == 1) {
        printf("请出入需要的碰撞灵敏度等级： 灵敏 0 ~ 3 迟钝\n");
        std::cin >> sensitivity_level;
    }
    if (flag < 0 || flag > 1) {
        printf("unkown flag = %d\n", flag);
        return;
    }
    carm_->set_collision_config(flag, sensitivity_level);
    printf("set collision check %d\n", flag);
}

/**
 * @brief 刷新机器，并使能机械臂到位置模式
 *
 */
void setReady() {
    int ret = carm_->set_ready();
    printf("set_carm_ready, ret = %d\n", ret);
}

/**
 * @brief 控制关节伺服是否使能
 *
 */
void setServoEnable() {
    int flag = 1;
    printf("请出入伺服控制： 0 失能 1 使能\n");
    std::cin >> flag;
    if (!flag) {
        printf("！！！伺服失能机械臂会失去动力下砸： 0 取消 1 确定\n");
        int ask = 0;
        std::cin >> ask;
        if (!ask) {
            printf("伺服失能已取消\n");
            return;
        }
    }
    carm_->set_servo_enable(flag);
}

/**
 * @brief 设置控制臂的方式
 *
 */
void setControlMode() {
    int mode;
    printf("请输入mode: 0-4： 0 空闲 1 PV模式 2 MIT模式 3 拖动模式 4 力位混合模式\n");
    std::cin >> mode;
    if (mode < 0 || mode > 4) {
        printf("请输入mode: 0-4,, 当前输入mode = %d\n", mode);
        return;
    }
    int ret = carm_->set_control_mode(mode);
    printf("set_control_mode, mode = %d, ret = %d\n", mode, ret);
}

/**
 * @brief 设置机械臂工具号
 *
 */
void setToolIndex() {
    int index;
    printf("请输入需要切换的工具号： 0 无工具 1 夹爪 ... \n");
    std::cin >> index;
    int ret = carm_->set_tool_index(index);
    printf("setToolIndex, index = %d, ret = %d\n", index, ret);
}

/**
 * @brief 设置速度等级, 0~10, 可以动态减速
 *
 */
void setSpeedLevel() {
    int speed_level = 1;
    printf("请出入目标速度档位： 0 ~ 10 \n");
    std::cin >> speed_level;
    if (speed_level < 0 || speed_level > 10) {
        printf("unkown speed_level = %d\n", speed_level);
        return;
    }
    int ret = carm_->set_speed_level(speed_level);
    printf("set_speed_level, speed_level = %d, ret = %d\n", speed_level, ret);
}

/**
 * @brief 显示臂当前姿态
 *
 */
void printState() {
    std::vector<double> joint_pos = carm_->get_joint_pos();
    std::array<double, 7> cart_pos = carm_->get_cart_pose();
    printf("current joint pos: %f, %f, %f, %f, %f, %f\n",
           joint_pos[0],
           joint_pos[1],
           joint_pos[2],
           joint_pos[3],
           joint_pos[4],
           joint_pos[5]);
    printf("current cart pos: %f, %f, %f, %f, %f, %f, %f\n",
           cart_pos[0],
           cart_pos[1],
           cart_pos[2],
           cart_pos[3],
           cart_pos[4],
           cart_pos[5],
           cart_pos[6]);
    printf("gripper state: %d, pos: %f, tau: %f\n",
           carm_->get_gripper_state(),
           carm_->get_gripper_pos(),
           carm_->get_gripper_tau());
}

/***********************************************************
 * 运动指令
 *
 ***********************************************************/

/**
 * @brief 关节移动基础指令
 *
 */
void moveToTestJoint() {
    std::vector<double> joint = {0.0, 0.361361, -0.646622, 0.0, 0.361361, 0.0};
    carm_->move_joint(joint);
    printf("move_joint 1 finished\n");
}

/**
 * @brief 关节移动基础指令，指定运动时间的
 *
 */
void moveJointWithTime() {
    double time = 1.5;
    std::vector<double> waypoints = {0.0, 0.361361, -0.646622, 0.0, 0.361361, 0.0};
    carm_->move_joint(waypoints, time);
}

/**
 * @brief 关节移动基础指令
 *
 */
void moveToTestPose() {
    std::array<double, 7> joint = {
            -0.010669, 0.000073, 0.415677, 0.733466, 0.000187, 0.679726, 0.000029};
    carm_->move_pose(joint);
    printf("move_pose 2 finished\n");
}

/**
 * @brief 关节移动基础指令，指定运动时间的
 *
 */
void movePoseWithTime() {
    double time = 1.5;
    std::array<double, 7UL> pose = {
            -0.010669, 0.000073, 0.415677, 0.733466, 0.000187, 0.679726, 0.000029};
    carm_->move_pose(pose, time);
}

/**
 * @brief 直线移动基础指令
 *
 */
void moveLineWithJoint() {
    std::vector<double> joint = {0.0, 0.361361, -0.646622, 0.0, 0.361361, 0.0};
    carm_->move_line_joint(joint);
}

/**
 * @brief 直线移动基础指令
 *
 */
void moveLineWithPose() {
    std::array<double, 7UL> pose = {
            -0.010669, 0.000073, 0.415677, 0.733466, 0.000187, 0.679726, 0.000029};
    carm_->move_line_pose(pose);
}

/**
 * @brief 循环测试点
 *
 */
void cycleTest() {
    int cycle_times;
    std::cout << "input cyc times" << std::endl;
    std::cin >> cycle_times;

    ctrl_c_flag = 0;
    error_flag = 0;
    std::vector<double> joint1 = {0.3, 0.361361, -0.646622, 0.0, 0.361361, 0.0};
    std::vector<double> joint2 = {-0.3, 0.361361, -0.646622, 0.0, 0.361361, 0.0};

    std::vector<double> joint_home = {0, 0.0, 0.0, 0, 0, 0};
    std::string task_key;
    for (int i = 0; i < cycle_times; i++) {
        carm_->move_joint(joint2);
        carm_->move_joint(joint_home);
        carm_->move_joint(joint1);
        carm_->move_joint(joint_home);
        if (error_flag || ctrl_c_flag) break;
        printf("cycleTest: %d/%d\n", i + 1, cycle_times);
    }
}

/**
 * @brief 运动学逆解接口
 *
 */
void inverseKineTest() {
    std::array<double, 7> quat_pose = {
            -0.010669, 0.000073, 0.415677, 0.733466, 0.000187, 0.679726, 0.000029};
    std::vector<double> ref_joint = {0.0, 0.361361, -0.646622, 0.0, 0.361361, 0.0};
    std::vector<double> jnt_value;
    int ret = carm_->inverse_kine(0, quat_pose, ref_joint, jnt_value);
    if (ret == 1)
        printf("jnt_value: %f, %f, %f, %f, %f, %f\n",
               jnt_value[0],
               jnt_value[1],
               jnt_value[2],
               jnt_value[3],
               jnt_value[4],
               jnt_value[5]);

    printf("inverseCompTest, ret = %d\n", ret);
}

/**
 * @brief 运动学正解接口
 *
 */
void forwardKineTest() {
    std::array<double, 7> quat_pose;
    std::vector<double> jnt_value = {0.0, 0.361361, -0.646622, 0.0, 0.361361, 0.0};
    int ret = carm_->forward_kine(0, jnt_value, quat_pose);
    if (ret == 1)
        printf("quat_pose: %f, %f, %f, %f, %f, %f, %f\n",
               quat_pose[0],
               quat_pose[1],
               quat_pose[2],
               quat_pose[3],
               quat_pose[4],
               quat_pose[5],
               quat_pose[6]);

    printf("forwardKineTest, ret = %d\n", ret);
}

/**
 * @brief 设置夹爪开合度和力矩
 *
 */
void setEndEffector() {
    double input_pos, input_tau;

    printf("请输入夹抓间隔(0-0.08m): \n");
    std::cin >> input_pos;
    printf("请输入夹抓扭矩(0-20N): \n");
    std::cin >> input_tau;
    int ret = carm_->set_gripper(input_pos, input_tau);
    printf("set_gripper, ret = %d, input_pos = %f, input_tau = %f\n", ret, input_pos, input_tau);
}

void movePvtWithJoint() {
    std::vector<double> time_1 = {2, 4, 6, 8};
    std::vector<std::vector<double>> waypoints = {// joint space
                                                  {0.3, 0.361361, -0.646622, 0.0, 0.361361, 0.0},
                                                  {0.0, 0.361361, -0.646622, 0.0, 0.361361, 0.0},
                                                  {-0.3, 0.361361, -0.646622, 0.0, 0.361361, 0.0},
                                                  {0, 0.0, 0.0, 0, 0, 0}};
    std::vector<double> joint = {0, 0.0, 0.0, 0, 0, 0};
    carm_->move_joint(joint);
    carm_->move_joint_traj(waypoints, {0, 0.04, 0.08, 0.0}, time_1);
}

void movePvtWithPose() {
    std::vector<double> time_1 = {2, 4, 6, 8};
    std::vector<std::vector<double>> waypoints = {// joint space
                                                  {0.3, 0.361361, -0.646622, 0.0, 0.361361, 0.0},
                                                  {0.0, 0.361361, -0.646622, 0.0, 0.361361, 0.0},
                                                  {-0.3, 0.361361, -0.646622, 0.0, 0.361361, 0.0},
                                                  {0, 0.0, 0.0, 0, 0, 0}};
    std::vector<std::array<double, 7UL>> quat_pose;
    int ret = carm_->forward_kine_array(0, waypoints, quat_pose);
    for (size_t i = 0; i < quat_pose.size(); i++) {
        printf("waypoint %zu: %f, %f, %f, %f, %f, %f, %f\n",
               i,
               quat_pose[i][0],
               quat_pose[i][1],
               quat_pose[i][2],
               quat_pose[i][3],
               quat_pose[i][4],
               quat_pose[i][5],
               quat_pose[i][6]);
    }
    // std::vector<std::array<double, 7UL>> waypoints = {
    //         {-0.009278, -0.002870, 0.416348, 0.725256, 0.109612, 0.672065, -0.101573},
    //         {-0.009711, 0.000000, 0.416348, 0.733493, 0.000000, 0.679697, 0.000000},
    //         {-0.009278, 0.002870, 0.416348, 0.725256, -0.109612, 0.672065, 0.101573},
    //         {0.000000, 0.000000, 0.235999, 0.707105, 0.000000, 0.707108, 0.000000}};

    std::vector<double> joint = {0, 0.0, 0.0, 0, 0, 0};
    carm_->move_joint(joint);
    carm_->move_pose_traj(quat_pose, {0, 0.04, 0.08, 0.0}, time_1);
}

// 服务器的IP地址和端口号
int main(int argc, char* argv[]) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;  // 指定处理函数
    sigemptyset(&sa.sa_mask);        // 清空信号掩码（不阻塞其他信号）
    sa.sa_flags = 0;                 // 默认标志

    // 注册信号处理
    if (sigaction(SIGINT, &sa, nullptr) == -1) {
        std::cerr << "无法注册信号处理器" << std::endl;
        return 1;
    }

    carm_ = std::make_shared<carm::CArmSingleCol>(carm_IP, 8090, 1);

    carm_->register_error_cbk("onCarmError", onCarmError);

    std::string cmd;
    const std::map<std::string, void (*)()> cmd_handle = {{"c", &connect},
                                                          {"r", &setReady},
                                                          {"cc", &setCollisionCheck},
                                                          {"cm", &setControlMode},
                                                          {"e", &emergency},
                                                          {"p", &printState},
                                                          {"le", &setSpeedLevel},
                                                          {"sti", &setToolIndex},
                                                          {"se", &setServoEnable},
                                                          {"mh", &moveToHome},
                                                          {"mj", &moveToTestJoint},
                                                          {"mjt", &moveJointWithTime},
                                                          {"mp", &moveToTestPose},
                                                          {"mpt", &movePoseWithTime},
                                                          {"mlj", &moveLineWithJoint},
                                                          {"mlp", &moveLineWithPose},
                                                          {"ct", &cycleTest},
                                                          {"sgg", &setEndEffector},
                                                          {"ik", &inverseKineTest},
                                                          {"fk", &forwardKineTest},
                                                          {"pvtj", &movePvtWithJoint},
                                                          {"pvtp", &movePvtWithPose}};
    while (std::cin >> cmd) {
        try {
            if (cmd == "q")
                break;
            else if (auto h = cmd_handle.find(cmd); h != cmd_handle.end())
                h->second();
            else
                printf("unkown cmd: %s\n", cmd.c_str());
        } catch (const std::exception& e) {
            printf("catch an exception, what = %s\n", e.what());
        } catch (...) {
            printf("catch an exception\n");
        }
    }

    return 0;
}