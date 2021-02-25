#include <iostream>
#include <thread>
#include "timer.h"
using namespace TimerNamespace;

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