# STM32 Projects

STM32嵌入式开发项目合集，基于HAL库和FreeRTOS实时操作系统。

## 项目列表

### 1. 电子天气时钟 (Weather Clock)
- **路径**: `STM32电子天气时钟/`
- **硬件**: STM32F103 + OLED + DHT11 + WiFi模块
- **功能**: 
  - 实时温湿度显示
  - 网络天气数据获取
  - 闹钟与定时功能
  - 低功耗模式

### 2. 电子阅读器 (Ebook Reader)
- **路径**: `基于STM32的电子阅读器/`
- **硬件**: STM32F4 + E-Ink显示屏 + SD卡 + 按键
- **功能**:
  - TXT/EPUB文本解析
  - 字体渲染与排版
  - 书签与阅读进度
  - 翻页与菜单操作

## 技术栈

- **MCU**: STM32F103 / STM32F407
- **框架**: STM32CubeMX + HAL库
- **RTOS**: FreeRTOS
- **外设**: I2C/SPI/UART/SDIO

## 开发环境

- **IDE**: Keil MDK / STM32CubeIDE
- **调试器**: ST-Link V2 / J-Link
- **串口工具**: VOFA+ / SecureCRT

## 项目结构

```
Project/
├── Core/           # 核心代码
├── Drivers/        # HAL驱动
├── Middleware/     # FreeRTOS/FATFS
└── User/           # 应用层代码
```

## 快速开始

1. 使用STM32CubeMX生成工程
2. 配置时钟树和外设
3. 移植FreeRTOS（如需要）
4. 编译下载调试

## 学习收获

- 掌握STM32 HAL库开发
- 理解RTOS任务调度机制
- 熟悉嵌入式调试技巧
- 硬件接口编程实践

## 相关项目

- [c-learning-projects](https://github.com/1184197761/c-learning-projects) - C语言基础
- [PSoC-EbookReader](https://github.com/1184197761/PSoC-EbookReader) - PSoC平台阅读器

---
*嵌入式开发学习与实践*
