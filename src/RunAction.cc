#include "RunAction.hh"

#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

RunAction::RunAction()
{
    // Register accumulables so Geant4 can merge them across threads.
    G4AccumulableManager::Instance()->RegisterAccumulable(fTrackerEdep);
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
    G4AccumulableManager::Instance()->Reset();
    G4cout << "\n=== Run " << run->GetRunID() << " started ===\n" << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    // Merge values from all worker threads.
    G4AccumulableManager::Instance()->Merge();

    int    nEvents = run->GetNumberOfEvent();
    double edep    = fTrackerEdep.GetValue();

    G4cout << "\n──────────────────────────────────────────────\n"
           << "  Run " << run->GetRunID() << " summary\n"
           << "  Events simulated : " << nEvents << "\n"
           << "  Tracker Edep     : " << G4BestUnit(edep, "Energy")
                                      << "  (total)\n"
           << "  Tracker Edep/evt : " << G4BestUnit(edep / std::max(nEvents, 1), "Energy")
                                      << "\n"
           << "──────────────────────────────────────────────\n" << G4endl;
}
