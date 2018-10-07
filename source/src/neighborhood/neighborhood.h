#ifndef MANEUVER_SCHEDULING_NEIGHBORHOOD_H
#define MANEUVER_SCHEDULING_NEIGHBORHOOD_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <vector>

#include "../problem/problem.h"
#include "../util/common.h"


namespace orcs {

    /**
     * Interface implemented by all classes that defines a neighborhood.
     */
    class Neighborhood {

    public:

        /**
         * Return the best neighbor of the given entry.
         *
         * @param   problem
         *          Instance of the problem being optimized.
         * @param   entry
         *          The start entry (solution and its evaluation).
         *
         * @return  A tuple of two elements, in which the first is the
         *          schedule, the second is its evaluation (i.e., a tuple
         *          containing the makespan and the sum of the completion times).
         */
        virtual std::tuple< Schedule, std::tuple<double, double> >
        best(const Problem& problem, const std::tuple< Schedule, std::tuple<double, double> >& entry) = 0;

        /**
         * Return a neighbor, randomly chosen, from the start entry.
         *
         * @param   problem
         *          Instance of the problem being optimized.
         * @param   entry
         *          The start entry (solution and its evaluation).
         * @param   generator
         *          The random number generator.
         * @param   feasible_only
         *          If set to true, only feasible schedules are returned.
         *          Otherwise, infeasible schedules can be returned.
         *
         * @return  A tuple of two elements, in which the first is the
         *          schedule, the second is its evaluation (i.e., a tuple
         *          containing the makespan and the sum of the completion times).
         */
        virtual std::tuple< Schedule, std::tuple<double, double> >
        any(const Problem& problem, const std::tuple< Schedule, std::tuple<double, double> >& entry,
                std::mt19937& generator, bool feasible_only = true) = 0;

    };

}

#endif
