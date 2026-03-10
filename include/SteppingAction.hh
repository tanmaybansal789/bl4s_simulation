#pragma once

#include "G4UserSteppingAction.hh"

class DetectorConstruction;
class EventAction;

// ---------------------------------------------------------------------------
// SteppingAction
//
// Runs after every individual tracking step.
// Records the energy deposited in the tracker logical volume.
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
