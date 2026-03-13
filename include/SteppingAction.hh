#pragma once

#include "G4UserSteppingAction.hh"

class DetectorConstruction;
class EventAction;

// ---------------------------------------------------------------------------
// SteppingAction
//
// Called after every tracking step.  Implements the trigger / veto logic:
//
//   S2 fired   → record global time as t = 0 for the decay clock
//   Stopper    → detect when the µ+ kinetic energy reaches ~0 (muon stopped)
//   S3 fired   → set veto flag (muon punched through without stopping)
//   CaloL/CaloR → accumulate e+ energy deposits for the asymmetry analysis
// ---------------------------------------------------------------------------
class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction(const DetectorConstruction* detector,
                   EventAction*               eventAction);
    ~SteppingAction() override = default;

    void UserSteppingAction(const G4Step* step) override;

private:
    const DetectorConstruction* fDetector    = nullptr;
    EventAction*                fEventAction = nullptr;
};
