import os
import re
import sys
import platform
import subprocess
import shutil
import socket
import sysconfig
from pathlib import Path
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

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

def get_possible_name():
    ext_suffix = sysconfig.get_config_var('EXT_SUFFIX')
    if not ext_suffix:
        # 兼容极老版本 Python
        ext_suffix = '.so' if platform.system() != 'Windows' else '.pyd'
    return f"carm_py{ext_suffix}"

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir) + "/src"


class CMakeBuild(build_ext):
    def run(self):
        possible_name = get_possible_name()
        possible_dir = Path(__file__).parent / "carm_py" / possible_name
        print("寻找： ", possible_dir)
        so_file = None
        if possible_dir.exists():
            so_file = possible_dir

        if not so_file:
            print(f"未找到源文件开始编译")
            try:
                out = subprocess.check_output(['cmake', '--version'])
            except OSError:
                raise RuntimeError("CMake must be installed to build the following extensions: " +
                                ", ".join(e.name for e in self.extensions))

            if platform.system() == "Windows":
                cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
                if cmake_version < '3.1.0':
                    raise RuntimeError("CMake >= 3.1.0 is required on Windows")

            carm_py_dir = Path(__file__).parent 
            carm_py_dir = carm_py_dir / "carm_py"
            for item in carm_py_dir.iterdir():
                if item.name == '__init__.py' or item.name == 'carm.py':
                    continue  # 跳过保留文件
                if item.is_dir():
                    shutil.rmtree(item)
                else:  # 是文件且不是 __init__.py
                    item.unlink()

            for ext in self.extensions:
                self.build_extension(ext)
                self.install_source()
                self.install_extension(ext)
            
        else:
            print(f"找到源文件跳过编译")
            for ext in self.extensions:
                self.install_source()
                self.install_extension(ext)
            

        # 构建完成后，手动重新运行 build_py
        print("\n🔄 重新运行 build_py 以复制新生成的文件...")
        
        # 方法一：通过 distutils 命令系统重新运行
        build_py_cmd = self.distribution.get_command_obj('build_py')
        build_py_cmd.ensure_finalized()
        build_py_cmd.run()
        
        # 或者方法二：直接调用 run_command
        # self.run_command('build_py')
        
        print("✅ build_py 重新运行完成")

    def install_source(self):
        lib_source = None
        # 确定文件扩展名和目标文件名
        if platform.system() == "Windows":
            # Windows 还需要 DLL
            if sys.maxsize > 2**32:
                lib_source = [f"../lib/x64/{cfg}/"]
            else:
                lib_source = [f"../lib/x86/{cfg}/"]
        else:
            lib_source = ["../lib/", "../poco/lib/"]

        # 确保 carm 目录存在
        for ls in lib_source:
            base_dir = Path(__file__).parent          # setup.py 所在目录
            lib_dir = base_dir / ls
            carm_dir = base_dir / "carm_py"
            carm_dir.mkdir(parents=True, exist_ok=True)

            if lib_dir.is_dir():
                # 递归复制整个目录到 target_dir/目录名
                shutil.copytree(lib_dir, carm_dir, dirs_exist_ok=True)
            elif lib_dir.is_file():
                shutil.copy2(lib_dir, carm_dir)
            else:
                print(f"⚠️ {lib_dir} 不存在或不是文件/目录")

    def install_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        print("目标安装路径", extdir)
        so_dir = Path(ext.sourcedir + "/../carm_py/")          # setup.py 所在目录
        extdir_dir = Path(extdir + "/carm_py/")          # setup.py 所在目录
        if so_dir.is_dir():
            # 递归复制整个目录到 target_dir/目录名
            shutil.copytree(so_dir, extdir_dir, dirs_exist_ok=True)
        elif so_dir.is_file():
            shutil.copy2(so_dir, extdir_dir)
        else:
            print(f"⚠️ {so_dir} 不存在或不是文件/目录")

    def build_extension(self, ext):
        # extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
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
        org_dir = Path(ext.sourcedir + "/../")
        build_dir = Path(ext.sourcedir + "/build")
        build_dir.mkdir(exist_ok=True, parents=True)
        print(f"使用build目录: {build_dir.absolute()}")
        for item in build_dir.iterdir():
            if item.is_dir():
                shutil.rmtree(item)
            else:
                item.unlink()
        
        # 切换到build目录
        os.chdir(build_dir)
        cmake_args = [f"-DCMAKE_BUILD_TYPE={cfg}",
                      f"-DPYTHON_VERSION={python_version}",
                      '-DCMAKE_POLICY_VERSION_MINIMUM=3.5'
                    ]
        
        if system == "Windows" :
            if is_64bit:
                cmake_configure = [
                    "cmake",
                    "-A",
                    "x64",
                ] + cmake_args + [".."]
            else:
                cmake_configure = [
                    "cmake",
                    "-A",
                    "Win32",
                ] + cmake_args + [".."]
        else:
            cmake_configure = [
                "cmake",
            ] + cmake_args + [".."]
        
        success, stdout, stderr = run_command(cmake_configure)
        if not success:
            print("❌ cmake配置失败")
            if stderr:
                print("错误信息:", stderr)
            os.chdir(org_dir)
            return 1
        
        # 运行cmake编译
        print(f"\n开始编译...")
        cmake_build = [
            "cmake",
            "--build", ".",
            "--config", f"{cfg}"
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
        os.chdir(org_dir)
        print(f"\n完成 {ext.name} 的处理")

setup(
    ext_modules=[CMakeExtension('carm_py')],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
    include_package_data=True,
)