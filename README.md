# ESP32-S3 桌面语音交互终端

<p align="center">
  <strong>可以离线唤醒、自然对话、表情反馈和语音控制的桌面 AI 终端</strong>
</p>

<p align="center">
  <a href="#项目简介">项目简介</a> ·
  <a href="#项目功能">项目功能</a> ·
  <a href="#项目优化">项目优化</a> ·
  <a href="#系统结构">系统结构</a> ·
  <a href="#硬件规格">硬件规格</a> ·
  <a href="#硬件设计图">硬件设计图</a> ·
  <a href="#后续优化方向">后续优化方向</a> ·
  <a href="#软件与固件">软件与固件</a> ·
  <a href="#版本记录">版本记录</a>
</p>

## 项目简介

当前版本：`v1.0.0` · 最近更新：`2026-09-12`

本项目基于 **ESP32-S3-WROOM-1** 开发，已完成硬件设计与固件移植，
成功跑通语音对话、唤醒词唤醒、表情动画显示、语音播报等功能。

喊一声「你好小智」或按一下按键就能和它说话。它会把你说的话转成文字
显示在屏幕上，用表情回应你，再用语音把答案讲出来。

硬件配 1.54 寸屏幕、ES8311 编解码器和 NS4150B 功放。
仓库公开原理图整图、PCB 布局照片、BOM、Gerber、固件工程和调试记录；
可编辑的 EasyEDA 工程源文件保留在本地。

### 项目状态

| 模块 | 状态 | 说明 |
| --- | --- | --- |
| 硬件设计 | 已完成 | 原理图、PCB、Gerber、BOM 和贴片坐标已整理 |
| 音频链路 | 已验证 | ES8311、NS4150B、麦克风和喇叭工作正常 |
| 显示界面 | 已验证 | ST7789 稳定显示表情动画和对话文字 |
| 语音交互 | 已验证 | 离线唤醒、语音识别、回答播报可完整运行 |
| 资源升级 | 已验证 | 支持网页主题包在线升级和中断后重试 |

### 项目速览

| 项目 | 当前情况 |
| --- | --- |
| 制作难度 | 中等，核心链路已验证，PCB 为双层、单面贴装 |
| 核心链路 | WiFi + I2S 音频 + SPI 显示 + MCP 设备控制 |
| 软件环境 | ESP-IDF v6.1，上游固件版本 2.5.0 |
| 预计物料成本 | 约 60–70 元，不含 PCB 打样、运费和工具 |
| 当前版本 | `v1.0.0` |
| 下一步重点 | 电池与便携供电、标准扩展接口、传感器和低功耗 |
| 文档状态 | 原理图、PCB 布局、BOM、接线和调试记录均已整理 |

### 实物展示

<table align="center">
  <tr>
    <td align="center">
      <a href="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/pcb_3d_iso.png">
        <img src="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/pcb_3d_iso.png" width="360" alt="整机 3D 渲染">
      </a>
      <br>
      <sub>整机 3D 渲染</sub>
    </td>
    <td align="center">
      <a href="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/device_photo.jpg">
        <img src="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/device_photo.jpg" width="260" alt="设备实拍">
      </a>
      <br>
      <sub>设备实拍</sub>
    </td>
  </tr>
</table>

## 项目功能

- **语音对话**：唤醒词或按键进入对话，说完自动识别并回答
- **屏幕反馈**：显示表情动画和对话文字
- **免按键唤醒**：离线唤醒词识别
- **语音播报**：回答通过喇叭实时播放
- **语音控制**：可以直接说「声音小一点」「屏幕调亮些」来控制设备
- **配网简单**：手机连上设备热点即可配置 WiFi
- **双 USB-C**：一个下载调试，一个原生 USB

## 项目优化

项目在模块方案基础上，重点完成了电源、接口、信号完整性和升级可靠性设计：

- **双 USB-C 架构**：下载口和原生 USB 分开，调试、供电和设备通信互不影响。
- **数字与音频供电分区**：BUCK 3.3V 供主控和屏幕，LDO 3.3V 从 USB 5V 独立
  降压供音频链路，避免开关电源噪声通过电源轨干扰 ES8311 和麦克风。
- **USB 信号完整性**：两条 USB 通道的 D+/D- 等宽等距，实测两对长度均为
  64.6 mm，长度差为 0，降低高速信号反射和耦合风险。
- **显示稳定性优化**：将 ST7789 的 SPI 时钟从 80 MHz 降到 40 MHz，解决长排线
  和杜邦线环境下出现的屏幕闪烁。
