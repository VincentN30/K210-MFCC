[TOC]



# Draco开发板Micropython使用教程

## 第一章：Draco开发板Micropython功能总览

### 1.功能实现

**Draco开发板的Micropython功能包括如下：**

#### **1.Micropython标准库**

- Builtin functions and exceptions – 内置函数与异常
- cmath – 复数运算函数功能
- gc – 控制垃圾收集器
- math – 数学函数功能
- sys – 系统特定功能
- uarray – 数组存储功能
- ubinascii – 二进制与 ASCII 码转换功能
- ucollections – 集合与容器类型
- uerrno – 系统错误码
- uhashlib – 哈希算法
- uheapq – 堆队列算法
- uio – 输入输出流
- ujson – JSON 编解码
- uos – 基本的操作系统服务
- ure – 正则表达式
- uselect – 在一组 streams 上等待事件
- usocket – socket 模块
- ussl – SSL/TLS 模块
- ustruct – 原生数据类型的打包和解包
- utime – 时间相关功能
- uzlib – zlib 解压
- _thread – 多线程支持

#### 2.Draco-Micropython特定库

在 RT-Thread 移植的 MicroPython 版本中，实现了如下特定功能库：

- micropython – 实现 MicroPython 内部功能访问与控制
- rtthread – RT-Thread 系统功能模块
- machine – 通用硬件控制模块
  - Pin
  - I2C
  - SPI
  - UART
  - LCD
  - RTC
  - PWM
  - ADC**（Draco开发板没有ADC模块，该功能空缺）**
  - WDT
  - TIMER 
- K210 - Draco开发板独有模块（基于draco开发板定制）
  - FPIOA
  - I2S
  - FFT
  - camera
  - Image
- network – 网络功能配置模块
  - wlan
#### 3.RT-AK-Micropython高性能AI库

  - RT-AK


## 第二章：首次使用Micropython功能

### 1.更新软件包

打开RT-Thread SDK管理器，确认当前Draco开发板已经更新到最新版，本次演示的版本为1.0.9。

![](./%5CFig%5C1.png)

下载完成后，新建项目工程，选择基于开发板，创建最新版的项目工程，注意此时的BSP版本已经更新到了1.0.9。

![](./%5CFig%5C2.png)

创建好的工程项目左侧的项目工程中包含以下内容。

![](./%5CFig%5C3.png)

打开RT-Thread Settings，进行配置。因为当前项目工程还没有开启Micropython功能，所以需要开启Micropython功能。

即：选择 软件包->语言包，开启Micropython功能，保存并编译。

![](./%5CFig%5C4.png)

编译完成后，在项目工程packages里将下载Micropython工具包，如下图所示。

![](./%5CFig%5C5.png)

至此，我们需要的所有软件包已经齐全。

### 2.运行Micropython

运行一个小栗子，我们点亮Draco开发板的LCD屏幕。整个过程我们需要在RT-Thread Settings里开启两个功能：

1.首先，我们需要开启RT-Thread底层的驱动：

点击 硬件 ->Enable LCD on SPI0

![](./%5CFig%5C6.png)

确保上述功能开启后，说明Draco底层驱动已经对接好。

2.其次，我们开启 软件包->语言包->Micropython->Hardware Module中的machine LCD，此时确定了从底层到micropython层面已经连接。

![](./%5CFig%5C7.png)

完成上述配置后，我们保存并编译程序。

连接开发板，同时下载程序。

![](./%5CFig%5C8.png)

下载完成后，通过串口已经可以连接开发板的micropython环境了。为了方便我们参考以下参考，在VScode中完成后续的python程序开发。

> vscode安装micropython插件：https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/packages-manual/micropython-docs/micropython-ide

连接vscode和Draco开发板，并运行下列代码

```python
from machine import LCD
def main():
    lcd = LCD()
    lcd.fill(lcd.GRAY)
    str = "hello RT-Thread"
    lcd.text(str, 10, 50, 16)
    
if __name__ == '__main__':
    main()
```

我们可以看到开发板的LCD亮起，并显示RT-Thread字样

