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

#include "arm_control_sdk/carm_dual.h"
#include "arm_control_sdk/data_type_def.h"

/**************************需要设置的参数*************************** */
// 请确认臂的IP
std::string carm_IP = "10.42.0.101";
/***************************************************************** */

std::shared_ptr<carm::CArmDualBot> carm_{nullptr};

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
    carm_->move_left_joint(joint);
    carm_->move_right_joint(joint);
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
    int ret = carm_->set_left_tool_index(index);
    ret += carm_->set_right_tool_index(index);
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
    std::vector<double> left_joint_pos = carm_->get_left_joint_pos();
    std::vector<double> right_joint_pos = carm_->get_right_joint_pos();
    std::array<double, 7> left_cart_pos = carm_->get_left_cart_pose();
    std::array<double, 7> right_cart_pos = carm_->get_right_cart_pose();
    printf("current joint pos: %f, %f, %f, %f, %f, %f\n",
           left_joint_pos[0],
           left_joint_pos[1],
           left_joint_pos[2],
           left_joint_pos[3],
           left_joint_pos[4],
           left_joint_pos[5]);
    printf("current joint pos: %f, %f, %f, %f, %f, %f\n",
           right_joint_pos[0],
           right_joint_pos[1],
           right_joint_pos[2],
           right_joint_pos[3],
           right_joint_pos[4],
           right_joint_pos[5]);
    printf("current cart pos: %f, %f, %f, %f, %f, %f, %f\n",
           left_cart_pos[0],
           left_cart_pos[1],
           left_cart_pos[2],
           left_cart_pos[3],
           left_cart_pos[4],
           left_cart_pos[5],
           left_cart_pos[6]);
    printf("current cart pos: %f, %f, %f, %f, %f, %f, %f\n",
           right_cart_pos[0],
           right_cart_pos[1],
           right_cart_pos[2],
           right_cart_pos[3],
           right_cart_pos[4],
           right_cart_pos[5],
           right_cart_pos[6]);
    printf("gripper state: %d, pos: %f, tau: %f\n",
           carm_->get_left_gripper_state(),
           carm_->get_left_gripper_pos(),
           carm_->get_left_gripper_tau());
    printf("gripper state: %d, pos: %f, tau: %f\n",
           carm_->get_right_gripper_state(),
           carm_->get_right_gripper_pos(),
           carm_->get_right_gripper_tau());
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
    std::vector<double> joint = {-1.811820, -1.549150, 0.350767, 0.128748, 1.160640, -0.113871};
    carm_->move_left_joint(joint);
    carm_->move_right_joint(joint);
    printf("move_joint 1 finished\n");
}

/**
 * @brief 关节移动基础指令，指定运动时间的
 *
 */
void moveJointWithTime() {
    double time = 1.5;
    std::vector<double> waypoints = {-1.811820, -1.549150, 0.350767, 0.128748, 1.160640, -0.113871};
    carm_->move_left_joint(waypoints, time);
    carm_->move_right_joint(waypoints, time);
}

/**
 * @brief 关节移动基础指令
 *
 */
void moveToTestPose() {
    std::array<double, 7> joint = {
            -0.054629, -0.266915, 0.409172, -0.640853, 0.765125, -0.059569, 0.018509};
    carm_->move_left_pose(joint);
    carm_->move_right_pose(joint);
    printf("move_pose 2 finished\n");
}

/**
 * @brief 关节移动基础指令，指定运动时间的
 *
 */
void movePoseWithTime() {
    double time = 1.5;
    std::array<double, 7UL> pose = {
            -0.054629, -0.266915, 0.409172, -0.640853, 0.765125, -0.059569, 0.018509};
    carm_->move_left_pose(pose, time);
    carm_->move_right_pose(pose, time);
}

/**
 * @brief 直线移动基础指令
 *
 */
void moveLineWithJoint() {
    std::vector<double> joint = {-1.845390, -1.074010, -0.190547, 0.128748, 1.250670, -0.113871};
    carm_->move_left_line_joint(joint);
    carm_->move_right_line_joint(joint);
}

/**
 * @brief 直线移动基础指令
 *
 */
