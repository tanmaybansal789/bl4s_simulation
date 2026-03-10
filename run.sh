#!/usr/bin/env zsh
# ─────────────────────────────────────────────────────────────────────────────
# run.sh  –  Build (if needed) and launch the BL4S simulation.
#
# Usage:
#   ./run.sh          →  interactive Qt GUI
#   ./run.sh batch    →  batch mode (run.mac, 100 events, no GUI)
# ─────────────────────────────────────────────────────────────────────────────

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
GEANT4_INSTALL="/Users/puru/geant4-install"

# Source Geant4 environment (sets library paths, data directories, etc.)
source "$GEANT4_INSTALL/bin/geant4.sh"

# Required on Apple Silicon: tells Qt to use a Metal-backed layer instead of
# the deprecated OpenGL surface, preventing crashes on M-series Macs.
export QT_MAC_WANTS_LAYER=1

# Build if the binary doesn't exist yet.
if [ ! -f "$BUILD_DIR/bl4s_sim" ]; then
    echo "==> Binary not found – building..."
    cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
        -DCMAKE_PREFIX_PATH="$GEANT4_INSTALL;/opt/homebrew/Cellar/qt@5/5.15.18" \
        -DCMAKE_BUILD_TYPE=Release
    cmake --build "$BUILD_DIR" -j"$(sysctl -n hw.logicalcpu)"
fi

cd "$BUILD_DIR"

if [ "$1" = "batch" ]; then
    ./bl4s_sim run.mac
else
    ./bl4s_sim
fi
