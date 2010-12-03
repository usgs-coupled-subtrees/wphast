#include "StdAfx.h"
#include "StopWatch.h"

CStopWatch::CStopWatch(void)
{
	this->timer.start.QuadPart = 0;
	this->timer.stop.QuadPart  = 0;
	::QueryPerformanceFrequency(&this->frequency);
}

double CStopWatch::LIToSecs(LARGE_INTEGER &l)
{
	return ((double)l.QuadPart/(double)this->frequency.QuadPart);
}

void CStopWatch::startTimer(void)
{
	::QueryPerformanceCounter(&timer.start);
}

void CStopWatch::stopTimer(void)
{
	::QueryPerformanceCounter(&this->timer.stop);
}

double CStopWatch::getElapsedTime(void)
{
	LARGE_INTEGER time;
	time.QuadPart = this->timer.stop.QuadPart - this->timer.start.QuadPart;
    return this->LIToSecs(time);
}
