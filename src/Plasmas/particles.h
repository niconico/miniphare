#ifndef PARTICLES_H
#define PARTICLES_H

#include <array>

#include "types.h"

class Particle
{
public:
    double weight ;  // variable weight

    double charge ;

    std::array<uint32, 3> icell ;   // node coordinate

    std::array<float, 3> delta ;   // value in [0, 1] in each direction

//    std::array<double, 3> position ;

    std::array<double, 3> v ;      // velocity in each direction

    double Ex, Ey, Ez ;    // electric field at the particle position
    double Bx, By, Bz ;    // magnetic field at the particle position


public:
    Particle( double weight, double charge,
              std::array<uint32, 3> icell,
              std::array<float, 3> delta,
              std::array<double, 3> v   );

//    Particle( double weight, double charge,
//              std::array<double, 3> position,
//              std::array<double, 3> v   );

};



#endif // PARTICLES_H

