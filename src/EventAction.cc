#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

EventAction::EventAction(RunAction* runAction)
    : fRunAction(runAction)
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
    fS2Fired     = false;
    fS3Fired     = false;
    fMuonStopped = false;
    fS2Time      = 0.;
    for (int s = 0; s < 4; ++s) {
        fCaloEnergy[s] = 0.;
        fCaloTime[s]   = -1.;
    }
}

void EventAction::AddCaloHit(G4int side, G4double time, G4double edep)
{
    if (side < 0 || side > 3) return;
    fCaloEnergy[side] += edep;
    if (fCaloTime[side] < 0.) fCaloTime[side] = time;  // first hit time
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    if (!fS2Fired || !fMuonStopped || fS3Fired) return;

    fRunAction->AddValidEvent();

    auto* am    = G4AnalysisManager::Instance();
    int   evtID = event->GetEventID();

    // fill data into analysis manager for each calorimeter side with a valid hit (time > 0)
    for (int s = 0; s < 4; ++s) {
        if (fCaloTime[s] < 0.) continue;
        G4double e  = fCaloEnergy[s] / MeV;

        G4double dt = (fCaloTime[s] - fS2Time) / ns;

        am->FillNtupleIColumn(0, evtID);
        am->FillNtupleIColumn(1, s);
        am->FillNtupleDColumn(2, dt);
        am->FillNtupleDColumn(3, e);
        am->AddNtupleRow();

        am->FillH1(s, dt);    // tL(0), tR(1), tU(2), tD(3)
        am->FillH1(4, e);     // eSpec
    }
}
