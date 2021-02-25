# cpptimer

#### 介绍
C++ Timer，纯C++实现的定时器，支持两种计数类型的定时器，
使用智能指针管理创建，可在多个线程中使用同一套定时器

#### 软件架构
软件架构说明


#### 安装教程

1.  复制src文件到你的工程即可使用
2.  也可以把src编译成lib库提供给App使用

#### 使用说明

1.  基本使用

```
    TimerSharedPtr timer = CTimer::Create();
    timer->SetTimeoutCb([](TimerBase* pTimer) {
        std::cout << "Timeout Once! id=" << pTimer->GetTimerID() << std::endl;
    });
    timer->Start(1000, true);
    
    while (true)
    {
        // 需要在使用的线程循环中调用 Loop 函数
        TimerContext::DefaultLoop();
    }
```

2.  多个线程中使用

```
bool bRunning = false;
void threadFunction()
{
    while (bRunning)
    {
        TimerContext::DefaultLoop();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
int main()
{
    TimerContext context;
    TimerSharedPtr timer = CTimer::Create(&context);
    
    timer->SetTimeoutCb([](TimerBase* pTimer) {
        std::cout << "Test Timer... id="<< pTimer->GetTimerID() << std::endl;
    });
    timer->SetTimerType(TimerBase::E_Accuracy);
    timer->Start(100, true);

    TimerSharedPtr timer2 = CTimer::Create();
    timer2->SetTimeoutCb([](TimerBase* pTimer) {
        std::cout << "Timeout Once! id=" << pTimer->GetTimerID() << std::endl;
    });
    timer2->Start(1000, true);
    
    bRunning = true;
    std::thread otherThread(threadFunction);
    while (true)
    {
        context.Loop();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    bRunning = false;
    otherThread.join();
}
```


#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request
5.  管理员审核合并

