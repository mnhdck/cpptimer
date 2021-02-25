/***********************************************************************************/
// Copyright (C) 2021 Meng Naihong.
//
// Licensed under the MIT License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
// Contact mengnaihong@foxmail.com for support.
/***********************************************************************************/
#pragma once
#include "def.h"
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace TimerNamespace
{
	class TimerBase;
	using TimerID = std::size_t;
	using TimerCallback = std::function<void(TimerBase*)>;
	using TimePoint = std::chrono::steady_clock::time_point;
	using TimerSharedPtr = std::shared_ptr<TimerBase>;

	class TIMER_API TimerContext
	{
	public:
		virtual ~TimerContext() {}

		static void DefaultLoop();
	
		static TimerContext* GetDefaultContext();

		static void SetDefaultContext(TimerContext* context);

		static TimerID GetNewTimerID();

		void Add(TimerSharedPtr timer);

		void Remove(TimerID id);	

		// must be called in your thread loop
		virtual void Loop();

	protected:
		std::unordered_map<TimerID, std::weak_ptr<TimerBase>> m_timers;
		static TimerContext* ms_context;
		static std::atomic<TimerID> ms_nMaxId;
	};

	// timer base
	class TIMER_API TimerBase
	{
	public:
		TimerBase();
		virtual ~TimerBase();
		enum TimerType
		{
			E_InAccuracy = 1,
			E_Accuracy,
		};

		virtual void Start(int msInterval, bool bRepeated = false) = 0;

		virtual void Stop() = 0;

		virtual void SetTimeoutCb(TimerCallback fun) = 0;


		bool IsRunning();

		TimerID GetTimerID();

		void SetTimerType(TimerType eType);

	protected:
		virtual void OnDispatch() = 0;

	protected:
		bool m_bIsRunning;
		TimerID m_timerId;
		TimerType m_timerType;
		TimerContext* m_pContext;
		friend TimerContext;
	};

	class TIMER_API CTimer :public TimerBase
	{
	public:
		~CTimer();

		static TimerSharedPtr Create(TimerContext* context = nullptr);

		virtual void Start(int msInterval, bool bRepeated);

		virtual void Stop();
	
		virtual void SetTimeoutCb(TimerCallback fun);

		unsigned long GetTimeoutCount();

	protected:
		virtual void OnDispatch();

	private:
		CTimer(TimerContext* context, TimerID id);

	private:
		TimePoint m_timeStart;
		TimerCallback m_cbTimeout;
		int m_nInterval;
		bool m_bRepeated;
		unsigned long m_nTimes;
	};
}