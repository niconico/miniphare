
#include <tuple>
#include <memory>

#include "grid/gridlayout.h"

#include "AMR/hierarchy.h"
#include "AMR/patch.h"
#include "AMR/mlmdinitializerfactory.h"

#include "Splitting/splittingstrategy.h"

#include "utilityphare.h"





/**
 * @brief Hierarchy::evolveHierarchy
 *
 * evolve fields and particle for a time step
 *
 */
void Hierarchy::evolveDomainForOneTimeStep()
{
    uint32 nbrLevels = static_cast<uint32>(patchTable_.size());

    // TODO need to include all interactions with children
    // https://hephaistos.lpp.polytechnique.fr/redmine/projects/hyb-par/wiki/BoundaryConditions#MLMD-time-integration

    for( uint32 iLevel=0 ; iLevel<nbrLevels ; iLevel++ )
    {
        uint32 nbrPatches = static_cast<uint32>(patchTable_[iLevel].size());

        for (uint32 iPatch=0; iPatch < nbrPatches; ++iPatch)
        {
            patchTable_[iLevel][iPatch]->evolve();
        }
    } // end loop on levels
}


/**
 * @brief Hierarchy::evaluateHierarchy
 *
 * Here, AMR patches will say whether they need refinement
 * the ouput of this method is used by updateHierarchy()
 *
 */
std::vector< std::vector<RefinementInfo> >
Hierarchy::evaluateRefinementNeed( uint32 refineRatio,
                              GridLayout const & baseLayout )
{

    std::vector< std::vector<RefinementInfo> > refinementTable ;

    uint32 nbrLevels = static_cast<uint32>(patchTable_.size()) ;

    for( uint32 iLevel=0 ; iLevel<nbrLevels ; iLevel++ )
    {
        std::vector< std::shared_ptr<Patch> > const & patchesAtLevel = patchTable_[iLevel] ;

        for( uint32 iPatch=0 ; iPatch<patchesAtLevel.size() ; ++iPatch )
        {
            auto const & patch = patchesAtLevel[iPatch] ;

            RefinementAnalyser analyser{} ;

            analyser( patch->data() ) ;

            // if the patch has to be refined we store a reference
            // for further use
            if( analyser.refinementNeeded() )
            {
                std::vector<Box> const & refinedList = analyser.refinedDomains() ;

                // TODO: design might change
                // We could decide to build a refinementVector
                // directly in analyser.refinedDomains()
                for( Box const & domain: refinedList )
                {
                    RefinementInfo refine{ patch, domain,
                                iLevel+1, refineRatio, baseLayout } ;

                    refinementTable[iLevel].push_back(std::move(refine)) ;
                }
            }
        }
    }

    return refinementTable ;
}



/**
 * @brief Hierarchy::updateHierarchy
 *
 * New patches are created here depending on the content
 * of refinementTable (and layoutTable)
 * These two tables have the same number of elements
 *
 * @param refinementTable
 * @param layoutTable
 */
void Hierarchy::updateHierarchy(
        std::vector< std::vector<RefinementInfo> > const & refinementTable,
        uint32 const & refineFactor,
        std::vector<uint32> const & orders,
        std::string const & pusher,
        std::vector<std::string> const & splitMethod )
{

    uint32 nbrLevels = static_cast<uint32>(refinementTable.size()) ;

    for( uint32 iLevel=0 ; iLevel<nbrLevels ; ++iLevel )
    {
        uint32 nbrPatches = static_cast<uint32>( refinementTable[iLevel].size() ) ;

        for( uint32 iPatch=0 ; iPatch<nbrPatches ; ++iPatch )
        {
            // GridLayout const & layout = layoutTable[iLevel][iPatch] ;
            RefinementInfo const & info = refinementTable[iLevel][iPatch] ;

            // create new Patch and update Hierarchy
            addNewPatch( info, refineFactor,
                         orders, pusher,
                         splitMethod ) ;

            // TODO: call patch.init to initialize patch content


        }
    }

}



void Hierarchy::addNewPatch( RefinementInfo const & info,
                             uint32 const & refineFactorFromMLMD,
                             std::vector<uint32> const & ordersFromMLMD,
                             std::string const & pusherFromMLMD,
                             std::vector<std::string> const & splitMethodFromMLMD)
{

    std::shared_ptr<Patch> coarsePatch = info.parentPatch ;
    Box refinedBox = info.refinedDomain ;
    uint32 refinedLevel = info.level ;

    GridLayout refinedLayout = buildLayout_( info ) ;

    // we need to build a factory for PatchData to be built
    std::unique_ptr<InitializerFactory>
            factory { new MLMDInitializerFactory( coarsePatch, refinedBox,
                                                  refinedLayout,
                                                  refineFactorFromMLMD,
                                                  ordersFromMLMD, pusherFromMLMD,
                                                  splitMethodFromMLMD ) } ;

    Patch theNewPatch{ refinedBox, refinedLayout, PatchData{*factory} };

    std::shared_ptr<Patch> patchPtr = std::make_shared<Patch>( std::move(theNewPatch) ) ;

    // Attach this new patch to the parent patch
    coarsePatch->updateChildren( patchPtr ) ;

    // update the hierarchy
    patchTable_[refinedLevel].push_back( patchPtr ) ;

}

/**
 * @brief Hierarchy::buildLayout is in charge of creating a GridLayout
 * using a RefinementInfo object
 *
 * This GridLayout will be directly used by addNewPatch(...) to
 * build the new patch
 *
 * @param info
 * @return
 */
GridLayout  Hierarchy::buildLayout_( RefinementInfo const & info )
{
    Box area = info.refinedDomain ;
    uint32 level = info.level ;

    uint32 refineRatio = info.refinementRatio ;
    GridLayout const & L0 = info.baseLayout ;

    // TODO: return the adequate GridLayout given newBox information
    // new spatial step sizes
    double dx = L0.dx()/std::pow( refineRatio, level ) ;
    double dy = L0.dy()/std::pow( refineRatio, level ) ;
    double dz = L0.dz()/std::pow( refineRatio, level ) ;

    // cell numbers
    uint32 nbx = static_cast<uint32>( std::ceil( (area.x1 - area.x0)/dx ) ) ;
    uint32 nby = static_cast<uint32>( std::ceil( (area.y1 - area.y0)/dy ) ) ;
    uint32 nbz = static_cast<uint32>( std::ceil( (area.z1 - area.z0)/dz ) ) ;

    // we create the layout of a new patch
    // and store it
    return GridLayout({{dx, dy, dz}}, {{nbx, nby, nbz}},
                      L0.nbDimensions(), L0.layoutName(),
                      Point{area.x0, area.y0, area.z0}, L0.order() ) ;
}





