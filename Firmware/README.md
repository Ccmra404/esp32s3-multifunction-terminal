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

### I2S（ES8311 编解码器）

| 信号 | GPIO |
| --- | --- |
| MCLK | IO38 |
| BCLK | IO14 |
| WS | IO13 |
| DIN（麦克风数据入） | IO12 |
| DOUT（音频输出） | IO45 |

### I2C（ES8311 控制）

| 信号 | GPIO |
| --- | --- |
| SDA | IO1 |
| SCL | IO2 |

### ST7789 1.54" 240×240（8Pin 排针）

| 排针 | 信号 | GPIO |
| --- | --- | --- |
| 1 | GND | — |
| 2 | VCC | 3V3 |
| 3 | SCK | IO41 |
| 4 | SDA | IO40 |
| 5 | RES | IO11 |
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

本固件的引脚分配与仓库根目录的 PCB 设计完全一致，详见
[`../README.md`](../README.md)。

实际验证时使用 ESP32-S3 开发板 + ES8311/NS4150B 音频模块 + ST7789 屏幕
在面包板上搭建，接线与本表一一对应。

## 后续计划

- [ ] 面包板搭建完成，点亮屏幕
- [ ] I2C 扫描到 ES8311
- [ ] I2S 出声
- [ ] 接入小智服务，完成语音对话
- [ ] 把验证结果与截图补充进仓库
