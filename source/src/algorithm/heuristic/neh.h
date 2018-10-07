#ifndef MANEUVER_SCHEDULING_NEH_H
#define MANEUVER_SCHEDULING_NEH_H

#include "../algorithm.h"


namespace orcs {

    /**
     * A greedy heuristic for the maneuver scheduling problem in the restoration
     * of electric power distribution networks. This greedy heuristic is based
     * on the insertion criterion of Nawaz, Enscore and Ham's heuristic (NEH)
     * for the flow-show problem.
     */
    class NEH : public Algorithm {

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