- **可靠的资源 OTA**：修复主题包下载中断后的恢复逻辑，保留下载地址、允许修复
  损坏分区，并在下载前批量擦除，避免 6 MB 级资源包因逐扇区擦除超时。
- **硬件与固件统一**：自定义 xiaozhi-esp32 板级配置，引脚、供电和原理图保持一致，
  同时保留 Gerber、BOM、贴片坐标和调试记录。
- **ESP-IDF 6.1 适配**：板级代码在 ESP-IDF v6.1 下编译运行，并适配 MCP、屏幕表情、
  音频链路和资源 OTA。

## 系统结构

```
                  ┌──────────────────────────────┐
                  │      ESP32-S3-WROOM-1        │
                  │   (16MB Flash / 8MB PSRAM)   │
  麦克风 ──I2S──▶ │                              │ ──SPI──▶ 1.54" 屏幕
  喇叭 ◀──I2S──   │        WiFi 2.4G             │ ──I2C──▶ ES8311 配置
                  └──────────────┬───────────────┘
                                  │
                          USB 5V ──┬── BUCK 3.3V（数字供电）
                                   └── LDO  3.3V（音频独立供电）
```

数字和音频各有一路 3.3V：BUCK 给 MCU、USB、屏幕供电；音频链路直接由
USB 5V 经 LDO 降压供电。两路电源彼此独立，避免 BUCK 的开关噪声通过
电源轨传进音频通路。

## 硬件规格

| 项目 | 参数 |
| --- | --- |
| 主控 | ESP32-S3-WROOM-1（16MB Flash / 8MB PSRAM） |
| 音频 | ES8311 编解码器 + NS4150B 功放，板载 MEMS 麦克风 |
| 显示 | 1.54 寸 ST7789，240×240 |
| 交互 | 唤醒词 + BOOT 按键 |
| USB | 双 Type-C：CH340 下载口 + 原生 USB |
| 电源 | USB 5V 输入保护 → BUCK 3.3V（数字）/ LDO 3.3V（音频），两路独立 |
| 板型 | 45 × 65 mm，双层板，器件单面贴装 |
| 预计物料成本 | 约 60–70 元（不含 PCB 打样、运费和工具） |

完整原理图和 PCB 布局图见下。BOM、Gerber 和贴片坐标位于 `Fabrication/`；
可编辑的 EasyEDA 工程源文件不公开。

## 硬件设计图

点击图片可以查看原始分辨率。

<table align="center">
  <tr>
    <td align="center">
      <a href="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/schematic_full.png">
        <img src="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/schematic_full.png" width="540" alt="完整原理图">
      </a>
      <br>
      <sub>完整原理图</sub>
    </td>
    <td align="center">
      <a href="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/pcb_layout.png">
        <img src="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/pcb_layout.png" width="300" alt="PCB 布局">
      </a>
      <br>
      <sub>PCB 布局</sub>
    </td>
  </tr>
</table>

## 硬件设计要点

**USB 差分对等长走线**

两条 USB 通道（CH340 下载口、ESP32 原生 USB）的 D+/D- 全程等宽等距走线，
两对实测长度均为 64.6 mm，长度差为 0。差分对之间保持距离，避免互相耦合。

**DCDC 开关节点紧凑布局**

BUCK 电路的电感紧贴芯片 SW 引脚，输入输出电容就近放置，把开关环路面积
压到最小。开关节点是主要的辐射源，环路越小，EMI 越低。

**模拟与数字供电分域**

BUCK 输出的 3.3V 供 MCU、USB、屏幕使用；音频链路（ES8311 + MEMS 麦克风）
由另一路 LDO 直接从 USB 5V 降压供电，不经过 BUCK。两路电源仅在 5V 输入端
汇合，LDO 在开关频率处有较高的抑制能力，可以减少数字电源纹波对音频通路的
干扰。

**GND 平面与回流路径**

顶层和底层各铺一块完整的 GND 平面。去耦电容就近打地过孔，缩短回流路径；
模拟音频区域下方保持地平面完整，不被数字信号切割。

**接口与连接器布局**

两个 Type-C 落在板子下边缘，方便插拔；LCD 排针紧贴主控下方，缩短 SPI 走线；
喇叭接口位于板边。天线净空区域保持无铜、无器件。

**上拉与保护**

I2C 总线配置上拉电阻；USB 入口经过保险丝、防静电管和防反灌二极管；
BOOT 与 RESET 按键配滤波电容，避免抖动误触发。

