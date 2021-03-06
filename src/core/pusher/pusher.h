#ifndef PUSHER_H
#define PUSHER_H

#include "core/BoundaryConditions/boundary_conditions.h"
#include "core/BoundaryConditions/leavingparticles.h"
#include "core/Interpolator/interpolator.h"
#include "data/Plasmas/particles.h"
#include "data/grid/gridlayout.h"
#include "data/vecfield/vecfield.h"

/**
 * @brief Abstract interface for Pushers.
 *
 * Specific pushers will need to define the move method.
 * LeavingParticles object is put here because needed by all pushers.
 */
class Pusher
{
protected:
    uint32 nbdims_;
    GridLayout layout_;
    std::string pusherType_;
    double dt_;
    LeavingParticles leavingParticles_;

public:
    Pusher(GridLayout layout, std::string pusherType, double dt)
        : nbdims_{layout.nbDimensions()}
        , layout_{std::move(layout)}
        , pusherType_{pusherType}
        , dt_{dt}
        , leavingParticles_{layout_}
    {
    }

    Pusher(Pusher const& source) = delete;
    Pusher& operator=(Pusher const& source) = delete;

    Pusher(Pusher&& toMove) = default;
    Pusher& operator=(Pusher&& source) = default;

    // Dont't forget =default HERE
    // or move operations won't be generated
    virtual ~Pusher() = default;

    virtual void move(std::vector<Particle> const& partIn, std::vector<Particle>& partOut, double m,
                      VecField const& E, VecField const& B, Interpolator& interpolator,
                      BoundaryCondition& boundaryCondition)
        = 0;


    double dt() const { return dt_; }

    std::string const& pusherType() const { return pusherType_; }

    LeavingParticles const& getLeavingParticles() const { return leavingParticles_; }
};


#endif // PUSHER_H
