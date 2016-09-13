#ifndef SPECIES_H
#define SPECIES_H

#include "Field/field.h"
#include "grid/gridlayout.h"
#include "particles.h"
#include "particleloader.h"


// load particles according to
//    - a profile
//    - a distribution strategy
/*

  ParticleLoader
  {
    DistributionStrategy;

    struct CellCenterPosition
    {
        double x,y,z;
    };
    std::vector<CellCenterPosition> cellpos; // is filled by who? layout.cellCenters()

    load()
    {

    }
  };

  */


class Species
{

private:
    GridLayout layout_;
    Field rho_;
    Field bulkVel_;
    std::vector<Particle> particleArray_;
    // ParticleLoader pload;

public:
    Species(GridLayout const& layout, std::string const& name);
    Species(GridLayout&& layout, std::string const& name);

    Species(Species const& source) = delete;
    Species& operator=(Species const& source)=delete;

    Species(Species&& source) = default;
    Species& operator=(Species&& source) = default;

    //~Species();

    void resetMoments() {rho_.zero(); bulkVel_.zero();}

    Field& rho() {return rho_;}
    Field const& rho() const {return rho_;}
    //Field& rho() {return rho_;}


#if 0
    Field* getChargeDensity();
    Field* getBulkVelocity();
    Field* getFLux();

    void computeChargeDensity();
    void computeFlux();
    void computeBulkVelocity();

    void dynamics();

private:
    Field* _rho;
    Field* _v;
    Field* _flux;
    Particles* _particle;
#endif

};

#endif // SPECIES_H
