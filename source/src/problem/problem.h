#ifndef MANEUVER_SCHEDULING_PROBLEM_H
#define MANEUVER_SCHEDULING_PROBLEM_H

#include <cstddef>
#include <tuple>
#include <vector>
#include <set>
#include <string>
#include <ostream>


namespace orcs {


    /**
     * Types of technologies used by switches.
     */
    enum class Technology {
        UNKNOWN,
        MANUAL,
        REMOTE
    };

    /**
     * A schedule (or a solution for the problem). It is encoded as a vector of
     * m+1 sequences of maneuvers (with each maneuver represented by its
     * respective switch ID). The (l,i)-th position is the i-th maneuver
     * performed by the l-th team. The sequence 0-th sequence (l = 0) contains
     * the sequence of remotely controlled switches.
     */
    using Schedule = std::vector< std::vector<int> >;

    /**
     * Create an empty schedule for m maintenance teams. The maintenance teams
     * are indexed from 1 to m. The index zero is used for scheduling the
     * remotely controlled switches.
     *
     * @param   m
     *          The number of maintenance teams available
     * @return  An empty schedule configured for m teams.
     */
    inline Schedule create_empty_schedule(int m) {
        return std::vector< std::vector<int> >(m+1, std::vector<int>());
    }

    /**
     * This class keeps the data of the maneuver scheduling problem in the
     * restoration of electric power distribution networks. The data of the
     * problem is read from a file.
     */
    class Problem {

    public:

        /**
         * Number of maneuvers. In scheduling problems it is equivalent to the
         * number of tasks.
         */
        int n;

        /**
         * Number of teams available. In scheduling problems it is equivalent to
         * the number of available machines.
         */
        int m;

        /**
         * Technology used on each switch.
         */
        std::vector<Technology> technology;

        /**
         * Time required by each switch to be maneuvered, in which p[i] is the
         * time required by switch i. In scheduling problems it is equivalent to
         * the processing time of tasks.
         */
        std::vector<double> p;

        /**
         * Displacement time between locations, in which s[i][j] is the time
         * taken to displace from i to j. In scheduling problems it is equivalent
         * to the setup time (setup dependent on the sequence and machine).
         */
        std::vector< std::vector< std::vector<double> > > s;

        /**
         * Set of predecessors of each switch maneuver, in which predecessors[j]
         * is the set of switches that must be maneuvered before switch j.
         */
        std::vector< std::set<int> > predecessors;

        /**
         * Set of successors of each switch maneuver, in which successors[i] is
         * the set of switches that cannot be maneuvered before switch i is
         * maneuvered.
         */
        std::vector< std::set<int> > successors;

        /**
         * Precedence matrix, in which precedence[i][j] is true if i must
         * precede j, false otherwise.
         */
        std::vector< std::vector<bool> > precedence;

        /**
         * Constructor.
         *
         * @param   filename
         *          Path to the file containing the data.
         */
        Problem(const std::string& filename);

        /**
         * Computes the makespan of a schedule (i.e., the moment in which the
         * last task/maneuver is completed).
         *
         * @param   schedule
         *          The schedule to evaluate.
         * @return  The makespan.
         */
        double makespan(const Schedule &schedule) const;

        /**
         * Compute the start times (i.e., the moments in which each
         * task/maneuver is started). The start times are returned in a
         * vector, in which the i-th value is the start time of the i-th
         * task/maneuvered.
         *
         * @param   schedule
         *          A schedule.
         * @return  A vector with start times.
         */
        std::vector<double> start_time(const Schedule &schedule) const;

        /**
         * Check if a schedule is feasible (i.e., satisfies all constraints of
         * the problem).
         *
         * @param   schedule
         *          A schedule.
         * @param   msg
         *          If the schedule is infeasible, this parameter will contain a
         *          description of the infeasibility. Otherwise, it will contain
         *          the content "Feasible solution.".
         * @return  True if the schedule is feasible, false otherwise.
         */
        bool is_feasible(const Schedule& schedule, std::string *msg = nullptr) const;

    };
}

/**
 * Overload the flow operator for printing a schedule to an output stream.
 *
 * @param   os
 *          The output stream.
 * @param   schedule
 *          The schedule to print.
 * @return  A reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, const orcs::Schedule& schedule);


#endif
