#include "gridlayoutimplinternals.h"

#include <cmath>
#include <stdexcept>




GridLayoutImplInternals::GridLayoutImplInternals(uint32 nbDims, Point origin, uint32 ghostParameter,
                                                 std::array<uint32, 3> nbrCellsXYZ,
                                                 std::array<double, 3> dxdydz)
    : nbdims_{nbDims}
    , interpOrder_{ghostParameter}
    , dx_{dxdydz[0]}
    , dy_{dxdydz[1]}
    , dz_{dxdydz[2]}
    , origin_{origin}
    , odxdydz_{{1. / dx_, 1. / dy_, 1. / dz_}}
    , nbrPhysicalCells_{{nbrCellsXYZ[0], nbrCellsXYZ[1], nbrCellsXYZ[2]}}
{
    computeNbrGhosts(ghostParameter);
}




void GridLayoutImplInternals::initPhysicalStart(const gridDataT& data)
{
    uint32 iprimal = static_cast<uint32>(data.primal);
    uint32 idual   = static_cast<uint32>(data.dual);

    physicalStartIndexTable_[iprimal][data.idirX] = nbrGhosts(QtyCentering::primal);
    physicalStartIndexTable_[iprimal][data.idirY] = nbrGhosts(QtyCentering::primal);
    physicalStartIndexTable_[iprimal][data.idirZ] = nbrGhosts(QtyCentering::primal);

    physicalStartIndexTable_[idual][data.idirX] = nbrGhosts(QtyCentering::dual);
    physicalStartIndexTable_[idual][data.idirY] = nbrGhosts(QtyCentering::dual);
    physicalStartIndexTable_[idual][data.idirZ] = nbrGhosts(QtyCentering::dual);
}




/**
 * @brief GridLayoutImplInternals::initPhysicalEnd intialize the table of indices
 * corresponding to the last node for primal and dual centering.
 * The formula is simple : the last index is obtained from the first one
 * (which is physicalStartIndex of primal/dual in a given direction)
 *  + the number of cells minus 1 for dual nodes only.
 */
void GridLayoutImplInternals::initPhysicalEnd(const gridDataT& data)
{
    uint32 iprimal = static_cast<uint32>(data.primal);
    uint32 idual   = static_cast<uint32>(data.dual);

    physicalEndIndexTable_[iprimal][data.idirX] = physicalStartIndexTable_[iprimal][data.idirX]
                                                  + nbrPhysicalCells_[data.idirX]
                                                  - isDual(data.primal);

    physicalEndIndexTable_[iprimal][data.idirY] = physicalStartIndexTable_[iprimal][data.idirY]
                                                  + nbrPhysicalCells_[data.idirY]
                                                  - isDual(data.primal);

    physicalEndIndexTable_[iprimal][data.idirZ] = physicalStartIndexTable_[iprimal][data.idirZ]
                                                  + nbrPhysicalCells_[data.idirZ]
                                                  - isDual(data.primal);


    physicalEndIndexTable_[idual][data.idirX] = physicalStartIndexTable_[idual][data.idirX]
                                                + nbrPhysicalCells_[data.idirX] - isDual(data.dual);

    physicalEndIndexTable_[idual][data.idirY] = physicalStartIndexTable_[idual][data.idirY]
                                                + nbrPhysicalCells_[data.idirY] - isDual(data.dual);

    physicalEndIndexTable_[idual][data.idirZ] = physicalStartIndexTable_[idual][data.idirZ]
                                                + nbrPhysicalCells_[data.idirZ] - isDual(data.dual);
}




/**
 * @brief GridLayoutImplInternals::initGhostEnd calculate and stores the index
 * of the last primal and dual nodes in each direction. The formula simply
 * consists in starting at physicalEndIndex() and to add the number of ghost nodes.
 */
