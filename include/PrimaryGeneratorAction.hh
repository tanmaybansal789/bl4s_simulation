#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include <memory>

// ---------------------------------------------------------------------------
// PrimaryGeneratorAction
//
// Fires a single particle towards the target at the start of each event.
// ---------------------------------------------------------------------------
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override = default;

    void GeneratePrimaries(G4Event* event) override;

    // Getter for our particle gun
    const G4ParticleGun* GetParticleGun() const { return fGun.get(); }

private:
    std::unique_ptr<G4ParticleGun> fGun;
};
