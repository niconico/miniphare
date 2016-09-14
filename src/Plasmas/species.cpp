#include "species.h"

/*
Species::Species(GridLayout&& layout, std::string const& name)

    : layout_{ std::move(layout) },
      rho_{ layout_.nx(), layout_.ny(), layout_.nz(), "rho_"+name },
      bulkVel_{ layout_.nx(), layout_.ny(), layout_.nz(),"bulkVel_"+name }
{

}
*/


Species::Species(GridLayout const& layout, double mass,
                 ParticleInitializer const& particleInitializer,
                 std::string const& name)
    : layout_{ layout },
      rho_    { layout_.nx(), layout_.ny(), layout_.nz(), "rho_"   + name },
      bulkVel_{ layout_.nx(), layout_.ny(), layout_.nz(),"bulkVel_"+ name },
      particleArray_{},
      particleInitializer_{ particleInitializer.clone() }
{

}



void Species::loadParticles()
{
    particleInitializer_->loadParticles(particleArray_);
}
