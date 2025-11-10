#pragma once
#include <vector>
#include <functional>

namespace ode
{
    // For single ODE systems, state is just one number,
    // but for systems of ODEs, State can be multiple numbers
    using State = std::vector<double>;

    // Time is 1-dimensional - it only moves forward in one direction.
    using Time = double;

    // The type for the ODE function, a function that defines the current state and time,
    using ODEFunction = std::function<State(const State &, Time)>;

    // The type for a single solution step
    struct Step
    {
        State state; // The state after the step
        Time time;   // The time after the step
    };

    // The type for a full solution, a series of steps
    using Solution = std::vector<Step>;

    // The config for the ODE solver
    struct SolverConfig
    {
        Time time_start;     // The starting time of the simulation
        Time time_end;       // The ending time of the simulation
        double step_size;    // The size of each time step
        State initial_state; // The initial state of the system

        // For adaptive methods
        double tolerance = 1e-6;
        double min_step = 1e-10;
        double max_step = 0.1;
    };
}