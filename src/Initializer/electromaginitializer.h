#ifndef ELECTROMAGINITIALIZER_H
#define ELECTROMAGINITIALIZER_H


#include<array>

#include "types.h"
#include "grid/gridlayout.h"
#include "vecfield/vecfield.h"

class ElectromagInitializer
{
private:

    //using VectorFunction = void   (*) (double x, double y, double z, std::array<double,3> vec);
    VectorFunction electricField;
    VectorFunction magneticField;

public:
    VecField E_;
    VecField B_;

    ElectromagInitializer(GridLayout const& layout,
                          VectorFunction electricField,
                          VectorFunction magneticField);




};

#endif // ELECTROMAGINITIALIZER_H
