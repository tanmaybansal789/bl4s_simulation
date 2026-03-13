import uproot
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from scipy.optimize import curve_fit

# Load histogram
f = uproot.open("muon_decay.root")
tL_vals,    tL_edges    = f["tL;1"].to_numpy()
tR_vals,    tR_edges    = f["tR;1"].to_numpy()
tU_vals,    tU_edges    = f["tU;1"].to_numpy()
tD_vals,    tD_edges    = f["tD;1"].to_numpy()
eSpec_vals, eSpec_edges = f["eSpec;1"].to_numpy()

centers_ns = (tL_edges[:-1] + tL_edges[1:]) / 2.0   # [ns]
centers_us = centers_ns * 1e-3                        # [µs]

# Constants
e_charge = 1.602176634e-19    # C
m_mu     = 1.883531627e-28    # kg
B_field  = 0.01               # T — must match simulation
g_true   = 2.0023318418
tau_mu   = 2197.0             # ns (muon lifetime)

omega_theory = g_true * e_charge * B_field / (2.0 * m_mu)
T_theory     = 2.0 * np.pi / omega_theory   # [s]

# Asymmetry calculation
def asymmetry(n1, n2, min_counts=5):
    total = n1 + n2
    mask  = total >= min_counts
    A     = np.where(mask, (n1 - n2) / (total + 1e-30), np.nan)
    sigma = np.where(mask,
                     np.sqrt(np.maximum(1.0 - A**2, 0.0)) / np.sqrt(total + 1e-30),
                     np.nan)
    return A, sigma, mask

A_LR, sLR, mLR = asymmetry(tL_vals, tR_vals, min_counts=5)
A_UD, sUD, mUD = asymmetry(tU_vals, tD_vals, min_counts=5)

def asym_model(t_ns, A_param, omega_fit, phi_fit):
    return A_param * np.sin(omega_fit * t_ns * 1e-9 + phi_fit)

fit_mask = (mLR & ~np.isnan(A_LR) & ~np.isnan(sLR)
            & (sLR > 0) & (centers_ns < 6000.))
t_fit = centers_ns[fit_mask]
y_fit = A_LR[fit_mask]
e_fit = sLR[fit_mask]

try:
    popt, pcov = curve_fit(
        asym_model, t_fit, y_fit,
        p0=[0.25, omega_theory, 0.0],
        sigma=e_fit, absolute_sigma=True, maxfev=20000)
    A_fit, omega_fit, phi_fit = popt
    perr = np.sqrt(np.diag(pcov))

    if A_fit < 0:
        A_fit = -A_fit
        phi_fit += np.pi
    T_fit  = 2.0 * np.pi / abs(omega_fit) * 1e9   # [ns]
    dT_fit = T_fit * perr[1] / abs(omega_fit)       # [ns], uncertainty
    g_fit  = 2.0 * abs(omega_fit) * m_mu / (e_charge * B_field)
    fit_ok = True
except Exception as exc:
    print(f"Fit failed: {exc}")
    A_fit, omega_fit, phi_fit = 0.25, omega_theory, 0.0
    T_fit, dT_fit, g_fit = T_theory * 1e9, 0.0, g_true
    fit_ok = False

t_fine_s  = np.linspace(0, 10e-6, 5000)
t_fine_us = t_fine_s * 1e6
A_fitted  = A_fit * np.sin(abs(omega_fit) * t_fine_s + phi_fit)
A_th_line = 0.25   * np.sin(omega_theory  * t_fine_s)           # thin reference

fig, axes = plt.subplots(2, 2, figsize=(14, 9))
fig.patch.set_facecolor("#f9f9f9")
for ax in axes.flat:
    ax.set_facecolor("#f9f9f9")

ax = axes[0, 0]
ax.step(centers_us, tL_vals, where="mid", color="steelblue",   lw=1.2, label="Left")
ax.step(centers_us, tR_vals, where="mid", color="tomato",      lw=1.2, label="Right",  alpha=0.85)
ax.step(centers_us, tU_vals, where="mid", color="seagreen",    lw=1.2, label="Up",     alpha=0.85)
ax.step(centers_us, tD_vals, where="mid", color="darkorange",  lw=1.2, label="Down",   alpha=0.85)
total_sum = tL_vals + tR_vals + tU_vals + tD_vals
N_peak    = np.max(total_sum + 1e-3)
ax.plot(centers_us, 0.25 * N_peak * np.exp(-centers_ns / tau_mu),
        color="gray", lw=0.9, ls="--", label=r"$e^{-t/\tau_\mu}$")
ax.set_xlabel(r"Decay time $t$ [µs]", fontsize=11)
ax.set_ylabel("Counts / bin", fontsize=11)
ax.set_title("Decay time spectra (all 4 arms)", fontsize=12)
ax.legend(fontsize=8, ncol=2)
ax.set_ylim(bottom=0)
ax.xaxis.set_major_locator(ticker.MultipleLocator(2))
ax.grid(True, ls=":", lw=0.5, alpha=0.6)

