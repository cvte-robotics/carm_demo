![alt text](picture/logo.png)

# arm_control_sdk软件安装与使用指南

---

## 目录

- [简介](#简介)
- [依赖环境](#库相关依赖)
- [C++开发指南](#C++库声明与使用)
  - [C++库结构](#C++库结构)
  - [如何声明SDK环境](#如何声明sdk环境)
  - [编译C++Demo](#编译C++demo)
  - [运行C++入门例程](#运行C++入门例程)
  - [C++库接口说明](#C++库接口说明)
- [Python 开发指南](#Python库安装与使用)
  - [如何安装卸载环境](#如何安装卸载环境)
  - [Python Demo运行](#PythonDemo运行)
- [ROS 环境支持](#ROS环境的使用)
  - [Python的ROS2快速体验](#Python的ROS2快速体验)
  - [C++的ROS编译和节点运行](#C++的ROS编译和节点运行)
  - [ROSTopic对照表](#ROSTopic对照表)
- [常见问题与技术支持](#常见问题与技术支持)
  - [提问与回答](#Q&A)

## 简介

本功能包是基于与机械臂网络通讯的方式，建立的功能使用接口，包含连接、运动、跟随、设置、获取等多种功能；

详细的产品安装、上位机使用说明和SDK使用说明，以及最新的SDK下载请到如下官方页面下载：

[https://cvte-bot.feishu.cn/wiki/GyGbwKeWMiqEfDk80QXc9zeCnjf](https://cvte-bot.feishu.cn/wiki/GyGbwKeWMiqEfDk80QXc9zeCnjf)

使用接口主要是**carm.CArmSingleCol()**（python接口同名）,是使用单个机械臂套装时使用，在使用双臂套装时候使用**carm.CArmDualBot()**（python接口同名）；python接口是基于pybind对C++的封装。

**环境要求：**

| SDK              | 系统类型                         | 架构类型                        | Python版本                                    |
| ---------------- | -------------------------------- | ------------------------------- | --------------------------------------------- |
| **C++**    | **✅Linux<br />✅Windows** | **✅x86_64<br />✅arm64** | ❌                                            |
| **Python** | **✅Linux<br />✅Windows** | **✅x86_64<br />✅arm64** | **✅Python 3.8, 3.9, 3.10, 3.11, 3.12** |

**推荐环境：**

| 芯片架构 | 操作系统             | Python版本   | ros版本  | ros2版本 |
| -------- | -------------------- | ------------ | -------- | -------- |
| ✅x86_64 | ✅Ubuntu 20.04.6 LTS | ✅Python 3.8 | ✅noetic | ✅foxy   |

**本功能包包括模块：**

+ arm_control_sdk.zip：SDK包，包含C++库和Python库，C++无需安装只需声明，python请参考[Python库安装与使用](##Python库安装与使用)
+ carm_ros：ros1消息包，暴露了部分C++库接口为话题， [详细接口含义见ros话题说明](##ROS环境的使用)
+ carm_ros2：ros2消息包，暴露了部分C++库接口为话题， [详细接口含义见ros话题说明](##ROS环境的使用)
+ cpp_test_demo：C++基础调用示例，基础指令运行测试单元， [详细指令见C++指令说明](##C++库安装与使用)
+ python：基于pybind对C++全接口的暴露，写的py使用示例，详细情况可见python/carm_api_demo.py

## 库相关依赖

arm_control_sdk 依赖分为内部依赖和第三方库依赖, 所有依赖均会打包，并独立存放，无需另外下载，不污染当前依赖环境，有些外部依赖如果用到可以选择性下载

+ 基础依赖（无需下载）
  1. jsoncpp
  2. poco
+ py依赖（选择性下载）
  1. pip
  2. pybind
+ ros依赖（选择性下载）
  1. ros1或者ros2

---

## C++库声明与使用

本功能包以arm_control_sdk.amd64.XXXXXX.zip包的形式安装相关的库(XXXXXX是打包日期)，zip包包括本库的lib，include， python，以及poco依赖。[使用方法见下文](#如何声明sdk环境)

### C++库结构

```plaintext
arm_control_sdk
├── bin
│   └── carm_remote
├── include
│   └── arm_control_sdk
│       ├── carm_cobot.h
│       ├── carm_dual.h
│       ├── data_type_def.h
│       └── gitlog.txt
├── lib
│   ├── cmake
│   │   └── arm_control_sdk
│   │       ├── arm_control_sdkConfig.cmake
│   │       └── arm_control_sdkConfigVersion.cmake
│   ├── libarm_control_sdk.so
│   ├── libcarm_poco_net.so
│   ├── liblocal_com.so
│   ├── libmlog.so
│   ├── libshmem_com.so
│   ├── libtcp_com.so
│   └── libterminal_helper.so
├── poco
│   └── ...
├── python
│   ├── carm
│   │   └── __init__.py
│   ├── install_carm.sh
│   ├── pyproject.toml
│   ├── setup.py
│   └── so
│       ├── carm_py.cpython-310-x86_64-linux-gnu.so
│       ├── carm_py.cpython-311-x86_64-linux-gnu.so
│       ├── carm_py.cpython-312-x86_64-linux-gnu.so
│       ├── carm_py.cpython-38-x86_64-linux-gnu.so
│       └── carm_py.cpython-39-x86_64-linux-gnu.so
└── setup.bash

33 directories, 947 files
```

### 如何声明sdk环境

#### Linux

+ 1、将zip包解压到arm_control_api下，覆盖旧的库
  ```plaintext
  # 来到本工程目录
  cd arm_control_api/
  # 解压C++库
  unzip arm_control_sdk.amd64.XXXXXX.zip
  ```
+ 2、声明环境变量
  ```
  source arm_control_sdk/setup.bash
  ```
+ 3、在自己的工程中声明库（声明的库需要在环境变量声明后才会起效）
  ```
  find_package(arm_control_sdk REQUIRED)             	# 声明sdk库
  add_executable(your_project_note your_project.cpp)   	# 添加你的编译选项
  include_directories(${arm_control_sdk_INCLUDE_DIRS})   	# 添加全局头文件声明
  target_link_libraries(your_project_note PRIVATE ${arm_control_sdk_LIBRARIES})	#添加局部连接库
  ```

#### Windows

Windows 下为预先编译好的库，Windows 下编译配置为 Debug，X64，X86，位于代码仓库 `lib/Windows/`下。可参考如下步骤将 SDK 加入项目的依赖项。

1. 使用 Visual Studio 创建 Debug X64 C++程序
2. 选择项目属性
3. 选择 `C++目录`
4. 设置 `头文件目录`和 `外部库目录`分别为**arm_control_sdk**下的 `include`和 `lib/windows/x64/Debug`

### 编译C++Demo

C++的使用用例在arm_control_api/cpp_test_demo中，下面是主要功能和用法介绍

+ 该例程主要通过简化的界面输入指令，来执行一些示例的函数映射表可见于test_carm_api.cpp最后。
+ 基础指令用于快速测试机器人的接口性能，能够快速利用api搭建测试场景。
+ 使用方法：先输入指令然后回车，按照得到的提示来输入参数再回车，部分接口已经有参数，可以手动更改。
+ 实现自己的函数是可仿照该接口调用形式。

编译例程：

```plaintext
cd ./cpp_test_demo
mkdir build && cd build
cmake ..
make
```

### 运行C++入门例程

运行:

> ./carm_demo

连接:

> c

复位:

> r

切换为MIT模式（可选）：

> cm

设置较慢的速度（可选）：

> le

运动到零位:

> mh

运动到指定关节点位:

> mj

查看运动后机器的关节位置和位姿：

> p

恢复的速度（可选）：

> le

运动到零位:

> mh

运动到指定笛卡尔点位:

> mp

循环测试:

> ct

打开关闭夹爪

> sgg

运行示例的PVT运动轨迹

> pvtj

### C++库接口说明

+ C++库包含所有功能指令，详细的函数定义见arm_control_sdk/include/carm_cobot.h，双臂请见arm_control_sdk/include/carm_dual.h，结构体定义请见arm_control_sdk/include/data_type_def.h
+ 本机械臂基础分为连接管理、基础控制、运动指令、状态获取、末端执行器、运动控制、配置设置、运动学、示教功能，以及回调注册函数。
+ 具体的输入输出接口参数含义请参考arm_control_sdk/include/arm_control_sdk的头文件中的注释

| 单臂接口 (carm_cobot.h)                                                             | 双臂接口 (carm_dual.h)                                                                                                                                                                                                 | 接口作用简述                |
| ----------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------- |
| **连接管理**                                                                  |                                                                                                                                                                                                                        |                             |
| ✅ CArmSingleCol                                                                    | ✅ CArmDualBot                                                                                                                                                                                                         | 创建机械臂控制对象          |
| ✅ connect()                                                                        | ✅ connect()                                                                                                                                                                                                           | 连接到机械臂控制器          |
| ✅ disconnect()                                                                     | ✅ disconnect()                                                                                                                                                                                                        | 断开与控制器连接            |
| ✅ is_connected()                                                                   | ✅ is_connected()                                                                                                                                                                                                      | 检查连接状态                |
| **基础控制**                                                                  |                                                                                                                                                                                                                        |                             |
| ✅ set_ready()                                                                      | ✅ set_ready()                                                                                                                                                                                                         | 控制器复位准备              |
| ✅ set_servo_enable()                                                               | ✅ set_servo_enable()                                                                                                                                                                                                  | 伺服使能控制                |
| ✅ set_control_mode()                                                               | ✅ set_control_mode()                                                                                                                                                                                                  | 设置控制模式                |
| ✅ emergency_stop()                                                                 | ✅ emergency_stop()                                                                                                                                                                                                    | 紧急停止                    |
| **状态获取**                                                                  |                                                                                                                                                                                                                        |                             |
| ✅ get_version()                                                                    | ✅ get_version()                                                                                                                                                                                                       | 获取控制器版本              |
| ✅ get_config()                                                                     | ✅ get_left_config()/✅ get_right_config()                                                                                                                                                                             | 获取机械臂配置参数          |
| ✅ get_status()                                                                     | ✅ get_left_status()/✅ get_right_status()                                                                                                                                                                             | 获取机械臂状态              |
| ✅ get_joint_pos()/vel()/tau()                                                      | ✅ get_left_joint_pos()/vel()/tau()/<br />✅ get_right_joint_pos()/vel()/tau()                                                                                                                                         | 获取关节位置/速度/力矩      |
| ✅ get_cart_pose()                                                                  | ✅ get_left_cart_pose()/<br />✅ get_right_cart_pose()                                                                                                                                                                 | 获取末端位姿                |
| ✅ get_joint_external_tau()                                                         | ✅ get_left_joint_external_tau()/<br />✅ get_right_joint_external_tau()                                                                                                                                               | 获取关节外力矩              |
| ✅ get_cart_external_tau()                                                          | ✅ get_left_cart_external_tau()/<br />✅ get_right_cart_external_tau()                                                                                                                                                 | 获取末端力控外力矩          |
| ✅ get_plan_joint_pos()/vel()/tau()                                                 | ✅ get_left_plan_joint_pos()/vel()/tau()/<br />✅ get_right_plan_joint_pos()/vel()/tau()                                                                                                                               | 获取规划目标值              |
| **末端执行器**                                                                |                                                                                                                                                                                                                        |                             |
| ✅ get_gripper_state()/pos()/vel()/tau()                                            | ✅ get_left_gripper_state()/pos()/vel()/tau()/<br />✅ get_right_gripper_state()/pos()/vel()/tau()                                                                                                                     | 获取夹爪状态/位置/速度/力矩 |
| ✅ set_gripper()                                                                    | ✅ set_left_gripper()/✅ set_right_gripper()                                                                                                                                                                           | 控制夹爪运动                |
| **运动控制**                                                                  |                                                                                                                                                                                                                        |                             |
| ✅ track_joint()/<br />✅ track_pose()                                              | ✅ track_left_joint()/<br />✅ track_right_joint()/<br />✅ track_left_pose()/<br />✅ track_right_pose()                                                                                                              | 实时跟随运动                |
| ✅ move_joint()/<br />✅ move_pose()                                                | ✅ move_left_joint()/<br />✅ move_right_joint()/<br />✅ move_left_pose()/<br />✅ move_right_pose()                                                                                                                  | 关节空间点到点运动          |
| ✅ move_line_joint()/<br />✅ move_line_pose()                                      | ✅ move_left_line_joint()/<br />✅ move_right_line_joint()/<br />✅ move_left_line_pose()/<br />✅ move_right_line_pose()                                                                                              | 直线运动                    |
| ✅ move_joint_traj()/<br />✅ move_pose_traj()                                      | ✅ move_left_joint_traj()/<br />✅ move_right_joint_traj()/<br />✅ move_left_pose_traj()/<br />✅ move_right_pose_traj()                                                                                              | 轨迹运动<br />时间最优规划  |
| **配置设置**                                                                  |                                                                                                                                                                                                                        |                             |
| ✅ set_speed_level()                                                                | ✅ set_speed_level()                                                                                                                                                                                                   | 设置速度等级                |
| ✅ set_tool_index()                                                                 | ✅ set_left_tool_index()/<br />✅ set_right_tool_index()                                                                                                                                                               | 设置工具号                  |
| ✅ get_tool_index()                                                                 | ✅ get_left_tool_index()/<br />✅ get_right_tool_index()                                                                                                                                                               | 获取工具号                  |
| ✅ get_tool_coordinate()                                                            | ✅ get_left_tool_coordinate()/<br />✅ get_right_tool_coordinate()                                                                                                                                                     | 获取工具坐标系              |
| ✅ set_collision_config()                                                           | ✅ set_collision_config()                                                                                                                                                                                              | 设置碰撞检测                |
| **运动学**                                                                    |                                                                                                                                                                                                                        |                             |
| ✅ inverse_kine()/forward_kine()/<br />✅ inverse_kine_array()/forward_kine_array() | ✅ inverse_kine_left()/forward_kine_left()<br />✅ inverse_kine_right()/forward_kine_right()<br />✅ inverse_kine_left_array()/forward_kine_left_array()<br />✅ inverse_kine_right_array()/forward_kine_right_array() | 正逆运动学计算              |
| **示教功能**                                                                  |                                                                                                                                                                                                                        |                             |
| ✅ trajectory_teach()/<br />✅ trajectory_recorder()/<br />✅ check_teach()         | ✅ trajectory_teach()/<br />✅ trajectory_recorder()/<br />✅ check_teach()                                                                                                                                            | 示教轨迹录制/回放           |
| **回调注册**                                                                  |                                                                                                                                                                                                                        |                             |
| ✅ register_error_cbk()                                                             | ✅ register_error_cbk()                                                                                                                                                                                                | 注册错误                    |
| ✅ register_completion_cbk()                                                        | ✅ register_completion_cbk()                                                                                                                                                                                           | 完成回调                    |
| ✅ register_joint_cbk()                                                             | ✅ register_left_joint_cbk()/<br />✅ register_right_joint_cbk()                                                                                                                                                      | 获取带时间戳的关节数据      |
| ✅ register_pose_cbk()                                                              | ✅ register_left_pose_cbk()/<br />✅ register_right_pose_cbk()                                                                                                                                                         | 获取带时间戳的末端数据      |
| ✅ register_external_force_cbk()                                                    | ✅ register_left_external_force_cbk()/<br />✅ register_right_external_force_cbk()                                                                                                                                     | 获取带时间戳的外力矩数据    |
| ✅ register_plan_joint_cbk()                                                        | ✅ register_left_plan_joint_cbk()/<br />✅ register_right_plan_joint_cbk()                                                                                                                                            | 获取带时间戳的关节控制数据  |

---

## Python库安装与使用

本python功能包基于pybind对C++接口的暴露，使用py接口需要先安装C++环境。python支持且仅支持跟C++所有同名接口，详细的函数定义见C++说明

### 如何安装卸载环境

+ 1、如需要python，到python环境中，并安装，python支持py3.8到py3.10

  ```plaintext
  cd arm_control_api/arm_control_sdk/python
  sudo chmod +x install_carm.py
  python3 install_carm.py
  ```
+ 2、卸载py环境

  ```plaintext
  pip uninstall carm
  ```

### PythonDemo运行

python示例中包括：

+ carm_api_demo.py：所有py接口的调用测试案例。
+ carm_dual_demo.py：关于双臂的使用demo。
+ carm_remote_master.py：基于ros2的简单接口，用于往外发布关节角作为遥操的主臂。
+ carm_remote_slave.py：基于ros2的简单接口，用于接受关节角调用接口作为遥操的从臂。
+ python可以直接运行py系统，声明相应接口后直接使用

  ```plaintext
  python3
  from carm import carm_py
  carm_ = carm_py.CArmSingleCol("10.42.0.101")
  carm_.set_ready()
  carm_.move_joint([0, 0, 0, 0, 0, 0])
  ```
+ carm_api_demo.py文件包含所有对py函数输入输出的调用案例，可以仿照此demo编写你自己的py调用函数。

  ```plaintext
  python3 carm_api_demo.py
  ```

---

## ROS环境的使用

本ros环境基于C++接口暴露部分常用指令为话题，使用ros接口需要先安装C++环境。分别提供了ros1和ros2代码，两个代码实现相同，且ros接口代码完全开放。运行ros需要自行安装基础的ros环境

### Python的ROS2快速体验

+ Python的ros2例程序，提供了简单的遥操体验和录包体验。运行carm_remote_master.py，并录制话题，再开启carm_remote_slave.py播放话题，可通过ros2的方式实现路径的复现与跟踪，不过体验这一历程需要安装rclpy的环境

  + 运行发布话题的一端去录制话题，开始录制后等待机械臂进入拖拽模式，拖拽一条自定义的轨迹，示教完成后关闭录制关闭

  ```plaintext
  # 录制关节消息
  # 窗口1
  python ./python/carm_remote_master.py --model 0

  # 窗口2
  source /opt/ros/foxy/setup.bash 
  ros2 bag record /move_tracking_joint
  Ctrl+C结束录制

  # 或者

  # 录制法兰末端位置消息
  # 窗口1
  python ./python/carm_remote_master.py --model 1

  # 窗口2
  ros2 bag record /move_tracking_pose
  Ctrl+C结束录制
  ```

  + 运行接收话题的一端去复现

  ```plaintext
  # 执行关节消息
  # 窗口1
  python ./python/carm_remote_slave.py

  # 窗口2
  ros2 bag play rosbag2_2025_07_28-10_57_44(修改为自己的包名字)
  ```

### C++的ROS编译和节点运行

+ ros1环境编译和运行

  + 运行ros主节点

  ```plaintext
  # 新开一个窗口1
  cd ./carm_ros
  source /opt/ros/noetic/setup.bash 
  catkin_make
  source devel/setup.bash 

  # 新开一个窗口2
  source /opt/ros/noetic/setup.bash 
  roscore

  # 回到窗口1
  rosrun carm_api carm_ros_node
  ```

  + 发送一个角度移动指令

  ```plaintext
  # 新开一个窗口3
  source /opt/ros/noetic/setup.bash 

  # pub发送目标角度
  rostopic pub /move_joint sensor_msgs/JointState "header:
    seq: 0
    stamp: {secs: 0, nsecs: 0}
    frame_id: ''
  name: ['joint_1', 'joint_2', 'joint_3', 'joint_4', 'joint_5', 'joint_6']
  position: [0, 0, 0, 0, 0, 0]
  velocity: [0, 0, 0, 0, 0, 0]
  effort: [0, 0, 0, 0, 0, 0]" -1
  ```

  + 播放之前录制的关节包模拟跟踪的效果

  ```plaintext
  # 新开一个窗口4，用于打开ros1和2的桥
  sudo apt install ros-foxy-ros1-bridge
  source /opt/ros/foxy/setup.bash 
  ros2 run ros1_bridge dynamic_bridge

  # 新开一个窗口5
  source /opt/ros/foxy/setup.bash 
  ros2 bag play rosbag2_2025_07_28-10_57_44(修改为自己的包名字)
  ```
+ ros2环境编译和运行

  + 运行ros主节点

  ```plaintext
  # 新开一个窗口1
  cd ./carm_ros2
  source /opt/ros/foxy/setup.bash 
  colcon build
  source install/setup.bash 
  ros2 run carm_api carm_ros_node
  ```

  + 发送一个角度移动指令

  ```plaintext
  # 新开一个窗口2
  source /opt/ros/foxy/setup.bash 
  ros2 topic pub /move_joint sensor_msgs/msg/JointState "{header: {stamp: {sec: 0, nanosec: 0}, frame_id: ''}, name: ['joint_1', 'joint_2', 'joint_3', 'joint_4', 'joint_5', 'joint_6'], position: [0.0, 0.0, 0.0, 0.0, 0.0, 0.0], velocity: [0.0, 0.0, 0.0, 0.0, 0.0, 0.0], effort: [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]}"
  ```

  + 播放之前录制的关节包模拟跟踪的效果

  ```plaintext
  # 窗口2
  ros2 bag play rosbag2_2025_07_28-10_57_44(修改为自己的包名字)
  ```

### ROSTopic对照表

| topic                | funtion                 | type                                              | 备注                                       |
| -------------------- | ----------------------- | ------------------------------------------------- | ------------------------------------------ |
| connect              | connect                 | **sub：**(String)IP                         | 连接该IP的机器                             |
| ready                | setReady                | **sub：**(Bool)0                            |                                            |
| emergency_stop       | emergency_stop          | **sub：**(Bool)0                            | 急停后用ready恢复机器                      |
| move_joint           | move_joint              | **sub：**(JointState)目标关节               | 自带规划，关节移动到目标关节               |
| move_pose            | move_pose               | **sub：**(Pose)目标位姿                     | 自带规划，关节移动到目标位姿               |
| move_line_joint      | move_line_joint         | **sub：**(JointState)目标关节               | 自带规划，末端直线移动到目标关节           |
| move_line_pose       | move_line_pose          | **sub：**(Pose)目标位姿                     | 自带规划，末端直线移动到目标位姿           |
| move_tracking_pose   | move_tracking_pose      | **sub：**(JointState)跟踪关节               | 用于高频连续接收关节值，并跟踪该关节值     |
| move_tracking_joint  | move_tracking_joint     | **sub：**(Pose)跟踪末端点                   | 用于高频连续接收末端位姿，并跟踪该末端位姿 |
| set_speed_level      | set_speed_level         | **sub：**(Int16MultiArray)速度等级/切换速度 | 速度等级 0~10 变化速度 20                  |
| set_servo_enable     | set_servo_enable        | **sub：**(Bool)0失能，1使能                 | 失能臂会失去动力下砸                       |
| set_collision_config | set_collision_config    | **sub：**(Int16MultiArray)开关01/碰撞等级   | 碰撞等级 0~3 0最灵敏                       |
| set_gripper          | set_gripper             | **sub：**(JointState)相当于关节             | 夹爪间距范围0-0.08m 扭矩(0-20N)            |
| set_control_mode     | set_control_mode        | **sub：**(Int8)模式                         | 机器控制模式 0~3                           |
| real_joint_state     | jointPublisher          | **pub：**(JointState)当前关节量             | 持续广播当前关节变量                       |
| flange_cart_state    | posePublisher           | **pub：**(PoseStamped)末端法兰位姿          | 持续广播当前末端法兰位姿                   |
| arm_state            | posePublisher           | **pub：**(Int16MultiArray)机器状态          | 持续广播机器状态                           |
| task_completion      | taskCompletionPublisher | **pub：**(string)已完成的任务编号           | 任务完成的时候广播一帧                     |
| carm_error           | errorPublisher          | **pub：**(string)机器报错信息               | 机器报错的时候广播一帧                     |

---

## 常见问题与技术支持

  SDK的使用适合二次开发者，简单使用请用上位机。

### Q&A

 **问：关节移动为什么能输入笛卡尔空间参数？**

  答：关节空间移动，是指规划器在关节空间下的规划，而不是指输入的参数一定是关节的位置变量；所以关节空间移动是指规划器在关节空间下规划，关注关节的移动，不能保证末端沿着直线运动；而笛卡尔空间移动的规划，关注机械臂末端在空间上的移动，可以保证末端的直线移动；两个接口均支持输入关节变量和笛卡尔变量。

 **问：关节和空间变量的数组元素分别是什么？**

  答：关节变量含义：[关节1位置，关节2位置，关节3位置，...], 空间变量：[X轴，Y轴，Z轴，四元素X，四元素Y，四元素Z，四元素W]

 **问：跟随和运动指令有什么区别？**

  答：跟随指令相当于透传指令，当需要给机器频繁发送到点指令，希望机器能跟随你的指令移动，则推荐使用跟随模式；运动指令相当于自带规划的指令，当需要让机器自主规划运动，到达某个位置时，则推荐使用运动指令。

 **问：获取机械臂信息的回调函数和直接获取机械臂信息的函数有什么区别？**

  答：因为获取机械臂的信息时需要添加线程锁来保证访问安全，如果高频使用会影响最新机械臂信息的更新同步；所以希望高频获取机械臂状态或者希望获取对应状态的准确时间戳的时候，建议使用回调的方式；当单次访问时，建议直接获取机械臂信息。
