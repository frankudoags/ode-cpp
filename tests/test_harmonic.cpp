#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "ode/rk4.hpp"
#include <cmath>

using namespace ode;

// Harmonic oscillator: d²x/dt² = -ω²x
// System: dx/dt = v, dv/dt = -ω²x
// Energy: E = 0.5*(v² + ω²x²) should be conserved

TEST_CASE("RK4: Harmonic oscillator energy conservation", "[rk4][harmonic]") {
    double omega = 2.0 * M_PI;
    
    ODEFunction harmonic = [omega](const State& y, Time t) -> State {
        double x = y[0];
        double v = y[1];
        return {v, -omega * omega * x};
    };
    
    SolverConfig config{
        .time_start = 0.0,
        .time_end = 2.0,  // Two periods
        .step_size = 0.01,
        .initial_state = {1.0, 0.0}  // x=1, v=0
    };
    
    RK4Solver solver;
    Solution sol = solver.solve(harmonic, config);
    
    // Energy function
    auto energy = [omega](const State& y) {
        double x = y[0];
        double v = y[1];
        return 0.5 * (v * v + omega * omega * x * x);
    };
    
    double E_initial = energy(sol.front().state);
    double E_final = energy(sol.back().state);
    double drift = std::abs(E_final - E_initial);
    
    // Energy should be conserved to high precision
    REQUIRE(drift < 1e-4);
}

TEST_CASE("RK4: Harmonic oscillator periodicity", "[rk4][harmonic]") {
    double omega = 2.0 * M_PI;  // Period = 1 second
    
    ODEFunction harmonic = [omega](const State& y, Time t) -> State {
        return {y[1], -omega * omega * y[0]};
    };
    
    State initial = {1.0, 0.0};
    
    SolverConfig config{
        .time_start = 0.0,
        .time_end = 1.0,  // Exactly one period
        .step_size = 0.01,
        .initial_state = initial
    };
    
    RK4Solver solver;
    Solution sol = solver.solve(harmonic, config);
    
    // After one period, should return to initial state
    State final = sol.back().state;
    
    REQUIRE_THAT(final[0], Catch::Matchers::WithinAbs(initial[0], 1e-3));
    REQUIRE_THAT(final[1], Catch::Matchers::WithinAbs(initial[1], 1e-3));
}

TEST_CASE("Harmonic oscillator: Amplitude and phase", "[rk4][harmonic]") {
    double omega = 1.0;
    
    ODEFunction harmonic = [omega](const State& y, Time t) -> State {
        return {y[1], -omega * omega * y[0]};
    };
    
    // Initial: x=0, v=1 → Solution: x(t) = sin(t)
    SolverConfig config{
        .time_start = 0.0,
        .time_end = M_PI / 2.0,  // Quarter period
        .step_size = 0.01,
        .initial_state = {0.0, 1.0}
    };
    
    RK4Solver solver;
    Solution sol = solver.solve(harmonic, config);
    
    // At t = π/2, should have x ≈ 1, v ≈ 0
    State final = sol.back().state;
    
    REQUIRE_THAT(final[0], Catch::Matchers::WithinAbs(1.0, 1e-4));
    REQUIRE_THAT(final[1], Catch::Matchers::WithinAbs(0.0, 1e-4));
}
