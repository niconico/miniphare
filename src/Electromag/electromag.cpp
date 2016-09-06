
#include "electromag.h"
#include "vecfield/vecfield.h"




Electromag::Electromag( std::array<AllocSizeT ,3> E_AllocSizes,
                        std::array<AllocSizeT ,3> B_AllocSizes,
                        std::string name )
    : E_( E_AllocSizes, "E"+name ),
      B_( B_AllocSizes, "B"+name )
{

}



