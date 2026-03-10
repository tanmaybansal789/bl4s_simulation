#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

ActionInitialization::ActionInitialization(const DetectorConstruction* detector)
    : fDetector(detector)
{}

// Master thread: only RunAction is needed (for output file management, etc.).
void ActionInitialization::BuildForMaster() const
{
    SetUserAction(new RunAction);
}

// Worker threads (or the single thread in sequential mode).
void ActionInitialization::Build() const
{
    auto* runAction   = new RunAction;
    auto* eventAction = new EventAction(runAction);

    SetUserAction(new PrimaryGeneratorAction);
    SetUserAction(runAction);
    SetUserAction(eventAction);
    SetUserAction(new SteppingAction(fDetector, eventAction));
}
