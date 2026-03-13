#pragma once

#include "G4VModularPhysicsList.hh"

// ---------------------------------------------------------------------------
// PhysicsList
//
// We use
//   - G4EmStandardPhysics 
//   - G4DecayPhysics - fixed up in ConstructParticle()/ConstructProcess() to use spin-aware versions for mu+/mu-
//   - G4StepLimiterPhysics
//
// ---------------------------------------------------------------------------
class PhysicsList : public G4VModularPhysicsList
{
public:
    PhysicsList();
    ~PhysicsList() override = default;

    void ConstructParticle() override;
    void ConstructProcess() override;
};