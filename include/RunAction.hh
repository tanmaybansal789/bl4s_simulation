#pragma once

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"

// ---------------------------------------------------------------------------
// RunAction
//
// Collects per-run statistics.
// Currently tracks:
//   - total energy deposited in the tracker (summed over all events / threads)
//
// Extend this class to add more accumulables (e.g. hit counts, step lengths).
// ---------------------------------------------------------------------------
class RunAction : public G4UserRunAction
{
public:
    RunAction();
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run)   override;

    // Called by EventAction to accumulate energy deposits.
    void AddTrackerEdep(double edep) { fTrackerEdep += edep; }

private:
    G4Accumulable<double> fTrackerEdep{0.0};
};
