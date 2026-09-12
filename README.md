# ESP32-S3 桌面语音交互终端

本项目基于 **ESP32-S3-WROOM-1** 开发，已完成硬件设计与固件移植，
成功跑通语音对话、唤醒词唤醒、表情动画显示、语音播报等功能。

喊一声「你好小智」或按一下按键就能和它说话。它会把你说的话转成文字
显示在屏幕上，用表情回应你，再用语音把答案讲出来。

硬件配 1.54 寸屏幕、ES8311 编解码器和 NS4150B 功放。
原理图、PCB、制板文件和固件全部开源。

<p align="center">
  <img src="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/pcb_3d_iso.png" width="560" alt="整机 3D 渲染">
  <br>
  <img src="https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/device_photo.jpg" width="420" alt="设备实拍">
</p>

## 能做什么

- **语音对话**：唤醒词或按键进入对话，说完自动识别并回答
- **屏幕反馈**：显示表情动画和对话文字
- **免按键唤醒**：离线唤醒词识别
- **语音播报**：回答通过喇叭实时播放
- **语音控制**：可以直接说「声音小一点」「屏幕调亮些」来控制设备
- **配网简单**：手机连上设备热点即可配置 WiFi
- **双 USB-C**：一个下载调试，一个原生 USB

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

完整的原理图、PCB 源文件、Gerber 和 BOM 都在本仓库。

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

## 仓库内容

| 路径 | 说明 |
| --- | --- |
| `ESP32-S3多功能终端.eprj2` | 嘉立创 EDA 工程（原理图 + PCB） |
| `Fabrication/` | Gerber、BOM、贴片坐标、板厂工艺参数 |
| `Firmware/` | 固件工程与板级配置 |
| `Documentation/images/` | 整机 3D 渲染图与实拍照片 |
