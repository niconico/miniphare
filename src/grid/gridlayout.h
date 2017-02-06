#ifndef GRIDLAYOUT_H
#define GRIDLAYOUT_H

#include <vector>
#include <array>
#include <memory>

#include "types.h"
#include "hybridenums.h"

#include "Electromag/electromag.h"
#include "Field/field.h"
#include "gridlayoutimpl.h"
#include "utility.h"


/**
 * @brief Gridlayout is a class used to handle all operations related to
 * a specific grid layout, for instance the Yee layout.
 *
 * A GridLayout is a central object in PHARE. All grid operation need to manipulate
 * a GridLayout. It is used to get start and end indices of ghost nodes, or of
 * nodes of the physical domain. GridLayout is the object that knows how to
 * calculate derivatives, and therefore enable other classes to be written
 * in a very general way, without knowing anything that is specific to the layout
 * of quantities on the grid.
 *
 * The GridLayout class follows the Pimpl idiom to be seen, from client code, as
 * a simple object living in its scope. The implementation pointer is abstract
 * and concrete child classes implement concret layouts, such as Yee.
 */
class GridLayout
{

    // ------------------------------------------------------------------------
    //                              PRIVATE
    // ------------------------------------------------------------------------

private:

    uint32 nbDims_ ;    // dimensionality (1, 2, 3)

    double dx_ ;        // mesh sizes
    double dy_ ;        // mesh sizes
    double dz_ ;        // mesh sizes

    double odx_ ;       // 1./dx
    double ody_ ;       // 1./dy
    double odz_ ;       // 1./dz

    uint32 nbrCellx_  ; // # of cells (not nodes) in each direction
    uint32 nbrCelly_  ;
    uint32 nbrCellz_  ;

    Point origin_;      // origin of the grid
    uint32 interpOrder_ ;                            // TODO find a better name.

                                                        // If interpOrder is the best so be it.

    std::unique_ptr<GridLayoutImpl> implPtr_;           // abstract private implementation

    using error = std::runtime_error;                   // TODO: find a better error handling
    static const std::string errorInverseMesh;


    // test the validity of the GridLayout construction
    void throwNotValid1D() const;
    void throwNotValid2D() const;
    void throwNotValid3D() const;


    // ------------------------------------------------------------------------
    //                          PUBLIC INTERFACE
    // ------------------------------------------------------------------------

public:

    static const uint32 minNbrCells = 10; // minimum nbr of cells in a
                                          // non-invariant direction

    GridLayout(std::array<double,3> dxdydz, std::array<uint32,3> nbrCells,
               uint32 nbDims      , std::string layoutName,
               Point origin,
               uint32 ghostParameter ); // TODO see if better name

    GridLayout(GridLayout const& source);
    GridLayout(GridLayout&& source);

    GridLayout& operator=(GridLayout const& source) = delete;
    GridLayout& operator=(GridLayout&& source) = delete;

    Point origin() const {return origin_;}

    double dx() const {return dx_;}
    double dy() const {return dy_;}
    double dz() const {return dz_;}

    double odx()const { return dx_ == 0. ? throw error(errorInverseMesh +" dz() (dz==0)"): odz_;}
    double ody()const { return dy_ == 0. ? throw error(errorInverseMesh +" dy() (dy==0)"): ody_;}
    double odz()const { return dz_ == 0. ? throw error(errorInverseMesh +" dz() (dz==0)"): odz_;}

    uint32 nbrCellx() const {return nbrCellx_;}
    uint32 nbrCelly() const {return nbrCelly_;}
    uint32 nbrCellz() const {return nbrCellz_;}

    uint32 nbDimensions() const { return nbDims_ ; }

    uint32 order() const { return interpOrder_ ; }

    uint32 physicalStartIndex(Field const& field, Direction direction) const;
    uint32 physicalEndIndex  (Field const& field, Direction direction) const;
    uint32 physicalStartIndex( QtyCentering centering, Direction direction ) const ;
    uint32 physicalEndIndex  ( QtyCentering centering, Direction direction ) const ;


    uint32 ghostStartIndex(Field const& field, Direction direction) const;
    uint32 ghostEndIndex  (Field const& field, Direction direction) const;

    AllocSizeT allocSize(HybridQuantity qtyType) const;
    AllocSizeT allocSizeDerived( HybridQuantity qty, Direction dir ) const ;

    void deriv(Field const& operand, Direction direction, Field& derivative) const;

    Point fieldNodeCoordinates( const Field & field, const Point & origin,
                                uint32 ix, uint32 iy, uint32 iz ) const;

    Point cellCenteredCoordinates(uint32 ix, uint32 iy, uint32 iz ) const;

    QtyCentering fieldCentering(Field const& field, Direction dir) const;

    uint32 nbrGhostCells( QtyCentering const& centering ) const;


};



#endif // GRIDLAYOUT_H
