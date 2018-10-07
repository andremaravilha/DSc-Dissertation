#ifndef MANEUVER_SCHEDULING_ILS_H
#define MANEUVER_SCHEDULING_ILS_H

#include <random>
#include "../algorithm.h"


namespace orcs {

    /**
     * This class implements an ILS-based heuristic for the maneuver scheduling
     * problem in the restoration of electric power distribution networks.
     */
    class ILS : public Algorithm {
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
         *          information about the solution process. It can be set to
         *          nullptr.
         * @return  A tuple of two elements, in which the first is the
         *          schedule and the second is its makespan.
         */
        std::tuple<Schedule, double> solve(const Problem& problem,
                const cxxproperties::Properties* opt_input = nullptr,
                cxxproperties::Properties* opt_output = nullptr);

    private:

        std::tuple<orcs::Schedule, std::tuple<double, double> > perturb(const Problem& problem, const std::tuple<Schedule, std::tuple<double, double> >& entry, std::mt19937& generator);

        void log_header(bool verbose = true);

        void log_footer(bool verbose = true);

        void log_start(const std::tuple<double, double>& start, double time,
                bool verbose = true);

        void log_iteration(long iteration, const std::tuple<double, double>& incumbent,
                const std::tuple<double, double>& before_ls,
                const std::tuple<double, double>& after_ls, double time, bool verbose = true);

    };

}


#endif
