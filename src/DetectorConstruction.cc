#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4Colour.hh"

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // ── Materials from NIST database ─────────────────────────────────────────
    auto* nist = G4NistManager::Instance();
    G4Material* air     = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* lead    = nist->FindOrBuildMaterial("G4_Pb");
    G4Material* silicon = nist->FindOrBuildMaterial("G4_Si");

    // ── World volume ──────────────────────────────────────────────────────────
    auto* worldSolid   = new G4Box("World",
                                   kWorldHalfX * cm,
                                   kWorldHalfY * cm,
                                   kWorldHalfZ * cm);
    auto* worldLogical = new G4LogicalVolume(worldSolid, air, "World");
    auto* worldPhys    = new G4PVPlacement(nullptr,          // no rotation
                                           G4ThreeVector(),  // at origin
                                           worldLogical,
                                           "World",
                                           nullptr,          // no mother
                                           false, 0,
                                           true);            // check overlaps

    // 1 tesla magnetic field in z direction
    G4ThreeVector fieldVector(0.0, 0.0, 1.0 * CLHEP::tesla);
    G4UniformMagField* magField = new G4UniformMagField(fieldVector);
    G4FieldManager* fieldMgr = worldLogical->GetFieldManager();
    fieldMgr->SetDetectorField(magField);
    fieldMgr->CreateChordFinder(magField);

    // Make the world volume invisible.
    worldLogical->SetVisAttributes(G4VisAttributes::GetInvisible());

    // ── Target (lead slab) ────────────────────────────────────────────────────
    auto* targetSolid = new G4Box("Target",
                                  kWorldHalfX * cm,
                                  kWorldHalfY * cm,
                                  kTargetHalfZ * cm);
    fTargetLogical = new G4LogicalVolume(targetSolid, lead, "Target");
    new G4PVPlacement(nullptr,
                      G4ThreeVector(0, 0, kTargetPosZ * cm),
                      fTargetLogical,
                      "Target",
                      worldLogical,
                      false, 0, true);

    // Grey, semi-transparent lead block.
    auto* targetVis = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.8));
    targetVis->SetForceSolid(true);
    fTargetLogical->SetVisAttributes(targetVis);

    // ── Tracker (silicon layer) ───────────────────────────────────────────────
    auto* trackerSolid = new G4Box("Tracker",
                                   kWorldHalfX * cm,
                                   kWorldHalfY * cm,
                                   kTrackerHalfZ * cm);
    fTrackerLogical = new G4LogicalVolume(trackerSolid, silicon, "Tracker");
    new G4PVPlacement(nullptr,
                      G4ThreeVector(0, 0, kTrackerPosZ * cm),
                      fTrackerLogical,
                      "Tracker",
                      worldLogical,
                      false, 0, true);


    // Cyan, semi-transparent silicon detector.
    auto* trackerVis = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0, 0.5));
    trackerVis->SetForceSolid(true);
    fTrackerLogical->SetVisAttributes(trackerVis);

    return worldPhys;
}
