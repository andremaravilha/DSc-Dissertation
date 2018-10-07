#ifndef MANEUVER_SCHEDULING_LOCAL_SEARCH_H
#define MANEUVER_SCHEDULING_LOCAL_SEARCH_H

#include <list>
#include <random>
#include "../algorithm/algorithm.h"
#include "../neighborhood/neighborhood.h"
#include "../problem/problem.h"


namespace orcs {

    namespace local_search {

        /**
         * Perform the standard best improving local search over a single neighborhood structure.
         *
         * @param   problem
         *          Instance of the problem being optimized.
         * @param   entry
         *          The start entry to perform the local search.
         * @param   neighborhood
         *          The neighborhood structure.
         *
         * @return  A tuple of two elements, in which the first is the
         *          schedule, the second is its evaluation (i.e., a tuple
         *          containing the makespan and the sum of the completion times).
         */
        std::tuple<orcs::Schedule, std::tuple<double, double> >
        standard(const Problem &problem,
                 const std::tuple<orcs::Schedule, std::tuple<double, double> > &entry,
                 Neighborhood &neighborhood);

        /**
         * Perform the local search according to variable neighborhood search (VND),
         * which explores multiple neighborhood structures simultaneously.
         *
         * @param   problem
         *          Instance of the problem being optimized.
         * @param   entry
         *          The start entry to perform the local search.
         * @param   neighborhoods
         *          List of neighborhood structures.
         *
         * @return  A tuple of two elements, in which the first is the
         *          schedule, the second is its evaluation (i.e., a tuple
         *          containing the makespan and the sum of the completion times).
         */
        std::tuple<orcs::Schedule, std::tuple<double, double> >
        vnd(const Problem &problem,
            const std::tuple<orcs::Schedule, std::tuple<double, double> > &entry,
            std::list<Neighborhood *> &neighborhoods);

        /**
         * Perform the local search according to randomized variable neighborhood
         * search (RVND), which explores multiple neighborhood structures simultaneously.
         *
         * @param   problem
         *          Instance of the problem being optimized.
         * @param   entry
         *          The start entry to perform the local search.
         * @param   neighborhoods
         *          List of neighborhood structures.
         * @param   generator
         *          Random number generator.
         *
         * @return  A tuple of two elements, in which the first is the
         *          schedule, the second is its evaluation (i.e., a tuple
         *          containing the makespan and the sum of the completion times).
         */
        std::tuple<orcs::Schedule, std::tuple<double, double> >
        rvnd(const Problem &problem,
             const std::tuple<orcs::Schedule, std::tuple<double, double> > &entry,
             std::list<Neighborhood *> &neighborhoods,
             std::mt19937 *generator = nullptr);

    }
}


#endif
