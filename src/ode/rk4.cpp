#include "ode/rk4.hpp"
/*
     RK4 (4th Order) Theory

    Classic Runge-Kutta Method** (4th order):

    k₁ = f(t_n, y_n)
    k₂ = f(t_n + h/2, y_n + h*k₁/2)
    k₃ = f(t_n + h/2, y_n + h*k₂/2)
    k₄ = f(t_n + h, y_n + h*k₃)

    y_{n+1} = y_n + (h/6)(k₁ + 2k₂ + 2k₃ + k₄)
 */
namespace ode
{
    State RK4Solver::step(const ODEFunction &f, Time t, const State &y, double h)
    {
        // instantiate the derivative at current state and time: k1 = f(t, y)
        State k1 = f(y, t);

        // compute k2 = f(t + h/2, y + h*k1/2)
        State y_k2 = add_states(y, k1, h / 2.0);
        State k2 = f(y_k2, t + h / 2.0);

        // compute k3 = f(t + h/2, y + h*k2/2)
        State y_k3 = add_states(y, k2, h / 2.0);
        State k3 = f(y_k3, t + h / 2.0);

        // compute k4 = f(t + h, y + h*k3)
        State y_k4 = add_states(y, k3, h);
        State k4 = f(y_k4, t + h);

        // combine to get the next state: y_next = y + (h/6)(k1 + 2*k2 + 2*k3 + k4)
        State y_next(y.size());
        for (size_t i = 0; i < y.size(); ++i)
        {
            y_next[i] = y[i] + (h / 6.0) * (k1[i] + 2 * k2[i] + 2 * k3[i] + k4[i]);
        }
        return y_next;
    }

    Solution RK4Solver::solve(const ODEFunction &f, const SolverConfig &config)
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