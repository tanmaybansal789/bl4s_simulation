#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4SystemOfUnits.hh"

// ---------------------------------------------------------------------------
// main
//
// Usage:
//   ./bl4s_sim              →  interactive Qt GUI (no macro)
//   ./bl4s_sim run.mac      →  batch mode, executes run.mac
//   ./bl4s_sim vis.mac      →  interactive GUI, loads vis.mac for setup
// ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    // ── UI session (interactive if no macro supplied) ─────────────────────────
    G4UIExecutive* ui = nullptr;
    if (argc == 1) {
        ui = new G4UIExecutive(argc, argv);  // auto-selects Qt if available
    }

    // ── Run manager (multi-threaded by default if compiled with MT support) ───
    auto* runManager = G4RunManagerFactory::CreateRunManager(
        G4RunManagerType::Default);

    // ── Register mandatory user classes ───────────────────────────────────────
    auto* detector = new DetectorConstruction;
    runManager->SetUserInitialization(detector);
    runManager->SetUserInitialization(new PhysicsList);
    runManager->SetUserInitialization(new ActionInitialization(detector));

    // ── Initialise visualisation drivers ──────────────────────────────────────
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // ── Get the pointer to the UI manager ────────────────────────────────────
    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    // ── Run: batch mode or interactive ────────────────────────────────────────
    if (argc > 1) {
        // Batch: execute the supplied macro file.
        G4String macroFile = argv[1];
        UImanager->ApplyCommand("/control/execute " + macroFile);
    } else {
        // Interactive: load the visualisation setup, then hand off to the GUI.
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
        delete ui;
    }

    // ── Cleanup ───────────────────────────────────────────────────────────────
    delete visManager;
    delete runManager;
    return 0;
}