<img src="./%5CFig%5C14.jpg" style="zoom:67%;css:left" />

至此，我们成功的点亮了LCD。更详细的硬件设置将在第四章进行详细的介绍。

**总结一下，我们在Draco中配置使用Micropython需要注意以下几点，所有的硬件功能都是类似设置方式：**

- **下载最新Draco的BSP**
- **配置RT-Thread底层驱动**
- **配置Micropython的功能**
- **编写并运行vscode端驱动Draco开发板**

### 3.查找官方Micropython参考例程

关于Micropython的官方参考例程请查阅项目工程中samples文件夹中内容

## 第三章：RT-Thread系统Micropython实现原理分析

### 1.Micropython和RT-Thread软件层面的关系

#### 1.Micropython总览

**1.官方micropython文件夹总览**

![](./%5CFig%5C10.png)

`driver`文件夹中存放硬件相关驱动

`extmod`文件夹中存放Micropython和操作系统接口程序

`lib`底层C库

`mpy-cross`类unix系统上用于编译py的编译器

`ports`官方给出的硬件开发板BSP

`py`Micropython核心实现，包括compiler、runtime、core library等

`tools`一些配置编写程序时需要的工具，如注册使用的py脚本等

**2.RT-Thread官方micropython文件夹总览**

RT-Thread的micropython总共包括两部分，如下两张图所示：分别放置在\packages\micropython-v1.13.0\port\modules\machine中和\extmods\k210中。

![](./%5CFig%5C11.png)

![](./%5CFig%5C12.png)

#### 2.Micropython对接RT-Thread系统总览

这一部分详细解释了如何在现有的Micropython基础上自行设计Micropython模块。

首先，C 语言和 Python 是两种完全不同的语言，如何使用 Python 来调用 C 语言所实现的函数是许多小伙伴非常疑惑的地方。 其实这个问题的关键点就在于如何用 C 语言的形式在 Python 中表述函数的入参和出参，我举一个例子来讲解这个问题， 请观察下列 Python 函数：

```python
def add(a, b):
    return a + b
```

这个函数输入两个参数，输出一个参数。此时如果我们能用 C 语言表示该 Python 函数的输入输出参数，我们就可以将一个实现该功能的 C 语言的函数对接到 MicroPython 中，我们假设这些参数的类型都为整形，通过自动生成器我们可以得到 如下样板函数：

```python
STATIC mp_obj_t add(
    mp_obj_t arg_1_obj,
    mp_obj_t arg_2_obj) {
    mp_int_t arg_1 = mp_obj_get_int(arg_1_obj);   /* 通过 Python 获取的第一个整形参数 arg_1 */
    mp_int_t arg_2 = mp_obj_get_int(arg_2_obj);   /* 通过 Python 获取的第二个整形参数 arg_1 */
    mp_int_t ret_val;

    /* Your code start! */

    ret_val = arg_1 + arg_2;  /* 在此处添加处理入参 arg_1 和 arg_2 的实现，并将结果赋给返回值 ret_val */

    /* Your code end! */

    return mp_obj_new_int(ret_val);               /* 向 python 返回整形参数 ret_val */
}
MP_DEFINE_CONST_FUN_OBJ_2(add_obj, add);
```

生成器会帮我们处理好需要导出到 MicroPython 的函数的入参和出参，而我们只需要编写相应的代码来处理这些输入参数，并且把返回值赋给输出参数即可。 你可以通过包含头文件的方式，任意调用你先前编写的任意 C 函数来对输入参数进行处理，或者根据输入参数来执行相应的动作。

编写完功能函数后，还要记得注册你的函数到模块中，其实也就是将你的函数添加到模块列表 `_globals_table[]` 中。至于为什么要添加 QSTR，不必太在意， 这是一种在 MicroPython 中节省字符串占用空间的机制，根据提示添加相应的代码就可以了。

最终使用 Python 调用 C 函数的效果如下：

```python
>>> import userfunc
>>> userfunc.add(666,777)
1443
```

>  原文请参考：https://summerlife.github.io/RT-MicroPython-Generator/

## 第四章：Micropython硬件模块分析