## 实测数据

| 项目 | 实测结果 |
| --- | --- |
| ST7789 SPI | 40 MHz 运行，80 MHz 下出现的偶发闪烁已消除 |
| USB 差分线 | 两对 D+/D- 均为 64.6 mm，长度差为 0 |
| ES8311 I2C | 100 kHz 标准速率下稳定通信 |
| 固件镜像 | 2,755,824 字节，应用分区剩余约 33% |
| 主题资源包 | 6,237,490 字节完整升级成功 |
| 资源升级耗时 | 约 111 秒，包含擦除、下载、校验和重新加载 |
| 唤醒与对话 | 唤醒词、麦克风、屏幕反馈和喇叭播报均已验证 |

## 后续优化方向

下一阶段会把设备从“桌面供电终端”扩展成可移动、可接传感器、可做机器人底座的
通用语音终端。完整任务拆分见 [ROADMAP.md](ROADMAP.md)。

| 方向 | 计划内容 | 目标 |
| --- | --- | --- |
| 电池与便携供电 | 单节锂电池、充电管理、保护、电量计和电源路径切换 | 断 USB 后持续运行，支持边充边用 |
| 电池信息交互 | I2C 电量计、低电量提醒、充电状态显示和语音播报 | 屏幕和语音都能看到剩余电量 |
| 扩展接口 | I2C、UART、GPIO 和标准电源扩展口 | 无需飞线即可接传感器和执行器 |
| 传感器扩展 | 温湿度、光照、人体存在、IMU 等模块 | 增加环境感知和状态联动 |
| 运动控制 | 舵机、云台或小车底盘接口 | 从桌面终端升级为机器人控制终端 |
| 音频升级 | 双麦克风阵列、回声消除和功放保护 | 提升远场拾音和免提对话体验 |
| 存储与低功耗 | microSD、资源缓存、休眠和唤醒策略 | 支持离线内容和更长时间待机 |
| 硬件迭代 | 四层板、EMC 优化、结构外壳和测试点 | 提高稳定性并形成完整产品外观 |

## 软件与固件

### 上游来源

