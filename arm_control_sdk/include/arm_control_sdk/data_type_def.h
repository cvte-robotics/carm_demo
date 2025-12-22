#pragma once
#include <algorithm>
#include <array>
#include <map>
#include <string>
#include <vector>
namespace carm {
/*******************************
 * 机器运行模式，以及运行状态
 */
enum class CarmState { Carm_Error = -1, Carm_Standby = 0, Carm_Running = 1 };
enum class FsmMode {
    ERROR = -1,    // 错误，异常
    IDLE,          // 空闲状态
    POSITION,      // 点位控制模式
    MIT,           // MIT控制模式
    CURRENT,       // 电流控制
    PF,            // PF力位混合模式
    TELEOPERATION  // 遥操作模式
};
static std::map<FsmMode, std::string> fsm_mode_str_map{{FsmMode::ERROR, "ERROR"},
                                                       {FsmMode::IDLE, "IDLE"},
                                                       {FsmMode::POSITION, "POSITION"},
                                                       {FsmMode::CURRENT, "CURRENT"},
                                                       {FsmMode::MIT, "MIT"},
                                                       {FsmMode::TELEOPERATION, "TELEOPERATION"},
                                                       {FsmMode::PF, "PF"}};

static std::map<std::string, FsmMode> str_map_fsm_mode{{"ERROR", FsmMode::ERROR},
                                                       {"IDLE", FsmMode::IDLE},
                                                       {"POSITION", FsmMode::POSITION},
                                                       {"CURRENT", FsmMode::CURRENT},
                                                       {"MIT", FsmMode::MIT},
                                                       {"TELEOPERATION", FsmMode::TELEOPERATION},
                                                       {"PF", FsmMode::PF}};

static std::map<CarmState, std::string> carm_state_str{{CarmState::Carm_Error, "Carm_Error"},
                                                       {CarmState::Carm_Standby, "Carm_Standby"},
                                                       {CarmState::Carm_Running, "Carm_Running"}};
/*******************************
 * 机械臂固有变量（py支持）
 */
struct ArmConfig {
    ArmConfig(int df) {
        dof = df;
        limit_upper.resize(dof);
        limit_lower.resize(dof);

        joint_vel.resize(dof);
        joint_acc.resize(dof);
        joint_dec.resize(dof);
        joint_jerk.resize(dof);
    }
    ArmConfig() = default;
    // 关节数量
    int dof = 0;
    // 关节限位
    std::vector<double> limit_upper;
    std::vector<double> limit_lower;
    // 关节最大速度、加速度、加加速
    std::vector<double> joint_vel;
    std::vector<double> joint_acc;
    std::vector<double> joint_dec;
    std::vector<double> joint_jerk;
};

/*******************************
 * 机械臂状态量（py支持）
 */
struct ArmStatus {
    int arm_index;               // 机械臂编号
    std::string arm_name;        // 臂名称
    bool arm_is_connected;       // 通过通讯进行控制时的标志
    int arm_dof;                 // 臂自由度
    bool servo_status;           // 伺服状态 1使能 0失能
    int state;                   // 0-standby,1-running,-1-error 控制器状态
    int fsm_state;               // 0-idle,1-position，2-MIT, 3-drag 控制器模式
    double speed_percentage;     // 速度百分比，通过set_speed_level()改
    bool on_debug_mode = false;  // 是否在仿真状态
};

}  // namespace carm