#pragma once

#include "FTFP_BERT.hh"
#include "G4StepLimiterPhysics.hh"

// ---------------------------------------------------------------------------
// PhysicsList
//
// Extends FTFP_BERT (Fritiof string + Bertini cascade), the Geant4-recommended
// list for high-energy hadron beams (SPS / BL4S range).
//
// To switch lists, change the base class (e.g. QGSP_BERT, FTFP_BERT_HP).
// G4StepLimiterPhysics is added so macro commands like /run/setCut work.
// ---------------------------------------------------------------------------
class PhysicsList : public FTFP_BERT
{
public:
    PhysicsList();
    ~PhysicsList() override = default;
};
