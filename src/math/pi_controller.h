#pragma once



class PI_Controller
{
public:
    PI_Controller(double sampling_time,
                  double gain_proportional,
                  double time_constant_inverse,
                  double gain_correction,
                  double feed_fwd,
                  double limit_lower,
                  double limit_upper
                  ) :
        _sampling_time(sampling_time),
        _gain_proportional(gain_proportional),
        _time_constant_inverse(time_constant_inverse),
        _gain_correction(gain_correction),
        _feed_fwd(feed_fwd),
        _limit_lower(limit_lower),
        _limit_upper(limit_upper),
        _integral_part(0),
        _anti_windup(0)
    {}

    double Update(double error);

    void Reset();


private:
    double _sampling_time;
    double _gain_proportional;
    double _time_constant_inverse;
    double _gain_correction;
    double _feed_fwd;
    double _limit_lower;
    double _limit_upper;


    double _integral_part;
    double _anti_windup;

};
