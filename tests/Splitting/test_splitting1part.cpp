
#include <memory>
#include <limits>


#include "Plasmas/particles.h"

#include "grid/gridlayout.h"
#include "IndexesAndWeights/indexesandweights.h"
#include "IndexesAndWeights/indexesandweightso1.h"
#include "IndexesAndWeights/indexesandweightso2.h"
#include "IndexesAndWeights/indexesandweightso3.h"
#include "IndexesAndWeights/indexesandweightso4.h"

#include "Splitting/splittingstrategy.h"
#include "Splitting/splittingstrategyfactory.h"

#include "test_commons.h"
#include "test_splitting1part.h"



std::vector<Split1PartParams> getSplit1ParticleParamsFromFile() ;

std::unique_ptr<IndexesAndWeights>  createIndexesAndWeights( uint32 const & order ) ;

GridLayout buildRefinedLayout( Split1PartParams const & inputs ) ;

void normalizeRefNodePosition1D(
        GridLayout const & coarseLayout,
        GridLayout const & refinedLayout,
        uint32 const refinementRatio,
        uint32 const & refNode, uint32 & normalizedRefNode ) ;



Split1PartParams split1ParticleInputs[] = {

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        1, 2, "splitOrder1", 20),                       // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        2, 2, "splitOrder2", 20),                       // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        3, 2, "splitOrder3", 20),                       // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        1, 2, "splitOrder1_RFn", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        1, 3, "splitOrder1_RFn", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        1, 4, "splitOrder1_RFn", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        1, 5, "splitOrder1_RFn", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        1, 10, "splitOrder1_RFn", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        1, 2, "splitOrderN_RF2", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        2, 2, "splitOrderN_RF2", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        3, 2, "splitOrderN_RF2", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        4, 2, "splitOrderN_RF2", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        1, 3, "splitOrderN_RF3", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        2, 3, "splitOrderN_RF3", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        3, 3, "splitOrderN_RF3", 20),                  // order, RF, splitMethod, refNode

    Split1PartParams({{0.1, 0., 0.}}, {{100, 0, 0}},   // dxdydz, nbrCellxyz
        1, "yee" , Point{0., 0., 0.},                  // dim, layout, origin
        4, 3, "splitOrderN_RF3", 20)                   // order, RF, splitMethod, refNode

};




class Split1ParticleTest: public ::testing::TestWithParam<Split1PartParams>
{
public:
    std::vector<double> expected_weights ;

    std::vector<double> actual_weights ;

    const double precision = std::numeric_limits<float>::epsilon() ;


    Split1PartParams inputs;


    ~Split1ParticleTest() = default ;

    void SetUp()
    {
        inputs = GetParam();

        GridLayout coarseLayout{ inputs.dxdydz, inputs.nbrCells,
                                 inputs.nbDim , inputs.layout,
                                 inputs.origin, inputs.interpOrder} ;


        GridLayout refinedLayout{ buildRefinedLayout(inputs) } ;


        // we need a reference node
        uint32 refNode = inputs.referenceNode ;


        SplittingStrategyFactory factory(inputs.splitMethod,
                                         inputs.refineFactor,
                                         inputs.interpOrder ) ;

        std::unique_ptr<SplittingStrategy>
                strategy = factory.createSplittingStrategy() ;


        const double motherWeight = 1. ;

        // the spatial step dx is discretized into
        // discreteNbr points
        uint32 discreteNbr = 10 ;

        // number of cells depending on the order
        // 1st order shape function covers 2*dx
        // 2nd order shape function covers 3*dx
        // 3rd order shape function covers 4*dx ...
        uint32 nbCells = discreteNbr*(inputs.interpOrder +1) ;
        uint32 nbPts = nbCells+1 ;

        float deltaMin = -0.5*static_cast<float>(inputs.interpOrder +1) ;

        expected_weights.assign( nbPts, 0. ) ;
        actual_weights.assign(nbPts, 0.) ;

        Particle normalizedMother ;
        uint32   normalizedRefNode ;

        /* ------------------------------------------------- */
        /* Expected weights are obtained by moving           */
        /* a mother particle (evolving on the coarse mesh)   */
        /* ------------------------------------------------- */

        // We now create the array of expected weights at a reference node.
        // To this sake we compute the contribution of a mother particle
        // at this reference node and next we move it slightly
        // delta += ik/discreteNbr
        //
        // Repeating this process nbPts times, we cover the variation
        // domain of the shape function
        //
        for( uint32 ik=0 ; ik<nbPts ; ik++ )
        {
            float delta = deltaMin + static_cast<float>(ik)/static_cast<float>(discreteNbr) ;

            // check the validity of delta (0 <= delta <= 1)
            // and do auto-correction
            float iCell = std::floor(delta) ;
            delta = delta - iCell ;

            // compute the shifted logical node
            // due to iCell displacement
            uint32 shiftedNode =  static_cast<uint32>(iCell) + refNode;

            // we now initialize a Particle at the reference node
            // Particle( weight, charge, icells, deltas, velocities )
            Particle mother( motherWeight, 1., {{shiftedNode, 1, 1}},
                           {{delta, 0., 0.}}, {{0., 0., 0.}} );

            // mother particle x coordinate
            double motherX = static_cast<double>(mother.icell[0])
                    + static_cast<double>(mother.delta[0]) ;

            // we need an IndexesAndWeights object
            std::unique_ptr<IndexesAndWeights> indexAndWeights
                         { createIndexesAndWeights(inputs.interpOrder) } ;

            // Now, starts the algorithm
            std::vector<uint32> const & motherInds  = indexAndWeights->computeIndexes( motherX ) ;
            std::vector<double> const & motherPonds = indexAndWeights->computeWeights( motherX ) ;

            std::vector<uint32>::const_iterator low ;
            low = std::lower_bound (motherInds.begin(), motherInds.end(), refNode) ;

            // We compute expected_weights[ik]
            // using the mother's contribution
            if( low != motherInds.end() ) {
                if( *low == refNode ) {
                    uint32 low_at = static_cast<uint32>( std::distance( motherInds.begin(), low ) ) ;

                    expected_weights[ik] = motherPonds[low_at] * mother.weight ;
                }
            }


            // compute the node index
            // on the refined layout
            SplittingStrategy::normalizeMotherPosition1D(
                        coarseLayout, refinedLayout,
                        inputs.refineFactor,
                        mother, normalizedMother) ;

            std::vector<Particle> childParticles ;
            strategy->split1D( normalizedMother, childParticles ) ;

            normalizeRefNodePosition1D(
                    coarseLayout, refinedLayout,
                    inputs.refineFactor,
                    refNode, normalizedRefNode ) ;

            // we need an IndexesAndWeights object
            std::unique_ptr<IndexesAndWeights> childIndexAndWeights
            { createIndexesAndWeights(inputs.interpOrder) } ;

            actual_weights[ik] = 0. ;
            for( Particle const & child : childParticles )
            {
                double childX = static_cast<double>(child.icell[0])
                        + static_cast<double>(child.delta[0]) ;

                std::vector<uint32> const & childInds  = childIndexAndWeights->computeIndexes( childX ) ;
                std::vector<double> const & childPonds = childIndexAndWeights->computeWeights( childX ) ;

                std::vector<uint32>::const_iterator found ;
                found = std::lower_bound (childInds .begin(),
                                          childInds .end(), normalizedRefNode) ;

                // We compute actual_weights[ik]
                // using the children's contributions
                if( found != childInds .end() ) {
                    if( *found == normalizedRefNode ) {
                        uint32 found_at = static_cast<uint32>( std::distance(childInds .begin(), found) ) ;

                        actual_weights[ik] += childPonds[found_at] * child.weight * inputs.refineFactor ;
                    }
                }
            }


        }

    }

};




