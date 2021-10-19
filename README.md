## stm32f429模板工程

> 注意：个人整理的stm32模板工程代码。未经严格测试，仅供参考！！！
>
> 移植过程参考了[嵌入式精选开源项目专栏](https://blog.csdn.net/mculover666/category_9826553.html)

### 硬件

#### 参数

+ stm32f429zet6
+ 32Mb sdram
+ 4Mb spiFlash

#### 图片

![](https://raw.githubusercontent.com/fanyoupeng/stm32f429/master/hardWare.PNG)

### 软件

#### 组件

加入了一些开源项目，如下：

+ [freertos](https://github.com/FreeRTOS/FreeRTOS-Kernel):实时系统
+ [letterShell](https://github.com/NevermindZZT/letter-shell) :一个功能强大的嵌入式shell
+ [cmBackTrace](https://github.com/armink/CmBacktrace):ARM Cortex-M 系列 MCU 错误追踪库
+ [easyFlash](https://github.com/armink/EasyFlash):开源的轻量级嵌入式Flash存储器库
+ [sfud](https://github.com/armink/SFUD):串行 Flash 通用驱动库

使用makefile构建工程，gcc编译，编译命令如下：

```
make clean
make all
```

#### 启动界面

![](https://raw.githubusercontent.com/fanyoupeng/stm32f429/master/shell.PNG)
