#include "PhysicsList.hh"

// ---------------------------------------------------------------------------
// FTFP_BERT base class handles all particle/process registration.
// We only add the StepLimiterPhysics plugin on top so that macro commands
// such as /run/setCut and per-volume step limits are available.
// ---------------------------------------------------------------------------
PhysicsList::PhysicsList()
    : FTFP_BERT(/* verbosity = */ 0)
{
    RegisterPhysics(new G4StepLimiterPhysics());
}