本章节针对所有模块的介绍与RT-Thread官网的描述互补，更详细的介绍可以参考RT-Thread官网

> https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/packages-manual/micropython-docs/spec-librarys/machine

### 1.machine模块

machine模块为MCU芯片常见设备控制模块.

#### machine.Pin

在创建`Pin`设备类对象时, 传入的`id`编号即是原理图中`IO0~IO47`引脚编号.

```python
from machine import Pin
# 查看Pin类成员
dir(Pin)
# machine.Pin( id, mode = -1, pull = -1，value)
# id: ('name', num) name为用户引脚别名, num为引脚编号(和原理图IO0-IO47对应).
pin30 = Pin(('rgb_r',30), mode=Pin.OUT_PP, value=1)
pin38 = Pin(('rgb_r',38), mode=Pin.OUT_PP, value=1)
pin39 = Pin(('rgb_g',39), mode=Pin.OUT_PP, value=1)
pin40 = Pin(('rgb_b',40), mode=Pin.OUT_PP, value=1)
# light led
pin30.value(0)
pin38.value(0)
pin39.value(0)
pin40.value(0)
```

#### machine.I2C

I2C设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板可注册的I2C设备有: `i2c0`, `i2c1`, `i2c2`. 因此使用时可在`machine.I2C(id= -1, scl, sda, freq=400000)` 的id参数中传入`0`, `1`, `2`.

#### machine.SPI

SPI设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板上可注册的有`spi1`总线, 同时可配置4个spi片选设备, 设备名为spi10, spi11, spi12, spi13, 因此可在`class machine.SPI(id, ...)`的id参数中传入如`10`, `11`, `12`, `13`.

#### machine.UART

UART设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板上可注册的有3个UART总线, 设备名为uart1, uart2, uart3, 因此可在`class machine.UART(id, ...)` 的id参数中可传入如`1`, `2`, `3`.

#### machine.LCD

LCD设备模块使用方法参考RT-Thread官方MicroPython文档.

#### machine.RTC

RTC设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板注册了一个设备名为`rtc`的rtc设备.

#### machine.PWM

PWM设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板上仅有一个pwm模块, 设备名为`"pwm"`, 因此在`class machine.PWM(id, channel, freq, duty)` 的id参数需要传入`"pwm"`设备名来查找和初始化设备.

#### machine.ADC

**K210芯片没有ADC设备**

#### machine.WDT

WDT设备模块使用方法参考RT-Thread官方MicroPython文档. Draco开发板上可注册2个, 设备名为"wdt0"和"wdt1"的设备, 因此在`class machine.WDT(id , timeout)` 的id参数可传入`"wdt0"`, `"wdt1"`或者 `0`, `1`来查找和使用wdt设备.

#### machine.TIMER

Timer设备模块主要使用方法参考RT-Thread官方MicroPython文档. Draco开发板上通过`driver/drv_hw_timer` 可以看到注册的Timer设备有`timer00`, `timer01`, `timer02`, `timer03` `timer10`, `timer11`, `timer12`, `timer13` 其中命名中的数字分别代表设备号和通道号, 例如`timer01` 代表timer的0号设备的1通道.

### 2.Draco定制模块

Draco定制模块为k210芯片上搭载的特定模块的控制模块.

#### k210.FPIOA

k210芯片上提供的一些特定设备都放在k210模块下.

> FPIOA（现场可编程 IO 阵列）允许用户将 255 个内部功能映射到芯片外围的 48 个自由 IO 上。主要功能:
>
> • 支持 IO 的可编程功能选择
>
> • 支持 IO 输出的 8 种驱动能力选择
>
> • 支持 IO 的内部上拉电阻选择
>
> • 支持 IO 的内部下拉电阻选择
>
> • 支持 IO 输入的内部施密特触发器设置
>
> • 支持 IO 输出的斜率控制
>
> • 支持内部输入逻辑的电平设置

**构造函数**

```
class k210.FPIOA()
```

**方法**

```
bool set_function(int pin, int func, int set_sl, int set_st,int set_io_driving)
```

设置引脚功能

