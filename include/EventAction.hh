#pragma once

#include "G4UserEventAction.hh"

class RunAction;

// ---------------------------------------------------------------------------
// EventAction
//
// Accumulates energy deposits over all steps within one event, then
// forwards the total to RunAction at the end of the event.
// ---------------------------------------------------------------------------
class EventAction : public G4UserEventAction
{
public:
    explicit EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event)   override;

    // Called by SteppingAction for each step inside the tracker.
    void AddTrackerEdep(double edep) { fTrackerEdep += edep; }

private:
    RunAction* fRunAction    = nullptr;
    double     fTrackerEdep  = 0.0;  // reset each event
};
