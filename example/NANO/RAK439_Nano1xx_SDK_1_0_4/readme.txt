==============================================
           RAK439  Nano1xx_SDK V1.0.4  release
==============================================
/common										该目录下的文件与平台无关。
/common/include 							该目录下是error code头文件、wifi驱动库头文件、socket头文件。
/common/mem									该目录下是内存操作程序。
/common/rw_os								该目录下是wifi驱动库OS接口程序。

/docs											该目录下是相关软件文档。

/examples_nos 								该目录下是不带OS的示例程序，工程支持KEIL和IAR。
/examples_os								该目录下是支持UCOSiii,FreeRTOS等操作系统的示例程序，工程支持KEIL和IAR。

/middleware									该目录下是一些独立的中间件代码，有FreeRTOS，PolarSSL等。

/platform/nano100/bsp					该目录下是不带OS、平台相关的文件。
/platform/nano100/bsp_os				该目录下是带OS、平台相关的文件。
/platform/nano100/libraries			该目录下是nano100的标准库。
/platform/nano100/rw_lib_platform.c	
												该文件是不含OS时，wifi驱动库和硬件平台之间的接口。
/platform/nano100/rw_lib_platform_os.c	
												该文件是含OS时，wifi驱动库和硬件平台之间的接口。

/rw_lib										该目录下是wifi驱动库文件。
