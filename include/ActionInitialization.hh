#pragma once

#include "G4VUserActionInitialization.hh"

class DetectorConstruction;

// ---------------------------------------------------------------------------
// ActionInitialization
//
// Registers all user action classes with Geant4.
// ---------------------------------------------------------------------------
class ActionInitialization : public G4VUserActionInitialization
{
public:
    explicit ActionInitialization(const DetectorConstruction* detector);
    ~ActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build()          const override;

private:
    const DetectorConstruction* fDetector = nullptr;
};
