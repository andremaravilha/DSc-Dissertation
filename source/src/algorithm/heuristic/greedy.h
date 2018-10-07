#ifndef MANEUVER_SCHEDULING_GREEDY_H
#define MANEUVER_SCHEDULING_GREEDY_H

#include "../algorithm.h"


namespace orcs {

    /**
     * A simple greedy heuristic for the maneuver scheduling problem in the
     * restoration of electric power distribution networks. This greedy heuristic
     * is based on the earliest start time (EST) rule.
     */
    class Greedy : public Algorithm {

    public:

        /**
         * Implements the strategy for solving the problem.
         *
         * @param   problem
         *          The instance of the problem to solve.
         * @param   opt_input
         *          Optional input arguments. It is useful to set parameters of
         *          the algorithm. It can be set to nullptr.
         * @param   opt_output
         *          Optional output arguments. It is useful to return additional
         *          information about the solution proccess. It can be set to
         *          nullptr.
         * @return  A tuple of two elements, in which the first is the
         *          schedule and the second is its makespan.
         */
        std::tuple<Schedule, double> solve(const Problem &problem,
                const cxxproperties::Properties *opt_input = nullptr,
                cxxproperties::Properties *opt_output = nullptr);

    };

}

#endif
