#pragma once

#include "G4VUserDetectorConstruction.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"

// ---------------------------------------------------------------------------
// DetectorConstruction
//
// Builds the experimental geometry:
//   World (Air)
//     └─ Target  (Lead slab  – where primaries interact)
//     └─ Tracker (Silicon layer – downstream of the target)
//
// Dimensions and materials can be changed here without touching any other file.
// ---------------------------------------------------------------------------
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction()  = default;
    ~DetectorConstruction() = default;

    // Called by Geant4 to build and return the physical world volume.
    G4VPhysicalVolume* Construct() override;

    // Accessors used by SteppingAction to identify sensitive volumes.
    const G4LogicalVolume* GetTargetLogical()  const { return fTargetLogical;  }
    const G4LogicalVolume* GetTrackerLogical() const { return fTrackerLogical; }

private:
    // ── Geometry parameters (edit here to change the setup) ────────────────
    static constexpr double kWorldHalfX   = 100.0;  // cm
    static constexpr double kWorldHalfY   = 100.0;  // cm
    static constexpr double kWorldHalfZ   = 200.0;  // cm

    static constexpr double kTargetHalfZ  =   5.0;  // cm  (lead slab thickness/2)
    static constexpr double kTargetPosZ   = -50.0;  // cm  (centre position in world)

    static constexpr double kTrackerHalfZ =   0.1;  // cm  (silicon layer thickness/2)
    static constexpr double kTrackerPosZ  =  50.0;  // cm  (centre position in world)

    // Logical volumes kept as members so SteppingAction can query them.
    G4LogicalVolume* fTargetLogical  = nullptr;
    G4LogicalVolume* fTrackerLogical = nullptr;

    // ── Helpers ────────────────────────────────────────────────────────────
    void DefineMaterials();
};
