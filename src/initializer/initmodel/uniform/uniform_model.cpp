

#include "uniform_model.h"
#include <initializer/fluidparticleinitializer.h>


void UniformModel::setNbrSpecies(uint32 nbrSpecies)
{
    n0_.resize(nbrSpecies);
    vx_.resize(nbrSpecies);
    vy_.resize(nbrSpecies);
    vz_.resize(nbrSpecies);
    Vth_.resize(nbrSpecies);
    speciesCharges_.resize(nbrSpecies);
    speciesMasses_.resize(nbrSpecies);
    nbrParticlesPerCell_.resize(nbrSpecies);
}

void UniformModel::setB0(double bx, double by, double bz)
{
    bx_ = bx;
    by_ = by;
    bz_ = bz;
}


void UniformModel::setE0(double ex, double ey, double ez)
{
    ex_ = ex;
    ey_ = ey;
    ez_ = ez;
}

void UniformModel::setDensity(double n, uint32 speciesIndex)
{
    n0_[static_cast<std::size_t>(speciesIndex)] = n;
}


void UniformModel::setV0(double vx, double vy, double vz, uint32 speciesIndex)
{
    vx_[static_cast<std::size_t>(speciesIndex)] = vx;
    vy_[static_cast<std::size_t>(speciesIndex)] = vy;
    vz_[static_cast<std::size_t>(speciesIndex)] = vz;
}


void UniformModel::setVth(double Vth, uint32 speciesIndex)
{
    Vth_[static_cast<std::size_t>(speciesIndex)] = Vth;
}

void UniformModel::setMass(double mass, uint32 speciesIndex)
{
    speciesMasses_[static_cast<std::size_t>(speciesIndex)] = mass;
}

void UniformModel::setCharges(double charge, uint32 speciesIndex)
{
    speciesCharges_[static_cast<std::size_t>(speciesIndex)] = charge;
}

void UniformModel::setNbrParticlesPerCell(uint32 nbr, uint32 speciesIndex)
{
    nbrParticlesPerCell_[static_cast<std::size_t>(speciesIndex)] = nbr;
}



std::unique_ptr<ScalarFunction> UniformModel::density(uint32 speciesIndex) const
{
    return std::unique_ptr<ScalarFunction>{new UniformScalarFunction{n0_[speciesIndex]}};
}



std::unique_ptr<ScalarFunction> UniformModel::thermalSpeed(uint32 speciesIndex) const
{
    return std::unique_ptr<ScalarFunction>{new UniformScalarFunction{Vth_[speciesIndex]}};
}


std::unique_ptr<VectorFunction> UniformModel::bulkVelocity(uint32 speciesIndex) const
{
    return std::unique_ptr<VectorFunction>{
        new UniformVectorFunction{vx_[speciesIndex], vy_[speciesIndex], vz_[speciesIndex]}};
}



uint32 UniformModel::nbParticlesPerCell(uint32 speciesIndex) const
{
    return nbrParticlesPerCell_[speciesIndex];
}


std::vector<uint32> UniformModel::nbrsParticlesPerCell() const
{
    return nbrParticlesPerCell_;
}



std::vector<std::unique_ptr<ScalarFunction>> UniformModel::densities_() const
{
    std::vector<std::unique_ptr<ScalarFunction>> densityFunctions;
    uint32 nbrSpecies = static_cast<uint32>(speciesMasses_.size());

    for (uint32 speciesIndex = 0; speciesIndex < nbrSpecies; ++speciesIndex)
    {
        densityFunctions.push_back(density(speciesIndex));
    }
    return densityFunctions;
}



std::vector<std::unique_ptr<ScalarFunction>> UniformModel::thermalSpeeds_() const
{
    std::vector<std::unique_ptr<ScalarFunction>> thermalSpeedFunctions;
    uint32 nbrSpecies = static_cast<uint32>(speciesMasses_.size());

    for (uint32 speciesIndex = 0; speciesIndex < nbrSpecies; ++speciesIndex)
    {
        thermalSpeedFunctions.push_back(thermalSpeed(speciesIndex));
    }
    return thermalSpeedFunctions;
}



std::vector<std::unique_ptr<VectorFunction>> UniformModel::bulkVelocities_() const
{
    std::vector<std::unique_ptr<VectorFunction>> bulkVelocityFunctions;
    uint32 nbrSpecies = static_cast<uint32>(speciesMasses_.size());

    for (uint32 speciesIndex = 0; speciesIndex < nbrSpecies; ++speciesIndex)
    {
        bulkVelocityFunctions.push_back(bulkVelocity(speciesIndex));
    }
    return bulkVelocityFunctions;
}



std::vector<std::unique_ptr<ParticleInitializer>> UniformModel::particleInitializers() const
{
    std::vector<std::unique_ptr<ScalarFunction>> densities      = densities_();
    std::vector<std::unique_ptr<ScalarFunction>> thermalSpeeds  = thermalSpeeds_();
    std::vector<std::unique_ptr<VectorFunction>> bulkVelocities = bulkVelocities_();

    std::size_t nbrSpecies = densities.size();

    std::vector<std::unique_ptr<ParticleInitializer>> partInits;

    for (std::size_t speciesIndex = 0; speciesIndex < nbrSpecies; ++speciesIndex)
    {
        std::unique_ptr<ParticleInitializer> pinit{new FluidParticleInitializer{
            layout_, std::move(densities[speciesIndex]), std::move(bulkVelocities[speciesIndex]),
            std::move(thermalSpeeds[speciesIndex]), nbrParticlesPerCell_[speciesIndex],
            speciesCharges_[speciesIndex]}};

        partInits.push_back(std::move(pinit));
    }
    return partInits;
}



std::unique_ptr<VectorFunction> UniformModel::electricFunction() const
{
    return std::unique_ptr<VectorFunction>{new UniformVectorFunction{ex_, ey_, ez_}};
}


std::unique_ptr<VectorFunction> UniformModel::magneticFunction() const
{
    return std::unique_ptr<VectorFunction>{new UniformVectorFunction{bx_, by_, bz_}};
}