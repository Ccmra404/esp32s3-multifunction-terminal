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
  <a href="#原理图">原理图</a> ·
  <a href="#pcb-布局">PCB 布局</a> ·
  <a href="#后续优化方向">后续优化方向</a> ·
  <a href="#固件">固件</a> ·
  <a href="#版本记录">版本记录</a>
</p>

## 项目简介

本项目基于 **ESP32-S3-WROOM-1** 开发，已完成硬件设计与固件移植，
成功跑通语音对话、唤醒词唤醒、表情动画显示、语音播报等功能。

喊一声「你好小智」或按一下按键就能和它说话。它会把你说的话转成文字
显示在屏幕上，用表情回应你，再用语音把答案讲出来。

硬件配 1.54 寸屏幕、ES8311 编解码器和 NS4150B 功放。
仓库公开原理图整图、PCB 布局照片、固件工程和调试记录；硬件工程源文件与
制板文件保留在本地。

### 项目状态

| 模块 | 状态 | 说明 |
| --- | --- | --- |
| 硬件设计 | 已完成 | 原理图和 PCB 已完成，展示图已整理 |
| 音频链路 | 已验证 | ES8311、NS4150B、麦克风和喇叭工作正常 |
| 显示界面 | 已验证 | ST7789 稳定显示表情动画和对话文字 |
| 语音交互 | 已验证 | 离线唤醒、语音识别、回答播报可完整运行 |
| 资源升级 | 已验证 | 支持网页主题包在线升级和中断后重试 |

### 实物展示

<p align="center">
  <img src="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/pcb_3d_iso.png" width="560" alt="整机 3D 渲染">
  <br>
  <img src="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/device_photo.jpg" width="420" alt="设备实拍">
</p>

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
  同时保留板级配置和调试记录。

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

完整原理图和 PCB 布局图见下。硬件工程源文件、Gerber、BOM 和贴片坐标不公开。

## 原理图

<p align="center">
  <a href="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/schematic_full.png">
    <img src="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/schematic_full.png" width="1100" alt="完整原理图">
  </a>
</p>

## PCB 布局

<p align="center">
  <a href="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/pcb_layout.png">
    <img src="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/pcb_layout.png" width="600" alt="PCB 布局">
  </a>
</p>

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

## 固件

固件基于开源项目 [78/xiaozhi-esp32](https://github.com/78/xiaozhi-esp32)（MIT），
新增了一套与本板引脚对应的板级配置。引脚定义与搭建记录见
[`Firmware/README.md`](Firmware/README.md)。

固件通过 MCP 工具把硬件能力开放给大模型，可以直接用语音控制设备，
例如「声音小一点」「屏幕调亮一些」。

### 编译烧录

需要 ESP-IDF v5.4 或以上：

```bash
cd Firmware/xiaozhi-esp32
idf.py set-target esp32s3
idf.py menuconfig     # Board Type 选 ESP32-S3 Terminal
idf.py build
idf.py -p COMx flash monitor
```

首次编译会自动下载依赖组件。

## 版本记录

当前版本：`v1.0.0`（2026-09-12）

| 版本 | 日期 | 主要内容 |
| --- | --- | --- |
| `v1.0.0` | 2026-09-12 | 完成硬件设计、固件板级适配、语音对话、显示和主题资源 OTA |

后续功能性、硬件、文档变更会持续记录到 [CHANGELOG.md](CHANGELOG.md)。

## 仓库内容

| 路径 | 说明 |
| --- | --- |
| `Firmware/` | 固件工程与板级配置 |
| `Documentation/images/` | 完整原理图、PCB 布局、整机 3D 渲染图与实拍照片 |
| `ROADMAP.md` | 电池、扩展接口、传感器和产品化规划 |
| `CHANGELOG.md` | 项目版本与变更记录 |

## 许可证

本仓库采用 [MIT License](LICENSE)。