- pin : 引脚号，取值范围 0 到 48
- func : 引脚功能，取值范围见下表, 可使用`dir(FPIOA)`查看该类成员函数和可配置功能.
- set_sl : （可选参数）
- set_st : （可选参数）
- set_io_driving : （可选参数）

```
int get_io_by_function(int func)
```

查看某功能所在引脚

- func : 取值范围如下:

```plaintext
['JTAG_TCLK', 'JTAG_TDI', 'JTAG_TMS', 'JTAG_TDO', 'SPI0_D0', 'SPI0_D1', 'SPI0_D2', 
'SPI0_D3', 'SPI0_D4', 'SPI0_D5', 'SPI0_D6', 'SPI0_D7', 'SPI0_SS0', 'SPI0_SS1', 
'SPI0_SS2', 'SPI0_SS3', 'SPI0_ARB', 'SPI0_SCLK', 'UARTHS_RX', 'UARTHS_TX', 'CLK_SPI1',
'CLK_I2C1', 'GPIOHS0', 'GPIOHS1', 'GPIOHS2', 'GPIOHS3', 'GPIOHS4', 'GPIOHS5', 
'GPIOHS6', 'GPIOHS7', 'GPIOHS8', 'GPIOHS9', 'GPIOHS10', 'GPIOHS11', 'GPIOHS12', 
'GPIOHS13', 'GPIOHS14', 'GPIOHS15', 'GPIOHS16', 'GPIOHS17', 'GPIOHS18', 'GPIOHS19', 
'GPIOHS20', 'GPIOHS21', 'GPIOHS22', 'GPIOHS23', 'GPIOHS24', 'GPIOHS25', 'GPIOHS26', 
'GPIOHS27', 'GPIOHS28', 'GPIOHS29', 'GPIOHS30', 'GPIOHS31', 'GPIO0', 'GPIO1', 'GPIO2', 
'GPIO3', 'GPIO4', 'GPIO5', 'GPIO6', 'GPIO7', 'UART1_RX', 'UART1_TX', 'UART2_RX', 
'UART2_TX', 'UART3_RX', 'UART3_TX', 'SPI1_D0', 'SPI1_D1', 'SPI1_D2', 'SPI1_D3', 
'SPI1_D4', 'SPI1_D5', 'SPI1_D6', 'SPI1_D7', 'SPI1_SS0', 'SPI1_SS1', 'SPI1_SS2', 
'SPI1_SS3', 'SPI1_ARB', 'SPI1_SCLK', 'SPI_SLAVE_D0', 'SPI_SLAVE_SS', 
'SPI_SLAVE_SCLK', 'I2S0_MCLK', 'I2S0_SCLK', 'I2S0_WS', 'I2S0_IN_D0', 'I2S0_IN_D1', 
'I2S0_IN_D2', 'I2S0_IN_D3', 'I2S0_OUT_D0', 'I2S0_OUT_D1', 'I2S0_OUT_D2', 
'I2S0_OUT_D3', 'I2S1_MCLK', 'I2S1_SCLK', 'I2S1_WS', 'I2S1_IN_D0', 'I2S1_IN_D1', 
'I2S1_IN_D2', 'I2S1_IN_D3', 'I2S1_OUT_D0', 'I2S1_OUT_D1', 'I2S1_OUT_D2', 
'I2S1_OUT_D3', 'I2S2_MCLK', 'I2S2_SCLK', 'I2S2_WS', 'I2S2_IN_D0', 'I2S2_IN_D1', 
'I2S2_IN_D2', 'I2S2_IN_D3', 'I2S2_OUT_D0', 'I2S2_OUT_D1', 'I2S2_OUT_D2', 
'I2S2_OUT_D3', 'I2C0_SCLK', 'I2C0_SDA', 'I2C1_SCLK', 'I2C1_SDA', 'I2C2_SCLK', 
'I2C2_SDA', 'CMOS_XCLK', 'CMOS_RST', 'CMOS_PWDN', 'CMOS_VSYNC', 'CMOS_HREF', 
'CMOS_PCLK', 'CMOS_D0', 'CMOS_D1', 'CMOS_D2', 'CMOS_D3', 'CMOS_D4', 'CMOS_D5', 
'CMOS_D6', 'CMOS_D7', 'SCCB_SCLK', 'SCCB_SDA', 'UART1_CTS', 'UART1_DSR', 'UART1_DCD', 
'UART1_RI', 'UART1_SIR_IN', 'UART1_DTR', 'UART1_RTS', 'UART1_OUT2', 'UART1_OUT1', 
'UART1_SIR_OUT', 'UART1_BAUD', 'UART1_RE', 'UART1_DE', 'UART1_RS485_EN', 'UART2_CTS', 
'UART2_DSR', 'UART2_DCD', 'UART2_RI', 'UART2_SIR_IN', 'UART2_DTR', 'UART2_RTS',
'UART2_OUT2', 'UART2_OUT1', 'UART2_SIR_OUT', 'UART2_BAUD', 'UART2_RE', 'UART2_DE', 
'UART2_RS485_EN', 'UART3_CTS', 'UART3_DSR', 'UART3_DCD', 'UART3_RI', 'UART3_SIR_IN', 
'UART3_DTR', 'UART3_RTS', 'UART3_OUT2', 'UART3_OUT1', 'UART3_SIR_OUT', 'UART3_BAUD', 
'UART3_RE', 'UART3_DE', 'UART3_RS485_EN', 'TIMER0_TOGGLE1', 'TIMER0_TOGGLE2', 
'TIMER0_TOGGLE3', 'TIMER0_TOGGLE4', 'TIMER1_TOGGLE1', 'TIMER1_TOGGLE2', 
'TIMER1_TOGGLE3', 'TIMER1_TOGGLE4', 'TIMER2_TOGGLE1', 'TIMER2_TOGGLE2', 
'TIMER2_TOGGLE3', 'TIMER2_TOGGLE4', 'CLK_SPI2', 'CLK_I2C2', 'DRIVING_0', 'DRIVING_1', 
'DRIVING_2', 'DRIVING_3', 'DRIVING_4', 'DRIVING_5', 'DRIVING_6', 'DRIVING_7', 
'DRIVING_8', 'DRIVING_9', 'DRIVING_10', 'DRIVING_11', 'DRIVING_12', 'DRIVING_13', 
'DRIVING_14', 'DRIVING_15']
```

