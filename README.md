# FreeRTOS 任务通知(事件组版)

功能：使用任务通知替代二值信号量，KEY0翻转LED0。
心得：  
1.C语言的强制类型转换：虽然地址都是同一个地方，但是他的名字经过转换后可以不一样。不过要注意生命周期  
如：Human_t *pFake = (Human_t *)pReal;pReal是TCB_t类型的。详细见关键代码  
1.1.分配内存和释放内存：  
MyParam_t *p = pvPortMalloc(sizeof(MyParam_t));  
vPortFree(p);  
## 1. 任务流程
任务通知是TCB自带的，无需创建。直接使用任务句柄
1. **定义任务句柄** (身份证)
2. **声明任务函数**
3. **编写任务函数实体** (任务 A: 负责闪灯)
4. **创建任务** (xTaskCreate)
5. **启动调度器**



## 2. 关键代码
```c
//lite版，代替信号量
BaseType_t xTaskNotifyGive( TaskHandle_t xTaskToNotify );//等于信号量+1，不传具体数据
uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit,   TickType_t xTicksToWait );
//xClearCountOnExit=pdTRUE(二值模式，清0)/pdFALSE(计数模式，-1)，xTicksToWait=portMAX_DELAY/pdMS_TO_TICKS(100)
//返回值：返回处理之前的通知值。>0，收到通知；=0，超时了没收到

//全功能版，代替任务组/邮箱
BaseType_t xTaskNotify( TaskHandle_t xTaskToNotify,  uint32_t ulValue,  eNotifyAction eAction );
//可传32位数据，位操作(目标任务句柄，要传的数据，核心动作)
//核心动作：eSetBits(按位或)，eSetValueWithOverwrite(强制覆盖)，eSetValueWithoutOverwrite(保留写入)，eIncrement(=通知值+1)，eNoAction(只唤醒任务)
BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, //进门之前干什么，一般填0
                            uint32_t ulBitsToClearOnExit,  //等到通知并返回之后，要清除哪些位？0=peek，0xFFFFFFFF=全部清零
                            uint32_t *pulNotificationValue, //用来接收数据的变量的地址（指针）
                            TickType_t xTicksToWait );//等待时间
BaseType_t xTaskNotifyAndQuery(+uint32_t *pulPreviousNotificationValue)
//在修改通知值之前，会把修改前的值保存到你指定的变量里

//返回pdTRUE/pdFALSE


//中断中：
void vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, 
                             BaseType_t *pxHigherPriorityTaskWoken );
//一般写法：
BaseType_t xHigherPriorityTaskWoken = pdFALSE;
vTaskNotifyGiveFromISR(TaskHandle, &xHigherPriorityTaskWoken);
portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // 强制切换，保证实时性









//心得1：通用套路，先把结构体在传入的时候强转成void*，在任务内再转回去
// 1. 定义结构体
typedef struct {
    int id;
    char name[10];
} MyParam_t;

// 2. 【关键点】定义变量
// 必须定义为【全局变量】或者【静态变量】或者【堆内存(malloc)】
// 绝对不能是普通的局部变量！（后面解释为什么）
MyParam_t global_param = {1, "TaskA"};

// 3. 创建任务（发送方）
// 动作：把 &global_param (MyParam_t*) 强转为 (void*)
xTaskCreate(StartTask_A, "TaskA", 128, (void*)&global_param, 1, NULL);

// 4. 任务运行（接收方）
void StartTask_A(void const * argument)
{
    // 动作：把 (void*) 强转回 (MyParam_t*)
    // 此时 p 里的地址就是 &global_param 的地址
    MyParam_t *p = (MyParam_t *)argument;

    printf("ID: %d", p->id); // 读到了 1，没问题！
}


         
