
#include <cmath>
#include <memory>

#include "core/Faraday/faradayfactory.h"
#include "core/Interpolator/interpolator.h"
#include "core/Interpolator/particlemesh.h"
#include "core/Solver/solver.h"
#include "core/pusher/pusherfactory.h"

#include "data/Field/field.h"
#include "data/Plasmas/electrons.h"
#include "data/Plasmas/ions.h"
#include "data/grid/gridlayout.h"
#include "data/vecfield/vecfieldoperations.h"



Solver::Solver(GridLayout const& layout, double dt,
               std::unique_ptr<SolverInitializer> solverInitializer)
    : layout_{layout}
    , EMFieldsPred_{{{layout.allocSize(HybridQuantity::Ex), layout.allocSize(HybridQuantity::Ey),
                      layout.allocSize(HybridQuantity::Ez)}},

                    {{layout.allocSize(HybridQuantity::Bx), layout.allocSize(HybridQuantity::By),
                      layout.allocSize(HybridQuantity::Bz)}},
                    "_pred"}
    ,

    EMFieldsAvg_{{{layout.allocSize(HybridQuantity::Ex), layout.allocSize(HybridQuantity::Ey),
                   layout.allocSize(HybridQuantity::Ez)}},

                 {{layout.allocSize(HybridQuantity::Bx), layout.allocSize(HybridQuantity::By),
                   layout.allocSize(HybridQuantity::Bz)}},
                 "_avg"}
    ,

    Jtot_{layout.allocSize(HybridQuantity::Ex),
          layout.allocSize(HybridQuantity::Ey),
          layout.allocSize(HybridQuantity::Ez),
          {{HybridQuantity::Ex, HybridQuantity::Ey, HybridQuantity::Ez}},
          "Jtot"}
    , faraday_{dt, layout}
    , ampere_{layout}
    , ohm_{layout}
    , interpolator_{solverInitializer->interpolationOrder}
    , pusher_{PusherFactory::createPusher(layout, solverInitializer->pusherType, dt)}

{
}




/**
 * @brief Solver::init  ueses interpolators to initialize species moments
 * @param ions
 */
void Solver::init(Ions& ions, BoundaryCondition const& boundaryCondition)
{
    for (uint32 iSpe = 0; iSpe < ions.nbrSpecies(); ++iSpe)
    {
        Species& species                 = ions.species(iSpe);
        std::vector<Particle>& particles = species.particles();

        computeChargeDensityAndFlux(interpolator_, species, layout_, particles);
    }

    ions.computeChargeDensity();
    boundaryCondition.applyDensityBC(ions.rho());

    boundaryCondition.applyFluxBC(ions);
    ions.computeBulkVelocity();
}




void Solver::solveStepPPC(Electromag& EMFields, Ions& ions, Electrons& electrons,
                          BoundaryCondition& boundaryCondition)
{
    VecField& B     = EMFields.getB();
    VecField& E     = EMFields.getE();
    VecField& Bpred = EMFieldsPred_.getB();
    VecField& Epred = EMFieldsPred_.getE();
    VecField& Bavg  = EMFieldsAvg_.getB();
    VecField& Eavg  = EMFieldsAvg_.getE();


    // -----------------------------------------------------------------------
    //
    //                              PREDICTOR 1
    //
    // -----------------------------------------------------------------------

    // Get B^{n+1} pred1 from E^n
    faraday_(E, B, Bpred);
    boundaryCondition.applyMagneticBC(Bpred);

    // Compute J
    ampere_(Bpred, Jtot_);
    boundaryCondition.applyCurrentBC(Jtot_);


    // --> Get electron moments at time n
    VecField const& Vepred1 = electrons.bulkVel(ions.bulkVel(), ions.rho(), Jtot_);
    Field const& PePred1    = electrons.pressure(ions.rho());

    // --> Get electric field E_{n+1} pred1 from Ohm's law
    ohm_(Bpred, ions.rho(), Vepred1, PePred1, Jtot_, Epred);
    boundaryCondition.applyElectricBC(Epred);


    // Get time averaged prediction (E,B)^{n+1/2} pred1
    // using (E^n, B^n) and (E^{n+1}, B^{n+1}) pred1
    timeAverage(E, Epred, Eavg);
    timeAverage(B, Bpred, Bavg);

    // Move ions from n to n+1 using (E^{n+1/2},B^{n+1/2}) pred 1
    // accumulate moments for each species and total ions.
    // last argument is 'predictor1_' so that particles at n+1 are stored
    // in a temporary buffer and particles at n are kept at n
    moveIons_(Eavg, Bavg, ions, boundaryCondition, predictor1_);


    // -----------------------------------------------------------------------
    //
    //                         PREDICTOR 2
    //
    // -----------------------------------------------------------------------

    // Get B^{n+1} pred2 from E^{n+1/2} pred1
    faraday_(Eavg, B, Bpred);
    boundaryCondition.applyMagneticBC(Bpred);

    // Eavg.zero();

    // Compute J
    ampere_(Bpred, Jtot_);
    boundaryCondition.applyCurrentBC(Jtot_);


    // --> Get electron moments at time n with Pred1 ion moments
    VecField const& Vepred2 = electrons.bulkVel(ions.bulkVel(), ions.rho(), Jtot_);
    Field const& PePred2    = electrons.pressure(ions.rho());

    // --> Get electric field E^{n+1} pred2 from Ohm's law
    // --> using (n^{n+1}, u^{n+1}) pred and B_{n+1} pred2
    ohm_(Bpred, ions.rho(), Vepred2, PePred2, Jtot_, Epred);
    boundaryCondition.applyElectricBC(Epred);


    // --> Get time averaged prediction (E^(n+1/2),B^(n+1/2)) pred2
    // --> using (E^n, B^n) and (E^{n+1}, B^{n+1}) pred2
    timeAverage(E, Epred, Eavg);
    timeAverage(B, Bpred, Bavg);


    // Get the CORRECTED positions and velocities
    // Move ions from n to n+1 using (E^{n+1/2},B^{n+1/2}) pred2
    // Last argument here is 'predictor2_' because we want to
    // update ions at n+1 in place, i.e. overwritting ions at n
    moveIons_(Eavg, Bavg, ions, boundaryCondition, predictor2_);

    // -----------------------------------------------------------------------
    //
    //                           CORRECTOR
    //
    // -----------------------------------------------------------------------

    // Get CORRECTED B^{n+1} from E^{n+1/2} pred2
    faraday_(Eavg, B, B);
    boundaryCondition.applyMagneticBC(B);

    // Compute J
    ampere_(B, Jtot_);
    boundaryCondition.applyCurrentBC(Jtot_);


    // --> Get electron moments at time n with Pred2 ion moments
    VecField const& Vecorr = electrons.bulkVel(ions.bulkVel(), ions.rho(), Jtot_);
    Field const& Pecorr    = electrons.pressure(ions.rho());

    // --> Get CORRECTED electric field E^{n+1} from Ohm's law
    // --> using (n^{n+1}, u^{n+1}) cor and B_{n+1} cor
    ohm_(B, ions.rho(), Vecorr, Pecorr, Jtot_, E);
    // BC Fields --> Apply boundary conditions on the electric field
    boundaryCondition.applyElectricBC(E);
}




