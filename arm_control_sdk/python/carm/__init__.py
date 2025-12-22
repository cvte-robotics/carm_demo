"""
CARM 包初始化文件
统一接口，支持 CArmSingleCol 和 CArmDualBot
"""

import os
import sys
import platform
from pathlib import Path

def _get_available_modules():
    """获取当前目录下所有可用的扩展模块文件"""
    package_dir = Path(__file__).parent
    
    # 所有可能的扩展名
    all_extensions = {".so", ".pyd", ".dll", ".dylib"}
    
    available_modules = []
    for file_path in package_dir.glob("*"):
        if file_path.suffix in all_extensions:
            available_modules.append(file_path.name)
    
    return sorted(available_modules)

# 首先尝试直接导入（最标准的方式）
try:
    from .carm_py import CArmSingleCol
    from .carm_py import CArmDualBot
    from .carm_py import ArmConfig
    from .carm_py import ArmStatus
    
    # 导出公共接口
    __all__ = [
        'CArmSingleCol',
        'CArmDualBot', 
        'ArmConfig',
        'ArmStatus'
    ]
    
    # 可选：打印加载成功信息（生产环境可以去掉）
    print(f"CARM 模块加载成功 (Python {sys.version_info.major}.{sys.version_info.minor})")
    
except ImportError as e:
    print(f"错误: CARM 模块加载失败 - {e}")
    
    # 调试信息：列出所有可用的扩展模块文件
    available_modules = _get_available_modules()
    
    if available_modules:
        print("当前目录下可用的扩展模块文件:")
        for module_file in available_modules:
            print(f"  - {module_file}")
        
        # 提供平台特定提示
        system = platform.system()
        if system == "Windows":
            print("\nWindows平台提示:")
            print("  - 确保 .pyd 文件存在且命名正确")
            print("  - 确保 Python 架构与模块匹配（32位/64位）")
            print("  - 确保 环境中存在所需要的.dll文件）")
        elif system == "Linux":
            print("\nLinux平台提示:")
            print("  - 确保 .so 文件存在且命名正确")
            print("  - 检查是否有依赖的共享库缺失")
    else:
        print("未找到任何扩展模块文件 (.so/.pyd/.dll/.dylib)")

    # 重新抛出异常或提供更友好的错误处理
    raise