**例程:**

FPIOA+PWM配置例程:

```python
from k210 import FPIOA
from machine import PWM

fpioa = FPIOA() 
# 查看fpio的方法和可选功能映射
dir(fpioa)
# TIMER2_TOGGLE1-4对应PWM0-3通道
# 设置30号IO引脚(LED)功能映射为TIMER2_TOGGLE1(PWM通道0), 
fpioa.set_function(30, fpioa.TIMER2_TOGGLE1)
pwm=PWM('pwm',0,1000,250)
pwm.init(0,1000,150)
pwm.init(0,1000,0)   
```

#### k210.I2S

**构造函数**

```
class k210.I2S(int bus, int mode)
```

- bus : I2S 总线编号，取值范围 0 到 2
- mode : 收/发模式，取值 TRANSMITTER(播放)，RECEIVER(录音)

**方法**

```
void init()
```

初始化

```
void set_param(int sample_rate, int bps, int track_num)
```

设置参数

- sample_rate: 采样率
- bps : 采样位数
- track_num : 音道数，取值 1 或 2

```
void play(bytearray pcm)
```

播放

- pcm : PCM 格式的音频数据

```
bytearray record(void)
```

录音

**示例**

参考工程目录`samples/python`路径下示例

#### k210.FFT

**构造函数**

```
class k210.FFT()
```

**方法**

```
list run(int[] input, int shift, int direction)
```

计算 fft

参数说明：

- input : 待变换的数据，类型：int 数组
- shift : 默认 0
- direction : 取值 DIR_BACKWARD/DIR_FORWARD

返回值说明：

- return : 返回包含实部和虚部的列表，虚/实为 int 类型

#### k210.camera

**构造函数**

```
class k210.camera()
```

**方法**

```
void reset()
```

初始化

```
void set_pixformat(int fmt)
```

设置图像格式

