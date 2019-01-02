#pragma once



class PI_Controller
{
public:
    PI_Controller(float sampling_time,
                  float gain_proportional,
                  float time_constant_inverse,
                  float gain_correction,
                  float feed_fwd,
                  float limit_lower,
                  float limit_upper ):
        _sampling_time(sampling_time),
        _gain_proportional(gain_proportional),
        _time_constant_inverse(time_constant_inverse),
        _gain_correction(gain_correction),
        _feed_fwd_stat(feed_fwd),
        _limit_lower(limit_lower),
        _limit_upper(limit_upper),
        _integral_part(0),
        _anti_windup(0)
    {}

    float Update(float error);
    float Update(float error, float feed_fwd_dyn);

    void Reset(float integral_part = 0);


private:
    float _sampling_time;
    float _gain_proportional;
    float _time_constant_inverse;
    float _gain_correction;
    float _feed_fwd_stat;
    float _limit_lower;
    float _limit_upper;


    float _integral_part;
    float _anti_windup;

};
