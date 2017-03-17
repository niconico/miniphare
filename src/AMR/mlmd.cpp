
#include <cmath>

#include "mlmd.h"
#include "hierarchy.h"
#include "patch.h"
#include "patchdata.h"
#include "Solver/solver.h"




MLMD::MLMD(InitializerFactory const& initFactory)
    : baseLayout_{ GridLayout{initFactory.gridLayout()} },
      patchHierarchy_{ std::make_shared<Patch>(
                           initFactory.getBox(), baseLayout_,
                           PatchData{initFactory}  ) }
{
    // will probably have to change the way objects are initialized.
    // if we want, at some point, start from an already existing hierarchy
    // (in case of restart for e.g.
}




void MLMD::initializeRootLevel()
{
    std::cout << "building root level...";
    Patch& rootLevel = patchHierarchy_.root();
    rootLevel.init();
    std::cout << " OK" << std::endl;

}


void MLMD::evolveFullDomain()
{

    // evolve fields and particle for a time step
    patchHierarchy_.evolveHierarchy() ;

#if 1
    // Here, AMR patches will say whether they need refinement
    // the ouput of this method is used by updateHierarchy()
    // Note for later: will probably not be called every time step.
    std::vector< std::vector<RefinementInfo> > refinementTable
            = patchHierarchy_.evaluateHierarchy( refinementRatio_, baseLayout_ ) ;

    // New patches are created here if necessary
    // it depends on evaluateHierarchy()
    patchHierarchy_.updateHierarchy( refinementTable ) ; // , refinedLayouts
#endif

}




