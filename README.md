# FreeRTOS CLI
功能：实现一个简单的交互式Shell。输入/help，返回所有指令
led on:开LED0，led off:关LED0  

心得：  
1.C 语言里根本就没有“字符串”这种数据类型。 所谓的字符串，本质上就是 “一大排连续的字符”。  
char *定义的是一个字符串，一个地址存放一个字符，直到最后加上\0。char *argv[10];像这样等于是存了十个字符串，有点像二维数组，不过第二维的大小是动态的，由\0控制尾巴。  
2.\0的操作：  
2.1 只要是处理字符串的函数（printf, strcpy, strlen），约定好：拿到首地址后，就无脑往后读，直到遇到数字 0 (\0) 为止。  
2.2 只要你在代码里敲出了 双引号，编译器就会自动在屁股后面补一个 \0  
2.3 {}大括号里不加，char arr[] = {'h', 'i'};不能用printf  
2.4 memcpy不加  
printf("%s"),天生接收指针。以\0结尾  
3.一个字节=8位比特（uint8），半个字=2个字节（uint16），一个字=4个字节（uint32）。接收数组大小也得对应上。  
4.表驱动法：  
const ShellCommand_t CmdTable[] = {  
    {"help",  Cmd_Help,  "Show help info"},  
    {"led",   Cmd_LED,   "Control LED: led 0/1 on/off"},  
    {NULL,    NULL,      NULL} // 结束标记  
};  //命令名，对应的函数地址，帮助信息  
具体的对LED执行什么操作，进入Cmd_LED函数后再实现，如on/off  
//使用时：CmdTable[0].name=xx;  
5.static（静态）用法：  
函数内，让变量即使出了函数也存在，不过还是只有他能用  
全局内，只能在当前.c文件里使用，出了就不让用  
6.const（只读，放rom里）用法：
全局内，让变量变为常量，存在ROM里   
函数内，得加上static，即 static const char msg[] = "Hello World";私有+放ROM   
7.extern，最好写在.h文件中。要用的.c文件里也行，但是不规范  
8.数组作为函数参数传递时，它会退化为指向数组首元素的指针  
## 1. 任务流程

1.  
2. 
3. 
4. 
5. 

## 2. 关键代码

```c

 char *strtok(char *str, const char *delimiters);//token=
 // 参数1：填你要切的数组名
 // 参数2：填分隔符（比如空格 " "，或者逗号 ","）
 // 返回第一次分割的字符串的前一段的开头地址，内部存的是后一段的开头
 token = strtok(NULL, " ");//token是指针
 // 参数1：必须填 NULL！
 // 意思是：“嘿，接着切刚才那个字符串，别换人”
 //直到遇到/0时，返回NULL  
 //不可切常量字符，只能切数组（可变）

int strcmp(const char *s1, const char *s2);//返回0，相等，否则不等
int strncmp(const char *s1, const char *s2, size_t n);//n：比较前n个字符