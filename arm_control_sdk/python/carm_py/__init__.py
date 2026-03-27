"""
CARM 包初始化文件
统一接口，支持 CArmSingleCol 和 CArmDualBot
"""

import os
import sys
import platform
import ctypes
import glob
from pathlib import Path

def _get_available_modules():
    """获取当前目录下所有可用的扩展模块文件"""
    package_dir = Path(__file__).parent
    
    # 所有可能的扩展名
    all_extensions = {".so", ".pyd", ".dll", ".dylib", ".py"}
    
    available_modules = []
    for file_path in package_dir.glob("*"):
        if file_path.suffix in all_extensions:
            available_modules.append(file_path.name)
    
    return sorted(available_modules)

def preload_libraries(package_dir=None):
    """简化版跨平台预加载所有依赖库"""
    if package_dir is None:
        package_dir = os.path.dirname(__file__)
    
    print(f"📁 加载库: {package_dir}")
    
    # 平台特定的扩展名
    if sys.platform == 'win32':
        lib_exts = ['*.dll', '*.pyd']
        load_func = ctypes.windll.LoadLibrary
    else:  # Linux/macOS
        lib_exts = ['*.so', '*.dylib']
        load_func = lambda path: ctypes.CDLL(path, mode=ctypes.RTLD_GLOBAL)
    
    # 查找所有库文件
    all_libs = []
    for ext in lib_exts:
        all_libs.extend(glob.glob(os.path.join(package_dir, ext)))
    
    if not all_libs:
        print("⚠️ 未找到库文件")
        return False
    
    print("\n🔄 加载依赖库...")
    # 最大依赖层级10层
    for i in range(10):
        unlocal = []
        for lib in sorted(all_libs):
            try:
                load_func(lib)
                # print(f"  ✅ {os.path.basename(lib)}")
            except Exception as e:
                unlocal.append(lib)
                # print(f"  ⚠️ {os.path.basename(lib)}: {e}")
        if unlocal:
            all_libs = unlocal
        else:
            all_libs = unlocal
            print("✅ 加载依赖库完成")
            break
    
    if all_libs:
        print("⚠️ 加载依赖库失败：", all_libs)
    return not all_libs

# 首先尝试直接导入（最标准的方式）
try:
    preload_libraries()

    from .carm_py import CArmSingleCol
    from .carm_py import CArmDualBot
    from .carm_py import ArmConfig
    from .carm_py import ArmStatus
    from .carm import Carm 
    
    # 导出公共接口
    __all__ = [
        'CArmSingleCol',
        'CArmDualBot', 
        'ArmConfig',
        'ArmStatus',
        'Carm'
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