#pragma once

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"

// ---------------------------------------------------------------------------
// DetectorConstruction
//
// Builds the muon decay experiment geometry
// ---------------------------------------------------------------------------
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction()  = default;
    ~DetectorConstruction() = default;

    G4VPhysicalVolume* Construct()          override;
    void               ConstructSDandField() override;

    // Getters for logical volumes
    G4LogicalVolume* GetS1Logical()      const { return fLogicS1;      }
    G4LogicalVolume* GetS2Logical()      const { return fLogicS2;      }
    G4LogicalVolume* GetS3Logical()      const { return fLogicS3;      }
    G4LogicalVolume* GetStopperLogical() const { return fLogicStopper; }
    G4LogicalVolume* GetCaloLLogical()   const { return fLogicCaloL;   }
    G4LogicalVolume* GetCaloRLogical()   const { return fLogicCaloR;   }
    G4LogicalVolume* GetCaloULogical()   const { return fLogicCaloU;   }
    G4LogicalVolume* GetCaloDLogical()   const { return fLogicCaloD;   }

private:
    G4LogicalVolume* fLogicS1      = nullptr;
    G4LogicalVolume* fLogicS2      = nullptr;
    G4LogicalVolume* fLogicS3      = nullptr;
    G4LogicalVolume* fLogicStopper = nullptr;
    G4LogicalVolume* fLogicCaloL   = nullptr;
    G4LogicalVolume* fLogicCaloR   = nullptr;
    G4LogicalVolume* fLogicCaloU   = nullptr;
    G4LogicalVolume* fLogicCaloD   = nullptr;
};
