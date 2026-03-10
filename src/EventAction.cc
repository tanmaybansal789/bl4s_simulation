#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"

EventAction::EventAction(RunAction* runAction)
    : fRunAction(runAction)
{}

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
    fTrackerEdep = 0.0;
}

void EventAction::EndOfEventAction(const G4Event* /*event*/)
{
    // Forward accumulated deposit to the run-level accumulator.
    fRunAction->AddTrackerEdep(fTrackerEdep);
}