-- fmt : 格式，取值 RGB565

```
void set_framesize(int width, int height)
```

设置图像大小

snapshot()

获取单幅图像

return :  Image对象

**注意:** 当开启OpenMV时, 该类将失效. 将使用`sensor`类代替. camera方法返回的Image类仅具有少量图像处理方法, 和OpenMV中的Image并非同一个类.

#### k210.Image

k210.Image为摄像头采集返回的图像格式, 包含了针对k210定制的相关处理函数. 主要用于`RGB565` (用于LCD显示) 和`RGB888 CHW`(用于AI输入) 格式的处理和转换.

**注意:** 该Image与**OpenMV**中的Image对象不同. 在Draco开发板上当使能了OpenMV时, 将自动禁用该类. 转而使用OpenMV的图像处理库.

image.width()

返回以像素计的图像的宽度。

image.height()

返回以像素计的图像的高度。

image.format()

返回用于灰度图的 `sensor.GRAYSCALE` 、用于RGB图像的 `sensor.RGB565` 和用于JPEG图像的 `sensor.JPEG` 。

image.size()

返回以字节计的图像大小。

image.get_pixel(x, y[, rgbtuple])

灰度图：返回(x, y)位置的灰度像素值。

RGB565l：返回(x, y)位置的RGB888像素元组(r, g, b)。

Bayer图像: 返回(x, y)位置的像素值。

不支持压缩图像。

image.set_pixel(x, y, pixel)

灰度图：将`(x, y)` 位置的像素设置为灰度值 pixel 。 RGB图像：将`(x, y)` 位置的像素设置为RGB888元组`(r, g, b)` pixel 。 对于bayer模式图像:将位置`(x, y)`的像素值设置为 pixel。

返回image对象，这样你可以使用 . 符号调用另一个方法。

x 和 y 可以独立传递，也可以作为元组传递。

pixel 可以是 `RGB888` 元组 (r, g, b) 或底层像素值（即 RGB565 图像的字节反转 RGB565 值或灰度图像的 8 位值。

不支持压缩图像。

image.to_rgb565()

用于将图片转换为rgb565格式. 该方法会返回新的图像buffer.

image.to_gray()

将图片转换为灰度图格式. 该方法会返回新的图像buffer.

image.resize(w, h)

缩放图片宽高为`w` 和`h` . 该方法会返回新的图像buffer.

image.crop(y, x, offset_h, offset_w)

从`image`的`(x, y)`起始位置裁剪图片, 裁剪大小为`offset_h`和`offset_w`.

### 3.OpenMV模块

Draco开发板上移植适配了OpenMV图像处理库, OpenMV使用参考:

