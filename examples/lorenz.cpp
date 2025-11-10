#include "ode/rk4.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

int main()
{
    using namespace ode;

    std::cout << "=== Lorenz Attractor (Chaotic System) ===\n\n";

    // Lorenz parameters (classic chaotic regime)
    double sigma = 10.0;
    double rho = 28.0;
    double beta = 8.0 / 3.0;

    // Lorenz equations:
    // dx/dt = σ(y - x)
    // dy/dt = x(ρ - z) - y
    // dz/dt = xy - βz
    ODEFunction lorenz = [sigma, rho, beta](const State &y, Time t) -> State {
        double x = y[0];
        double y_val = y[1];
        double z = y[2];

        return {
            sigma * (y_val - x),
            x * (rho - z) - y_val,
            x * y_val - beta * z
        };
    };

    SolverConfig config{
        .time_start = 0.0,
        .time_end = 50.0,
        .step_size = 0.01,
        .initial_state = {1.0, 1.0, 1.0}
    };

    std::cout << "Solving Lorenz system...\n";
    RK4Solver solver;
    Solution sol = solver.solve(lorenz, config);

    std::cout << "Steps: " << sol.size() << "\n";
    std::cout << "Final state: ("
              << sol.back().state[0] << ", "
              << sol.back().state[1] << ", "
              << sol.back().state[2] << ")\n\n";

    // Save to file
    std::ofstream file("lorenz.dat");
    for (const auto &point : sol)
    {
        file << point.time << " "
             << point.state[0] << " "
             << point.state[1] << " "
             << point.state[2] << "\n";
    }
    file.close();

    std::cout << "Data saved to lorenz.dat\n";
    std::cout << "Plot 3D with: gnuplot -e \"splot 'lorenz.dat' u 2:3:4 w l\"\n";

    return 0;
}
