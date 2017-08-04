#pragma once

class Lowpass
{
public:
    Lowpass (double sampling_time,double  time_constant) :
        _sampling_time(sampling_time),
        _state(0.0)
    {
        _coefficient = 1.0/(time_constant/sampling_time +1.0);
    }

    double Update(double input);
    void Reset(double state) {_state  = state;}

private:
    double _sampling_time;
    double _coefficient;
    double _state;
};