[OpenMV图像处理的方法 · OpenMV中文入门教程](https://book.openmv.cc/image/)

**！！！注意：Draco开发板OpenMV使用注意事项**

* OpenMV中自带了摄像头驱动, 在开启OpenMV时, 需要将RT-Thread Settings中`硬件 ->Enable Camera -> Select camera type` 配置选择为`Camera using other` 选项, 关闭`driver` 下的摄像头驱动. 否则会报重复定义的错误.
* 备注: K210上OpenMV`snapshot()`函数 **该函数在K210上将返回一个`list` 包含2副图像`[Image_rgb565, Image_rgb888]`** 如下:

```python
img1, img2 = sensor.snapshot()
# img1为RGB565用于LCD显示, img2位RGB888用于AI输入
```

## 第五章：RT-AK-Micropython高性能AI模块

#### rt-ak模块使用流程总览：

rt-ak micropython的使用分为两种方式：

1.将AI模型编译到整个工程项目中，通过

![](./%5CFig%5C13.png)

#### ai_find(name)

**描述:** 该函数通过一个字符串类型的名字, 用于查找一个已注册到RT-AK中的模型. 若查找成功将返回一个`Model`对象实例, 否则返回`None`

**参数:**

- `name` 为要查找的模型名, 模型为通过RT-AK工具预转换注入工程的模型.

**示例:**

```python
model=rt_ak.ai_find("cifar10_mb")
```

**备注:** 该方法对应RT-AK C接口`rt_ai_find()`

#### ai_load(buffer, name)

**描述:** 该方法与`ai_find` 类似, 用于从文件系统中加载模型,  需要首先从文件系统中把模型文件read到内存buffer中, 调用该方法加载buffer.

**参数:**

- `buffer` 内存中模型buffer
- `name` 传入该模型的模型名

**示例:**

```python
f=open('yolov3.kmodel')
yv3_kmodel=f.read()
yolo=rt_ak.ai_load(yv3_kmodel, 'yolov3')
rt_ak.ai_init(yolo)
```

**备注:** `ai_find` 方法用于查找已编译存在固件中的模型. `ai_load` 用于动态的从文件系统中加载模型. 通常更具情况使用其中一个方法.

#### ai_init(model, size=0)

**描述:** 该函数初始化一个模型对象. `model`参数必须传入, `size`为运行时需要的内存, 在K210上指示模型输入的buffer大小. 可选参数. 无返回值.

**参数:**

- `model` 通过`ai_find` 或 `ai_load` 加载的模型对象.
- `size` 可选参数, 用于预分配内存.

**示例:**

```python
model=rt_ak.ai_find("cifar10_mb")
rt_ak.ai_init(model)
```

**备注:** 该函数对应RT-AK C接口`rt_ai_init()`.

#### ai_run(model, input)

**描述:** 该函数执行模型一次前向推理, 参数`model`为要进行推理的模型, `input`为模型的输入数据, 无返回值.

**参数:**

- `model` 通过`ai_find` 或 `ai_load` 加载的模型对象.
- `input` 模型的输入.

**示例:**

```python
model=rt_ak.ai_find("cifar10_mb")
rt_ak.ai_init(model)
rt_ak.ai_run(model,img_888_chw)
```

**备注:** 该函数对于RT-AK C接口`rt_ai_run()`.

#### ai_output(model, index=0)

**描述:** 该函数返回最近一次执行模型推理的输出结果. 参数`model`为模型, `index`为要获取的输出索引. 返回值为结果的`list`对象.

**参数:**

- `model` 通过`ai_find` 或 `ai_load` 加载的模型对象.
- `index` 模型输出索引.

**示例:**

```python
model=rt_ak.ai_find("cifar10_mb")
rt_ak.ai_init(model)
rt_ak.ai_run(model,img_888_chw)
predicts=rt_ak.ai_output(model,0)
predicts

# out[]: [0.0, 0.9, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0]
```

**备注:** 该函数对于RT-AK C接口`rt_ai_output()`.

#### ai_free(model)

**描述:** 用于释放模型相关内存, 在模型不再使用时, 应调用此函数对模型进行内存释放.

**参数:**

- `model` 要被释放的模型.

## 第六章：基于文件系统的使用说明

### 1.必要的软件

- 准备一张sd卡，放入Draco开发板卡槽。
- `RT-Thread MicroPython IDE` 串口命令行工具后端源代码，其功能为连接 MicroPython 设备，并基于该设备执行 MicroPython 开发过程中的各种操作。**该工具放置在项目更目录下的./third_party文件夹中。**

### 2.使用方法

在命令行中输入如下命令，安装该工具所依赖的软件包：

```
python -m pip install -i https://pypi.tuna.tsinghua.edu.cn/simple click pyserial python-dotenv pyinstaller
```

**命令使用方式**

- 运行功能脚本

安装后可以直接运行 cli.py 脚本来操作 MicroPython 设备。

例如 `python cli.py -p COM18 repl` 的意思是连接 COM18 串口并进入 repl 模式。

- 运行可执行文件

可执行文件存放在 dist 文件夹下，里面存放了不同操作系统下的可执行文件：

例如 `./cli.exe -p COM18 repl` 的意思是连接 COM18 串口并进入 repl 模式。

**连接串口**

使用任何命令都需要指定本次操作的串口，例如 `python cli.py -p COM18` 的意思是对 COM18 串口的设备进行操作，后续所有的命令前也都需要添加该格式。

**查询系统中可用串口**

- python cli.py -p query portscan

**进入 repl 模式**

连接串口并打开 repl 模式的命令如下：

- python cli.py -p COM18 repl

在当前终端接入 MicroPython 的 repl ，在终端使用 `ctrl +x` 退出 repl 模式。

**查询文件系统列表**

| 命令        | 功能                                       |
| ----------- | ------------------------------------------ |
| ls          | 打印出开发板上 / 目录中的文件列表          |
| ls -r       | 递归打印出 / 目录中文件列表                |
| ls -r -l    | 递归打印出 / 目录中文件列表以及 crc 校验值 |
| ls /scripts | 打印出开发板上 /scripts 文件夹中的文件列表 |

例如：

- python cli.py -p COM18 ls

**创建删除文件夹/文件**

| 命令           | 功能                                                   |
| -------------- | ------------------------------------------------------ |
| mkdir dir_name | 创建文件夹，名为 dir_name                              |
| rmdir dir_name | 递归地删除 dir_name 文件夹中的所有文件，最终删除文件夹 |
| rm filename    | 可以用来删除某个特定文件                               |

例如：

- python cli.py -p COM18 mkdir dir_test_name

**文件传输操作**

| 命令                | 功能                                                       |
| ------------------- | ---------------------------------------------------------- |
| get xx.py xx.py     | 从开发板中获取 xx.py 到本地，并将该文件命名为 xx.py        |
| put xx.py xx.py     | 将本地文件传入到开发板中（注意写入的文件必须是 unix 格式） |
| put dir_name remote | 将本地的 dir_name文件夹推送到开发板上，并且重命名为 remote |

例如：

- python cli.py -p COM18 get board.py local.py

**代码文件运行**

| 命令                 | 功能                                                         |
| -------------------- | ------------------------------------------------------------ |
| run xx.py            | 在开发板上执行本地目录下的 xx.py 文件                        |
| run none -d hello.py | 执行设备上的 `hello.py` 文件，注意如果该程序不返回，则程序无法从终端返回 |

**文件夹同步**

```
python cli.py -p com18 sync -l "G:\sync_dir" -i "G:\file_list_cache"
```

- `-l` 参数后面跟想要同步到远端根目录的本地文件夹地址

- `-i` 参数后面**设备文件系统中文件列表，缓存在本地的存储文件**

  对每一个开发板需要指定一个新的文件，否则会导致无法正确同步文件，如果不能确定指定的缓存文件是否正确，可以删除掉本地的缓存文件，并重新指定一个新的文件地址，同步代码会重新从设备文件系统中读取先关信息，并写入到这个文件里。

**关闭 repl 命令行回显的方法**

向串口发送 b'\xe8' 字符将会关闭回显功能，向串口发送 b'\xe9' 将会重新打开回显功能。该功能可用在按下 `CTRL + E` 进入粘贴模式前，关闭回显，使得输入的内容不显示在终端上。

**重新打包 cli 工具**

cli 在不同的系统上运行，可能需要重新打包，如果该工具在您的系统上无法运行，可以尝试用如下命令重新打包，然后在插件中重新替换 cli 工具即可。插件后端一般路径如下：

```
.vscode\extensions\rt-thread.rt-thread-micropython-1.x.x\ampy
```

**在 windows 系统上打包 cli**

```
pyinstaller.exe -F .\cli.py -p ampy
```

**在 linux（包括 Deepin Mac 等类 linux 操作系统） 系统上打包 cli**

```
pyinstaller -F cli.py -p ampy
```

**linux 下将当前用话加入到 dialout 用户组**

执行下面的命令，当使用 your_username 进行串口操作时，就不需要再输入 sudo 来频繁获取权限了。

```
sudo usermod -aG dialout your_username
```

## 第七章：参考文献

1.[microPython源码分析.1](https://cloud.tencent.com/developer/article/1812932)

2.[microPython源码分析.2](https://cloud.tencent.com/developer/article/1812895?from=article.detail.1812932)

3.[microPython源码分析.3](https://cloud.tencent.com/developer/article/1812931?from=article.detail.1812895)

4.[MicroPython 函数库](https://docs.singtown.com/micropython/zh/latest/pyboard/library/machine.html)