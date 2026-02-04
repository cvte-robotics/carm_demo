#!/usr/bin/env python3
"""
Python 编译脚本 - 简化版本
"""

import os
import sys
import shutil
import subprocess
import argparse
import platform
import multiprocessing
from pathlib import Path

cfg = "Debug"
# 设置环境变量，强制使用UTF-8编码
env = os.environ.copy()
env['PYTHONIOENCODING'] = 'utf-8'
env['PYTHONUTF8'] = '1'

def run_command(cmd, cwd=None, check=True):
    """运行命令并处理输出"""
    print(f"执行命令: {' '.join(cmd)}")
    try:
        result = subprocess.run(
            cmd,
            cwd=cwd,
            capture_output=True,
            text=True,
            check=check,
            encoding='utf-8',
            errors='replace'  # 或使用 'ignore'
        )
        return True, result.stdout, result.stderr
    except subprocess.CalledProcessError as e:
        return False, e.stdout, e.stderr

def main():
    # 获取平台信息
    system = platform.system()
    python_version = f"{sys.version_info.major}.{sys.version_info.minor}"
    machine = platform.machine().lower()
    is_64bit = sys.maxsize > 2**32

    print("\n" + "=" * 60)
    print(f"平台: {system}")
    print(f"Python: {python_version}")
    print(f"架构: {machine}")
    print("=" * 60)
    
    # 准备build目录
    build_dir = Path("./src/build")
    build_dir.mkdir(exist_ok=True, parents=True)
    print(f"使用build目录: {build_dir.absolute()}")
    
    # 清理目录
    print("清理build目录...")
    for item in build_dir.iterdir():
        if item.is_dir():
            shutil.rmtree(item)
        else:
            item.unlink()
    
    # 获取CPU核心数
    cpu_cores = multiprocessing.cpu_count() or 4
    print(f"将使用 {cpu_cores} 个CPU核心进行并行编译")
    
    # 切换到build目录
    original_dir = os.getcwd()
    os.chdir(build_dir)
    
    if system == "Windows" :
        if is_64bit:
            cmake_configure = [
                "cmake",
                "-A",
                "x64",
                f"-DCMAKE_BUILD_TYPE={cfg}",
                f"-DPYTHON_VERSION={python_version}",
                ".."
            ]
        else:
            cmake_configure = [
                "cmake",
                "-A",
                "Win32",
                f"-DCMAKE_BUILD_TYPE={cfg}",
                f"-DPYTHON_VERSION={python_version}",
                ".."
            ]
    else:
        cmake_configure = [
            "cmake",
            f"-DCMAKE_BUILD_TYPE={cfg}",
            f"-DPYTHON_VERSION={python_version}",
            ".."
        ]
    
    success, stdout, stderr = run_command(cmake_configure)
    if not success:
        print("❌ cmake配置失败")
        os.chdir(original_dir)
        return 1
    
    # 运行cmake编译
    print(f"\n开始编译，使用 {cpu_cores} 个并行任务...")
    cmake_build = [
        "cmake",
        "--build", ".",
        "--config", f"{cfg}",
        "--parallel", str(cpu_cores)
    ]
    
    success, stdout, stderr = run_command(cmake_build)
    if success:
        print(f"✅ Python {python_version} 编译成功!")
        
        # 内部安装
        print("\n运行初始化命令...")
        cmake_install = [
            "cmake",
            "--build", ".",
            "--target", "install",
            "--config", f"{cfg}"
        ]
        
        success, stdout, stderr = run_command(cmake_install)
        if success:
            print(f"✅ Python {python_version} 初始化成功!")
        else:
            print(f"❌ Python {python_version} 初始化失败!")
    else:
        print(f"❌ Python {python_version} 编译失败!")
    
    # 返回原始目录
    os.chdir(original_dir)
    print(f"\n完成 {python_version} 的处理")
    return 0

if __name__ == "__main__":
    if len(sys.argv) > 1:
        if sys.argv[1] in ["--help", "-h"]:
            print("""
        用法: python build_carm.py [选项]
        选项:
        --Release, -r
        --Debug, -d
        --help, -h
            """)
            sys.exit(0)
        elif sys.argv[1] in ["--Release", "-r"]:
            cfg = "Release"
        elif sys.argv[1] in ["--Debug", "-d"]:
            cfg = "Debug"
        else:
            print(f"❌ 未知参数: {sys.argv[1]}")
            print("使用 --help 查看帮助")
            sys.exit(1)

    sys.exit(main())