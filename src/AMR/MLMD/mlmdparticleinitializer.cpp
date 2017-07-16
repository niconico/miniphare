
#include "AMR/MLMD/mlmdparticleinitializer.h"
#include "AMR/particleselector.h"




void MLMDParticleInitializer::loadParticles(std::vector<Particle>& particlesArray) const
{
    ParticleSelector const& selector = *selector_;

    Particle normalizedMother;

    // the ParticleInitializer has a private access to the ion of the Parent Patch
    for (Particle const& mother : particleSource_.particles())
    {
        // look if the 'big' particle is within the Patch domain
        if (selector.pick(mother))
        {
            SplittingStrategy::normalizeMotherPosition(coarseLayout_, refinedLayout_, mother,
                                                       normalizedMother);

            std::vector<Particle> childParticles;
            // We need to split particle and grab its children
            strategy_->split1D(normalizedMother, childParticles);

            // For the considered Species
            // we fill the particle array of the new patch
            for (Particle const& child : childParticles)
            {
                particlesArray.push_back(child);
            }
        }
    }
}