ax = axes[0, 1]
centers_e = (eSpec_edges[:-1] + eSpec_edges[1:]) / 2.0
ax.step(centers_e, eSpec_vals, where="mid", color="mediumslateblue", lw=1.2)
ax.fill_between(centers_e, eSpec_vals, step="mid",
                color="mediumslateblue", alpha=0.2)
ax.axvline(52.8, color="darkviolet", lw=0.9, ls="--",
           label=r"$m_\mu c^2/2 \approx 52.8$ MeV (endpoint)")
ax.set_xlabel("Positron energy at calorimeter [MeV]", fontsize=11)
ax.set_ylabel("Counts / bin", fontsize=11)
ax.set_title(r"Michel spectrum (e$^+$ from $\mu^+$ decay)", fontsize=12)
ax.legend(fontsize=8)
ax.set_xlim(0, 60)
ax.set_ylim(bottom=0)
ax.xaxis.set_major_locator(ticker.MultipleLocator(10))
ax.grid(True, ls=":", lw=0.5, alpha=0.6)

ax = axes[1, 0]
ax.errorbar(centers_us[mLR], A_LR[mLR], yerr=sLR[mLR],
            fmt="o", ms=2.0, color="#5b2c8d", lw=0.0,
            elinewidth=0.7, capsize=1.5, alpha=0.85, label="L/R data")
fit_label = (rf"Fit: $A\sin(\omega t + \phi)$,  $T = {T_fit:.0f}$ ns"
             if fit_ok else rf"Theory: $T = {T_theory*1e9:.0f}$ ns")
ax.plot(t_fine_us, A_fitted,  color="#e07b00", lw=1.8, label=fit_label)
ax.plot(t_fine_us, A_th_line, color="gray",    lw=0.8, ls=":",
        label=rf"Theory ($T = {T_theory*1e9:.0f}$ ns)")
ax.axhline(0, color="gray", lw=0.6)
ax.set_ylim(-0.55, 0.55)
ax.set_xlabel(r"Decay time $t$ [µs]", fontsize=11)
ax.set_ylabel(r"Asymmetry $A_{LR}(t)$", fontsize=11)
ax.set_title("L/R asymmetry — spin precession signal", fontsize=12)
ax.legend(fontsize=8, loc="upper right")
ax.xaxis.set_major_locator(ticker.MultipleLocator(1))
ax.yaxis.set_major_locator(ticker.MultipleLocator(0.25))
ax.grid(True, ls=":", lw=0.5, alpha=0.6)

if fit_ok:
    result_str = (rf"$A = {A_fit:.3f}$" + "\n"
                  + rf"$T_{{fit}} = {T_fit:.1f} \pm {dT_fit:.1f}$ ns" + "\n"
                  + rf"$g_\mu^{{fit}} = {g_fit:.4f}$  (true: {g_true:.4f})")
    ax.text(0.03, 0.06, result_str, transform=ax.transAxes, fontsize=8.5,
            color="#e07b00", verticalalignment="bottom",
            bbox=dict(boxstyle="round,pad=0.35", fc="white", ec="#e07b00", alpha=0.85))

ax = axes[1, 1]
ax.errorbar(centers_us[mUD], A_UD[mUD], yerr=sUD[mUD],
            fmt="o", ms=2.0, color="#1a7a4a", lw=0.0,
            elinewidth=0.7, capsize=1.5, alpha=0.85, label="U/D data")
ax.axhline(0, color="gray", lw=1.0, ls="-", label="Expected (flat)")
ax.set_ylim(-0.55, 0.55)
ax.set_xlabel(r"Decay time $t$ [µs]", fontsize=11)
ax.set_ylabel(r"Asymmetry $A_{UD}(t)$", fontsize=11)
ax.set_title("U/D asymmetry — systematic cross-check", fontsize=12)
ax.legend(fontsize=8, loc="upper right")
ax.xaxis.set_major_locator(ticker.MultipleLocator(1))
ax.yaxis.set_major_locator(ticker.MultipleLocator(0.25))
ax.grid(True, ls=":", lw=0.5, alpha=0.6)
ax.text(0.03, 0.90,
        "B along Y so spin precesses in XZ plane\n so no U/D oscillation expected",
        transform=ax.transAxes, fontsize=8, color="gray", verticalalignment="top")

plt.suptitle("BL4S Muon Decay Simulation", fontsize=13, fontweight="bold", y=1.01)
plt.tight_layout(pad=1.5)
plt.savefig("muon_decay_plots.png", dpi=150, bbox_inches="tight")
plt.show()

print("Saved muon_decay_plots.png")
print(f"Theory:  T = {T_theory*1e9:.1f} ns,  g = {g_true:.7f}")
if fit_ok:
    print(f"Fitted:  T = {T_fit:.1f} ± {dT_fit:.1f} ns,  g = {g_fit:.5f},  A = {A_fit:.3f}")
total_hits = int(np.sum(tL_vals + tR_vals + tU_vals + tD_vals))
print(f"Total calo hits: {total_hits}")