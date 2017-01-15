#ifndef PUSHER1D_H
#define PUSHER1D_H


#include "pusher/pusher.h"



class Pusher1D : public Pusher
{
private:



public:
    Pusher1D( std::unique_ptr<PusherType> impl )
        : Pusher( std::move(impl) ) {}

//    Pusher1D(Pusher1D const& source) = delete;
//    Pusher1D& operator=(Pusher1D const& source) = delete;

//    Pusher1D(Pusher1D&& toMove)      = default;
//    Pusher1D& operator=(Pusher1D&& source) = default;

    virtual ~Pusher1D() {}

    virtual void move(std::vector<Particle> & partIn ,
                      std::vector<Particle> & partOut,
                      double dt, double m,
                      VecField const & E ,
                      VecField const & B ,
                      Interpolator & interpolator ) override
    {
        impl_->move1D( partIn, partOut,
                       dt, m, E, B,
                       interpolator ) ;
    }

};



#endif // PUSHER1D_H