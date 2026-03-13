#include "RunAction.hh"

#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

RunAction::RunAction()
{
    G4AccumulableManager::Instance()->RegisterAccumulable(fNValidEvents);

    // setup analysis manager
    auto* am = G4AnalysisManager::Instance();
    am->SetDefaultFileType("root");
    am->SetFileName("muon_decay");
    am->SetVerboseLevel(1);

    am->SetNtupleMerging(true);

    am->CreateNtuple("Positrons", "Muon decay positron hits");
    am->CreateNtupleIColumn("eventID");    // 0
    am->CreateNtupleIColumn("side");       // 1
    am->CreateNtupleDColumn("decayTime");  // 2  [ns]
    am->CreateNtupleDColumn("energy");     // 3  [MeV]
    am->FinishNtuple();

    am->CreateH1("tL",    "Decay time - Left  calorimeter [ns]", 200, 0., 10000.);
    am->CreateH1("tR",    "Decay time - Right calorimeter [ns]", 200, 0., 10000.);
    am->CreateH1("tU",    "Decay time - Up    calorimeter [ns]", 200, 0., 10000.);
    am->CreateH1("tD",    "Decay time - Down  calorimeter [ns]", 200, 0., 10000.);
    am->CreateH1("eSpec", "Positron energy spectrum [MeV]",        60, 0.,    60.);
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
    G4AccumulableManager::Instance()->Reset();

    auto* am = G4AnalysisManager::Instance();

    am->Reset();
    am->OpenFile();

    G4cout << "\n=== Run " << run->GetRunID() << " started ===\n" << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    G4AccumulableManager::Instance()->Merge();

    auto* am = G4AnalysisManager::Instance();
    am->Write();
    am->CloseFile(false);

    G4cout << "\n──────────────────────────────────────────────\n"
           << "  Run " << run->GetRunID() << " summary\n"
           << "  Events simulated : " << run->GetNumberOfEvent() << "\n"
           << "  Valid events     : " << fNValidEvents.GetValue()
           << "  (mu stopped, S2 fired, S3 not fired)\n"
           << "  Output file      : muon_decay.root\n"
           << "  Histogram tL/tR  : decay time, left/right calo\n"
           << "  Histogram tU/tD  : decay time, up/down calo\n"
           << "  Ntuple Positrons : per-hit data\n"
           << "──────────────────────────────────────────────\n" << G4endl;
}
