#ifndef SPECIES_H
#define SPECIES_H

#include "data/Field/field.h"
#include "data/vecfield/vecfield.h"
#include "particleinitializer.h"
#include "particles.h"


class GridLayout;



/**
 * @brief The Species class contains particles and moments of a specific
 * species of Particle. A Species is a heavy object (it contains particles)
 * and is therefore a move only object.
 */
class Species
{
private:
    double mass_;
    std::string name_;
    Field rho_;
    VecField flux_;
    std::vector<Particle> particleArray_;
    std::unique_ptr<ParticleInitializer> particleInitializer_;


public:
    Species(GridLayout const& layout, double mass,
            std::unique_ptr<ParticleInitializer> particleInitializer, std::string const& name);

    Species(Species const& source) = delete;
    Species& operator=(Species const& source) = delete;

    Species(Species&& source) = default;
    Species& operator=(Species&& source) = default;

    void resetMoments()
    {
        rho_.zero();
        flux_.zero();
    }

    void resetParticles() { particleArray_.clear(); }

    Field& rho() { return rho_; }
    Field const& rho() const { return rho_; }

    VecField& flux() { return flux_; }
    VecField const& flux() const { return flux_; }

    Field& flux(uint32 iComponent) { return flux_.component(iComponent); }
    Field const& flux(uint32 iComponent) const { return flux_.component(iComponent); }

    std::vector<Particle>& particles() { return particleArray_; }
    std::vector<Particle> const& particles() const { return particleArray_; }

    double mass() const { return mass_; }
    std::string name() const { return name_; }
    std::vector<Particle>::size_type nbrParticles() const { return particleArray_.size(); }

    void loadParticles();

    // void compute1DChargeDensityAndFlux(Interpolator & project );
};

#endif // SPECIES_H
