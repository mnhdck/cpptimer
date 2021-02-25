#include "timer.h"
#include <mutex>
#include <iostream>

#define DELETE_DEFAULT false
namespace TimerNamespace
{
	std::atomic<TimerID> TimerContext::ms_nMaxId = 0;
	TimerContext* TimerContext::ms_context = new TimerContext();

	void TimerContext::DefaultLoop()
	{
		ms_context->Loop();
	}

	TimerContext* TimerContext::GetDefaultContext()
	{
		return ms_context;
	}

	void TimerContext::SetDefaultContext(TimerContext* context)
	{
		if (context)
		{
			if (DELETE_DEFAULT && ms_context)
			{
				delete ms_context;
				ms_context = nullptr;
			}
			ms_context = context;
		}
	}

	void TimerContext::Add(TimerSharedPtr timer)
	{
		if (timer)
			m_timers[timer->GetTimerID()] = timer;
	}

	void TimerContext::Remove(TimerID id)
	{
		m_timers.erase(id);
	}

	void TimerContext::Loop() {
		// copy on write
		std::unordered_map<TimerID, std::weak_ptr<TimerBase>> copys = m_timers;

		//std::copy(m_timers.begin(), m_timers.end(), copys);
		for (auto it : copys)
		{
			TimerSharedPtr sp = it.second.lock();
			if (sp && sp->IsRunning())
				sp->OnDispatch();
		}
	}

	TimerID TimerContext::GetNewTimerID()
	{
		ms_nMaxId++;
		return ms_nMaxId;
	}

	TimerBase::TimerBase()
		: m_bIsRunning(false)
		, m_timerId(0)
		, m_timerType(TimerType::E_InAccuracy)
	{

	}

	TimerBase::~TimerBase()
	{
	}

	bool TimerBase::IsRunning()
	{
		return m_bIsRunning;
	}

	void TimerBase::SetTimerType(TimerType eType)
	{
		m_timerType = eType;
	}

	TimerID TimerBase::GetTimerID()
	{
		return m_timerId;
	}


	/*******************************  CTimer  ********************************/
	CTimer::CTimer(TimerContext* context, TimerID id)
		: m_nInterval(0)
		, m_bRepeated(false)
		, m_nTimes(0)
	{
		m_timerId = id;
		m_pContext = context;
	}

	CTimer::~CTimer() {
		if (m_pContext)
			m_pContext->Remove(m_timerId);
	}

	TimerSharedPtr CTimer::Create(TimerContext* context)
	{
		TimerContext* pCtx = context;
		if (pCtx == nullptr)
			pCtx = TimerContext::GetDefaultContext();

		TimerID timerId = context->GetNewTimerID();
		TimerSharedPtr sp = std::shared_ptr<CTimer>(new CTimer(context, timerId));
		pCtx->Add(sp);
		return sp;
	}

	void CTimer::Start(int msInterval, bool bRepeated)
	{
		Stop();

		if (msInterval > 0)
		{
			m_nInterval = msInterval;
			m_bRepeated = bRepeated;
			m_bIsRunning = true;
			m_timeStart = std::chrono::steady_clock::now();
		}
		else
		{
			std::cout << "CTimer:: Interval not big than 0!" << std::endl;
		}
	}

	void CTimer::Stop()
	{
		m_bIsRunning = false;
		m_nInterval = 0;
		m_bRepeated = false;

		m_nTimes = 0;
	}

	void CTimer::SetTimeoutCb(TimerCallback fun)
	{
		m_cbTimeout = fun;
	}

	void CTimer::OnDispatch()
	{
		TimePoint now = std::chrono::steady_clock::now();
		std::chrono::milliseconds dura = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_timeStart);
		if (m_timerType == TimerType::E_Accuracy)
		{
			//int nMod = dura.count() % m_nInterval;

			if (m_nTimes < (dura.count() / m_nInterval))
			{
				m_nTimes++;
				if (m_bRepeated == false)
					Stop();

				m_cbTimeout(this);
			}
		}
		else
		{
			if (dura.count() > m_nInterval)
			{
				if (m_timerId == 1) printf("CTimer:: Timer1 m_cbTimeout!\n");

				m_nTimes++;
				if (m_bRepeated == false)
					Stop();

				m_timeStart = now;
				m_cbTimeout(this);
			}
		}
	}

	unsigned long CTimer::GetTimeoutCount()
	{
		return m_nTimes;
	}
}