#pragma once
#include "types.hpp"
namespace ode
{
    // The interface for ODE solvers
    class Solver
    {
    public:
        virtual ~Solver() = default;

        // Solve the ODE defined by ode function with the given config
        virtual Solution solve(const ODEFunction &f, const SolverConfig &config) = 0;

        // Single step (useful for testing), h is the step size for this step
        virtual State step(const ODEFunction &f, Time t, const State &y, double h) = 0;

        // get the name of the solver
        virtual std::string name() const = 0;

        // get the order of accuracy of the solver
        virtual int order() const = 0;

        // reserve space for solution optimization
        virtual void reserve(Solution &solution, const SolverConfig &config)
        {
            // Estimate number of steps and reserve space, added +1 to include initial state
            size_t steps = static_cast<size_t>((config.time_end - config.time_start) / config.step_size) + 1;
            solution.reserve(steps);
        }

        // add two states with scaling
        // result = a + scale_b * b
        virtual State add_states(const State &a, const State &b, double scale_b = 1.0)
        {
            State result(a.size());
            for (size_t i = 0; i < a.size(); ++i)
            {
                result[i] = a[i] + scale_b * b[i];
            }
            return result;
        }
    };
}