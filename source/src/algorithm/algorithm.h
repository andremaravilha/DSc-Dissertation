#ifndef MANEUVER_SCHEDULING_ALGORITHM_H
#define MANEUVER_SCHEDULING_ALGORITHM_H

#include <cxxproperties.hpp>
#include "../problem/problem.h"


namespace orcs {

    /**
     * Interface implemented by classes that implements some algorithm for
     * solving the maneuver scheduling problem in electric power distribution
     * networks.
     */
    class Algorithm {

    public:

        /**
         * Implements the algorithm for solving the problem.
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
        virtual std::tuple<Schedule, double> solve(const Problem& problem,
                const cxxproperties::Properties* opt_input = nullptr,
                cxxproperties::Properties* opt_output = nullptr) = 0;

    };
}

#endif
