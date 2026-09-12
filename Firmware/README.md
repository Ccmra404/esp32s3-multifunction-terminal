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

### 音频模块接线（9 脚排针，1~9 从左到右）

模块丝印的 DIN / DOUT 是**站在模块自己的角度**命名的，和 ESP32 的方向相反，
接线时按下表对照，不要只看信号名。

| 模块脚 | 模块丝印 | 含义 | 接到 ESP32 | 方向 |
| --- | --- | --- | --- | --- |
| 1 | GND | 地 | GND | — |
| 2 | +5V | 供电 | 5V | — |
| 3 | **DIN** | **模块的输入**，接收播放音频 | **IO11** | ESP32 发出 → 模块 |
| 4 | LRCK | 声道时钟 | IO12 | ← ESP32 |
| 5 | **DOUT** | **模块的输出**，发送录音数据 | **IO13** | 模块 → ESP32 收到 |
| 6 | SCLK | 位时钟 | IO14 | ← ESP32 |
| 7 | MCLK | 主时钟 | IO6 | ← ESP32 |
| 8 | SCL | I2C 时钟 | IO4 | ← ESP32 |
| 9 | SDA | I2C 数据 | IO5 | ↔ ESP32 |

> **最容易接错的地方**：模块 DIN 要接 ESP32 的 **IO11**（ESP32 的数据输出脚），
> 模块 DOUT 要接 **IO13**（ESP32 的数据输入脚）。接反了麦克风和喇叭都不工作，
> 因为变成了「输出接输出、输入接输入」。

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

> 说明：接线质量改善之后，`100 kHz` 标准速率下已不再出现丢包，
> 这份重试补丁**没有保留**在最终代码中（`managed_components` 每次编译
> 都会重新下载，改动本身也留不住）。若以后换线材又出现丢包，先排查接线。

### 6. 屏幕复位脚与音频引脚冲突

模板里屏幕 RES 用 `IO11`，而音频模块的 DIN 也需要 `IO11`。

**处理**：屏幕 RES 改到 `IO7`。

### 7. 模块 DIN / DOUT 与 ESP32 方向相反（麦克风和喇叭都不工作）

症状：I2C 正常、`Es8311AudioCodec initialized` 打印成功，但说话没有任何反应，
屏幕上的状态也不变化。

原因：模块排针上的 `DIN` / `DOUT` 是站在**模块自己**的角度命名的，
和 ESP32 的 `din` / `dout` 正好相反：

- 模块 `DIN` 是模块的**输入**，要接 ESP32 的 **输出**脚 `IO11`
- 模块 `DOUT` 是模块的**输出**，要接 ESP32 的 **输入**脚 `IO13`

接反后变成「输出接输出、输入接输入」，两个方向都传不了数据。
代码配置本身没有错，错在接线。

**处理**：把模块第 3 脚（DIN）改接 `IO11`，第 5 脚（DOUT）改接 `IO13`，
然后喇叭和麦克风同时恢复正常。

**判断方法**：如果 I2C 能扫到 `0x18`、初始化日志正常，但既听不到声音、
说话也没反应，优先怀疑这两根线接反。

## 验证状态

| 项目 | 状态 |
| --- | --- |
| 屏幕点亮并显示界面 | ✅ |
| I2C 识别到 ES8311（0x18） | ✅ |
| ES8311 初始化无错误 | ✅ |
| WiFi 配网 | ✅ |
| 设备激活 | ✅ |
| 喇叭放音 | ✅ |
| 麦克风拾音 | ✅ |
| 唤醒词「你好小智」 | ✅ |
| 语音对话（说 → 识别 → 回答 → 播报） | ✅ |
