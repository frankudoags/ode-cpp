#pragma once
#include "solver.hpp"

namespace ode {

class RK2Solver : public Solver {
public:
    Solution solve(const ODEFunction &f, const SolverConfig &config) override;
    State step(const ODEFunction &f, Time t, const State &y, double h) override;
    std::string name() const override { return "Classic Runge-Kutta Method** (2nd order)"; }
    int order() const override { return 2; }
};

}