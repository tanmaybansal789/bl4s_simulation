#pragma once

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"

// ---------------------------------------------------------------------------
// RunAction
//
// Builds the ROOT file used in DAQ analysis
// ---------------------------------------------------------------------------
class RunAction : public G4UserRunAction
{
public:
    RunAction();
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run)   override;

    // Called by EventAction when a valid event is written to the ntuple.
    void AddValidEvent() { fNValidEvents += 1; }

private:
    // Thread-safe counter merged across workers at end of run.
    G4Accumulable<G4int> fNValidEvents{0};
};
