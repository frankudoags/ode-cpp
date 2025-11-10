#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "ode/euler.hpp"
#include "ode/rk2.hpp"
#include "ode/rk4.hpp"
#include "ode/rk45.hpp"
#include <cmath>

using namespace ode;

// Test ODE: dy/dt = -λy (exponential decay)
// Exact solution: y(t) = y₀ * exp(-λt)

ODEFunction make_decay(double lambda) {
    return [lambda](const State& y, Time t) -> State {
        return {-lambda * y[0]};
    };
}

double exact_decay(double t, double y0, double lambda) {
    return y0 * std::exp(-lambda * t);
}

TEST_CASE("Euler: Exponential decay accuracy", "[euler][analytical]") {
    Euler solver;
    
    double lambda = 1.0;
    double y0 = 1.0;
    auto f = make_decay(lambda);
    
    SolverConfig config{
        .time_start = 0.0,
        .time_end = 1.0,
        .step_size = 0.01,  // Small step for reasonable accuracy
        .initial_state = {y0}
    };
    
    Solution sol = solver.solve(f, config);
    
    double y_computed = sol.back().state[0];
    double y_exact = exact_decay(config.time_end, y0, lambda);
    double error = std::abs(y_computed - y_exact);
    
    // Euler has O(h) global error, so with h=0.01, expect ~1% error
    REQUIRE(error < 0.01);
}

TEST_CASE("RK2: Exponential decay accuracy", "[rk2][analytical]") {
    RK2Solver solver;
    
    double lambda = 1.0;
    double y0 = 1.0;
    auto f = make_decay(lambda);
    
    SolverConfig config{
        .time_start = 0.0,
        .time_end = 1.0,
        .step_size = 0.1,  // Can use larger step than Euler
        .initial_state = {y0}
    };
    
    Solution sol = solver.solve(f, config);
    
    double y_computed = sol.back().state[0];
    double y_exact = exact_decay(config.time_end, y0, lambda);
    double error = std::abs(y_computed - y_exact);
    
    // RK2 has O(h²) global error
    REQUIRE(error < 0.001);
}

TEST_CASE("RK4: Exponential decay accuracy", "[rk4][analytical]") {
    RK4Solver solver;
    
    double lambda = 1.0;
    double y0 = 1.0;
    auto f = make_decay(lambda);
    
    SolverConfig config{
        .time_start = 0.0,
        .time_end = 1.0,
        .step_size = 0.1,
        .initial_state = {y0}
    };
    
    Solution sol = solver.solve(f, config);
    
    double y_computed = sol.back().state[0];
    double y_exact = exact_decay(config.time_end, y0, lambda);
    double error = std::abs(y_computed - y_exact);
    
    // RK4 has O(h⁴) global error - very accurate!
    REQUIRE(error < 1e-6);
}

TEST_CASE("RK45: Exponential decay with adaptive stepping", "[rk45][analytical]") {
    RK45Solver solver;
    
    double lambda = 1.0;
    double y0 = 1.0;
    auto f = make_decay(lambda);
    
    SolverConfig config{
        .time_start = 0.0,
        .time_end = 1.0,
        .step_size = 0.1,
        .initial_state = {y0},
        .tolerance = 1e-8
    };
    
    Solution sol = solver.solve(f, config);
    
    double y_computed = sol.back().state[0];
    double y_exact = exact_decay(config.time_end, y0, lambda);
    double error = std::abs(y_computed - y_exact);
    
    // RK45 should meet tolerance
    REQUIRE(error < 1e-6);
}

TEST_CASE("Solvers: Initial condition preserved", "[all]") {
    double y0 = 5.0;
    State initial = {y0};
    
    auto f = make_decay(1.0);
    
    SolverConfig config{
        .time_start = 0.0,
        .time_end = 1.0,
        .step_size = 0.1,
        .initial_state = initial
    };
    
    SECTION("Euler") {
        Euler solver;
        Solution sol = solver.solve(f, config);
        REQUIRE(sol.front().state[0] == y0);
    }
    
    SECTION("RK4") {
        RK4Solver solver;
        Solution sol = solver.solve(f, config);
        REQUIRE(sol.front().state[0] == y0);
    }

    SECTION("RK45") {
        RK45Solver solver;
        Solution sol = solver.solve(f, config);
        REQUIRE(sol.front().state[0] == y0);
    }
}
