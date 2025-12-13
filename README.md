# FreeRTOS 任务通知

功能：。
心得：  
1.C语言的强制类型转换：虽然地址都是同一个地方，但是他的名字经过转换后可以不一样。不过要注意生命周期  
如：Human_t *pFake = (Human_t *)pReal;pReal是TCB_t类型的。详细见关键代码  
1.1.分配内存和释放内存：  
MyParam_t *p = pvPortMalloc(sizeof(MyParam_t));  
vPortFree(p);  
## 1. 任务流程

1. **定义任务句柄** (身份证)
2. **声明任务函数**
3. **编写任务函数实体** (任务 A: 负责闪灯)
4. **创建任务** (xTaskCreate)
5. **启动调度器**



## 2. 关键代码
```c
















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


         
