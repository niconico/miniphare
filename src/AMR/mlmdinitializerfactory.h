#ifndef MLMDINITIALIZERFACTORY_H
#define MLMDINITIALIZERFACTORY_H

#include "Initializer/ionsinitializer.h"
#include "Initializer/electromaginitializer.h"
#include "Initializer/ohminitializer.h"
#include "Initializer/solverinitializer.h"
#include "Initializer/initializerfactory.h"
#include "Plasmas/particles.h"
#include <AMR/patch.h>





/**
 * @brief The MLMDInitializerFactory class creates objects to initialize MLMD patches
 *
 * The factory can be used either for Patch initialization or Patch boundary conditions.
 * Patch initialization need an MLMD factory because fields, ions, Solver, etc. of a refined
 * patch are initialized from the Parent Patch and not from user inputs.
 * Methods such as createIonInitializer() and createElectromagInitializer()
 * can be used to initialier patch BCs.
 */
class MLMDInitializerFactory : public InitializerFactory
{
private:
    std::shared_ptr<Patch> parentPatch_;

public:
    MLMDInitializerFactory(std::shared_ptr<Patch> parentPatch) : parentPatch_{parentPatch}{}

    virtual std::unique_ptr<IonsInitializer> createIonsInitializer() const;
    virtual std::unique_ptr<ElectromagInitializer> createElectromagInitializer() const ;
    virtual std::unique_ptr<SolverInitializer> createSolverInitializer() const ;
    virtual std::unique_ptr<OhmInitializer> createOhmInitializer() const;

};

#endif // MLMDINITIALIZERFACTORY_H
