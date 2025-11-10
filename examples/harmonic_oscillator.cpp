#include "ode/rk4.hpp"
#include "ode/rk45.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>

/**
 * Example 2: Harmonic Oscillator (2D System)
 *
 * Problem: d²x/dt² = -ω²x (undamped spring-mass system)
 *
 * Convert to first-order system:
 *   dx/dt = v
 *   dv/dt = -ω²x
 *
 * State vector: y = [position, velocity]
 *
 * Exact solution: x(t) = A*cos(ωt) + B*sin(ωt)
 * With x(0) = 1, v(0) = 0: x(t) = cos(ωt)
 *
 * Key property: Total energy E = ½(v² + ω²x²) is conserved
 *
 * This example demonstrates:
 * - Multi-dimensional ODE systems (2D)
 * - Energy conservation in physical systems
 * - How RK4 preserves important system properties
 */

int main()
{
    using namespace ode;

    std::cout << "=== Harmonic Oscillator ===\n\n";

    double omega = 2.0 * M_PI;

    ODEFunction harmonic = [omega](const State &y, Time t) -> State {
        double x = y[0];
        double v = y[1];
        return {v, -omega * omega * x};
    };

    State initial = {1.0, 0.0};

    SolverConfig config{
        .time_start = 0.0,
        .time_end = 2.0,
        .step_size = 0.01,
        .initial_state = initial
    };

    RK4Solver rk4_solver;
    Solution sol = rk4_solver.solve(harmonic, config);

    auto energy = [omega](const State &y) {
        double x = y[0];
        double v = y[1];
        return 0.5 * (v * v + omega * omega * x * x);
    };

    double E_initial = energy(sol.front().state);
    double E_final = energy(sol.back().state);

    std::cout << std::fixed << std::setprecision(8);
    std::cout << "Initial energy: " << E_initial << "\n";
    std::cout << "Final energy:   " << E_final << "\n";
    std::cout << "Energy drift:   " << std::abs(E_final - E_initial) << "\n";

    return 0;
}