// convenience function that counts the maximum number of particles over
// all species. this is useful to allocated the temporary particle buffer
std::vector<Particle>::size_type maxNbrParticles(Ions const& ions)
{
    // find the largest particles number accross all species
    auto nbrParticlesMax = ions.species(0).nbrParticles();
    auto nbrSpecies      = ions.nbrSpecies();

    for (uint32 ispe = 0; ispe < nbrSpecies; ++ispe)
    {
        Species const& species = ions.species(ispe);
        auto nbrParticles      = species.nbrParticles();

        if (nbrParticlesMax < nbrParticles)
        {
            nbrParticlesMax = nbrParticles;
        }
    }
    return nbrParticlesMax;
}




// this routine move the ions for all species, accumulate their moments
// and compute the total ion moments.
void Solver::moveIons_(VecField const& E, VecField const& B, Ions& ions,
                       BoundaryCondition& boundaryCondition, uint32 predictorStep)
{
    // the temporary buffer must be big enough to hold the max
    // number of particles
    auto nbrParticlesMax = maxNbrParticles(ions);
    if (particleArrayPred_.size() < nbrParticlesMax)
    {
        particleArrayPred_.resize(nbrParticlesMax);
    }


    for (uint32 ispe = 0; ispe < ions.nbrSpecies(); ++ispe)
    {
        Species& species                 = ions.species(ispe);
        std::vector<Particle>& particles = species.particles();


        // at the first predictor step we must not overwrite particles
        // at t=n with particles at t=n+1 because time n will be used
        // in the second push. we rather put particles at time n+1 in a
        // temporary buffer particleArrayPred_
        if (predictorStep == predictor1_)
        {
            // resize the buffer so that charge density and fluxes use
            // no more than the right number of particles
            // particleArrayPred_ has a capacity that is large enough for all
            // particle arrays for all species.
            particleArrayPred_.resize(particles.size());


            // move all particles of that species from n to n+1
            // and put the advanced particles in the predictor buffer 'particleArrayPred_'
            pusher_->move(particles, particleArrayPred_, species.mass(), E, B, interpolator_,
                          boundaryCondition);

            // we do not update GCA particles (flag set to false)
            boundaryCondition.applyIncomingParticleBC(particleArrayPred_, pusher_->pusherType(),
                                                      pusher_->dt(), species.name(), false);

            computeChargeDensityAndFlux(interpolator_, species, layout_, particleArrayPred_);
        }

        // we're at pred2, so we can update particles in place as we won't
        // need their properties at t=n anymore
        else if (predictorStep == predictor2_)
        {
            // move all particles of that species from n to n+1
            pusher_->move(particles, particles, species.mass(), E, B, interpolator_,
                          boundaryCondition);

            // ------------------------------------------------------
            //                INCOMING PARTICLE BC
            // ------------------------------------------------------
            // we update GCA particles (flag set to true)
            boundaryCondition.applyIncomingParticleBC(particles, pusher_->pusherType(),
                                                      pusher_->dt(), species.name(), true);

            computeChargeDensityAndFlux(interpolator_, species, layout_, particles);
        }



    } // end loop on species


    ions.computeChargeDensity();
    boundaryCondition.applyDensityBC(ions.rho());

    boundaryCondition.applyFluxBC(ions);
    ions.computeBulkVelocity();
}