软件基于 GitHub 开源项目 [78/xiaozhi-esp32](https://github.com/78/xiaozhi-esp32)（MIT）
编译和适配，当前上游固件版本为 `2.5.0`。本项目新增了
`main/boards/esp32s3-terminal/` 板级配置，并在 `main/Kconfig.projbuild` 和
`main/CMakeLists.txt` 中完成注册。

这不是 Arduino 工程，而是 ESP-IDF 的 CMake/Ninja 工程。板级文件位于：

```text
Firmware/xiaozhi-esp32/main/boards/esp32s3-terminal/
├── config.h
├── config.json
└── esp32s3_terminal_board.cc
```

### 软件架构

```text
语音唤醒 / 对话 / MCP 设备控制
            │
       Application
            │
  AudioService + LVGL + OTA
            │
ESP-IDF + FreeRTOS + WiFi + TLS
            │
ESP32-S3-WROOM-1-N16R8
```

固件通过 MCP 工具把屏幕、音量等能力开放给大模型，可以直接用语音控制设备，
例如「声音小一点」「屏幕调亮一些」。

### 开发环境

以下环境为本项目实际编译、烧录和运行验证环境：

| 工具 | 版本 |
| --- | --- |
| ESP-IDF | v6.1 |
| 目标芯片 | ESP32-S3 |
| Python | 3.12.10 |
| Xtensa GCC | 15.2.0 |
| CMake | 4.0.3 |
| Ninja | 1.12.1 |
| 固件版本 | 2.5.0 |

板级适配细节和接线记录见 [`Firmware/README.md`](Firmware/README.md)。

### 编译烧录

安装 ESP-IDF v6.1 后，在工程目录执行：

```bash
cd Firmware/xiaozhi-esp32
idf.py set-target esp32s3
idf.py menuconfig     # Board Type 选 ESP32-S3 Terminal
idf.py build
idf.py -p COMx flash monitor
```

首次编译会通过组件管理器自动下载依赖。也可以直接使用
[v1.0.0 Release](https://github.com/Ccmra404/esp32s3-multifunction-terminal/releases/tag/v1.0.0)
中的固件 bin。

## 快速复刻

1. 从 `Fabrication/` 获取 Gerber、BOM、贴片坐标和板厂参数。
2. 按 BOM 完成 PCB 贴装，先检查 5V、BUCK 3.3V 和 LDO 3.3V 是否短路。
3. 连接 ST7789 屏幕和 ES8311/NS4150B 音频模块，麦克风、I2C、时钟等关键连接
   尽量控制在 5 cm 内，接线参考 [`Firmware/README.md`](Firmware/README.md)。
4. 可直接使用 [v1.0.0 Release](https://github.com/Ccmra404/esp32s3-multifunction-terminal/releases/tag/v1.0.0)
   中的固件 bin，或安装 ESP-IDF 后自行编译烧录。
5. 首次上电测试 WiFi、屏幕、麦克风、喇叭和唤醒词，再进行资源包 OTA。

上电检查建议：

| 检查项 | 预期结果 |
| --- | --- |
| 5V 输入 | 电压稳定，保险丝和防反灌器件无异常发热 |
| 3.3V 电源 | 数字与音频两路电压正常，上电无短路 |
| I2C | 能识别 ES8311 地址 `0x18` |
| 屏幕 | 无闪烁、无花屏，方向和颜色正确 |
| 音频 | 喇叭有提示音，麦克风可被唤醒词触发 |
| 网络 | 能进入配网、连接 WiFi 并完成设备激活 |

## 版本记录

当前版本：`v1.0.0`（2026-09-12）

发布页：[GitHub Release v1.0.0](https://github.com/Ccmra404/esp32s3-multifunction-terminal/releases/tag/v1.0.0)

| 版本 | 日期 | 主要内容 |
| --- | --- | --- |
| `v1.0.0` | 2026-09-12 | 完成硬件设计、固件板级适配、语音对话、显示和主题资源 OTA |

后续功能性、硬件、文档变更会持续记录到 [CHANGELOG.md](CHANGELOG.md)。

## 常见问题

**屏幕闪烁或偶发黑屏**

先检查排线长度和供电，再把 ST7789 SPI 时钟控制在 40 MHz。当前固件已按
稳定优先配置。

**麦克风没有反应，但喇叭正常**

优先检查麦克风和 ES8311 相关走线长度。根据模块资料建议，关键连接尽量控制在
5 cm 内，使用短而固定的线材，避免长杜邦线带来的容性负载和干扰。

**ES8311 偶发无法识别**

检查 I2C 是否被拉高、走线是否过长，并确认总线速率和环境干扰。当前接线在
100 kHz 下已稳定运行。

**主题资源 OTA 失败**

当前固件会保留失败任务的下载地址，并允许覆盖修复损坏分区。升级前确保
`assets` 空间足够，网络过程中不要断开设备电源。

**为什么仓库里没有可编辑的 EasyEDA 工程**

公开仓库只提供原理图和 PCB 展示图；可编辑工程源文件保留在本地，BOM、
Gerber 和贴片坐标正常提供。

## 已知问题

| 现象 | 当前状态 | 说明 |
| --- | --- | --- |
| ES8311 偶发 I2C 写入失败 | 已缓解 | 改善供电和缩短走线后稳定运行，但冷启动仍偶发，需要继续观察 |
| 80 MHz SPI 下屏幕闪烁 | 已修复 | 当前固定为 40 MHz，优先保证显示稳定 |
| 主题资源中断后无法重试 | 已修复 | 新固件保留下载任务，并支持覆盖修复损坏分区 |
| 6 MB 资源包升级耗时较长 | 已优化 | 批量擦除后约 111 秒完成，仍受网络和 Flash 写入速度限制 |
| 电池供电与续航 | 计划中 | 需要等电池电源路径和电量计版本完成后再补实测数据 |

## 维护约定

- 新功能、硬件修改和文档更新先记录到 `CHANGELOG.md` 的 `Unreleased`。
- `ROADMAP.md` 中的任务完成后，再移动到 README 的正式功能或优化章节。
- PCB 或原理图重新设计时，新增对应版本图片和版本记录，不覆盖旧版本。
- 稳定版本使用 Git tag 发布，并记录兼容性和实测结果。

## 仓库内容

| 路径 | 说明 |
| --- | --- |
| `Fabrication/` | Gerber、BOM、贴片坐标、板厂工艺参数 |
| `Firmware/` | 固件工程与板级配置 |
| `Documentation/images/` | 完整原理图、PCB 布局、整机 3D 渲染图与实拍照片 |
| `ROADMAP.md` | 电池、扩展接口、传感器和产品化规划 |
| `CHANGELOG.md` | 项目版本与变更记录 |
| `Documentation/releases/` | 各版本发布说明 |

## 许可证

本仓库采用 [MIT License](LICENSE)。
