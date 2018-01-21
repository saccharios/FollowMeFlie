#pragma once

template <typename T>
class Double_Buffer_Bidirectional
{
    public:
        Double_Buffer_Bidirectional():
            _buffer1 (),
            _buffer2 (),
            _side_a_pointer(& _buffer2),
            _side_b_pointer(& _buffer1)
        {}
        
        Double_Buffer_Bidirectional(T const & init_value):
            _buffer1 (init_value),
            _buffer2 (init_value),
            _side_a_pointer (& _buffer2),
            _side_b_pointer (& _buffer1)
        {}

        void
        swap()
        {
            T * tmp = _side_b_pointer;
            _side_b_pointer = _side_a_pointer;
            _side_a_pointer = tmp;
        }

        T &
        side_a()
        {
            return *_side_a_pointer;
        };

        T const &
        side_a() const
        {
            return *_side_a_pointer;
        };

        T &
        side_b()
        {
            return *_side_b_pointer;
        }

        T const &
        side_b() const
        {
            return *_side_b_pointer;
        }

    private:
        T
        _buffer1;
        
        T
        _buffer2;
        
        T *
        _side_a_pointer;
        
        T *
        _side_b_pointer;
};

template <typename T> class
Double_Buffer : private Double_Buffer_Bidirectional <T>
{
    public:
        using Double_Buffer_Bidirectional<T>::Double_Buffer_Bidirectional;

        T &
        write()
        {
            return Double_Buffer_Bidirectional<T>::side_a();
        };

        T const &
        read() const
        {
            return Double_Buffer_Bidirectional<T>::side_b();
        }

        void
        swap()
        {
            Double_Buffer_Bidirectional<T>::swap();
        }


};
