#include "ode/euler.hpp"
#include "ode/rk4.hpp"
#include "ode/rk45.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

/**
 * Example 1: Exponential Decay
 *
 * Problem: dy/dt = -λy, y(0) = y₀
 * Exact solution: y(t) = y₀ * e^(-λt)
 *
 * This is the simplest ODE - perfect for testing basic functionality.
 */

int main()
{
    using namespace ode;

    std::cout << "=== Exponential Decay: dy/dt = -λy ===\n\n";

    // Parameters
    double lambda = 0.5;
    double y0 = 10.0;

    // Define ODE: dy/dt = -λy
    ODEFunction decay = [lambda](const State &y, Time t) -> State {
        return {-lambda * y[0]};
    };

    // Exact solution for comparison
    auto exact = [lambda, y0](Time t) {
        return y0 * std::exp(-lambda * t);
    };

    // Configuration
    SolverConfig config{
        .time_start = 0.0,
        .time_end = 5.0,
        .step_size = 0.1,
        .initial_state = {y0}
    };

    // Test all three solvers
    std::cout << std::fixed << std::setprecision(6);

    // Euler
    std::cout << "Euler Method:\n";
    Euler euler_solver;
    Solution euler_sol = euler_solver.solve(decay, config);
    std::cout << "Final value: " << euler_sol.back().state[0]
              << " (exact: " << exact(config.time_end) << ")\n";
    std::cout << "Error: " << std::abs(euler_sol.back().state[0] - exact(config.time_end)) << "\n\n";

    // RK4
    std::cout << "RK4 Method:\n";
    RK4Solver rk4_solver;
    Solution rk4_sol = rk4_solver.solve(decay, config);
    std::cout << "Final value: " << rk4_sol.back().state[0]
              << " (exact: " << exact(config.time_end) << ")\n";
    std::cout << "Error: " << std::abs(rk4_sol.back().state[0] - exact(config.time_end)) << "\n\n";

    // RK45
    std::cout << "RK45 Adaptive Method:\n";
    RK45Solver rk45_solver;
    config.tolerance = 1e-6;
    Solution rk45_sol = rk45_solver.solve(decay, config);
    std::cout << "Final value: " << rk45_sol.back().state[0]
              << " (exact: " << exact(config.time_end) << ")\n";
    std::cout << "Error: " << std::abs(rk45_sol.back().state[0] - exact(config.time_end)) << "\n";
    std::cout << "Steps taken: " << rk45_sol.size() << " (vs " << euler_sol.size() << " fixed)\n\n";

    // Print some trajectory points
    std::cout << "Sample trajectory (RK4):\n";
    std::cout << "Time\t\tComputed\tExact\t\tError\n";
    std::cout << "----\t\t--------\t-----\t\t-----\n";
    for (size_t i = 0; i < rk4_sol.size(); i += 10)
    {
        Time t = rk4_sol[i].time;
        double y_computed = rk4_sol[i].state[0];
        double y_exact = exact(t);
        std::cout << t << "\t" << y_computed << "\t" << y_exact
                  << "\t" << std::abs(y_computed - y_exact) << "\n";
    }

    return 0;
}