void normalizeRefNodePosition1D(
        GridLayout const & coarseLayout,
        GridLayout const & refinedLayout,
        uint32 const refinementRatio,
        uint32 const & refNode, uint32 & normalizedRefNode )
{
    double origL0_x = coarseLayout.origin().x_ ;
    double origL1_x = refinedLayout.origin().x_ ;

    double normDif_x = (origL0_x - origL1_x)/refinedLayout.dx() ;

    normalizedRefNode = static_cast<uint32>(normDif_x) + refNode*refinementRatio ;

}


/**
 * @brief buildRefinedLayout return a refined GridLayout
 * In this method we just build a refined mesh covering the same
 * physical domain, with the same origin.
 * The number of cells depends on the refinement factor.
 * The spatial step sizes depend on the refinement factor.
 *
 *
 * @param inputs
 * @return
 */
GridLayout buildRefinedLayout( Split1PartParams const & inputs )
{
    uint32 RF = inputs.refineFactor ;

    std::array<double, 3> dxdydz ;

    dxdydz[0] = inputs.dxdydz[0]/RF ;
    dxdydz[1] = inputs.dxdydz[1]/RF ;
    dxdydz[2] = inputs.dxdydz[2]/RF ;

    std::array<uint32, 3> nbrCells ;

    nbrCells[0] = inputs.nbrCells[0]*RF ;
    nbrCells[1] = inputs.nbrCells[1]*RF ;
    nbrCells[2] = inputs.nbrCells[2]*RF ;

    return GridLayout { dxdydz, nbrCells,
                inputs.nbDim , inputs.layout,
                inputs.origin, inputs.interpOrder} ;
}




std::unique_ptr<IndexesAndWeights>  createIndexesAndWeights( uint32 const & order )
{
    std::unique_ptr<IndexesAndWeights> indexAndWeight_ptr {nullptr} ;

    switch (order)
    {
    case 1:
        indexAndWeight_ptr = std::unique_ptr<IndexesAndWeightsO1>( new IndexesAndWeightsO1(order) ) ;
        break;
    case 2:
        indexAndWeight_ptr = std::unique_ptr<IndexesAndWeightsO2>( new IndexesAndWeightsO2(order) ) ;
        break;
    case 3:
        indexAndWeight_ptr = std::unique_ptr<IndexesAndWeightsO3>( new IndexesAndWeightsO3(order) ) ;
        break;
    case 4:
        indexAndWeight_ptr = std::unique_ptr<IndexesAndWeightsO4>( new IndexesAndWeightsO4(order) ) ;
        break;
    }

    return indexAndWeight_ptr ;
}




TEST_P(Split1ParticleTest, shapes)
{

    EXPECT_TRUE( AreVectorsEqual(expected_weights, actual_weights, precision ) );

}



INSTANTIATE_TEST_CASE_P(Split1ParticleTest, Split1ParticleTest,
                        testing::ValuesIn( split1ParticleInputs ) );

