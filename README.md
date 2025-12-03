# FreeRTOS 任务挂起与恢复测试


功能：使用两个按钮控制 LED0 任务的挂起和恢复，同时串口打印。

注意：切换分支前，若想保留修改，必须先 Commit

在git graph里reset了之后，必须在本地git push -f
## 1. 任务流程

1. **定义任务句柄** (身份证)
2. **声明任务函数**
3. **编写任务函数实体** (任务 A: 负责闪灯)
4. **创建任务** (xTaskCreate)
5. **启动调度器**

## 2. 关键代码
```c

if (flag[0] == 0)
    {
      vTaskSuspend(LED0_Handle);
      printf("suspend task0 successfully\r\n");
      flag[0] = 1;
    }
    else if (flag[1] == 0)
    {
      vTaskResume(LED0_Handle);
      printf("resume task0 successfully\r\n");
      flag[1] = 1;
    }
