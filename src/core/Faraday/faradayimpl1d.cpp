
#include "utilities/hybridenums.h"

#include "faradayimpl1d.h"


/**
 * @brief FaradayImpl1D::FaradayImpl1D
 * The partial derivative fields now receive the adequate HybridQuantity in order
 * to get the appropriate QtyCentering.
 * For example:  dxEz_ must be centered like By
 *
 * Conceptually, it might sound weird because dxEz_ and By do not have the same
 * physical dimension.
 * However, we must keep in mind that we have created HybridQuantity mainly
 * because of centering and layout issues.
 *
 * @param dt
 * @param layout
 */
FaradayImpl1D::FaradayImpl1D(double dt, GridLayout const& layout)
    : FaradayImplInternals(dt, layout)
    , dxEz_(layout.allocSizeDerived(HybridQuantity::Ez, Direction::X), HybridQuantity::By, "_dxEz")
    , dxEy_(layout.allocSizeDerived(HybridQuantity::Ey, Direction::X), HybridQuantity::Bz, "_dxEy")
{
}


void FaradayImpl1D::operator()(VecField const& E, VecField const& B, VecField& Bnew)
{
    const Field& Ey = E.component(VecField::VecY);
    const Field& Ez = E.component(VecField::VecZ);
    const Field& Bx = B.component(VecField::VecX);
    const Field& By = B.component(VecField::VecY);
    const Field& Bz = B.component(VecField::VecZ);

    Field& Bxnew = Bnew.component(VecField::VecX);
    Field& Bynew = Bnew.component(VecField::VecY);
    Field& Bznew = Bnew.component(VecField::VecZ);


    // warning: we add a loop over the whole domain.
    // should be better than having one loop with polymorphic
    // derivative. (should be tested).
    layout_.deriv(Ez, Direction::X, dxEz_);
    layout_.deriv(Ey, Direction::X, dxEy_);



    uint32 iStart = layout_.physicalStartIndex(Bx, Direction::X);
    uint32 iEnd   = layout_.physicalEndIndex(Bx, Direction::X);

    for (uint32 ix = iStart; ix <= iEnd; ++ix)
    {
        Bxnew(ix) = Bx(ix);
    }


    iStart = layout_.physicalStartIndex(By, Direction::X);
    iEnd   = layout_.physicalEndIndex(By, Direction::X);

    for (uint32 ix = iStart; ix <= iEnd; ++ix)
    {
        Bynew(ix) = By(ix) + dt_ * dxEz_(ix);
    }


    iStart = layout_.physicalStartIndex(Bz, Direction::X);
    iEnd   = layout_.physicalEndIndex(Bz, Direction::X);

    for (uint32 ix = iStart; ix <= iEnd; ++ix)
    {
        Bznew(ix) = Bz(ix) - dt_ * dxEy_(ix);
    }
}
