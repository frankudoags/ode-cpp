#pragma once
#include "solver.hpp"

namespace ode
{

    class RK45Solver : public Solver
    {
    public:
        Solution solve(const ODEFunction &f, const SolverConfig &config) override;
        State step(const ODEFunction &f, Time t, const State &y, double h) override;
        std::string name() const override { return "RK45"; }
        int order() const override { return 4; }

    private:
        // Embedded RK method: computes both 4th and 5th order estimates
        std::pair<State, State> step_embedded(const ODEFunction &f, Time t, const State &y, double h);
        double compute_error(const State &y4, const State &y5);
        double adjust_step_size(double h, double error, double tolerance);
    };

}