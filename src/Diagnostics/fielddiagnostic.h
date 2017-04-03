#ifndef FIELDDIAGNOSTIC_H
#define FIELDDIAGNOSTIC_H

#include <vector>
#include <array>

#include "Field/field.h"
#include "grid/gridlayout.h"
#include "vecfield/vecfield.h"
#include "Field/field.h"
#include "Diagnostics/diagnostics.h"


class FieldDiagnostic : public Diagnostic
{


protected:

    void fillDiagData1D_(Field const& field,
                         GridLayout const& layout,
                         std::string const& id,
                         DiagPack& pack);

    void fillDiagData2D_(Field const& field,
                         GridLayout const& layout,
                         std::string const& id,
                         DiagPack& pack);

    void fillDiagData3D_(Field const& field,
                         GridLayout const& layout,
                         std::string const& id,
                         DiagPack& pack);

    void addVecField_(std::string const& id, VecField const& vecField, GridLayout const& layout);

    void addField_(std::string const& id, Field const& vecField, GridLayout const& layout);

    FieldDiagnostic(std::string diagName)
        : Diagnostic{diagName} {}

};




#endif // FIELDDIAGNOSTIC_H