void GridLayoutImplInternals::initGhostEnd(const gridDataT& data)
{
    uint32 iprimal = static_cast<uint32>(data.primal);
    uint32 idual   = static_cast<uint32>(data.dual);

    ghostEndIndexTable_[iprimal][data.idirX]
        = physicalEndIndexTable_[iprimal][data.idirX] + nbrGhosts(data.primal);

    ghostEndIndexTable_[iprimal][data.idirY]
        = physicalEndIndexTable_[iprimal][data.idirY] + nbrGhosts(data.primal);

    ghostEndIndexTable_[iprimal][data.idirZ]
        = physicalEndIndexTable_[iprimal][data.idirZ] + nbrGhosts(data.primal);

    ghostEndIndexTable_[idual][data.idirX]
        = physicalEndIndexTable_[idual][data.idirX] + nbrGhosts(data.dual);

    ghostEndIndexTable_[idual][data.idirY]
        = physicalEndIndexTable_[idual][data.idirY] + nbrGhosts(data.dual);

    ghostEndIndexTable_[idual][data.idirZ]
        = physicalEndIndexTable_[idual][data.idirZ] + nbrGhosts(data.dual);
}




/**
 * @brief GridLayoutImplInternals::derivedCentering this function returns the centering
 * (primal or dual) of a quantity after a first order derivation. dual becomes primal
 * and primal becomes dual. hybridQuantityCentering is used to know if the HybridQuantity
 * 'qty' is primal or dual in the Direction 'dir'
 */
QtyCentering GridLayoutImplInternals::derivedCentering(HybridQuantity qty, Direction dir) const
{
    uint32 iField = static_cast<uint32>(qty);
    uint32 idir   = static_cast<uint32>(dir);

    QtyCentering newCentering = changeCentering(hybridQtyCentering_[iField][idir]);

    return newCentering;
}


std::array<uint32, NBR_COMPO> GridLayoutImplInternals::physicalNodeNbrFromCentering_(
    std::array<QtyCentering, NBR_COMPO> const& qtyCenterings) const
{
    gridDataT data{};

    uint32 nx = 1;
    uint32 ny = 1;
    uint32 nz = 1;

    nx = nbrPhysicalCells_[data.idirX] + 1 - isDual(qtyCenterings[data.idirX]);
    ;

    if (nbdims_ >= 2)
    {
        ny = nbrPhysicalCells_[data.idirY] + 1 - isDual(qtyCenterings[data.idirY]);
    }

    if (nbdims_ == 3)
    {
        nz = nbrPhysicalCells_[data.idirZ] + 1 - isDual(qtyCenterings[data.idirZ]);
    }

    return {{nx, ny, nz}};
}




/**
 * @brief GridLayoutImplInternals::nodeNbrFromCentering_ returns an array containing
 * the total number of nodes (ghosts + physical) in each direction.
 * The calculation is easy : there are nbrPhysicalCells + 1 nodes in the domain
 * + 2 times the number of ghost nodes.
 */
std::array<uint32, NBR_COMPO> GridLayoutImplInternals::nodeNbrFromCentering_(
    std::array<QtyCentering, NBR_COMPO> const& qtyCenterings) const
{
    gridDataT data{};
    std::array<uint32, NBR_COMPO> nbrNodes = physicalNodeNbrFromCentering_(qtyCenterings);

    nbrNodes[0] += 2 * nbrGhosts(qtyCenterings[data.idirX]);

    if (nbdims_ >= 2)
    {
        nbrNodes[1] += 2 * nbrGhosts(qtyCenterings[data.idirY]);
    }

    if (nbdims_ == 3)
    {
        nbrNodes[2] += 2 * nbrGhosts(qtyCenterings[data.idirZ]);
    }

    return nbrNodes;
}




/**
 * @brief GridLayoutImplInternals::allocSize_
 * @return An AllocSizeT object, containing the size to which allocate arrays
 * of an HybridQuantity 'qty' in every directions.
 */
AllocSizeT GridLayoutImplInternals::allocSize_(HybridQuantity qty) const
{
    uint32 iQty = static_cast<uint32>(qty);

    auto sizeArray = nodeNbrFromCentering_(hybridQtyCentering_[iQty]);

    return AllocSizeT(sizeArray[0], sizeArray[1], sizeArray[2]);
}




