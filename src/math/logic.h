#pragma once




class Positive_Edge_Detector
{
public:
    Positive_Edge_Detector() : _previous(false){}
    bool Update(bool input)
    {
        if(input && !_previous)
        {
            _previous = input;
            return true;
        }
        else
        {
            _previous = input;
            return false;
        }
    }

private:
    bool _previous;
};
class Negative_Edge_Detector
{
public:
    Negative_Edge_Detector() : _previous(false){}
    bool Update(bool input)
    {
        if(!input && _previous)
        {
            _previous = input;
            return true;
        }
        else
        {
            _previous = input;
            return false;
        }
    }

private:
    bool _previous;
};
