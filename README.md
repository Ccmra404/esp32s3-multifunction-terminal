# ESP32-S3 多功能终端主板

基于 **ESP32-S3-WROOM-1** 的功能集成主板设计：语音交互、LCD 显示、双路 USB-C 供电与下载。使用嘉立创 EDA 专业版完成原理图与 PCB 设计，两层板、器件单面贴装，可直接投产。

## 预览

![ESP32-S3 多功能终端主板 3D 渲染](https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/pcb_3d_iso.png)

![3D 俯视](https://gcore.jsdelivr.net/gh/Ccmra404/esp32s3-multifunction-terminal@main/Documentation/images/pcb_3d_top.png)

## 硬件规格

| 项目 | 参数 |
| --- | --- |
| 主控 | ESP32-S3-WROOM-1（16MB Flash / 8MB PSRAM） |
| 音频 | ES8311 编解码器 + NS4150B 功放，数字麦克风与喇叭输出 |
| 显示 | 8Pin SPI LCD 接口（SCK / MOSI / DC / RES / CS / BL） |
| 下载 | CH340 USB 转串口 + 自动下载电路（DTR/RTS 控制 RESET/BOOT） |
| USB | 双 Type-C：一路下载调试，一路原生 USB（IO19/IO20） |
| 电源 | USB 5V 输入保护 -> BUCK 3.3V（数字域）-> LDO 3.3V（模拟域） |
| 板型 | 45 x 65 mm，两层板，单面贴装 |

## PCB 设计数据

| 指标 | 数值 |
| --- | --- |
| 器件总数 | 68 |
| 网络总数 | 59（未布线 0） |
| 焊盘总数 | 261 |
| 过孔 | 86 |
| 走线总长 | 1188.2 mm |
| 铺铜 | 顶层 + 底层，共 2 块 GND 平面 |
| 铜层 | 2 层 |

## 电源架构

```
USB1 --[保险丝 F3]--|>|--+
                          +-- +5V --+-- BUCK(U6) -- L2 -- 3V3（数字域）
USB2 --[保险丝 F2]--|>|--+           |                      |
                                     |                      +-- LDO(U4) -- +3.3V（模拟域）
                                     +-- NS4150B 功放
```

音频与数字供电**分域隔离**：BUCK 输出供 MCU、USB、LCD 使用，经 LDO 二次稳压后为 ES8311 与数字麦克风供电，降低开关电源纹波对音频链路的干扰。

## 关键设计点

**USB 差分对**

CH340 下载通道与 ESP32-S3 原生 USB 通道均为 USB 全速 12 Mbps。D+/D- 差分对全程等宽等距走线，两对实测长度均为 64.6 mm，长度差为 0。

**自动下载电路**

采用双三极管方案，由 CH340 的 DTR / RTS 控制 ESP32-S3 的 RESET 与 BOOT 引脚。上位机拉低对应信号即可自动进入下载模式，无需手动按键。

**音频电源**

ES8311 的模拟电源由 LDO 单独供给，配合输入输出滤波电容与参考电压去耦，抑制数字开关噪声串入音频通路。

**GND 平面**

顶层与底层各铺一块 GND 覆铜，保证模拟音频信号的回流路径不被数字信号切割；去耦电容就近打地过孔，缩短回流路径。

## 文件说明

| 文件 | 说明 |
| --- | --- |
| `ESP32-S3多功能终端.eprj2` | 嘉立创 EDA 专业版工程（原理图 + PCB 源文件） |
| `Fabrication/Gerber.zip` | 可直接投产的 Gerber 制版文件 |
| `Fabrication/BOM.csv` | 物料清单，含立创商城编号 |
| `Fabrication/PickAndPlace.csv` | SMT 贴片坐标文件 |
| `Fabrication/PCB_Info.txt` | 板厂工艺参数汇总 |

## 固件

`Firmware/` 目录保存基于开源项目 [78/xiaozhi-esp32](https://github.com/78/xiaozhi-esp32)
修改的固件工程，其中新增了与本板引脚分配对应的板级配置
`main/boards/esp32s3-terminal/`。编译与烧录说明见
[`Firmware/README.md`](Firmware/README.md)。

> 功能验证使用 ESP32-S3 开发板 + ES8311/NS4150B 音频模块 + ST7789 屏幕
> 在面包板上搭建，引脚与 PCB 设计一致。

## 工具链

- 嘉立创 EDA 专业版（原理图 / PCB / Gerber / BOM 导出）
- 立创商城（器件选型与料号）

## 说明

本仓库为硬件设计工程，包含完整的原理图与 PCB 源文件、制板文件与物料清单，可直接打样投产。
