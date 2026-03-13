#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4Mag_SpinEqRhs.hh"
#include "G4ClassicalRK4.hh"
#include "G4ChordFinder.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    auto* nist = G4NistManager::Instance();

    auto* air       = nist->FindOrBuildMaterial("G4_AIR");
    auto* scint     = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    auto* alumin    = nist->FindOrBuildMaterial("G4_Al");
    auto* leadGlass = nist->FindOrBuildMaterial("G4_GLASS_LEAD");

    // We use 40x40x30 cm world, with the gun at z=−10 cm and the calos extending to ±14.5 cm in X/Y.
    auto* worldSolid   = new G4Box("World", 20.*cm, 20.*cm, 15.*cm);
    auto* worldLogical = new G4LogicalVolume(worldSolid, air, "World");
    auto* worldPhys    = new G4PVPlacement(nullptr, G4ThreeVector(),
                                           worldLogical, "World",
                                           nullptr, false, 0, /*checkOverlaps=*/true);
    worldLogical->SetVisAttributes(G4VisAttributes::GetInvisible());

    auto place = [&](G4LogicalVolume* lv, const char* name, G4ThreeVector pos) {
        new G4PVPlacement(nullptr, pos, lv, name, worldLogical, false, 0, true);
    };

    // S1 Scintillator at z = −8
    auto* s1Solid = new G4Box("S1", 5.*cm, 5.*cm, 0.25*cm);
    fLogicS1 = new G4LogicalVolume(s1Solid, scint, "S1");
    place(fLogicS1, "S1", G4ThreeVector(0., 0., -8.*cm));
    {
        auto* vis = new G4VisAttributes(G4Colour(0.2, 0.2, 1.0, 0.6));
        vis->SetForceSolid(true);
        fLogicS1->SetVisAttributes(vis);
    }

    // S2 Scintillator t=0 trigger at z = −2 cm
    auto* s2Solid = new G4Box("S2", 2.5*cm, 2.5*cm, 0.25*cm);
    fLogicS2 = new G4LogicalVolume(s2Solid, scint, "S2");
    place(fLogicS2, "S2", G4ThreeVector(0., 0., -2.*cm));
    {
        auto* vis = new G4VisAttributes(G4Colour(0.0, 0.8, 0.0, 0.6));
        vis->SetForceSolid(true);
        fLogicS2->SetVisAttributes(vis);
    }

    // aluminium cylinder at z = 0 
    // 1 cm diameter × 3 cm length rod (R=0.5 cm, halfZ=1.5 cm).
    auto* stopSolid = new G4Tubs("Stopper", 0., 0.5*cm, 1.5*cm, 0., 360.*deg);
    fLogicStopper = new G4LogicalVolume(stopSolid, alumin, "Stopper");
    place(fLogicStopper, "Stopper", G4ThreeVector(0., 0., 0.));
    fLogicStopper->SetUserLimits(new G4UserLimits(0.1*mm));
    {
        auto* vis = new G4VisAttributes(G4Colour(0.7, 0.7, 0.7, 0.8));
        vis->SetForceSolid(true);
        fLogicStopper->SetVisAttributes(vis);
    }

    // S3 Scintillator veto at z = 3
    auto* s3Solid = new G4Box("S3", 5.*cm, 5.*cm, 0.25*cm);
    fLogicS3 = new G4LogicalVolume(s3Solid, scint, "S3");
    place(fLogicS3, "S3", G4ThreeVector(0., 0., +3.*cm));
    {
        auto* vis = new G4VisAttributes(G4Colour(0.2, 0.2, 1.0, 0.6));
        vis->SetForceSolid(true);
        fLogicS3->SetVisAttributes(vis);
    }

    // 4 calorimeters 8 cm from center
    // I still don't rlly get why we need 4 instead of just 2, but that is the design we have rn so it's fine
    {
        auto* vis = new G4VisAttributes(G4Colour(0.8, 0.1, 0.1, 0.5));
        vis->SetForceSolid(true);

        auto* caloLR = new G4Box("CaloLR", 1.5*cm, 6.5*cm, 6.5*cm);

        fLogicCaloL = new G4LogicalVolume(caloLR, leadGlass, "CaloL");
        fLogicCaloL->SetVisAttributes(vis);
        place(fLogicCaloL, "CaloL", G4ThreeVector(-8.*cm, 0., 0.));

        fLogicCaloR = new G4LogicalVolume(caloLR, leadGlass, "CaloR");
        fLogicCaloR->SetVisAttributes(vis);
        place(fLogicCaloR, "CaloR", G4ThreeVector(+8.*cm, 0., 0.));

        auto* caloUD = new G4Box("CaloUD", 6.5*cm, 1.5*cm, 6.5*cm);

        fLogicCaloU = new G4LogicalVolume(caloUD, leadGlass, "CaloU");
        fLogicCaloU->SetVisAttributes(vis);
        place(fLogicCaloU, "CaloU", G4ThreeVector(0., +8.*cm, 0.));

        fLogicCaloD = new G4LogicalVolume(caloUD, leadGlass, "CaloD");
        fLogicCaloD->SetVisAttributes(vis);
        place(fLogicCaloD, "CaloD", G4ThreeVector(0., -8.*cm, 0.));
    }

    return worldPhys;
}

void DetectorConstruction::ConstructSDandField()
{
    // T is roughly 739 ns → ~14.8 bins/cycle at 50 ns/bin so should be sufficient.
    auto* magField = new G4UniformMagField(G4ThreeVector(0., 0.01*tesla, 0.));

    // Spin integration using Runge-Kutta
    auto* spinEqn  = new G4Mag_SpinEqRhs(magField);
    auto* stepper  = new G4ClassicalRK4(spinEqn, 12);
    auto* chordFinder = new G4ChordFinder(magField, 1.e-2*mm, stepper);

    // Register on the thread-local field manager so that it's used in the tracking of all volumes.
    auto* globalFldMgr =
        G4TransportationManager::GetTransportationManager()->GetFieldManager();
    globalFldMgr->SetDetectorField(magField);
    globalFldMgr->SetChordFinder(chordFinder);
}
