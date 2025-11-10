#include "ode/euler.hpp"
#include "ode/rk4.hpp"
#include "ode/rk45.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>

/**
 * Example 4: Solver Comparison and Benchmarking
 *
 * Compares the three main solvers on the same problem:
 * - Euler (1st order)
 * - RK4 (4th order fixed-step)
 * - RK45 (4th/5th order adaptive)
 *
 * Test problem: dy/dt = -y, y(0) = 1
 * Exact solution: y(t) = e^(-t)
 *
 * Metrics compared:
 * - Accuracy (error vs exact solution)
 * - Efficiency (number of steps taken)
 * - Performance (execution time)
 *
 * This demonstrates the accuracy-efficiency trade-off:
 * - Euler: Fast but inaccurate
 * - RK4: Excellent accuracy with fixed steps
 * - RK45: Automatic accuracy control, fewest steps for given tolerance
 *
 * Key insights:
 * - Higher order methods allow larger steps for same accuracy
 * - Adaptive methods find optimal step size automatically
 * - For same step size, RK4 is ~100x more accurate than Euler
 */

int main()
{
    using namespace ode;
    using namespace std::chrono;

    std::cout << "=== Solver Comparison ===\n\n";
    std::cout << "Solving dy/dt = -y with y(0) = 1\n\n";

    // Test problem: dy/dt = -y, y(0) = 1
    double lambda = 1.0;
    ODEFunction problem = [lambda](const State &y, Time t) -> State {
        return {-lambda * y[0]};
    };

    auto exact = [](Time t) { return std::exp(-t); };

    SolverConfig config{
        .time_start = 0.0,
        .time_end = 5.0,
        .step_size = 0.1,
        .initial_state = {1.0}
    };

    std::cout << std::fixed << std::setprecision(8);
    std::cout << std::setw(15) << "Solver"
              << std::setw(15) << "Final Value"
              << std::setw(15) << "Error"
              << std::setw(15) << "Steps"
              << std::setw(15) << "Time (ms)\n";
    std::cout << std::string(75, '-') << "\n";

    double y_exact = exact(config.time_end);

    // Euler
    {
        Euler solver;
        auto start = high_resolution_clock::now();
        Solution sol = solver.solve(problem, config);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count() / 1000.0;

        double error = std::abs(sol.back().state[0] - y_exact);
        std::cout << std::setw(15) << solver.name()
                  << std::setw(15) << sol.back().state[0]
                  << std::setw(15) << error
                  << std::setw(15) << sol.size()
                  << std::setw(15) << duration << "\n";
    }

    // RK4
    {
        RK4Solver solver;
        auto start = high_resolution_clock::now();
        Solution sol = solver.solve(problem, config);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count() / 1000.0;

        double error = std::abs(sol.back().state[0] - y_exact);
        std::cout << std::setw(15) << "RK4"
                  << std::setw(15) << sol.back().state[0]
                  << std::setw(15) << error
                  << std::setw(15) << sol.size()
                  << std::setw(15) << duration << "\n";
    }

    // RK45
    {
        RK45Solver solver;
        config.tolerance = 1e-6;
        auto start = high_resolution_clock::now();
        Solution sol = solver.solve(problem, config);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count() / 1000.0;

        double error = std::abs(sol.back().state[0] - y_exact);
        std::cout << std::setw(15) << "RK45"
                  << std::setw(15) << sol.back().state[0]
                  << std::setw(15) << error
                  << std::setw(15) << sol.size()
                  << std::setw(15) << duration << "\n";
    }

    std::cout << "\nExact value: " << y_exact << "\n";

    return 0;
}
