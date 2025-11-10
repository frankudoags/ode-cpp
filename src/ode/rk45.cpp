#include "ode/rk45.hpp"
#include <cmath>
#include <algorithm>

/**
 *    RK45 (Runge-Kutta-Fehlberg) Method Implementation
 *
 *    Reference: https://en.wikipedia.org/wiki/Runge-Kutta-Fehlberg_method
 *
 *    This is an ADAPTIVE method that automatically adjusts step size:
 *    - Computes BOTH 4th-order and 5th-order estimates
 *    - Compares them to estimate local truncation error
 *    - Accepts/rejects steps based on tolerance
 *    - Adjusts h for next step
 *
 *    Fehlberg's coefficients (1969) provide an embedded pair:
 *    - 4th order formula: used for stepping
 *    - 5th order formula: used for error estimation
 * 
 */

namespace ode
{
    // Helper: Compute linear combination y + h * (c1*k1 + c2*k2 + ...)
    State combine_slopes(const State &y, double h,
                         const State &k1, double c1,
                         const State &k2, double c2,
                         const State &k3, double c3,
                         const State &k4, double c4)
    {
        State result(y.size());
        for (size_t i = 0; i < y.size(); ++i)
        {
            result[i] = y[i] + h * (c1 * k1[i] + c2 * k2[i] + c3 * k3[i] + c4 * k4[i]);
        }
        return result;
    }

    State combine_slopes5(const State &y, double h,
                          const State &k1, double c1,
                          const State &k2, double c2,
                          const State &k3, double c3,
                          const State &k4, double c4,
                          const State &k6, double c6)
    {
        State result(y.size());
        for (size_t i = 0; i < y.size(); ++i)
        {
            result[i] = y[i] + h * (c1 * k1[i] + c2 * k2[i] + c3 * k3[i] + c4 * k4[i] + c6 * k6[i]);
        }
        return result;
    }

    // Embedded RK45 Step - The Core Algorithm
    std::pair<State, State> RK45Solver::step_embedded(
        const ODEFunction &f, Time t, const State &y, double h)
    {
        // Fehlberg's RK45 coefficients (exact fractions)

        // k1 = f(t, y)
        State k1 = f(y, t);

        // k2 = f(t + h/4, y + h*k1/4)
        State y_temp = add_states(y, k1, h / 4.0);
        State k2 = f(y_temp, t + h / 4.0);

        // k3 = f(t + 3h/8, y + h*(3k1/32 + 9k2/32))
        y_temp = y;
        for (size_t i = 0; i < y.size(); ++i)
        {
            y_temp[i] += h * (3.0 / 32.0 * k1[i] + 9.0 / 32.0 * k2[i]);
        }
        State k3 = f(y_temp, t + 3.0 * h / 8.0);

        // k4 = f(t + 12h/13, y + h*(1932k1/2197 - 7200k2/2197 + 7296k3/2197))
        y_temp = y;
        for (size_t i = 0; i < y.size(); ++i)
        {
            y_temp[i] += h * (1932.0 / 2197.0 * k1[i] - 7200.0 / 2197.0 * k2[i] + 7296.0 / 2197.0 * k3[i]);
        }
        State k4 = f(y_temp, t + 12.0 * h / 13.0);

        // k5 = f(t + h, y + h*(439k1/216 - 8k2 + 3680k3/513 - 845k4/4104))
        y_temp = y;
        for (size_t i = 0; i < y.size(); ++i)
        {
            y_temp[i] += h * (439.0 / 216.0 * k1[i] - 8.0 * k2[i] + 3680.0 / 513.0 * k3[i] - 845.0 / 4104.0 * k4[i]);
        }
        State k5 = f(y_temp, t + h);

        // k6 = f(t + h/2, y + h*(-8k1/27 + 2k2 - 3544k3/2565 + 1859k4/4104 - 11k5/40))
        y_temp = y;
        for (size_t i = 0; i < y.size(); ++i)
        {
            y_temp[i] += h * (-8.0 / 27.0 * k1[i] + 2.0 * k2[i] - 3544.0 / 2565.0 * k3[i] + 1859.0 / 4104.0 * k4[i] - 11.0 / 40.0 * k5[i]);
        }
        State k6 = f(y_temp, t + h / 2.0);

        // 4th order estimate (used for stepping)
        State y4 = combine_slopes(y, h, k1, 25.0 / 216.0, k3, 1408.0 / 2565.0, k4, 2197.0 / 4104.0, k5, -1.0 / 5.0);

        // 5th order estimate (used for error estimation)
        State y5 = combine_slopes5(y, h, k1, 16.0 / 135.0, k3, 6656.0 / 12825.0, k4, 28561.0 / 56430.0, k5, -9.0 / 50.0, k6, 2.0 / 55.0);

        return {y4, y5};
    }

    // Error Estimation
    double RK45Solver::compute_error(const State &y4, const State &y5)
    {
        double max_error = 0.0;

        for (size_t i = 0; i < y4.size(); ++i)
        {
            // Compute relative error
            double diff = std::abs(y5[i] - y4[i]);
            double scale = std::abs(y5[i]) + 1e-10; // Avoid division by zero
            double relative_error = diff / scale;

            max_error = std::max(max_error, relative_error);
        }

        return max_error;
    }

    // Adaptive Step Size Control
    double RK45Solver::adjust_step_size(double h, double error, double tolerance)
    {
        if (error == 0.0)
        {
            // Perfect accuracy - increase step size
            return h * 2.0;
        }

        // Safety factor to be conservative
        const double safety = 0.9;

        // Optimal step size formula for 4th order method
        // h_new = h * (tolerance / error)^(1/4)
        double factor = safety * std::pow(tolerance / error, 0.25);

        // Limit how much we change step size (prevent oscillation)
        factor = std::max(0.1, std::min(factor, 5.0));

        return h * factor;
    }

    // Single Step (for interface compatibility)
    State RK45Solver::step(const ODEFunction &f, Time t, const State &y, double h)
    {
        // For single non-adaptive step, use 4th order estimate
        auto [y4, y5] = step_embedded(f, t, y, h);
        return y4;
    }

    // Adaptive Solver - Main Algorithm
    Solution RK45Solver::solve(const ODEFunction &f, const SolverConfig &config)
    {
        Solution solution;
        reserve(solution, config);

        // Initialize
        Time t = config.time_start;
        State y = config.initial_state;
        double h = config.step_size; // Initial step size guess

        // Store initial condition
        solution.push_back({y, t});

        // Adaptive stepping loop
        while (t < config.time_end)
        {
            // Ensure we don't overshoot the end
            if (t + h > config.time_end)
            {
                h = config.time_end - t;
            }

            // Enforce step size bounds
            h = std::max(config.min_step, std::min(h, config.max_step));

            // Compute embedded step
            auto [y4, y5] = step_embedded(f, t, y, h);

            // Estimate error
            double error = compute_error(y4, y5);

            if (error <= config.tolerance || h <= config.min_step)
            {
                // ✓ Accept step
                y = y4; // Use 4th order estimate
                t += h;
                solution.push_back({y, t});

                // Adjust step size for next iteration
                if (error > 0.0 && h > config.min_step)
                {
                    h = adjust_step_size(h, error, config.tolerance);
                }
            }
            else
            {
                // ✗ Reject step - error too large
                // Reduce step size and retry without updating t or y
                h = adjust_step_size(h, error, config.tolerance);
            }
        }

        return solution;
    }

}