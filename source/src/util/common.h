#ifndef MANEUVER_SCHEDULING_UTILS_H
#define MANEUVER_SCHEDULING_UTILS_H

#include <cstddef>
#include <random>
#include <string>
#include <tuple>
#include <list>
#include "../problem/problem.h"


namespace orcs {

    namespace common {

        /**
         * Threshold used when comparing equality between floating point values.
         */
        constexpr double THRESHOLD = 1e-5;

        /**
         * Compare two numbers. This function considers a threshold value to
         * compare the numbers. If the difference between them is less then
         * THRESHOLD, then the numbers are considered as equals.
         *
         * @param   first
         *          A number.
         * @param   second
         *          A number.
         *
         * @return  Return -1 if first is less then second, 0 if both numbers
         *          are equal, 1 if less is greater than second.
         */
        int compare(double first, double second);

        /**
         * Compare equality between two numbers. This function considers a
         * threshold value to compare the numbers. If the difference between
         * them is less then THRESHOLD, then the numbers are considered as
         * equals.
         *
         * @param   first
         *          A number.
         * @param   second
         *          A number.
         *
         * @return  True if the numbers are equal, false otherwise.
         */
        bool equal(double first, double second);

        /**
         * Check if the first number is greater than the second one. This
         * function considers a threshold value to compare the numbers. If the
         * difference between them is less then THRESHOLD, then the numbers are
         * considered as equals.
         *
         * @param   first
         *          A number.
         * @param   second
         *          A number.
         *
         * @return  True if first is greater than second, false otherwise.
         */
        bool greater(double first, double second);

        /**
         * Check if the first number is less than the second one. This function
         * considers a threshold value to compare the numbers. If the difference
         * between them is less then THRESHOLD, then the numbers are considered
         * as equals.
         *
         * @param   first
         *          A number.
         * @param   second
         *          A number.
         *
         * @return  True if first is lower than second, false otherwise.
         */
        bool less(double first, double second);

        /**
         * Check if the first number is greater or equal to the second one. This
         * function considers a threshold value to compare the numbers. If the
         * difference between them is less then THRESHOLD, then the numbers are
         * considered as equals.
         *
         * @param   first
         *          A number.
         * @param   second
         *          A number.
         *
         * @return  True if first is greater or equal to the second number,
         *          false otherwise.
         */
        bool greater_or_equal(double first, double second);

        /**
         * Check if the first number is less or equal to the second one. This
         * function considers a threshold value to compare the numbers. If the
         * difference between them is less then THRESHOLD, then the numbers are
         * considered as equals.
         *
         * @param   first
         *          A number.
         * @param   second
         *          A number.
         *
         * @return  True if first is lower or equal to the second number, false
         *          otherwise.
         */
        bool less_or_equal(double first, double second);

        /**
         * Compare tuples. This function considers a threshold value to compare
         * the values. If the difference between them is less then THRESHOLD,
         * then the values are considered as equals.
         * @param   t1
         *          First tuple.
         * @param   t2
         *          Second tuple.
         * @return  Return -1 if first is less then second, 0 if both numbers
         *          are equal, 1 if less is greater than second.
         */
        template<class... T1, class... T2>
        int compare(const std::tuple<T1...>& fist, const std::tuple<T2...>& second);

        /**
         * Compare equality between two tuples. This function considers a
         * threshold value to compare the numbers. If the difference between
         * them is less then THRESHOLD, then the numbers are considered as
         * equals.
         *
         * @param   first
         *          A tuple.
         * @param   second
         *          A tuple.
         *
         * @return  True if the tuples are equal, false otherwise.
         */
        template<class... T1, class... T2>
        bool equal(const std::tuple<T1...>& first, const std::tuple<T2...>& second);

        /**
         * Check if the first tuple is greater than the second one. This
         * function considers a threshold value to compare the numbers. If the
         * difference between them is less then THRESHOLD, then the numbers are
         * considered as equals.
         *
         * @param   first
         *          A tuple.
         * @param   second
         *          A tuple.
         *
         * @return  True if first is greater than second, false otherwise.
         */
        template<class... T1, class... T2>
        bool greater(const std::tuple<T1...>& first, const std::tuple<T2...>& second);

        /**
         * Check if the first tuple is less than the second one. This function
         * considers a threshold value to compare the numbers. If the difference
         * between them is less then THRESHOLD, then the numbers are considered
         * as equals.
         *
         * @param   first
         *          A tuple.
         * @param   second
         *          A tuple.
         *
         * @return  True if first is less than second, false otherwise.
         */
        template<class... T1, class... T2>
        bool less(const std::tuple<T1...>& first, const std::tuple<T2...>& second);

        /**
         * Check if the first tuple is greater or equal to the second one. This
         * function considers a threshold value to compare the numbers. If the
         * difference between them is less then THRESHOLD, then the numbers are
         * considered as equals.
         *
         * @param   first
         *          A tuple.
         * @param   second
         *          A tuple.
         *
         * @return  True if first tuple is greater or equal to second one, false
         *          otherwise.
         */
        template<class... T1, class... T2>
        bool greater_or_equal(const std::tuple<T1...>& first, const std::tuple<T2...>& second);

