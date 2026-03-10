# BL4S Simulation

A [Geant4](https://geant4.org/) Monte Carlo simulation of a fixed-target particle physics experiment, built for the [Beamline for Schools (BL4S)](https://beamlineforschools.cern.ch/) programme at CERN.

A 120 GeV proton beam strikes a lead target, producing a hadronic shower. Secondary particles are tracked downstream into a silicon detector, where the deposited energy is recorded.

---

## Requirements

| Dependency | Version | Notes |
|---|---|---|
| Geant4 | 11.2.2 | Installed at `/Users/puru/geant4-install` |
| CMake | ≥ 3.16 | |
| Qt | 5.15 | At `/opt/homebrew/Cellar/qt@5/5.15.18` — needed for the GUI |
| AppleClang / GCC | ≥ C++17 | |

---

## Quick Start

```bash
# Interactive 3-D GUI (shows geometry + particle tracks)
./run.sh

# Batch mode — 100 events, prints energy statistics, no window
./run.sh batch
```

Or manually from the `build/` directory:

```bash
source /Users/puru/geant4-install/bin/geant4.sh
cd build/
./bl4s_sim          # GUI
./bl4s_sim run.mac  # batch
```

---

## Project Structure

```
bl4s_simulation/
├── main.cc                      # Entry point
├── CMakeLists.txt               # Build system
├── run.sh                       # Convenience launcher
│
├── include/                     # Header files (.hh)
│   ├── DetectorConstruction.hh
│   ├── PhysicsList.hh
│   ├── ActionInitialization.hh
│   ├── PrimaryGeneratorAction.hh
│   ├── RunAction.hh
│   ├── EventAction.hh
│   └── SteppingAction.hh
│
├── src/                         # Implementation files (.cc)
│   ├── DetectorConstruction.cc
│   ├── PhysicsList.cc
│   ├── ActionInitialization.cc
│   ├── PrimaryGeneratorAction.cc
│   ├── RunAction.cc
│   ├── EventAction.cc
│   └── SteppingAction.cc
│
├── macros/
│   ├── vis.mac                  # Visualisation setup (GUI mode)
│   └── run.mac                  # Batch run (100 events)
│
└── build/                       # CMake output — compiled binary lives here
```

---

## File-by-File Reference

### `main.cc`

The program entry point. Responsible for:

- Creating the Geant4 **run manager** (multi-threaded by default).
- Registering the three mandatory user classes: detector, physics list, and action initialisation.
- Starting the **visualisation manager** (registers all available graphics drivers).
- Deciding whether to run in **interactive mode** (no argument → opens the Qt window and loads `vis.mac`) or **batch mode** (a `.mac` file is passed as an argument).

You should rarely need to edit this file. The interesting physics configuration lives in the classes it instantiates.

---

### `CMakeLists.txt`

The CMake build script. It:

- Requires C++17.
- Finds the Geant4 installation and links all its UI and visualisation components.
- Globs all `.cc` files from `src/` into the `bl4s_sim` executable.
- Copies every `.mac` file from `macros/` into the build directory so the binary can find them at runtime.

**To rebuild after changing source files:**
```bash
cd build/
make -j$(sysctl -n hw.logicalcpu)
```

---

### `run.sh`

A convenience shell script that:

1. Sources the Geant4 environment (`geant4.sh` — sets library paths and data directories).
2. Sets `QT_MAC_WANTS_LAYER=1` — required on Apple Silicon to prevent Qt from crashing when opening an OpenGL-backed window.
3. Runs CMake + `make` automatically if the binary does not yet exist.
4. Launches `./bl4s_sim` in GUI or batch mode depending on whether `batch` was passed as an argument.

---

### `include/` & `src/` — The Physics Classes

Geant4 applications are structured around a fixed set of user-provided classes. Each pair of `.hh`/`.cc` files below is one of those classes.

---

#### `DetectorConstruction` — *What the experiment looks like*

Builds the physical geometry of the experiment. Geant4 calls `Construct()` once at initialisation.

The geometry (along the beam axis, +Z direction):

```
 [beam gun]          [Lead target]        [Silicon tracker]
  z = −180 cm          z = −50 cm             z = +50 cm
                   ┌──────────────┐       ┌──────────────┐
  ══════════════>  │   Pb  10 cm  │  ...  │   Si  2 mm   │
                   └──────────────┘       └──────────────┘
```

| Volume | Material | Half-thickness | Position (Z) | Colour in GUI |
|---|---|---|---|---|
| World | Air | 200 cm | origin | invisible |
| Target | Lead (`G4_Pb`) | 5 cm | −50 cm | grey |
| Tracker | Silicon (`G4_Si`) | 1 mm | +50 cm | cyan |

**To modify the geometry**, change the `constexpr` constants at the top of `DetectorConstruction.hh` — no other files need editing:

```cpp
static constexpr double kTargetHalfZ  =  5.0;  // cm
static constexpr double kTargetPosZ   = -50.0;  // cm
static constexpr double kTrackerHalfZ =  0.1;   // cm (= 2 mm)
static constexpr double kTrackerPosZ  = 50.0;   // cm
```

Materials come from Geant4's built-in NIST database (`G4NistManager`), so their full elemental composition and density are already defined correctly.

---

#### `PhysicsList` — *What physics processes are active*

Inherits from **FTFP_BERT**, the Geant4 reference physics list recommended for high-energy hadron beam experiments (SPS, LHC test-beams, BL4S).

- **FTFP** (Fritiof String Model) handles interactions above ~3 GeV — relevant for the primary 120 GeV proton.
- **BERT** (Bertini Intranuclear Cascade) handles lower-energy secondaries below ~3 GeV.
- `G4StepLimiterPhysics` is added on top, enabling the `/run/setCut` macro command.

**To switch physics lists**, change the base class in `PhysicsList.hh`:
```cpp
// e.g. swap FTFP_BERT for QGSP_BERT or FTFP_BERT_HP (adds high-precision neutrons)
class PhysicsList : public FTFP_BERT   // ← change this line
```

---

#### `ActionInitialization` — *Wires everything together*

Tells Geant4 which user action objects to use. In multi-threaded mode, `Build()` is called once per worker thread and `BuildForMaster()` is called on the master thread.

This is a plumbing file — you generally only edit it if you add a new action class (e.g. a `StackingAction`).

---

#### `PrimaryGeneratorAction` — *The particle beam*

Fires one particle per event using `G4ParticleGun`.

Default beam configuration:

| Parameter | Value |
|---|---|
| Particle | Proton |
| Kinetic energy | 120 GeV |
| Start position | (0, 0, −180 cm) — upstream of the target |
| Direction | +Z (straight at the target) |

**To change the beam**, edit `PrimaryGeneratorAction.cc`:

```cpp
// Change particle type:
G4ParticleTable::GetParticleTable()->FindParticle("pi+");  // pion
G4ParticleTable::GetParticleTable()->FindParticle("e-");   // electron

// Change energy:
fGun->SetParticleEnergy(10.0 * GeV);

// Add a beam spread (replace G4ParticleGun with G4GeneralParticleSource):
// See Geant4 GPS documentation for more advanced beam profiles.
```

---

#### `RunAction` — *Per-run statistics*

Runs at the start and end of each `/run/beamOn N` call.

- Registers `fTrackerEdep` as a `G4Accumulable` — Geant4 automatically sums this across all worker threads at the end of the run.
- Prints a summary at the end:

```
──────────────────────────────────────────────
  Run 0 summary
  Events simulated : 100
  Tracker Edep     : 3.34 GeV  (total)
  Tracker Edep/evt : 33.4 MeV
──────────────────────────────────────────────
```

**To add more observables** (e.g. hit count, max energy deposit), add another `G4Accumulable<T>` member, register it in the constructor, and accumulate it from `EventAction`.

---

#### `EventAction` — *Per-event accumulation*

Resets `fTrackerEdep` to zero at the start of each event. At the end of the event, forwards the total to `RunAction::AddTrackerEdep()`.

`SteppingAction` calls `AddTrackerEdep()` for each step it finds inside the tracker, building up the total step-by-step.

---

#### `SteppingAction` — *Per-step scoring*

Called by Geant4 after every single tracking step (the finest granularity available).

Checks whether the current step is inside the tracker logical volume. If it is, it reads `step->GetTotalEnergyDeposit()` and adds it to `EventAction`. Steps in any other volume (world, target) are ignored.

**To score in a different volume**, change the volume comparison:
```cpp
if (volume != fDetector->GetTrackerLogical()) return;
// ↑ swap GetTrackerLogical() for GetTargetLogical() to score in the lead instead
```

---

### `macros/vis.mac` — *Interactive GUI setup*

Executed automatically when you launch the GUI (`./bl4s_sim` with no arguments). Controls everything about the visualisation:

```
/run/initialize                         ← must come first (builds geometry)
/vis/open TSG 800x600-0+0              ← open Qt window (TSG = Metal-compatible on Apple Silicon)
/vis/drawVolume                         ← render the geometry
/vis/viewer/set/viewpointThetaPhi 70 20 ← oblique camera angle
/vis/viewer/zoom 1.5
/vis/scene/add/axes 0 0 0 50 cm        ← show X/Y/Z axes
/vis/scene/add/trajectories smooth      ← draw particle tracks
/vis/scene/endOfEventAction accumulate 10
/run/beamOn 5                           ← fire 5 events on startup
```

Common things to change here:

| Goal | Command |
|---|---|
| More events on startup | `/run/beamOn 20` |
| Show more tracks at once | `endOfEventAction accumulate 50` |
| Top-down view | `/vis/viewer/set/viewpointThetaPhi 0 0` |
| Side view | `/vis/viewer/set/viewpointThetaPhi 90 0` |

---

### `macros/run.mac` — *Batch mode script*

Used when running `./bl4s_sim run.mac` or `./run.sh batch`. Disables visualisation, initialises the simulation, and runs 100 events. Results are printed to the terminal by `RunAction`.

**To run more events**, change `/run/beamOn 100` to any number.

---

## How a Single Event Flows Through the Code

```
main.cc  →  G4RunManager::BeamOn(1)
               │
               ├─ RunAction::BeginOfRunAction()        [once per /run/beamOn]
               │
               ├─ EventAction::BeginOfEventAction()    [once per event]
               │     resets fTrackerEdep = 0
               │
               ├─ PrimaryGeneratorAction::GeneratePrimaries()
               │     fires 1 proton at 120 GeV along +Z
               │
               ├─ [Geant4 tracks primary + all secondaries]
               │     for each step in the tracker:
               │       SteppingAction::UserSteppingAction()
               │         → EventAction::AddTrackerEdep(edep)
               │
               ├─ EventAction::EndOfEventAction()
               │     → RunAction::AddTrackerEdep(total)
               │
               └─ RunAction::EndOfRunAction()          [once per /run/beamOn]
                     prints summary
```

---

## Extending the Simulation

| What you want to add | Where to look |
|---|---|
| Different target material | `DetectorConstruction.hh` constants / `G4NistManager` |
| Multiple detector layers | Add more `G4PVPlacement` calls in `DetectorConstruction.cc` |
| Magnetic field | Create a `G4MagneticField` subclass, add `FieldConstruction.hh/cc` |
| Beam divergence / spread | Replace `G4ParticleGun` with `G4GeneralParticleSource` in `PrimaryGeneratorAction` |
| CSV / ROOT output | Add a file-writing call inside `RunAction::EndOfRunAction()` |
| Different physics | Change the base class of `PhysicsList` |
| More observables | Add `G4Accumulable` members to `RunAction`, score in `SteppingAction` |
