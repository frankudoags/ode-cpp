#include "ode/rk2.hpp"

/*
     RK2 (Midpoint Method) Theory

    Classic Runge-Kutta Method** (2nd order):
    k₁ = f(t_n, y_n)
    k₂ = f(t_n + h/2, y_n + h*k₁/2)
    y_{n+1} = y_n + h*k₂
 */

namespace ode
{
    State RK2Solver::step(const ODEFunction &f, Time t, const State &y, double h)
    {
        // instantiate the derivative at current state and time: k1 = f(t, y)
        State k1 = f(y, t);

        // To compute the midpoint state, we first calculate y_mid = y + (h/2) * k1
        State y_mid(y.size());
        for (size_t i = 0; i < y.size(); ++i)
        {
            y_mid[i] = y[i] + (0.5 * h * k1[i]);
        }
        // Compute the derivative at the midpoint: k2 = f(t + h/2, y_mid)
        State k2 = f(y_mid, t + 0.5 * h);

        // Return the next state using the average of the slopes
        State y_next(y.size());
        for (size_t i = 0; i < y.size(); ++i)
        {
            y_next[i] = y[i] + h * k2[i];
        }
        return y_next;
    }
    Solution RK2Solver::solve(const ODEFunction &f, const SolverConfig &config)
    {
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

        return solution;
    }
}