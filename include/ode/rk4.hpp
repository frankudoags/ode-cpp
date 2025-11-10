#pragma once
#include "solver.hpp"

namespace ode {

class RK4Solver : public Solver {
public:
    Solution solve(const ODEFunction &f, const SolverConfig &config) override;
    State step(const ODEFunction &f, Time t, const State &y, double h) override;
    std::string name() const override { return "RK4 Classic Runge-Kutta Method (4th order)"; }
    int order() const override { return 4; }
};

}