AllocSizeT GridLayoutImplInternals::allocSizeDerived_(HybridQuantity qty, Direction dir) const
{
    gridDataT data{};

    uint32 iDerivedDir = static_cast<uint32>(dir);
    uint32 iQty        = static_cast<uint32>(qty);

    // get the centering of the derivative of 'qty' in the direction of derivation
    QtyCentering newCentering = derivedCentering(qty, dir);


    // now copy the original centering...
    std::array<QtyCentering, NBR_COMPO> qtyCenterings{{hybridQtyCentering_[iQty][data.idirX],
                                                       hybridQtyCentering_[iQty][data.idirY],
                                                       hybridQtyCentering_[iQty][data.idirZ]}};


    // ...and permute the centering in the direction of derivation
    qtyCenterings[iDerivedDir] = newCentering;

    // get the total number of nodes (ghost + physical) for the new centering
    auto sizeArray = nodeNbrFromCentering_(qtyCenterings);

    // and ship it in a AllocSizeT structure.
    return AllocSizeT(sizeArray[0], sizeArray[1], sizeArray[2]);
}




uint32 GridLayoutImplInternals::physicalStartIndex_(QtyCentering centering,
                                                    Direction direction) const
{
    uint32 icentering = static_cast<uint32>(centering);
    uint32 iDir       = static_cast<uint32>(direction);
    return physicalStartIndexTable_[icentering][iDir];
}


uint32 GridLayoutImplInternals::physicalStartIndex_(HybridQuantity const& hybridQuantity,
                                                    Direction direction) const
{
    uint32 iQty       = static_cast<uint32>(hybridQuantity);
    uint32 iDir       = static_cast<uint32>(direction);
    uint32 iCentering = static_cast<uint32>(hybridQtyCentering_[iQty][iDir]);

    return physicalStartIndexTable_[iCentering][iDir];
}

uint32 GridLayoutImplInternals::physicalStartIndex_(Field const& field, Direction direction) const
{
    return physicalStartIndex_(field.hybridQty(), direction);
}




uint32 GridLayoutImplInternals::physicalEndIndex_(QtyCentering centering, Direction direction) const
{
    uint32 icentering = static_cast<uint32>(centering);
    uint32 iDir       = static_cast<uint32>(direction);

    return physicalEndIndexTable_[icentering][iDir];
}


uint32 GridLayoutImplInternals::physicalEndIndex_(HybridQuantity const& hybridQuantity,
                                                  Direction direction) const
{
    uint32 iQty       = static_cast<uint32>(hybridQuantity);
    uint32 iDir       = static_cast<uint32>(direction);
    uint32 iCentering = static_cast<uint32>(hybridQtyCentering_[iQty][iDir]);

    return physicalEndIndexTable_[iCentering][iDir];
}


uint32 GridLayoutImplInternals::physicalEndIndex_(Field const& field, Direction direction) const
{
    return physicalEndIndex_(field.hybridQty(), direction);
}




uint32 GridLayoutImplInternals::ghostStartIndex_(QtyCentering centering, Direction direction) const
{
    // ghostStartIndex is always the first node
    return 0;
}


uint32 GridLayoutImplInternals::ghostStartIndex_(HybridQuantity const& hybridQuantity,
                                                 Direction direction) const
{
    // ghostStartIndex is always the first node
    return 0;
}

uint32 GridLayoutImplInternals::ghostStartIndex_(Field const& field, Direction direction) const
{
    // ghostStartIndex is always the first node
    return 0;
}




uint32 GridLayoutImplInternals::ghostEndIndex_(QtyCentering centering, Direction direction) const
{
    uint32 iCentering = static_cast<uint32>(centering);
    uint32 iDir       = static_cast<uint32>(direction);

    return ghostEndIndexTable_[iCentering][iDir];
}

uint32 GridLayoutImplInternals::ghostEndIndex_(HybridQuantity const& hybridQuantity,
                                               Direction direction) const
{
    uint32 iQty       = static_cast<uint32>(hybridQuantity);
    uint32 iDir       = static_cast<uint32>(direction);
    uint32 iCentering = static_cast<uint32>(hybridQtyCentering_[iQty][iDir]);
    return ghostEndIndexTable_[iCentering][iDir];
}

uint32 GridLayoutImplInternals::ghostEndIndex_(Field const& field, Direction direction) const
{
    return ghostEndIndex_(field.hybridQty(), direction);
}




QtyCentering GridLayoutImplInternals::changeCentering(QtyCentering centering) const
{
    QtyCentering newCentering = QtyCentering::primal;

    if (centering == QtyCentering::primal)
    {
        newCentering = QtyCentering::dual;
    }

    return newCentering;
}




