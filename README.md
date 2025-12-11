# FreeRTOS 事件组

功能：使用两个按键控制事件组的两位，当两位都置1时翻转LED0，并且清空。
注意使用宏定义确定某一位具体是哪个事件，免得混乱。并且最后写在.h文件中  
如：第0位给按键任务用的  
#define EVENT_KEY_PRESS    (1 << 0)  // 0x00000001
区分WaitBits(ALL BITS)和xEventGroupSync
## 1. 任务流程

1. **定义任务句柄** (身份证)
2. **声明任务函数**
3. **编写任务函数实体** (任务 A: 负责闪灯)
4. **创建任务** (xTaskCreate)
5. **启动调度器**

## 2. 关键代码

```c
#include "event_groups.h"
EventGroupHandle_t myEventGroup;

EventGroupHandle_t xEventGroupCreate(void);
void               vEventGroupDelete(EventGroupHandle_t xEventGroup);
EventBits_t        xEventGroupWaitBits( EventGroupHandle_t xEventGroup, //事件组句柄
                                        const EventBits_t uxBitsToWaitFor, //填入掩码，你想等待哪一位变为1？如第三位，0000 1000=1<<3，可用|拼接
                                        const BaseType_t   xClearOnExit, //pdTRUE退出时清除/pdFALSE不清除
                                        const BaseType_t  xWaitForAllBits, //pdTRUE全1才醒/pdFALSE有一个1就醒
                                        TickType_t    xTicksToWait   ) //超时时间
                    返回值：清除前的值
EventBits_t        xEventGroupSetBits( EventGroupHandle_t xEventGroup, 
                                       const EventBits_t uxBitsToSet   ) //填入想设置的位，十六进制
EventBits_t        xEventGroupClearBits( EventGroupHandle_t xEventGroup, 
                                       const EventBits_t uxBitsToClear   ) 
EventBits_t        xEventGroupGetBits(xEventGroup);   
EventBits_t        xEventGroupSync( //同步等待，不同任务运行到这的时候进入阻塞，等待标志位完成
                                        EventGroupHandle_t xEventGroup,  // 1. 句柄
                                        const EventBits_t uxBitsToSet,   // 2. 我是谁？(我要置位哪一位)
                                        const EventBits_t uxBitsToWaitFor, // 3. 我在等谁？(所有人的掩码)
                                        TickType_t xTicksToWait);           // 4. 死等还是超时？     
BaseType_t         xEventGroupSetBitsFromISR(
                                        EventGroupHandle_t xEventGroup,     // 1. 哪个组？
                                        const EventBits_t uxBitsToSet,      // 2. 置哪一位？
                                        BaseType_t *pxHigherPriorityTaskWoken // 3. 有没有VIP醒了？)

## 补课  
十六进制的一位，由四个二进制位组成。所以一个十六位可以表示四个十六进制数。0xF=1111; uint16_t data = 0x1234  
data & 0xFF   
0XFF应该是八位的，但在运算时，编译器会自动把它前面补零，对齐到和 data 一样的长度（16位）   

位操作：
&:有0则0保留1位，用来清零某些位同时保留某些位,即掩码功能
如: uint8_t status=0x03;//0011
    status = status & 0xFD; //0011 & 1101 = 0001

|:有1则1，用于打开某些位 并且不影响其他位，或者叫做拼接
如：uint8_t flags = 0x01;//0001
    flags = flags | 0x04;//0001 | 0100 = 0101

^:相同为0，不同为1
如 0x01^0x01=0x00;

~:取反，配合 & 来清零。
如想清零 Bit 2，需要 & 11111011。
与其手写 0xFB，不如写：& ~(1 << 2)。
1 << 2 是 00000100，取反就是 11111011，这就自动生成了掩码。

<</>>:位操作，全体左移或右移，空出来的位置补0，出去的位置直接丢
常用来造掩码：
1.造单位掩码
比如说想造第3位的掩码。先准备一个1(0000 0001),再(1 << 3)得(0000 1000)。位数从0开始
清0掩码使用~，如~(1<<3)//0000 1000 → 1111 0111
2.造多位掩码
使用|进行拼接，如
mask=(1<<5) | (1<<1) //0010 0000 | 0000 0010 =0010 0010   
         
