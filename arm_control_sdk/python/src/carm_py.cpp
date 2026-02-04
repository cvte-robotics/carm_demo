#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "arm_control_sdk/carm_cobot.h"
#include "arm_control_sdk/carm_dual.h"
// #include "arm_control_sdk/carm_kernel.h"

namespace py = pybind11;
PYBIND11_MODULE(carm_py, m) {
    m.doc() = "CARM pybind11 C++ extension";
    py::class_<carm::ArmConfig>(m, "ArmConfig")
            .def(py::init<int>())  // 构造函数（dof）
            .def_readwrite("dof", &carm::ArmConfig::dof)
            .def_readwrite("limit_upper", &carm::ArmConfig::limit_upper)
            .def_readwrite("limit_lower", &carm::ArmConfig::limit_lower)
            .def_readwrite("joint_vel", &carm::ArmConfig::joint_vel)
            .def_readwrite("joint_acc", &carm::ArmConfig::joint_acc)
            .def_readwrite("joint_dec", &carm::ArmConfig::joint_dec)
            .def_readwrite("joint_jerk", &carm::ArmConfig::joint_jerk);

    py::class_<carm::ArmStatus>(m, "ArmStatus")
            .def(py::init<>())
            .def_readwrite("arm_index", &carm::ArmStatus::arm_index)
            .def_readwrite("arm_name", &carm::ArmStatus::arm_name)
            .def_readwrite("arm_is_connected", &carm::ArmStatus::arm_is_connected)
            .def_readwrite("arm_dof", &carm::ArmStatus::arm_dof)
            .def_readwrite("servo_status", &carm::ArmStatus::servo_status)
            .def_readwrite("state", &carm::ArmStatus::state)
            .def_readwrite("fsm_state", &carm::ArmStatus::fsm_state)
            .def_readwrite("speed_percentage", &carm::ArmStatus::speed_percentage)
            .def_readwrite("on_debug_mode", &carm::ArmStatus::on_debug_mode);

    py::class_<carm::CArmSingleCol>(m, "CArmSingleCol")
            .def(py::init<const std::string&, int, double>(),
                 py::arg("server_ip") = "10.42.0.101",
                 py::arg("port") = 8090,
                 py::arg("timeout") = 1.0)  // 暴露构造函数
            .def("connect",
                 &carm::CArmSingleCol::connect,
                 py::arg("server_ip") = "10.42.0.101",
                 py::arg("port") = 8090,
                 py::arg("timeout") = 1.0)  // 链接臂
            .def("disconnect",
                 &carm::CArmSingleCol::disconnect)  // 断开臂
            .def("is_connected",
                 &carm::CArmSingleCol::is_connected)  // 断开臂
            .def("set_ready",
                 &carm::CArmSingleCol::set_ready)  // 初始化机械臂
            .def("set_servo_enable", &carm::CArmSingleCol::set_servo_enable, py::arg("enable"))
            // 1-position 点位控制模式, 2-MIT 力矩模式， 3-drag 拖动模式, 4-PF 力位混合模式
            .def("set_control_mode", &carm::CArmSingleCol::set_control_mode, py::arg("mode"))
            // 机械臂状态获取
            .def("get_version", &carm::CArmSingleCol::get_version)
            .def("get_config", &carm::CArmSingleCol::get_config)
            .def("get_status", &carm::CArmSingleCol::get_status)

            .def("get_joint_pos", &carm::CArmSingleCol::get_joint_pos)
            .def("get_joint_vel", &carm::CArmSingleCol::get_joint_vel)
            .def("get_joint_tau", &carm::CArmSingleCol::get_joint_tau)
            .def("get_plan_joint_pos", &carm::CArmSingleCol::get_plan_joint_pos)
            .def("get_plan_joint_vel", &carm::CArmSingleCol::get_plan_joint_vel)
            .def("get_plan_joint_tau", &carm::CArmSingleCol::get_plan_joint_tau)
            .def("get_plan_cart_pose", &carm::CArmSingleCol::get_plan_cart_pose)
            .def("get_cart_pose", &carm::CArmSingleCol::get_cart_pose)
            .def("get_joint_external_tau", &carm::CArmSingleCol::get_joint_external_tau)
            .def("get_cart_external_force", &carm::CArmSingleCol::get_cart_external_force)

            .def("register_joint_cbk", &carm::CArmSingleCol::register_joint_cbk, py::arg("cbk"))
            .def("release_joint_cbk", &carm::CArmSingleCol::release_joint_cbk)
            .def("register_pose_cbk", &carm::CArmSingleCol::register_pose_cbk, py::arg("cbk"))
            .def("release_pose_cbk", &carm::CArmSingleCol::release_pose_cbk)
            .def("register_plan_joint_cbk",
                 &carm::CArmSingleCol::register_plan_joint_cbk,
                 py::arg("cbk"))
            .def("release_plan_joint_cbk", &carm::CArmSingleCol::release_plan_joint_cbk)
            .def("register_plan_pose_cbk",
                 &carm::CArmSingleCol::register_plan_pose_cbk,
                 py::arg("cbk"))
            .def("release_plan_pose_cbk", &carm::CArmSingleCol::release_plan_pose_cbk)
            .def("register_external_force_cbk",
                 &carm::CArmSingleCol::register_external_force_cbk,
                 py::arg("cbk"))
            .def("release_external_force_cbk", &carm::CArmSingleCol::release_external_force_cbk)

            .def("get_gripper_state", &carm::CArmSingleCol::get_gripper_state)
            .def("get_gripper_pos", &carm::CArmSingleCol::get_gripper_pos)
            .def("get_gripper_vel", &carm::CArmSingleCol::get_gripper_vel)
            .def("get_gripper_tau", &carm::CArmSingleCol::get_gripper_tau)
            .def("get_plan_gripper_pos", &carm::CArmSingleCol::get_plan_gripper_pos)
            .def("get_plan_gripper_tau", &carm::CArmSingleCol::get_plan_gripper_tau)

            .def("track_joint",
                 &carm::CArmSingleCol::track_joint,
                 py::arg("targets"),
                 py::arg("gripper_pos") = -1)
            .def("track_pose",
                 &carm::CArmSingleCol::track_pose,
                 py::arg("targets"),
                 py::arg("gripper_pos") = -1)

            .def("move_joint",
                 &carm::CArmSingleCol::move_joint,
                 py::arg("targets"),
                 py::arg("desire_time") = -1,
                 py::arg("is_sync") = true)
            .def("move_pose",
                 &carm::CArmSingleCol::move_pose,
                 py::arg("targets"),
                 py::arg("desire_time") = -1,
                 py::arg("is_sync") = true)

            .def("move_line_joint",
                 &carm::CArmSingleCol::move_line_joint,
                 py::arg("targets"),
                 py::arg("is_sync") = true)
            .def("move_line_pose",
                 &carm::CArmSingleCol::move_line_pose,
                 py::arg("targets"),
                 py::arg("is_sync") = true)

            .def("move_joint_traj",
                 &carm::CArmSingleCol::move_joint_traj,
                 py::arg("target_pos"),
                 py::arg("gripper_pos") = std::vector<double>(),
                 py::arg("stamps") = std::vector<double>(),
                 py::arg("is_sync") = true)
            .def("move_pose_traj",
                 &carm::CArmSingleCol::move_pose_traj,
                 py::arg("target_pos"),
                 py::arg("gripper_pos") = std::vector<double>(),
                 py::arg("stamps") = std::vector<double>(),
                 py::arg("is_sync") = true)

            .def("emergency_stop", &carm::CArmSingleCol::emergency_stop)  // 紧急急停

            .def("set_gripper",
                 &carm::CArmSingleCol::set_gripper,
                 py::arg("pos"),
                 py::arg("tau") = 10)

            .def("set_speed_level",
                 &carm::CArmSingleCol::set_speed_level,
                 py::arg("level"),
                 py::arg("response_level") = 20)
            .def("set_tool_index", &carm::CArmSingleCol::set_tool_index, py::arg("index"))
            .def("get_tool_index", &carm::CArmSingleCol::get_tool_index)
            .def("get_tool_coordinate", &carm::CArmSingleCol::get_tool_coordinate, py::arg("index"))

            .def("set_collision_config",
                 &carm::CArmSingleCol::set_collision_config,
                 py::arg("enable_flag") = true,
                 py::arg("sensitivity_level") = 0)

            .def("trajectory_teach",
                 &carm::CArmSingleCol::trajectory_teach,
                 py::arg("off_on"),
                 py::arg("name"))
            .def("trajectory_recorder", &carm::CArmSingleCol::trajectory_recorder, py::arg("name"))
            .def("check_teach",
                 [](carm::CArmSingleCol& self) {
                     std::vector<std::string> traj_list;
                     int ret = self.check_teach(traj_list);
                     return std::make_tuple(ret, traj_list);
                 })

            // 运动学接口
            .def(
                    "inverse_kine_array",
                    [](carm::CArmSingleCol& self,
                       int tool_index,
                       const std::vector<std::array<double, 7>>& quat_pose,
                       const std::vector<std::vector<double>>& ref_joint) {
                        std::vector<std::vector<double>> jnt_value;
                        int ret = self.inverse_kine_array(
                                tool_index, quat_pose, ref_joint, jnt_value);
                        return std::make_tuple(ret, jnt_value);
                    },
                    py::arg("tool_index"),
                    py::arg("quat_pose"),
                    py::arg("ref_joint"))

            .def(
                    "forward_kine_array",
                    [](carm::CArmSingleCol& self,
                       int tool_index,
                       const std::vector<std::vector<double>>& jnt_value) {
                        std::vector<std::array<double, 7>> quat_pose;
                        int ret = self.forward_kine_array(tool_index, jnt_value, quat_pose);
                        return std::make_tuple(ret, quat_pose);
                    },
                    py::arg("tool_index"),
                    py::arg("jnt_value"))

            .def(
                    "inverse_kine",
                    [](carm::CArmSingleCol& self,
                       int tool_index,
                       const std::array<double, 7>& quat_pose,
                       const std::vector<double>& ref_joint) {
                        std::vector<double> jnt_value;
                        int ret = self.inverse_kine(tool_index, quat_pose, ref_joint, jnt_value);
                        return std::make_tuple(ret, jnt_value);
                    },
                    py::arg("tool_index"),
                    py::arg("quat_pose"),
                    py::arg("ref_joint"))

            .def(
                    "forward_kine",
                    [](carm::CArmSingleCol& self,
                       int tool_index,
                       const std::vector<double>& jnt_value) {
                        std::array<double, 7> quat_pose;
                        int ret = self.forward_kine(tool_index, jnt_value, quat_pose);
                        return std::make_tuple(ret, quat_pose);
                    },
                    py::arg("tool_index"),
                    py::arg("jnt_value"))
            .def("register_error_cbk",
                 &carm::CArmSingleCol::register_error_cbk,
                 py::arg("key"),
                 py::arg("cbk"))
            .def("release_error_cbk", &carm::CArmSingleCol::release_error_cbk, py::arg("key"))
            .def("register_completion_cbk",
                 &carm::CArmSingleCol::register_completion_cbk,
                 py::arg("key"),
                 py::arg("cbk"))
            .def("release_completion_cbk",
                 &carm::CArmSingleCol::release_completion_cbk,
                 py::arg("key"));

    py::class_<carm::CArmDualBot>(m, "CArmDualBot")
            .def(py::init<const std::string&, int, double, int, int>(),
                 py::arg("server_ip") = "10.42.0.101",
                 py::arg("port") = 8090,
                 py::arg("timeout") = 1.0,
                 py::arg("left_index") = 0,
                 py::arg("right_index") = 1)
            .def("connect",
                 &carm::CArmDualBot::connect,
                 py::arg("server_ip") = "10.42.0.101",
                 py::arg("port") = 8090,
                 py::arg("timeout") = 1.0)
            .def("disconnect", &carm::CArmDualBot::disconnect)
            .def("is_connected", &carm::CArmDualBot::is_connected)
            .def("set_ready", &carm::CArmDualBot::set_ready)
            .def("set_servo_enable", &carm::CArmDualBot::set_servo_enable, py::arg("enable"))
            .def("set_control_mode", &carm::CArmDualBot::set_control_mode, py::arg("mode"))
            .def("get_version", &carm::CArmDualBot::get_version)
            .def("get_left_config", &carm::CArmDualBot::get_left_config)
            .def("get_right_config", &carm::CArmDualBot::get_right_config)
            .def("get_left_status", &carm::CArmDualBot::get_left_status)
            .def("get_right_status", &carm::CArmDualBot::get_right_status)
            .def("get_left_joint_pos", &carm::CArmDualBot::get_left_joint_pos)
            .def("get_right_joint_pos", &carm::CArmDualBot::get_right_joint_pos)
            .def("get_left_joint_vel", &carm::CArmDualBot::get_left_joint_vel)
            .def("get_right_joint_vel", &carm::CArmDualBot::get_right_joint_vel)
            .def("get_left_joint_tau", &carm::CArmDualBot::get_left_joint_tau)
            .def("get_right_joint_tau", &carm::CArmDualBot::get_right_joint_tau)
            .def("get_left_plan_joint_pos", &carm::CArmDualBot::get_left_plan_joint_pos)
            .def("get_right_plan_joint_pos", &carm::CArmDualBot::get_right_plan_joint_pos)
            .def("get_right_plan_joint_vel", &carm::CArmDualBot::get_right_plan_joint_vel)
            .def("get_right_plan_joint_vel", &carm::CArmDualBot::get_right_plan_joint_vel)
            .def("get_left_plan_joint_tau", &carm::CArmDualBot::get_left_plan_joint_tau)
            .def("get_right_plan_joint_tau", &carm::CArmDualBot::get_right_plan_joint_tau)
            .def("get_left_cart_pose", &carm::CArmDualBot::get_left_cart_pose)
            .def("get_right_cart_pose", &carm::CArmDualBot::get_right_cart_pose)
            .def("get_left_joint_external_tau", &carm::CArmDualBot::get_left_joint_external_tau)
            .def("get_right_joint_external_tau", &carm::CArmDualBot::get_right_joint_external_tau)
            .def("get_left_cart_external_force", &carm::CArmDualBot::get_left_cart_external_force)
            .def("get_right_cart_external_force", &carm::CArmDualBot::get_right_cart_external_force)

            .def("register_left_joint_cbk",
                 &carm::CArmDualBot::register_left_joint_cbk,
                 py::arg("cbk"))
            .def("release_left_joint_cbk", &carm::CArmDualBot::release_left_joint_cbk)

            .def("register_left_plan_pose_cbk",
                 &carm::CArmDualBot::register_left_plan_pose_cbk,
                 py::arg("cbk"))
            .def("release_left_plan_pose_cbk", &carm::CArmDualBot::release_left_plan_pose_cbk)

            .def("register_left_pose_cbk",
                 &carm::CArmDualBot::register_left_pose_cbk,
                 py::arg("cbk"))
            .def("release_left_pose_cbk", &carm::CArmDualBot::release_left_pose_cbk)

            .def("register_left_plan_joint_cbk",
                 &carm::CArmDualBot::register_left_plan_joint_cbk,
                 py::arg("cbk"))
            .def("release_left_plan_joint_cbk", &carm::CArmDualBot::release_left_plan_joint_cbk)

            .def("register_left_external_force_cbk",
                 &carm::CArmDualBot::register_left_external_force_cbk,
                 py::arg("cbk"))
            .def("release_left_external_force_cbk",
                 &carm::CArmDualBot::release_left_external_force_cbk)

            .def("register_right_joint_cbk",
                 &carm::CArmDualBot::register_right_joint_cbk,
                 py::arg("cbk"))
            .def("release_right_joint_cbk", &carm::CArmDualBot::release_right_joint_cbk)

            .def("register_right_plan_pose_cbk",
                 &carm::CArmDualBot::register_right_plan_pose_cbk,
                 py::arg("cbk"))
            .def("release_right_plan_pose_cbk", &carm::CArmDualBot::release_right_plan_pose_cbk)

            .def("register_right_pose_cbk",
                 &carm::CArmDualBot::register_right_pose_cbk,
                 py::arg("cbk"))
            .def("release_right_pose_cbk", &carm::CArmDualBot::release_right_pose_cbk)

            .def("register_right_plan_joint_cbk",
                 &carm::CArmDualBot::register_right_plan_joint_cbk,
                 py::arg("cbk"))
            .def("release_right_plan_joint_cbk", &carm::CArmDualBot::release_right_plan_joint_cbk)

            .def("register_right_external_force_cbk",
                 &carm::CArmDualBot::register_right_external_force_cbk,
                 py::arg("cbk"))
            .def("release_right_external_force_cbk",
                 &carm::CArmDualBot::release_right_external_force_cbk)

            .def("get_left_gripper_state", &carm::CArmDualBot::get_left_gripper_state)
            .def("get_right_gripper_state", &carm::CArmDualBot::get_right_gripper_state)
            .def("get_left_gripper_pos", &carm::CArmDualBot::get_left_gripper_pos)
            .def("get_right_gripper_pos", &carm::CArmDualBot::get_right_gripper_pos)
            .def("get_left_gripper_vel", &carm::CArmDualBot::get_left_gripper_vel)
            .def("get_right_gripper_vel", &carm::CArmDualBot::get_right_gripper_vel)
            .def("get_left_gripper_tau", &carm::CArmDualBot::get_left_gripper_tau)
            .def("get_right_gripper_tau", &carm::CArmDualBot::get_right_gripper_tau)
            .def("get_left_plan_gripper_pos", &carm::CArmDualBot::get_left_gripper_pos)
            .def("get_right_plan_gripper_pos", &carm::CArmDualBot::get_right_gripper_pos)
            .def("get_left_plan_gripper_tau", &carm::CArmDualBot::get_left_plan_gripper_tau)
            .def("get_right_plan_gripper_tau", &carm::CArmDualBot::get_right_plan_gripper_tau)

            .def("get_left_hand_state", &carm::CArmDualBot::get_left_hand_state)
            .def("get_right_hand_state", &carm::CArmDualBot::get_right_hand_state)
            .def("get_left_hand_pos", &carm::CArmDualBot::get_left_hand_pos)
            .def("get_right_hand_pos", &carm::CArmDualBot::get_right_hand_pos)
            .def("get_left_hand_vel", &carm::CArmDualBot::get_left_hand_vel)
            .def("get_right_hand_vel", &carm::CArmDualBot::get_right_hand_vel)
            .def("get_left_hand_tau", &carm::CArmDualBot::get_left_hand_tau)
            .def("get_right_hand_tau", &carm::CArmDualBot::get_right_hand_tau)
            .def("get_left_plan_hand_pos", &carm::CArmDualBot::get_left_plan_hand_pos)
            .def("get_right_plan_hand_pos", &carm::CArmDualBot::get_right_plan_hand_pos)
            .def("get_left_plan_hand_vel", &carm::CArmDualBot::get_left_plan_hand_vel)
            .def("get_right_plan_hand_vel", &carm::CArmDualBot::get_right_plan_hand_vel)
            .def("get_left_plan_hand_tau", &carm::CArmDualBot::get_left_plan_hand_tau)
            .def("get_right_plan_hand_tau", &carm::CArmDualBot::get_right_plan_hand_tau)

            .def("track_left_joint",
                 &carm::CArmDualBot::track_left_joint,
                 py::arg("targets"),
                 py::arg("gripper_pos") = -1)
            .def("track_right_joint",
                 &carm::CArmDualBot::track_right_joint,
                 py::arg("targets"),
                 py::arg("gripper_pos") = -1)
            .def("track_left_pose",
                 &carm::CArmDualBot::track_left_pose,
                 py::arg("targets"),
                 py::arg("gripper_pos") = -1)
            .def("track_right_pose",
                 &carm::CArmDualBot::track_right_pose,
                 py::arg("targets"),
                 py::arg("gripper_pos") = -1)
            .def("move_left_joint",
                 &carm::CArmDualBot::move_left_joint,
                 py::arg("target_pos"),
                 py::arg("desire_time") = -1,
                 py::arg("is_sync") = true)
            .def("move_right_joint",
                 &carm::CArmDualBot::move_right_joint,
                 py::arg("target_pos"),
                 py::arg("desire_time") = -1,
                 py::arg("is_sync") = true)
            .def("move_left_pose",
                 &carm::CArmDualBot::move_left_pose,
                 py::arg("target_pos"),
                 py::arg("desire_time") = -1,
                 py::arg("is_sync") = true)
            .def("move_right_pose",
                 &carm::CArmDualBot::move_right_pose,
                 py::arg("target_pos"),
                 py::arg("desire_time") = -1,
                 py::arg("is_sync") = true)
            .def("move_left_line_joint",
                 &carm::CArmDualBot::move_left_line_joint,
                 py::arg("target_pos"),
                 py::arg("is_sync") = true)
            .def("move_right_line_joint",
                 &carm::CArmDualBot::move_right_line_joint,
                 py::arg("target_pos"),
                 py::arg("is_sync") = true)
            .def("move_left_line_pose",
                 &carm::CArmDualBot::move_left_line_pose,
                 py::arg("target_pos"),
                 py::arg("is_sync") = true)
            .def("move_right_line_pose",
                 &carm::CArmDualBot::move_right_line_pose,
                 py::arg("target_pos"),
                 py::arg("is_sync") = true)
            .def("move_left_joint_traj",
                 &carm::CArmDualBot::move_left_joint_traj,
                 py::arg("target_pos"),
                 py::arg("gripper_pos") = std::vector<double>(),
                 py::arg("stamps") = std::vector<double>(),
                 py::arg("is_sync") = true)
            .def("move_right_joint_traj",
                 &carm::CArmDualBot::move_right_joint_traj,
                 py::arg("target_pos"),
                 py::arg("gripper_pos") = std::vector<double>(),
                 py::arg("stamps") = std::vector<double>(),
                 py::arg("is_sync") = true)
            .def("move_left_pose_traj",
                 &carm::CArmDualBot::move_left_pose_traj,
                 py::arg("target_pos"),
                 py::arg("gripper_pos") = std::vector<double>(),
                 py::arg("stamps") = std::vector<double>(),
                 py::arg("is_sync") = true)
            .def("move_right_pose_traj",
                 &carm::CArmDualBot::move_right_pose_traj,
                 py::arg("target_pos"),
                 py::arg("gripper_pos") = std::vector<double>(),
                 py::arg("stamps") = std::vector<double>(),
                 py::arg("is_sync") = true)
            .def("emergency_stop", &carm::CArmDualBot::emergency_stop)
            .def("set_left_gripper",
                 &carm::CArmDualBot::set_left_gripper,
                 py::arg("pos"),
                 py::arg("tau") = 10)
            .def("set_right_gripper",
                 &carm::CArmDualBot::set_right_gripper,
                 py::arg("pos"),
                 py::arg("tau") = 10)
            .def("set_left_hand",
                 &carm::CArmDualBot::set_left_hand,
                 py::arg("pos"),
                 py::arg("vel") = {},
                 py::arg("tau") = {})
            .def("set_right_hand",
                 &carm::CArmDualBot::set_right_hand,
                 py::arg("pos"),
                 py::arg("vel") = {},
                 py::arg("tau") = {})
            .def("set_speed_level",
                 &carm::CArmDualBot::set_speed_level,
                 py::arg("level"),
                 py::arg("response_level") = 20)
            .def("set_left_tool_index", &carm::CArmDualBot::set_left_tool_index, py::arg("index"))
            .def("set_right_tool_index", &carm::CArmDualBot::set_right_tool_index, py::arg("index"))
            .def("get_left_tool_index", &carm::CArmDualBot::get_left_tool_index)
            .def("get_right_tool_index", &carm::CArmDualBot::get_right_tool_index)
            .def("get_left_tool_coordinate",
                 &carm::CArmDualBot::get_left_tool_coordinate,
                 py::arg("index"))
            .def("get_right_tool_coordinate",
                 &carm::CArmDualBot::get_right_tool_coordinate,
                 py::arg("index"))
            .def("set_collision_config",
                 &carm::CArmDualBot::set_collision_config,
                 py::arg("enable_flag") = true,
                 py::arg("sensitivity_level") = 0)
            .def("trajectory_teach",
                 &carm::CArmDualBot::trajectory_teach,
                 py::arg("off_on"),
                 py::arg("name"))
            .def("trajectory_recorder", &carm::CArmDualBot::trajectory_recorder, py::arg("name"))
            .def("check_teach",
                 [](carm::CArmDualBot& self) {
                     std::vector<std::string> traj_list;
                     int ret = self.check_teach(traj_list);
                     return std::make_tuple(ret, traj_list);
                 })
            .def(
                    "inverse_kine_left_array",
                    [](carm::CArmDualBot& self,
                       int tool_index,
                       const std::vector<std::array<double, 7>>& quat_pose,
                       const std::vector<std::vector<double>>& ref_joint) {
                        std::vector<std::vector<double>> jnt_value;
                        int ret = self.inverse_kine_left_array(
                                tool_index, quat_pose, ref_joint, jnt_value);
                        return std::make_tuple(ret, jnt_value);
                    },
                    py::arg("tool_index"),
                    py::arg("quat_pose"),
                    py::arg("ref_joint"))
            .def(
                    "forward_kine_left_array",
                    [](carm::CArmDualBot& self,
                       int tool_index,
                       const std::vector<std::vector<double>>& jnt_value) {
                        std::vector<std::array<double, 7>> quat_pose;
                        int ret = self.forward_kine_left_array(tool_index, jnt_value, quat_pose);
                        return std::make_tuple(ret, quat_pose);
                    },
                    py::arg("tool_index"),
                    py::arg("jnt_value"))
            .def(
                    "inverse_kine_left",
                    [](carm::CArmDualBot& self,
                       int tool_index,
                       const std::array<double, 7>& quat_pose,
                       const std::vector<double>& ref_joint) {
                        std::vector<double> jnt_value;
                        int ret =
                                self.inverse_kine_left(tool_index, quat_pose, ref_joint, jnt_value);
                        return std::make_tuple(ret, jnt_value);
                    },
                    py::arg("tool_index"),
                    py::arg("quat_pose"),
                    py::arg("ref_joint"))
            .def(
                    "forward_kine_left",
                    [](carm::CArmDualBot& self,
                       int tool_index,
                       const std::vector<double>& jnt_value) {
                        std::array<double, 7> quat_pose;
                        int ret = self.forward_kine_left(tool_index, jnt_value, quat_pose);
                        return std::make_tuple(ret, quat_pose);
                    },
                    py::arg("tool_index"),
                    py::arg("jnt_value"))
            .def(
                    "inverse_kine_right_array",
                    [](carm::CArmDualBot& self,
                       int tool_index,
                       const std::vector<std::array<double, 7>>& quat_pose,
                       const std::vector<std::vector<double>>& ref_joint) {
                        std::vector<std::vector<double>> jnt_value;
                        int ret = self.inverse_kine_right_array(
                                tool_index, quat_pose, ref_joint, jnt_value);
                        return std::make_tuple(ret, jnt_value);
                    },
                    py::arg("tool_index"),
                    py::arg("quat_pose"),
                    py::arg("ref_joint"))
            .def(
                    "forward_kine_right_array",
                    [](carm::CArmDualBot& self,
                       int tool_index,
                       const std::vector<std::vector<double>>& jnt_value) {
                        std::vector<std::array<double, 7>> quat_pose;
                        int ret = self.forward_kine_right_array(tool_index, jnt_value, quat_pose);
                        return std::make_tuple(ret, quat_pose);
                    },
                    py::arg("tool_index"),
                    py::arg("jnt_value"))
            .def(
                    "inverse_kine_right",
                    [](carm::CArmDualBot& self,
                       int tool_index,
                       const std::array<double, 7>& quat_pose,
                       const std::vector<double>& ref_joint) {
                        std::vector<double> jnt_value;
                        int ret = self.inverse_kine_right(
                                tool_index, quat_pose, ref_joint, jnt_value);
                        return std::make_tuple(ret, jnt_value);
                    },
                    py::arg("tool_index"),
                    py::arg("quat_pose"),
                    py::arg("ref_joint"))
            .def(
                    "forward_kine_right",
                    [](carm::CArmDualBot& self,
                       int tool_index,
                       const std::vector<double>& jnt_value) {
                        std::array<double, 7> quat_pose;
                        int ret = self.forward_kine_right(tool_index, jnt_value, quat_pose);
                        return std::make_tuple(ret, quat_pose);
                    },
                    py::arg("tool_index"),
                    py::arg("jnt_value"))
            .def("register_error_cbk",
                 &carm::CArmDualBot::register_error_cbk,
                 py::arg("key"),
                 py::arg("cbk"))
            .def("release_error_cbk", &carm::CArmDualBot::release_error_cbk, py::arg("key"))
            .def("register_completion_cbk",
                 &carm::CArmDualBot::register_completion_cbk,
                 py::arg("key"),
                 py::arg("cbk"))
            .def("release_completion_cbk",
                 &carm::CArmDualBot::release_completion_cbk,
                 py::arg("key"));
}
