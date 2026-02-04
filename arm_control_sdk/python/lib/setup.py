from setuptools import setup, find_packages
import os
import platform
import shutil
from pathlib import Path

# 检查 carm 目录是否存在
carm_dir = Path("carm")
if not carm_dir.exists():
    raise FileNotFoundError("⚠️  警告: carm 目录不存在，可能未正确准备文件")

carm_pkg_data = None
system = platform.system()

if system == "Windows": #windows
    carm_pkg_data = ["carm_py*.pyd", "*.dll"]

    # 检查 Windows 文件
    pyd_files = list(carm_dir.glob("carm_py*.pyd"))
    dll_files = list(carm_dir.glob("*.dll"))
    if not pyd_files or not dll_files:
        raise FileNotFoundError("❌ 错误: 未找到 carm_py*.pyd 以及 *.dll 文件，请运行install_carm.py")

else: # linux
    carm_pkg_data = ["carm_py*.so"]

    # 检查 .so 文件
    so_files = list(carm_dir.glob("carm_py*.so"))
    if not so_files:
        raise FileNotFoundError("❌ 错误: 未找到 carm_py*.so 文件，请运行install_carm.py")

setup(
    name="carm",
    version="1.0.260203",
    packages=find_packages(),
    package_data={
        "carm": carm_pkg_data,
    },
    include_package_data=True,
    zip_safe=False,
    install_requires=[],
    python_requires=">=3.6",
    description="ROS2 pybind11 example hybrid package.",
)
