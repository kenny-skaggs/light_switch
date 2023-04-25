#pragma once

class Timer
{
    public:
    void setGoalTime(unsigned long time)
    {
        goalTime = time;
    }

    bool isGoalTimeMet()
    {
        return millis() > goalTime;
    }

    private:
    unsigned long goalTime = 0;
};
