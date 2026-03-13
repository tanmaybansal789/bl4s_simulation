#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

SteppingAction::SteppingAction(const DetectorConstruction* detector,
                               EventAction*               eventAction)
    : fDetector(detector), fEventAction(eventAction)
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    G4LogicalVolume* volume =
        step->GetPreStepPoint()->GetTouchableHandle()
             ->GetVolume()->GetLogicalVolume();

    const G4Track*  track        = step->GetTrack();
    const G4String& particleName = track->GetDefinition()->GetParticleName();

    // ── S2: set decay clock t = 0 ─────────────────────────────────────────
    if (volume == fDetector->GetS2Logical() && particleName == "mu+") {
        fEventAction->SetS2Time(step->GetPreStepPoint()->GetGlobalTime());
    }

    // ── Stopper: detect when the µ+ has come to rest ──────────────────────
    if (volume == fDetector->GetStopperLogical() && particleName == "mu+") {
        if (step->GetPostStepPoint()->GetKineticEnergy() < 1.*eV) {
            fEventAction->SetMuonStopped();
        }
    }

    // ── S3: punch-through veto (primary muon only) ────────────────────────
    if (volume == fDetector->GetS3Logical() && particleName == "mu+") {
        fEventAction->SetS3Fired();
    }

    // ── Calorimeters: accumulate positron energy deposits ─────────────────
    // side: 0=L, 1=R, 2=U, 3=D.  Threshold (40 MeV) applied in EventAction.
    if (particleName == "e+") {
        G4double edep = step->GetTotalEnergyDeposit();
        if (edep > 0.) {
            G4double time = step->GetPreStepPoint()->GetGlobalTime();
            G4int side = -1;
            if      (volume == fDetector->GetCaloLLogical()) side = 0;
            else if (volume == fDetector->GetCaloRLogical()) side = 1;
            else if (volume == fDetector->GetCaloULogical()) side = 2;
            else if (volume == fDetector->GetCaloDLogical()) side = 3;

            if (side >= 0)
                fEventAction->AddCaloHit(side, time, edep);
        }
    }
}
