

#include "diagnosticmanager.h"
#include "FieldDiagnostics/Electromag/emdiagnostic.h"
#include "FieldDiagnostics/Electromag/emdiagnosticfactory.h"
#include "FieldDiagnostics/Fluid/fluiddiagnostic.h"
#include "FieldDiagnostics/Fluid/fluiddiagnosticfactory.h"
#include "ParticleDiagnostics/particlediagnostic.h"

#include "utilities/particleselectorfactory.h"
#include <utilities/print/outputs.h>

uint32 DiagnosticsManager::id = 0;

DiagnosticsManager::DiagnosticsManager(std::unique_ptr<DiagnosticInitializer> initializer)
    : fluidDiags_{}
    , emDiags_{}
    , exportStrat_{ExportStrategyFactory::makeExportStrategy(initializer->exportType)}
    , scheduler_{}
{
    Logger::Debug << "Building Diagnostics manager\n";
    Logger::Debug << "\t - Electromag Diagnostics : " << initializer->emInitializers.size() << "\n";
    Logger::Debug << "\t - Fluid Diagnostics      : " << initializer->fluidInitializers.size()
                  << "\n";
    Logger::Debug << "\t - Particle Diagnostics   : " << initializer->partInitializers.size()
                  << "\n";
    Logger::Debug.flush();

    // first initialize all electromagnetic diagnostics
    for (uint32 iDiag = 0; iDiag < initializer->emInitializers.size(); ++iDiag)
    {
        newEMDiagnostic(initializer->emInitializers[iDiag].typeName,
                        initializer->emInitializers[iDiag].diagName,
                        initializer->emInitializers[iDiag].path,
                        initializer->emInitializers[iDiag].computingIterations,
                        initializer->emInitializers[iDiag].writingIterations);
    }

    // then initialize all Fluid diagnostics
    for (uint32 iDiag = 0; iDiag < initializer->fluidInitializers.size(); ++iDiag)
    {
        newFluidDiagnostic(initializer->fluidInitializers[iDiag].typeName,
                           initializer->fluidInitializers[iDiag].diagName,
                           initializer->fluidInitializers[iDiag].path,
                           initializer->fluidInitializers[iDiag].speciesName,
                           initializer->fluidInitializers[iDiag].computingIterations,
                           initializer->fluidInitializers[iDiag].writingIterations);
    }

    // then initialize all Particle diagnostics
    for (uint32 iDiag = 0; iDiag < initializer->partInitializers.size(); ++iDiag)
    {
        newParticleDiagnostic(initializer->partInitializers[iDiag]);
    }

    // then initialize all orbit, probes, etc. diagnostics
    Logger::Info << Logger::hline;
    Logger::Info.flush();
}

// TODO : add 'id' to diagnostic fields too so that each of them
// know their id.
void DiagnosticsManager::newFluidDiagnostic(std::string type, std::string diagName,
                                            std::string path, std::string speciesName,
                                            std::vector<uint32> const& computingIterations,
                                            std::vector<uint32> const& writingIterations)
{
    std::unique_ptr<FluidDiagnostic> fd
        = FluidDiagnosticFactory::createFluidDiagnostic(id, diagName, path, type, speciesName);
    fluidDiags_.push_back(std::move(fd));
    scheduler_.registerDiagnostic(id, computingIterations, writingIterations);
    id++; // new diagnostic identifier
}

void DiagnosticsManager::newEMDiagnostic(std::string type, std::string diagName, std::string path,
                                         std::vector<uint32> const& computingIterations,
                                         std::vector<uint32> const& writingIterations)
{
    std::unique_ptr<EMDiagnostic> emd
        = EMDiagnosticFactory::createEMDiagnostic(id, diagName, path, type);
    emDiags_.push_back(std::move(emd));
    scheduler_.registerDiagnostic(id, computingIterations, writingIterations);
    id++; // new diagnostic identifier
}


void DiagnosticsManager::newParticleDiagnostic(PartDiagInitializer const& init)
{
    std::unique_ptr<ParticleSelector> selector
        = ParticleSelectorFactory::createParticleSelector(init.selectorType, init.selectorParams);

    std::unique_ptr<ParticleDiagnostic> partd{new ParticleDiagnostic{
        id, init.diagName, init.path, init.speciesName, std::move(selector)}};

    partDiags_.push_back(std::move(partd));
    scheduler_.registerDiagnostic(id, init.computingIterations, init.writingIterations);
    id++; // new diagnostic identifier
}

/**
 * @brief DiagnosticsManager::compute will calculate all diagnostics that need to be.
 * @param timeManager is used to know the current time/iteration
 * @param patchData owns the data used to get the Diagnotic calculated
 * @param layout has all information about the grid
 */
void DiagnosticsManager::compute(Time const& timeManager, Hierarchy const& hierarchy)
{
    Logger::Debug << "Computing diagnostics\n";
    Logger::Debug.flush();
    for (auto& diag : emDiags_)
    {
        if (scheduler_.isTimeToCompute(timeManager, diag->id()))
            diag->compute(hierarchy);
    }

    for (auto& diag : fluidDiags_)
    {
        if (scheduler_.isTimeToCompute(timeManager, diag->id()))
            diag->compute(hierarchy);
    }

    for (auto& diag : partDiags_)
    {
        if (scheduler_.isTimeToCompute(timeManager, diag->id()))
            diag->compute(hierarchy);
    }

    // other kind of diagnostics here...
}

/**
 * @brief DiagnosticsManager::save will save Diagnostics to the disk
 *
 * The function loop over all Diagnostic, ask the scheduler if it is time to
 * save data to disk and calls the ExportStrategy if yes.
 * As soon as data is written, we get rid of it.
 *
 * @param timeManager is used to get the current time and iteration
 */
void DiagnosticsManager::save(Time const& timeManager)
{
    Logger::Debug << "Writing diagnostics on disk\n";
    Logger::Debug.flush();
    for (auto& diag : emDiags_)
    {
        if (scheduler_.isTimeToWrite(timeManager, diag->id()))
        {
            exportStrat_->saveEMDiagnostic(*diag, timeManager);
            diag->flushPacks();
        }
    }

    for (auto& diag : fluidDiags_)
    {
        if (scheduler_.isTimeToWrite(timeManager, diag->id()))
        {
            exportStrat_->saveFluidDiagnostic(*diag, timeManager);
            diag->flushPacks();
        }
    }

    for (auto& diag : partDiags_)
    {
        if (scheduler_.isTimeToWrite(timeManager, diag->id()))
        {
            exportStrat_->saveParticleDiagnostic(*diag, timeManager);
            diag->flushPacks();
        }
    }
}