        /**
         * Check if the first tuple is greater or less to the second one. This
         * function considers a threshold value to compare the numbers. If the
         * difference between them is less then THRESHOLD, then the numbers are
         * considered as equals.
         *
         * @param   first
         *          A tuple.
         * @param   second
         *          A tuple.
         *
         * @return  True if first tuple is less or equal to second one, false
         *          otherwise.
         */
        template<class... T1, class... T2>
        bool less_or_equal(const std::tuple<T1...>& first, const std::tuple<T2...>& second);

        /**
         * Compute the makespan and the sum of completion times of the work of
         * all teams (including the dummy team).
         *
         * @param   problem
         *          An instance of the problem.
         * @param   schedule
         *          Schedule to evaluate
         * @return  A tuple with two values, in which the first is the makespan
         *          and the second is the sum of completion times of the work of
         *          all teams (including the dummy team).
         */
        std::tuple<double, double> evaluate(const Problem& problem, const Schedule& schedule);

        /**
         * Randomly chooses an element from the container accordingly to their
         * respective weights.
         *
         * @param   values
         *          Container with values.
         * @param   weights
         *          Weights for each value.
         * @param   generator
         *          Random number generator.
         * @return  A tuple in which the first value is the element chosen from
         *          the container values and the second is the index of the
         *          element in the container.
         */
        template <class TObject, template<class, class...> class TContainer1, template<class, class...> class TContainer2, class TRandom>
        std::tuple<TObject, std::size_t> choose(const TContainer1<TObject>& values, const TContainer2<double>& weights, TRandom& generator);

        /**
         * String formatter.
         *
         * @param   str
         *          String to format.
         * @param   args
         *          Arguments to replace the patterns in str.
         * @return  A formatted string.
         */
        template <class... T>
        std::string format(const std::string& str, T... args);

        /**
         * Print a solution to the output stream.
         * @param os An output stream
         * @param schedule The solution to print.
         */
        void print_solution(std::ostream& os, const Schedule& schedule);

        /**
         * Print a solution to the output stream (it includes the moment in which each switch
         * is maneuvered).
         * @param os An output stream
         * @param schedule The solution to print.
         * @param problem The instance of the problem.
         */
        void print_solution(std::ostream& os, const Schedule& schedule, const Problem& problem);

        /**
         * Auxiliary structure used to compare tuples.
         */
        template<class T1, class T2, std::size_t index, std::size_t size>
        struct tuple_comparator {
            static constexpr int compare(const T1& t1, const T2& t2) {
                int result = common::compare(std::get<index>(t1), std::get<index>(t2));
                if (result == 0) {
                    return tuple_comparator<T1, T2, index+1, size>::compare(t1, t2);
                } else {
                    return result;
                }
            }
        };

        /**
         * Auxiliary structure used to compare tuples.
         */
        template<class T1, class T2, std::size_t size>
        struct tuple_comparator<T1, T2, size, size> {
            static constexpr int compare(const T1& t1, const T2& t2) {
                return 0;
            }
        };

    }

}


/*
 * Definition of template functions.
 */

template<class... T1, class... T2>
int orcs::common::compare(const std::tuple<T1...>& first, const std::tuple<T2...>& second) {
    return tuple_comparator<std::tuple<T1...>, std::tuple<T2...>, 0, sizeof...(T1)>::compare(first, second);
}

template<class... T1, class... T2>
bool orcs::common::equal(const std::tuple<T1...>& first, const std::tuple<T2...>& second) {
    return compare(first, second) == 0;
}

template<class... T1, class... T2>
bool orcs::common::greater(const std::tuple<T1...>& first, const std::tuple<T2...>& second) {
    return compare(first, second) == 1;
}

template<class... T1, class... T2>
bool orcs::common::less(const std::tuple<T1...>& first, const std::tuple<T2...>& second) {
    return compare(first, second) == -1;
}

template<class... T1, class... T2>
bool orcs::common::greater_or_equal(const std::tuple<T1...>& first, const std::tuple<T2...>& second) {
    return compare(first, second) != -1;
}

template<class... T1, class... T2>
bool orcs::common::less_or_equal(const std::tuple<T1...>& first, const std::tuple<T2...>& second) {
    return compare(first, second) != 1;
}

template <class TObject, template<class, class...> class TContainer1, template<class, class...> class TContainer2, class TRandom>
std::tuple<TObject, std::size_t> orcs::common::choose(const TContainer1<TObject>& values, const TContainer2<double>& weights, TRandom& generator) {

    // Sum all weights
    double sum = 0.0;
    for (const auto& w : weights) {
        sum += w;
    }

    // Get a random number
    double random_value = (generator() / (double) generator.max()) * sum;

    // Choose a value
    std::size_t index = 0;
    auto iter_values = values.begin();
    auto iter_weights = weights.begin();

    double aux = 0.0;
    while (iter_values != values.end()) {
        if (aux + *iter_weights >= random_value) {
            break;
        } else {
            aux += *iter_weights;
            ++index;
            ++iter_values;
            ++iter_weights;
        }
    }

    // Return the chosen value
    return {*iter_values, index};
}

template <class... T>
std::string orcs::common::format(const std::string& str, T... args) {

    // Define the buffer size
    std::size_t buffer_size = std::max(static_cast<std::size_t>(1024),
                                       static_cast<std::size_t>(2 * str.length()));

    // Create the formatted string
    char* buffer = new char[buffer_size];
    sprintf(buffer, str.c_str(), args...);
    std::string formatted_str(buffer);
    delete[] buffer;

    // Return the formatted string as a std::string object
    return formatted_str;
}

#endif
