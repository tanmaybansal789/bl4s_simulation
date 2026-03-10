#include "PrimaryGeneratorAction.hh"

#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : fGun(std::make_unique<G4ParticleGun>(/* nParticlesPerEvent = */ 1))
{
    // ── Beam configuration ───────────────────────────────────────────────────
    // Particle: proton (change to "pi+", "e-", "mu-", etc. as needed).
    G4ParticleDefinition* proton =
        G4ParticleTable::GetParticleTable()->FindParticle("proton");
    fGun->SetParticleDefinition(proton);

    // Energy: 120 GeV (typical SPS primary beam).
    fGun->SetParticleEnergy(120.0 * GeV);

    // Starting position: upstream of the target, on the beam axis.
    fGun->SetParticlePosition(G4ThreeVector(0.0, 0.0, -180.0 * cm));

    // Direction: along +Z (towards the target).
    fGun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    fGun->GeneratePrimaryVertex(event);
}
