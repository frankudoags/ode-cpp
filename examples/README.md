# ODE Solver Examples

This directory contains example programs demonstrating the ODE solvers.

## Building the Examples

```bash
# From project root
make clean
make build
```

## Running the Examples

### 1. Exponential Decay (Simple Test)
```bash
./build/exponential_decay
```

Tests all three solvers (Euler, RK4, RK45) on the simplest ODE: dy/dt = -λy

Expected output: Shows accuracy comparison and error against exact solution.

### 2. Harmonic Oscillator (2D System)
```bash
./build/harmonic_oscillator
```

Simulates a spring-mass system. Tests energy conservation.

### 3. Lorenz Attractor (Chaotic 3D)
```bash
./build/lorenz
```

Generates the famous butterfly attractor. Saves data to `lorenz.dat`.

Plot with: `gnuplot -e "splot 'lorenz.dat' u 2:3:4 w l"`

### 4. Solver Comparison
```bash
./build/solver_comparison
```

Benchmarks all solvers on the same problem, showing:
- Accuracy
- Number of steps
- Execution time

## Expected Results

**Exponential Decay:**
- Euler: ~0.5% error
- RK4: <0.0001% error  
- RK45: <0.0001% error (fewer steps than RK4)

**Harmonic Oscillator:**
- Energy should be conserved to within 1e-4
- After 2 periods, should return to initial state

**Lorenz:**
- Chaotic behavior (small changes in initial conditions lead to vastly different trajectories)
- Classic butterfly shape in 3D plot

**Solver Comparison:**
- RK4 is ~100x more accurate than Euler with same step size
- RK45 automatically finds optimal step size
- RK4 is fastest for fixed-step problems
- RK45 is best for adaptive accuracy requirements
