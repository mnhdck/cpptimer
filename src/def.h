#pragma once
#pragma warning (disable : 4251)

#define TimerNamespace cutar
// windows export
#ifdef WIN32
#ifndef LIBTIMER_EXPORTS
#define TIMER_API _declspec(dllimport)
#else
#define TIMER_API _declspec(dllexport)
#endif 
#else
#define TIMER_API
#endif