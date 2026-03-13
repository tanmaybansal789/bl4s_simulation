#include "RunAction.hh"

#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

RunAction::RunAction()
{
    G4AccumulableManager::Instance()->RegisterAccumulable(fNValidEvents);

    // ── Analysis manager ─────────────────────────────────────────────────────
    // The constructor is called once per thread (master + each worker).
    // The analysis manager is per-thread in MT mode; output files are merged
    // automatically when the master calls Write()/CloseFile().
    auto* am = G4AnalysisManager::Instance();
    am->SetDefaultFileType("root");
    am->SetFileName("muon_decay");
    am->SetVerboseLevel(1);

    // Enable ntuple merging in multi-threaded mode.
    // Without this, worker-thread ntuple rows are written to per-thread files
    // (muon_decay_t0.root, etc.) and NOT merged into the master output.
    // Histograms merge automatically; ntuples require this explicit flag.
    am->SetNtupleMerging(true);

    // ── Ntuple: one row per calorimeter-side per valid event ─────────────────
    //   col 0  eventID   – Geant4 event number
    //   col 1  side      – 0 = left calorimeter, 1 = right calorimeter
    //   col 2  decayTime – time since S2 trigger (ns); proxy for decay time
    //   col 3  energy    – total energy deposited in that calorimeter (MeV)
    am->CreateNtuple("Positrons", "Muon decay positron hits");
    am->CreateNtupleIColumn("eventID");    // 0
    am->CreateNtupleIColumn("side");       // 1
    am->CreateNtupleDColumn("decayTime");  // 2  [ns]
    am->CreateNtupleDColumn("energy");     // 3  [MeV]
    am->FinishNtuple();

    // ── Histograms ───────────────────────────────────────────────────────────
    // Decay time spectra: 200 bins × 50 ns = 10 µs range.
    // At B = 0.01 T the spin-precession period is ~739 ns → ~14.8 bins/cycle,
    // giving enough resolution to clearly see the L/R asymmetry oscillation.
    // H1 indices: tL=0, tR=1, tU=2, tD=3, eSpec=4
    am->CreateH1("tL",    "Decay time – Left  calorimeter [ns]", 200, 0., 10000.);
    am->CreateH1("tR",    "Decay time – Right calorimeter [ns]", 200, 0., 10000.);
    am->CreateH1("tU",    "Decay time – Up    calorimeter [ns]", 200, 0., 10000.);
    am->CreateH1("tD",    "Decay time – Down  calorimeter [ns]", 200, 0., 10000.);
    am->CreateH1("eSpec", "Positron energy spectrum [MeV]",        60, 0.,    60.);
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
    G4AccumulableManager::Instance()->Reset();

    auto* am = G4AnalysisManager::Instance();
    // Reset keeps the histogram/ntuple definitions alive between interactive
    // runs but clears the fill counts, so /vis/plot sees fresh data each time.
    am->Reset();
    am->OpenFile();

    G4cout << "\n=== Run " << run->GetRunID() << " started ===\n" << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    G4AccumulableManager::Instance()->Merge();

    auto* am = G4AnalysisManager::Instance();
    am->Write();
    // CloseFile(false) writes the ROOT file but keeps histogram data in memory
    // so the GUI can access them via /vis/plot or /vis/reviewPlots after the run.
    // Pass true (or omit the arg) in batch mode if you don't need in-session plotting.
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
