#include "ode/euler.hpp"
#include <cmath>

/*
     Euler Method Theory

    Classic Euler Method:

    y_{n+1} = y_n + h * f(t_n, y_n)
 */

namespace ode
{

    State Euler::step(const ODEFunction &f, Time t, const State &y, double h)
    {
        // instantiate the derivative at current state and time
        State k1 = f(y, t);
        // prepare the next state vector, same size as current state
        State y_next(y.size());
        // Compute the next state using Euler's method, y_next = y + h * f(t, y)
        for (size_t i = 0; i < y.size(); ++i)
        {
            y_next[i] = y[i] + h * k1[i];
        }

        return y_next;
    }

    Solution Euler::solve(const ODEFunction &f, const SolverConfig &config)
    {
        // Prepare the solution vector, starting time, and initial state from config
        Solution solution;
        reserve(solution, config);
        
        Time t = config.time_start;
        State y = config.initial_state;

        // Store the initial state
        solution.push_back({y, t});

        // Main integration loop
        // we compute steps until we reach the end time
        // we keep advancing time by step size h, ensuring we don't exceed time_end by adjusting h if necessary with a min function
        // if the last step would overshoot time_end, we adjust h to exactly reach time_end
        while (t < config.time_end)
        {
            double h = std::min(config.step_size, config.time_end - t);
            y = step(f, t, y, h);
            t += h;
            solution.push_back({y, t});
        }

        // Return the solution
        return solution;
    }



}