/**
 * @brief fieldNodeCoordinates returns a Point,
 * the idea is to make in every initializer
 * method, 3 nested loops over primal PhysicalStart/End indices and to get
 * the physical coordinate of those mesh nodes for the considered field.
 * @param field
 * the returned point depends on the field's centering
 * @param origin
 * @param ix is a primal or dual index
 * @param iy is a primal or dual index
 * @param iz is a primal or dual index
 * @return Point
 * the desired field-centered coordinate
 */
Point GridLayoutImplInternals::fieldNodeCoordinates_(const Field& field, const Point& origin,
                                                     uint32 ix, uint32 iy, uint32 iz) const
{
    uint32 idirX = static_cast<uint32>(Direction::X);
    uint32 idirY = static_cast<uint32>(Direction::Y);
    uint32 idirZ = static_cast<uint32>(Direction::Z);

    const uint32 idual = static_cast<uint32>(QtyCentering::dual);

    std::array<double, 3> halfCell{{0, 0, 0}};

    uint32 iQty = static_cast<uint32>(field.hybridQty());

    std::array<uint32, 3> centering = {{static_cast<uint32>(hybridQtyCentering_[iQty][idirX]),
                                        static_cast<uint32>(hybridQtyCentering_[iQty][idirY]),
                                        static_cast<uint32>(hybridQtyCentering_[iQty][idirZ])}};

    uint32 ixStart = physicalStartIndexTable_[centering[idirX]][idirX];
    uint32 iyStart = physicalStartIndexTable_[centering[idirY]][idirY];
    uint32 izStart = physicalStartIndexTable_[centering[idirZ]][idirZ];

    for (uint32 idir = idirX; idir <= idirZ; ++idir)
    {
        if (centering[idir] == idual)
            halfCell[idir] = 0.5;
    }

    // A shift of +dx/2, +dy/2, +dz/2 is necessary to get the physical
    // coordinate on the dual mesh
    // No shift for coordinate on the primal mesh
    // This shift DOES NOT DEPEND ON the interpolation order
    // Because,
    // if ix is primal then ixStart is primal
    // if ix is dual   then ixStart is dual
    // if iy is primal then iyStart is primal ...

    double x = ((ix - ixStart) + halfCell[0]) * dx_ + origin.x;
    double y = ((iy - iyStart) + halfCell[1]) * dy_ + origin.y;
    double z = ((iz - izStart) + halfCell[2]) * dz_ + origin.z;

    return Point(x, y, z);
}




/**
 * @brief cellCenteredCoordinates returns a cell-centered Point.
 * The idea is to call this method in every initializer method
 * using 3 nested loops over primal PhysicalStart/End indices.
 * This function will typically used to evaluate the density at initialization
 * phases, since it is usually defined at cell centers.
 * @param origin
 * @param ix is a primal index
 * @param iy is a primal index
 * @param iz is a primal index
 * @return Point
 * the desired cell-centered (dual/dual/dual) coordinate
 */
Point GridLayoutImplInternals::cellCenteredCoordinates_(uint32 ix, uint32 iy, uint32 iz) const
{
    uint32 idirX   = static_cast<uint32>(Direction::X);
    uint32 idirY   = static_cast<uint32>(Direction::Y);
    uint32 idirZ   = static_cast<uint32>(Direction::Z);
    uint32 iprimal = static_cast<uint32>(QtyCentering::primal);

    uint32 ixStart = physicalStartIndexTable_[iprimal][idirX];
    uint32 iyStart = physicalStartIndexTable_[iprimal][idirY];
    uint32 izStart = physicalStartIndexTable_[iprimal][idirZ];

    double halfCell = 0.5;
    // A shift of +dx/2, +dy/2, +dz/2 is necessary to get the
    // cell center physical coordinates,
    // because this point is located on the dual mesh

    double x = ((ix - ixStart) + halfCell) * dx_ + origin_.x;
    double y = ((iy - iyStart) + halfCell) * dy_ + origin_.y;
    double z = ((iz - izStart) + halfCell) * dz_ + origin_.z;

    return Point(x, y, z);
}




/**
 * @brief computeOffsets computes the number of ghost cells for fields.
 * On the primal mesh the number of ghosts depends on centeredOffset_
 * On the dual mesh the number of ghosts depends on
 * leftOffset_ and rightOffset_
 *
 * This is explained in details on the wiki page
 * https://hephaistos.lpp.polytechnique.fr/redmine/projects/hyb-par/wiki/PrimalDual
 *
 * @param ghostParameter, corresponds to the interpolation order
 */
