#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include <memory>

// ---------------------------------------------------------------------------
// PrimaryGeneratorAction
//
// Fires a single particle towards the target at the start of each event.
//
// Default beam:  proton, 120 GeV, along +Z axis, starting at (0,0,-180 cm).
// Change particle type, energy, and position in PrimaryGeneratorAction.cc.
// ---------------------------------------------------------------------------
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override = default;

    void GeneratePrimaries(G4Event* event) override;

    // Read-only access to the gun (e.g. from EventAction for logging).
    const G4ParticleGun* GetParticleGun() const { return fGun.get(); }

private:
    std::unique_ptr<G4ParticleGun> fGun;
};