void moveLineWithPose() {
    std::array<double, 7UL> pose = {
            -0.105816, -0.416322, 0.384208, -0.631610, 0.772836, -0.054676, 0.028356};
    carm_->move_left_line_pose(pose);
    carm_->move_right_line_pose(pose);
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
    std::vector<double> joint1 = {-0.293546, -0.519677, 0.201610, 0.073816, -0.867285, -0.000572};
    std::vector<double> joint2 = {-0.975243, -0.338856, 0.161555, -0.472458, -1.107230, 0.014687};

    std::vector<double> joint_home = {0, 0.0, 0.0, 0, 0, 0};
    std::string task_key;
    for (int i = 0; i < cycle_times; i++) {
        carm_->move_left_joint(joint2);
        sleep(2);
        carm_->move_left_joint(joint_home);
        usleep(500000);
        carm_->move_left_joint(joint1);
        sleep(2);
        carm_->move_left_joint(joint_home);
        usleep(500000);
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
            -0.012700, -0.001048, 0.304054, 0.696427, 0.000067, 0.717628, 0.000208};
    std::vector<double> ref_joint = {
            -0.000191, -3.000412, 1.399824, -0.000191, -0.000191, -0.000191};
    std::vector<double> jnt_value;
    int ret = carm_->inverse_kine_left(0, quat_pose, ref_joint, jnt_value);
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
    std::vector<double> jnt_value = {
            -0.000191, -3.000412, 1.399824, -0.000191, -0.000191, -0.000191};
    int ret = carm_->forward_kine_left(0, jnt_value, quat_pose);
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
    int ret = carm_->set_left_gripper(input_pos, input_tau);
    printf("set_gripper, ret = %d, input_pos = %f, input_tau = %f\n", ret, input_pos, input_tau);
}

void movePvtWithJoint() {
    // std::vector<double> time_1 = {0.5, 1, 1.5, 2, 3, 4, 5};
    std::vector<double> time_1 = {2, 4, 8, 10, 12, 14, 15};

    std::vector<std::vector<double>> waypoints = {
            // joint space
            {-0.906196, -0.481900, -0.173381, 0.113871, 0.538072, -0.113871},
            {-0.274472, -1.962170, 0.501450, 0.101664, 0.929085, -0.113871},
            {-0.304608, -0.477682, -0.015449, 0.102427, 0.324826, -0.113871},
            {0.520905, -1.539180, -0.261883, 0.174907, 0.330930, -0.113871},
            {0.368315, -0.866924, 0.037956, 0.153926, 0.682269, -0.113871},
            {-0.266079, -1.972910, 0.150493, 0.139048, -0.586900, -0.113489},
            {-0.737202, -0.567801, 0.112726, 0.030708, 0.440033, -0.113489}};

    std::string task_key;
    bool is_joint_val = true;

    std::vector<double> joint = {-1.811820, -1.549150, 0.350767, 0.128748, 1.160640, -0.113871};
    carm_->move_left_joint(joint);
    sleep(1);
    carm_->move_left_joint_traj(waypoints, {0, 0.05, 0.06, 0.08, 0.04, 0.02, 0.03, 0}, time_1);
}

void movePvtWithPose() {
    // std::vector<double> time_1 = {0.5, 1, 1.5, 2, 3, 4, 5};
    std::vector<double> time_1 = {2, 4, 8, 10, 12, 14, 15};

    std::vector<std::array<double, 7>> waypoints_2 = {
            // cart space
            {0.336502, -0.420934, 0.088224, 0.901308, -0.428020, 0.066644, 0.000643},
            {0.163011, -0.038252, 0.463007, 0.959030, -0.095890, 0.266548, 0.004249},
            {0.494966, -0.152516, 0.054170, 0.986160, -0.141519, 0.086341, -0.002477},
            {0.276861, 0.164755, 0.640094, 0.715226, 0.190485, 0.638835, -0.209898},
            {0.444405, 0.180890, 0.233322, 0.978961, 0.183073, 0.065787, -0.061569},
            {0.134926, -0.043977, 0.692571, 0.351793, -0.083686, 0.922327, 0.136203},
            {0.357181, -0.322710, 0.072989, 0.947380, -0.319955, 0.009592, -0.002949}};

    std::string task_key;
    bool is_joint_val = true;

    std::vector<double> joint = {-1.811820, -1.549150, 0.350767, 0.128748, 1.160640, -0.113871};
    carm_->move_left_joint(joint);
    sleep(1);
    carm_->move_left_pose_traj(waypoints_2, {0, 0.05, 0.06, 0.08, 0.04, 0.02, 0.03, 0}, time_1);
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

    carm_ = std::make_shared<carm::CArmDualBot>(carm_IP, 8090, 1);

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