void GridLayoutImplInternals::computeNbrGhosts(uint32 ghostParameter)
{
    /* for first order Interpolation, there is no primal ghost node neeeded
       for particle/mesh interactions. However one ghost node is required
       for calculating Laplacians so we add one.
    */
    if (ghostParameter == 1)
    {
        nbrPrimalGhosts_ = static_cast<uint32>(std::floor((ghostParameter + 1) / 2.));
        nbrDualGhosts_   = static_cast<uint32>(std::floor((ghostParameter + 1) / 2.));
    }

    /* for interpolation order larger than 1, there is at least 1 primal ghost
       node so Laplacians can be calculated OK */
    else if (ghostParameter > 1)
    {
        nbrPrimalGhosts_ = static_cast<uint32>(std::floor(ghostParameter / 2.));
        nbrDualGhosts_   = static_cast<uint32>(std::floor((ghostParameter + 1) / 2.));
    }
}




/**
 * @brief GridLayoutImplInternals::nbrGhosts
 * @param centering QtyCentering::primal or QtyCentering::dual
 * @return the number of ghost nodes on each side of the mesh for a given centering
 */
uint32 GridLayoutImplInternals::nbrGhosts(QtyCentering centering) const noexcept
{
    uint32 nbrGhosts = nbrPrimalGhosts_;

    if (centering == QtyCentering::dual)
    {
        nbrGhosts = nbrDualGhosts_;
    }
    return nbrGhosts;
}




/**
 * @brief GridLayoutImplInternals::isDual the method is used in index calculations
 * when we sometimes need to substract 1 when the centering is dual
 * @param centering QtyCentering::primal or QtyCentering::dual
 * @return return 1 if the centering is dual or 0 otherwise.
 */
uint32 GridLayoutImplInternals::isDual(QtyCentering centering) const noexcept
{
    uint32 isdual = 0;

    if (centering == QtyCentering::dual)
    {
        isdual = 1;
    }

    return isdual;
}




QtyCentering GridLayoutImplInternals::fieldCentering_(Field const& field, Direction dir) const
{
    uint32 iDir = static_cast<uint32>(dir);

    uint32 iQty = static_cast<uint32>(field.hybridQty());

    return hybridQtyCentering_[iQty][iDir];
}



/**
 * @brief GridLayoutImplYee::deriv1D It was decided to compute the
 * derivative on the entire physical domain.
 * In the case of Maxwell Ampere (dual centering of the operand),
 * it will therefore be necessary to get the values
 * of the operand outside the physical domain before applying
 * Maxwell Ampere
 *
 * @param operand is always primal in the case of Maxwell Faraday
 * rotational operator (dEz/dy, dEy/dz, dEx/dz, dEz/dx, dEy/dx, dEx/dy)
 * operand is always dual in the case of Maxwell Ampere
 * rotational operator (dBz/dy, dBy/dz, dBx/dz, dBz/dx, dBy/dx, dBx/dy)
 *
 * @param derivative
 */
void GridLayoutImplInternals::deriv1D_(Field const& operand, Field& derivative) const
{
    uint32 iDirX = static_cast<uint32>(Direction::X);

    uint32 iQtyOperand = static_cast<uint32>(operand.hybridQty());

    QtyCentering opCentering = hybridQtyCentering_[iQtyOperand][iDirX];


    // The QtyCentering of derivative is given by
    // iQty = static_cast<uint32>( derivative.hybridQty() )
    // hybridQtyCentering_[iQty][idir]
    uint32 iDerStart = physicalStartIndex_(derivative, Direction::X);
    uint32 iDerEnd   = physicalEndIndex_(derivative, Direction::X);

    uint32 iOpStart = physicalStartIndex_(operand, Direction::X);

    uint32 iOp = iOpStart;
    if (opCentering == QtyCentering::dual)
    {
        --iOp;
    }

    for (uint32 iDer = iDerStart; iDer <= iDerEnd; ++iDer)
    {
        derivative(iDer) = odxdydz_[0] * (operand(iOp + 1) - operand(iOp));
        ++iOp;
    }
}
