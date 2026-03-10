#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4SystemOfUnits.hh"

SteppingAction::SteppingAction(const DetectorConstruction* detector,
                               EventAction*               eventAction)
    : fDetector(detector), fEventAction(eventAction)
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // Only record deposits in the tracker.
    G4LogicalVolume* volume =
        step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

    if (volume != fDetector->GetTrackerLogical()) return;

    double edep = step->GetTotalEnergyDeposit();
    fEventAction->AddTrackerEdep(edep);
}
