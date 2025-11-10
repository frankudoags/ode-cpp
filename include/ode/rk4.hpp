#pragma once
#include "solver.hpp"

namespace ode {

class RK4Solver : public Solver {
public:
    Solution solve(const ODEFunction &f, const SolverConfig &config) override;
    State step(const ODEFunction &f, Time t, const State &y, double h) override;
    std::string name() const override { return "Classic Runge-Kutta Method** (4th order)"; }
    int order() const override { return 4; }

private:
    // Helper: add two states
    State add_states(const State& a, const State& b, double scale_b = 1.0) const;
};

}