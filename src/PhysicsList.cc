#include "PhysicsList.hh"

#include "G4EmStandardPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4StepLimiterPhysics.hh"

#include "G4DecayWithSpin.hh"
#include "G4MuonDecayChannelWithSpin.hh"
#include "G4MuonRadiativeDecayChannelWithSpin.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4DecayTable.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"

PhysicsList::PhysicsList()
    : G4VModularPhysicsList()
{
    SetVerboseLevel(0);
    // standard EM physics, nothing special here
    RegisterPhysics(new G4EmStandardPhysics());
    // decay framework - will be replaced with spin-aware version in ConstructProcess()
    RegisterPhysics(new G4DecayPhysics());
    // step limiter - we need this so muons don't just skip through the stopper and S3
    RegisterPhysics(new G4StepLimiterPhysics());
}

void PhysicsList::ConstructParticle()
{
    G4VModularPhysicsList::ConstructParticle();

    // Swap out G4MuonDecayChannel -> G4MuonDecayChannelWithSpin
    {
        auto* table = new G4DecayTable();
        table->Insert(new G4MuonDecayChannelWithSpin("mu+", 0.986));
        table->Insert(new G4MuonRadiativeDecayChannelWithSpin("mu+", 0.014));
        G4MuonPlus::MuonPlusDefinition()->SetDecayTable(table);
    }
    {
        auto* table = new G4DecayTable();
        table->Insert(new G4MuonDecayChannelWithSpin("mu-", 0.986));
        table->Insert(new G4MuonRadiativeDecayChannelWithSpin("mu-", 0.014));
        G4MuonMinus::MuonMinusDefinition()->SetDecayTable(table);
    }
}

void PhysicsList::ConstructProcess()
{
    // let all registered modules build their processes first.
    G4VModularPhysicsList::ConstructProcess();

    // for mu+/mu-, remove the default G4Decay process and replace with G4DecayWithSpin
    auto* pTable = G4ParticleTable::GetParticleTable();
    for (const char* name : {"mu+", "mu-"}) {
        auto* particle = pTable->FindParticle(name);
        if (!particle) continue;

        auto* pMan = particle->GetProcessManager();

        G4VProcess* oldDecay = pMan->GetProcess("Decay");
        if (oldDecay) pMan->RemoveProcess(oldDecay);

        // plug in our new decay with spin
        auto* dws = new G4DecayWithSpin();
        pMan->AddProcess(dws);
        pMan->SetProcessOrdering(dws, idxPostStep);
        pMan->SetProcessOrdering(dws, idxAtRest);
    }
}
