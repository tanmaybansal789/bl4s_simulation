#include "PrimaryGeneratorAction.hh"

#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : fGun(std::make_unique<G4ParticleGun>(1))
{
    G4ParticleDefinition* muPlus =
        G4ParticleTable::GetParticleTable()->FindParticle("mu+");
    fGun->SetParticleDefinition(muPlus);
    // In the doc they calculated that the energy would go to 35.7MeV so that's what's used here
    fGun->SetParticleEnergy(35.7 * MeV);
    // starts just ahead of S2, on beam axis.
    fGun->SetParticlePosition(G4ThreeVector(0.0, 0.0, -10.0 * cm));
    // Beam direction: along +Z towards setup
    fGun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
    // Beam polarized on -Z to get the most asymmetry
    fGun->SetParticlePolarization(G4ThreeVector(0.0, 0.0, -1.0));
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    fGun->GeneratePrimaryVertex(event);
}
