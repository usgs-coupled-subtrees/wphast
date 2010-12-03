#pragma once

typedef struct {
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
} stopWatch;

class CStopWatch
{
public:
	CStopWatch(void);
	void startTimer(void);
	void stopTimer(void);
	double getElapsedTime(void);
private:
	stopWatch timer;
	LARGE_INTEGER frequency;
	double LIToSecs(LARGE_INTEGER &l);
};
