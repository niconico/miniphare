#ifndef PATCHBOUNDARYCONDITION_H
#define PATCHBOUNDARYCONDITION_H

#include "AMR/patch.h"

#include "BoundaryConditions/boundary_conditions.h"
#include "BoundaryConditions/boundary.h"

#include "Initializer/initializerfactory.h"

#include <memory>




/**
 * @brief The PatchBoundaryCondition class
 *
 * PatchBoundaryCondition constructor basically needs:
 * + a PRA struct (containing innerBox and outerBox)
 * + a pointer to the coarse (parent) patch
 * + a copy of the coarse (parent) patch
 *
 *
 */
class PatchBoundaryCondition : public BoundaryCondition
{

private:
    PRA refinedPRA_;
    std::shared_ptr<Patch> parent_;

    // these boundaries know what they are : patchboundary
    std::vector<std::unique_ptr<Boundary>> boundaries_;
    GridLayout layout_;


public:
    PatchBoundaryCondition( PRA const & refinedPRA, std::shared_ptr<Patch> coarsePatch,
                            GridLayout const & coarseLayout )
        : refinedPRA_{refinedPRA}, parent_{coarsePatch},
          boundaries_{}, layout_{coarseLayout} {}

    virtual void applyMagneticBC(VecField& B) const override;
    virtual void applyElectricBC(VecField& E) const override;
    virtual void applyCurrentBC(VecField& J)  const override;
    virtual void applyDensityBC(Field& N)     const override;
    virtual void applyBulkBC(VecField& Vi)    const override;
    virtual void applyParticleBC(std::vector<Particle>& particleArray,
                                 LeavingParticles const& leavingParticles)  const override;

};

#endif // PATCHBOUNDARYCONDITION_H
