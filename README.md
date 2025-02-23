<div align=center><img src="ResourceFiles\BasicFiles\Logo.png" width=200 /></div>

## 🔎 这是什么？
- 司天监是一款基于LCU API的助手类工具

## 🚀 快速使用
### 🔨 从源码构建
#### 以下工具为构建必须
- MSVC/LLVM（需要C++20支持）
- Qt 6.5.0+（带OpenSSLv3）
- CMake、Ninja
- LLVM请使用clang-cl编译器
- Windows10 SDK 10.0.18362.0或更新

##### 构建注意事项：
- 需要在CMakeLists.txt文件中设置CMAKE_PREFIX_PATH，否则可能会失败：
```CMake
set(CMAKE_PREFIX_PATH "Qt安装路径/msvc20xx_64") # 示例 C:/Application/Qt/6.8.2/msvc2022_64
```
- LLVM工具链需要先运行以下命令在执行CMake：
```shell
vcvarsall.bat x64
```
- 可以参考或者使用[build.ps1](build.ps1)进行编译：
```PowerShell
$perfix = "Qt安装路径/msvc20xx_64" # 示例 C:/Application/Qt/6.8.2/msvc2022_64
```
- 资源文件从[Data Dragon](https://developer.riotgames.com/docs/lol#data-dragon)下载后使用[script.py](ScriptFiles/script.py)脚本处理：
```python
version = '资源文件版本'
dragon_path = '解压后的资源路径' + version
```
### 💻 下载编译好的二进制文件
#### 下载[Release](https://github.com/Weijcc/ImperialAstronomer/releases)页面的二进制文件
- 需要操作系统Windows10 1903或更新，仅支持64位
- 需要支持AVX2的处理器，如果您的处理器不支持，请删除/arch:AVX2参数后从源码编译
- 工具运行需要管理员权限

### 📄 源码相关
- [build.ps1](build.ps1) - 编译脚本
- [script.py](ScriptFiles/script.py) - 资源处理脚本
- [LeagueAPI.cpp](SourceFiles/LeagueAPI.cpp) - LCU API实现
- 其它文件 - UI相关

### ⚖️ 许可证
- 司天监使用 [GPLv3](https://github.com/Weijcc/ImperialAstronomer/blob/master/LICENSE) 许可证
- 禁止针对代码和二进制文件的商业行为

### 🎊 感谢
- http(s)使用了第三方库[cpp-httplib](https://github.com/yhirose/cpp-httplib)在此表达我的谢意！
- 资源文件下载自[Data Dragon](https://developer.riotgames.com/docs/lol#data-dragon)在此表达我的谢意！
- 应用程序开发框架使用了[Qt](https://www.qt.io/)在此表达我的谢意！
- LCU API参考自[lcu.kebs.dev](https://lcu.kebs.dev/)在此表达我的谢意！
- 使用了JetBrains的字体[JetBrains Mono](https://www.jetbrains.com/lp/mono/)在此表达我的谢意！
- 使用了华为的字体[HarmonyOS Sans](https://developer.huawei.com/consumer/cn/design/resource-V1/)在此表达我的谢意！
- 背景短语引自[番茄](https://fanqienovel.com/)网络小说[十日终焉-杀虫队队员](https://fanqienovel.com/page/7143038691944959011)在此表达我的谢意！

### ⭐ 点个Star
- 非常感谢！