#pragma once


template<unsigned int NDelay>
class OnDelay
{
public:
   void Activate(bool in)
   {
      _input = in;
   }

   bool Value()
   {
       if(!_input)
       {
           cntr = 0;
       }
       else if(cntr < NDelay)
       {
           ++cntr;
       }
       return (cntr >= NDelay);
   }

private:
    unsigned int cntr = 0;
    bool _input = false;
};
