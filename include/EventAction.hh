#pragma once

#include "G4UserEventAction.hh"
#include "G4Types.hh"

class RunAction;

// ---------------------------------------------------------------------------
// EventAction
//
// Tracks state for each event in the experiment.
//   S2 trigger time        : sets t = 0 for the decay clock
//   S3 veto flag           : marks events where the muon punched through
//   Muon-stopped flag      : set when the mu+ KE reaches ~0 in stopper
//   Calorimeter accumulators: total energy and first-hit time for L/R/U/D
// ---------------------------------------------------------------------------
class EventAction : public G4UserEventAction
{
public:
    explicit EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event)   override;

    // Setters for event state
    void SetS2Time(G4double t) { if (!fS2Fired) { fS2Time = t; fS2Fired = true; } }
    void SetS3Fired()          { fS3Fired     = true; }
    void SetMuonStopped()      { fMuonStopped = true; }

    // side: 0=left, 1=right, 2=up, 3=down
    void AddCaloHit(G4int side, G4double time, G4double edep);

    // Getters for event state
    G4bool   IsS2Fired() const { return fS2Fired; }
    G4double GetS2Time() const { return fS2Time;  }

private:
    RunAction* fRunAction = nullptr;

    G4bool   fS2Fired     = false;
    G4bool   fS3Fired     = false;
    G4bool   fMuonStopped = false;
    G4double fS2Time      = 0.;

    // Per-calorimeter accumulators: L=0, R=1, U=2, D=3
    G4double fCaloEnergy[4] = {0., 0., 0., 0.};
    G4double fCaloTime[4]   = {-1., -1., -1., -1.};  // -1 = no hit yet
};
