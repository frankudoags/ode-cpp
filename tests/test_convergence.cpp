#include <catch2/catch_test_macros.hpp>
#include "ode/euler.hpp"
#include "ode/rk4.hpp"
#include <cmath>
#include <vector>

using namespace ode;

// Verify that methods have correct convergence order
// Error should decrease as h^p where p is the order

TEST_CASE("Euler: Convergence order", "[euler][convergence]") {
    double lambda = 1.0;
    double y0 = 1.0;
    double t_end = 1.0;
    
    ODEFunction f = [lambda](const State& y, Time t) -> State {
        return {-lambda * y[0]};
    };
    
    double y_exact = y0 * std::exp(-lambda * t_end);
    
    // Test with different step sizes
    std::vector<double> step_sizes = {0.1, 0.05, 0.025};
    std::vector<double> errors;
    
    Euler solver;
    
    for (double h : step_sizes) {
        SolverConfig config{
            .time_start = 0.0,
            .time_end = t_end,
            .step_size = h,
            .initial_state = {y0}
        };
        
        Solution sol = solver.solve(f, config);
        double error = std::abs(sol.back().state[0] - y_exact);
        errors.push_back(error);
    }
    
    // For 1st order method: error ~ h
    // Halving step size should halve error
    for (size_t i = 0; i < errors.size() - 1; ++i) {
        double ratio = errors[i] / errors[i + 1];
        // Ratio should be ~2 for 1st order
        REQUIRE(ratio > 1.8);
        REQUIRE(ratio < 2.2);
    }
}

TEST_CASE("RK4: Convergence order", "[rk4][convergence]") {
    double lambda = 1.0;
    double y0 = 1.0;
    double t_end = 1.0;
    
    ODEFunction f = [lambda](const State& y, Time t) -> State {
        return {-lambda * y[0]};
    };
    
    double y_exact = y0 * std::exp(-lambda * t_end);
    
    std::vector<double> step_sizes = {0.1, 0.05, 0.025};
    std::vector<double> errors;
    
    RK4Solver solver;
    
    for (double h : step_sizes) {
        SolverConfig config{
            .time_start = 0.0,
            .time_end = t_end,
            .step_size = h,
            .initial_state = {y0}
        };
        
        Solution sol = solver.solve(f, config);
        double error = std::abs(sol.back().state[0] - y_exact);
        errors.push_back(error);
    }
    
    // For 4th order method: error ~ h⁴
    // Halving step size should reduce error by factor of 16
    for (size_t i = 0; i < errors.size() - 1; ++i) {
        double ratio = errors[i] / errors[i + 1];
        // Ratio should be ~16 for 4th order
        REQUIRE(ratio > 12.0);
        REQUIRE(ratio < 20.0);
    }
}

TEST_CASE("Solvers: Smaller steps increase accuracy", "[all]") {
    double lambda = 1.0;
    double y0 = 1.0;
    
    ODEFunction f = [lambda](const State& y, Time t) -> State {
        return {-lambda * y[0]};
    };
    
    double y_exact = y0 * std::exp(-lambda);
    
    SECTION("Euler") {
        Euler solver;
        
        SolverConfig config1{0.0, 1.0, 0.1, {y0}};
        SolverConfig config2{0.0, 1.0, 0.01, {y0}};
        
        Solution sol1 = solver.solve(f, config1);
        Solution sol2 = solver.solve(f, config2);
        
        double error1 = std::abs(sol1.back().state[0] - y_exact);
        double error2 = std::abs(sol2.back().state[0] - y_exact);
        
        // Smaller step should be more accurate
        REQUIRE(error2 < error1);
    }
    
    SECTION("RK4") {
        RK4Solver solver;
        
        SolverConfig config1{0.0, 1.0, 0.1, {y0}};
        SolverConfig config2{0.0, 1.0, 0.01, {y0}};
        
        Solution sol1 = solver.solve(f, config1);
        Solution sol2 = solver.solve(f, config2);
        
        double error1 = std::abs(sol1.back().state[0] - y_exact);
        double error2 = std::abs(sol2.back().state[0] - y_exact);
        
        REQUIRE(error2 < error1);
    }
}
