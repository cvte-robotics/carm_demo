from setuptools import setup, find_packages
import os
import sys
import platform
import shutil
import subprocess
from pathlib import Path

offline_install = False
if len(sys.argv) > 1:
    if sys.argv[1] in ["--help", "-h"]:
        print("""
    用法: python install_carm.py [选项]
    离线安装只用于DEB包环境下安装。

    选项:
    --offline, -o    离线安装模式
    --help, -h       显示此帮助信息

    示例:
    python install_carm.py          # 在线安装
    python install_carm.py --offline  # 离线安装
    python install_carm.py -o       # 离线安装(简写)
    python install_carm.py --help   # 显示帮助
        """)
        sys.exit(0)
    elif sys.argv[1] in ["--offline", "-o"]:
        offline_install = True
    else:
        print(f"❌ 未知参数: {sys.argv[1]}")
        print("使用 --help 查看帮助")
        sys.exit(1)

"""CARM 安装主函数"""

# 获取平台信息
system = platform.system()
python_version = f"{sys.version_info.major}{sys.version_info.minor}"
machine = platform.machine().lower()
is_64bit = sys.maxsize > 2**32

print(f"平台: {system}")
print(f"Python: {python_version}")
print(f"架构: {machine}")

if system == "Windows":
    # Windows架构映射
    arch_map = {
        'x86_64': 'amd64',
        'amd64': 'amd64',
        'x64': 'amd64',
        'i386': 'win32',
        'i686': 'win32',
        'x86': 'win32',
        'arm64': 'arm64',
        'aarch64': 'arm64'
    }
    
    # 获取标准化架构
    normalized_arch = 'amd64'  # 默认值
    for key, value in arch_map.items():
        if key in machine:
            normalized_arch = value
            break
    
    # Windows平台标签
    platform_tag = f"win_{normalized_arch}" if normalized_arch != "win32" else "win32"
    possible_name = f"carm_py.cp{python_version}-{platform_tag}.pyd"

else:
    # Linux/macOS架构映射
    arch_map = {
        'x86_64': 'x86_64',
        'amd64': 'x86_64',
        'i386': 'i386',
        'i686': 'i386',
        'aarch64': 'aarch64',
        'arm64': 'aarch64'
    }
    
    # 获取标准化架构
    normalized_arch = 'x86_64'  # 默认值
    for key, value in arch_map.items():
        if key in machine:
            normalized_arch = value
            break
    
    # 确定系统标签
    if system == "Linux":
        system_tag = "linux-gnu"
    elif system == "Darwin":
        system_tag = "darwin"
    else:
        system_tag = system.lower()

    possible_name = f"carm_py.cpython-{python_version}-{normalized_arch}-{system_tag}.so"

so_file = None
for pattern in [possible_name, f"lib/so/{possible_name}"]:
    if Path(pattern).exists():
        so_file = Path(pattern)
        break

if not so_file:
    raise FileNotFoundError(f"未找到 CARM 模块文件 (lib/so/{possible_name})")
else:
    print(f"找到源文件: {so_file}")

source_file = None
# 确定文件扩展名和目标文件名
if system == "Windows":
    ext = ".pyd"
    target_name = "carm_py.pyd"
    # Windows 还需要 DLL
    if is_64bit:
        dll_source = "../../lib/x64/Debug"
    else:
        dll_source = "../../lib/x86/Debug"
    source_file = "lib/so/carm_py.pyd"
else:
    ext = ".so"
    target_name = "carm_py.so"
    source_file = "lib/so/carm_py.so"

# 确保 carm 目录存在
carm_dir = Path("lib/carm")
carm_dir.mkdir(exist_ok=True)

# 复制主模块文件
target_file = carm_dir / target_name
shutil.copy2(so_file, target_file)
print(f"复制: {so_file} -> {target_file}")

# 处理 Windows DLL
carm_pkg = [f"carm_py*{ext}"]

if system == "Windows":
    # 复制 DLL 文件
    dll_source_dir = Path(dll_source)
    if dll_source_dir.exists():
        for dll in dll_source_dir.glob("*.dll"):
            target_dll = carm_dir / dll.name
            shutil.copy2(dll, target_dll)
            print(f"复制 DLL: {dll} -> {target_dll}")
        carm_pkg.append("*.dll")
    else:
        print(f"警告: 未找到 DLL 目录: {dll_source}")



"""运行pip安装当前目录"""
# 构建安装命令
if offline_install:
    # 使用家目录下的 packages
    packages_dir = os.path.expanduser("./offline_packages")
    
    # 检查目录是否存在
    if not os.path.exists(packages_dir):
        print(f"❌ 错误：离线包目录 '{packages_dir}' 不存在")
        print("请先运行：python offline_packages.sh")
        sys.exit(1)
    
    # 检查目录是否为空
    if not any(Path(packages_dir).iterdir()):
        print(f"⚠️  警告：离线包目录 '{packages_dir}' 为空")
        print("请先运行：python offline_packages.sh")
    
    # 离线安装命令
    cmd = [
        sys.executable, "-m", "pip", "install",
        "--no-index",
        f"--find-links={packages_dir}",
        "--upgrade",
        "./lib"
    ]
    print(f"📦 离线安装模式（使用 {packages_dir}）")
else:
    # 在线安装命令
    cmd = [sys.executable, "-m", "pip", "install", "./lib"]
    print("🌐 在线安装模式")

print(f"运行: {' '.join(cmd)}")
result = subprocess.run(cmd)
if result.returncode == 0:
    print("✅ 安装成功")
else:
    print("❌ 安装失败")