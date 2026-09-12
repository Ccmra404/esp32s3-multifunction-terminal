# 固件说明

本目录保存 ESP32-S3 多功能终端主板的固件工程。

## 上游来源

固件基于开源项目 **[78/xiaozhi-esp32](https://github.com/78/xiaozhi-esp32)**（MIT License），
采用克隆后直接修改的方式维护。上游代码的版权与许可归原作者所有。

## 本工程所做的修改

在上游基础上新增了一套板级配置，对应本仓库硬件设计的引脚分配：

```
main/boards/esp32s3-terminal/
├── config.h                  引脚定义
├── config.json               板级描述
└── esp32s3_terminal_board.cc 板级初始化
```

同时在上游的板型列表里注册了该板：

- `main/Kconfig.projbuild`：新增 `BOARD_TYPE_ESP32S3_TERMINAL`
- `main/CMakeLists.txt`：新增对应的 `BOARD_DIR` 分支

## 引脚分配

本板级配置的引脚与上游 `movecall/moji-esp32s3` 参考设计保持一致，
该参考设计正是为同一款 ES8311 + NS4150B 音频模块编写的。

### I2S（ES8311 编解码器）

| 信号 | GPIO |
| --- | --- |
| MCLK | IO6 |
| BCLK | IO14 |
| WS | IO12 |
| DIN（麦克风数据入） | IO13 |
| DOUT（音频输出） | IO11 |

### I2C（ES8311 控制）

| 信号 | GPIO |
| --- | --- |
| SDA | IO5 |
| SCL | IO4 |

### 功放

| 信号 | GPIO |
| --- | --- |
| NS4150B 使能 | IO9 |

### ST7789 1.54" 240×240（8Pin 排针）

| 排针 | 信号 | GPIO |
| --- | --- | --- |
| 1 | GND | — |
| 2 | VCC | 3V3 |
| 3 | SCK | IO41 |
| 4 | SDA | IO40 |
| 5 | RES | IO7 |
| 6 | DC | IO39 |
| 7 | CS | IO10 |
| 8 | BL | IO42 |

### 其它

| 功能 | GPIO |
| --- | --- |
| BOOT 按键 | IO0 |
| 原生 USB D- | IO19 |
| 原生 USB D+ | IO20 |

## 编译与烧录

需要先安装 [ESP-IDF](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32s3/get-started/index.html)
（建议 v5.4 及以上），然后在工程根目录执行：

```bash
idf.py set-target esp32s3
idf.py menuconfig
#   Xiaozhi Assistant -> Board Type -> ESP32-S3 Terminal (1.54-inch ST7789 + ES8311)
idf.py build
idf.py -p COMx flash monitor
```

## 硬件对应关系

面包板验证平台使用 **ESP32-S3 开发板 + ES8311/NS4150B 音频模块 + ST7789 屏幕**，
引脚按上表连接。

> 注：面包板验证用的引脚与仓库根目录的 PCB 设计**不是同一组**。PCB 是另一套
> 分配，这里选用的是上游 Moji 参考设计的引脚，因为它和手中的音频模块是配套
> 验证过的组合。

## 调试记录

搭建过程中遇到并解决的问题，记录下来供复刻时参考。

### 1. 音频模块未连接时设备反复重启

`Es8311AudioCodec::ResetCodec()` 用 `ESP_ERROR_CHECK` 包裹 I2C 写入，
模块不在线时直接 abort，导致重启循环。

**处理**：先把音频模块接好再上电，无需修改代码。

### 2. I2C 引脚与参考设计不一致

最初按自制 PCB 的分配使用 `IO1/IO2`，但这款音频模块的配套参考设计
（`movecall/moji-esp32s3`）用的是 `IO5/IO4`。

**处理**：改用参考设计的引脚。

### 3. MCLK 选到被 PSRAM 占用的引脚

`ESP32-S3-WROOM-1-N16R8` 的八线 PSRAM 占用了模组内部若干 IO。
最初把 MCLK 分配到 `IO38`，导致 I2S 一配置、后续 I2C 写入就失败。

**处理**：MCLK 改到 `IO6`。

### 4. 杜邦线过长导致 I2C 频繁超时

模块说明书要求 I2C 走线不超过 5cm。杜邦线接线时出现：

- 扫描出 `0x4A`~`0x55` 等连续的幽灵地址（总线容性负载过大的典型现象）
- 写入和读取随机失败，日志表现为 100ms 超时

**处理**：把 SCL / SDA / MCLK / SCLK 四根线焊接到模块上并尽量缩短，
MCLK 与 I2C 走线分开。焊接后 100 kHz 标准速率下零失败。

### 5. ES8311 偶发丢包

厂商驱动源码中有这样的注释：

> Due to occasional failures during the first I2C write with the ES8311 chip,
> a second write is performed to ensure reliability.

说明这颗芯片的 I2C 存在偶发 NACK。

**处理**：在 `es8311_write_reg()` / `es8311_read_reg()` 中增加 3 次重试。
正常接线时不会触发，仅在受干扰时兜底。

### 6. 屏幕复位脚与音频引脚冲突

模板里屏幕 RES 用 `IO11`，而音频模块的 DIN 也需要 `IO11`。

**处理**：屏幕 RES 改到 `IO7`。

## 验证状态

| 项目 | 状态 |
| --- | --- |
| 屏幕点亮并显示界面 | ✅ |
| I2C 识别到 ES8311（0x18） | ✅ |
| ES8311 初始化无错误 | ✅ |
| WiFi 配网 | ✅ |
| 设备激活 | ✅ |
| 麦克风拾音 / 喇叭放音 | 待验证 |
