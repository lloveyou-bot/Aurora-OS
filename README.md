##### 🌌 Aurora OS

> 从零开始构建的 x86 操作系统 —— 让极光照亮底层世界


## 📖 项目简介

Aurora OS 是一个从零开始、手把手构建的 x86 架构操作系统。项目灵感来源于《30天自制操作系统》和开源社区 OS 开发经验，旨在通过实践深入理解计算机启动、内存管理、任务调度、中断处理等核心原理。

> "Aurora" 意为极光，寓意在底层世界探索中绽放的光芒 ✨


## ✨ 功能特性

### 🔥 核心功能
- 自定义 BIOS 引导加载器
- 32位保护模式切换
- C 语言内核
- 键盘中断驱动
- Shell 命令行终端
- 虚拟文件系统
- 15+ 内置命令
- 文本文件管理器
- VESA 图形模式支持

### 📦 系统架构
- x86 32位架构
- 实模式 → 保护模式切换
- GDT/IDT 中断管理
- PIC 可编程中断控制器
- VGA 文本显示 (80x25)
- VESA 图形模式 (1024x768x32)

### 🛠️ 支持的命令

| 命令 | 功能 |
|------|------|
| `/help` | 显示帮助 |
| `/clear` | 清屏 |
| `/echo` | 打印文字 |
| `/ls` | 列出文件 |
| `/cat` | 查看文件 |
| `/mkdir` | 创建目录 |
| `/touch` | 创建文件 |
| `/rm` | 删除文件 |
| `/pwd` | 显示路径 |
| `/cd` | 切换目录 |
| `/meminfo` | 内存信息 |
| `/reboot` | 重启系统 |
| `/version` | 版本信息 |
| `/explorer` | 文件管理器 |
| `/desktop` | 图形桌面 |


## 🔧 开发环境

| 工具 | 用途 |
|------|------|
| **NASM** | x86 汇编器 |
| **i686-elf-gcc** | C 交叉编译器 |
| **i686-elf-ld** | 链接器 |
| **QEMU** | 模拟器 |
| **PowerShell** | 构建脚本 |


## 🚀 快速开始

## 如何使用？
1.克隆仓库（大家应该都会叭）
2. 安装依赖
NASM：nasm.us

i686-elf-gcc：lordmilko/i686-elf-tools

QEMU：qemu.org

3. 构建系统
powershell
.\boot\build.ps1
4. 运行系统
powershell
## 文本模式
qemu-system-x86_64 -m 32 -drive file=aurora.img,format=raw,if=floppy -boot a

## 图形模式
qemu-system-x86_64 -m 64 -vga std -drive file=aurora.img,format=raw,if=floppy -boot a
## 📄 许可证
本项目采用 MIT 许可证，详情见 LICENSE 文件。

## 🙏 致谢
《30天自制操作系统》（川合秀实）

OSDev Wiki

rCore 开源操作系统训练营
