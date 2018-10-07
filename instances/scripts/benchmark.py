# MIT License
# 
# Copyright (c) 2018 André L. Maravilha
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""
This script generates the instances used in the experiments described in the PhD dissertation.
"""


import os
import argparse
import scheduling_generator as schd


class Group:
    """
    This class defines the settings of a group of instances.

    :param name: Name of this group of instances.
    :param prefix: Prefix used to name the instance files.
    :param suffix: Suffix used to name the instance files.
    :param seeds: List containing the seeds used to create the instances.
    :param p_limits: A list as [LB, UB] that limits the maneuver times, in which LB and UB are, 
           respectively, the lower and upper bounds for maneuver times.
    :param s_limits: A list as [LB, UB] that limits the travel times, in which LB and UB are, 
           respectively, the lower and upper bounds for travel times.
    :param tx_remote: Proportion of remotely maneuverable switches.
    :param data: Other data.
    """
    
    def __init__(self, name, prefix, suffix, seeds, p_limits, s_limits, tx_remote, data):
        self.name = name
        self.prefix = prefix
        self.suffix = suffix
        self.seeds = seeds
        self.p_limits = p_limits
        self.s_limits = s_limits
        self.tx_remote = tx_remote
        self.data = data


def create_instances(group, output_dir):

    # Log
    print("\nCreating instances from group {}...".format(group.name), flush=True)

    # Create the output directories if they do not exist
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    if not os.path.exists(os.path.join(output_dir, "files")):
        os.makedirs(os.path.join(output_dir, "files"))

    if not os.path.exists(os.path.join(output_dir, "figures")):
        os.makedirs(os.path.join(output_dir, "figures"))

    # Calculate the number of instance into the group
    total_instances = len(group.seeds) * sum(len(data['n']) * len(data['m']) * sum(len(value[1]) for value in data['precedence']) for data in group.data)
    count_instances = 0

    # Create instance files
    for data in group.data:
        for n in data['n']:
            for m in data['m']:
                for precedence_tag, precedence_params in data['precedence']:
                    for precedence_param in precedence_params:
                        for seed_idx in range(0, len(group.seeds)):

                            # Seed
                            seed = group.seeds[seed_idx]

                            # Precedence type
                            precedence_type = ""
                            if precedence_tag == "R":
                                precedence_type = "random"
                            elif precedence_tag == "M":
                                precedence_type = "mixed"
                            elif precedence_tag == "I":
                                precedence_type = "independent"
                            elif precedence_tag == "T":
                                precedence_type = "intree"
                            elif precedence_tag == "S":
                                precedence_type = "sequential"

                            # Create the instance
                            if precedence_tag == "R":
                                instance = schd.create_instance(n, m, group.p_limits, group.s_limits, group.tx_remote, 
                                        precedence_type, precedence_param/100.0, None, True, True, seed)
                            else:
                                instance = schd.create_instance(n, m, group.p_limits, group.s_limits, group.tx_remote, 
                                        precedence_type, None, precedence_param, True, True, seed)

                            # Name of instance file
                            prefix_tag = "{}-".format(group.prefix) if len(group.prefix) > 0 else ""
                            suffix_tag = "-{}".format(group.suffix) if len(group.suffix) > 0 else ""
                            instance_tag = "{:03d}-{:02d}-{}-{:02d}-{:02d}".format(n, m, precedence_tag, precedence_param, seed_idx + 1)
                            filename = prefix_tag + instance_tag + suffix_tag

                            # Write instance file
                            path = os.path.join(output_dir, "files", filename + ".txt")
                            schd.write_instance(path, instance)

                            # Save precedence graph to file
                            path = os.path.join(output_dir, "figures", filename)
                            #schd.plot_precedence_graph(instance, path, ["png", "eps", "pdf"])
                            schd.plot_precedence_graph(instance, path, ["png"])

                            # Log
                            count_instances += 1
                            progress = (count_instances / total_instances) * 100
                            log_end = "" if count_instances < total_instances else "\n"
                            print("\rProgress: {} of {} ({:.2f}%)".format(count_instances, total_instances, progress), end=log_end, flush=True)


def main():

    # Argument parser
    parser = argparse.ArgumentParser(description="Benchmark instances")
    parser.add_argument("path", help="Path to directory where the instances will be saved.", type=str)
    args = parser.parse_args()

    # Common settings
    p_limits = [1, 4]    # Minimum and maximum values for maneuver time
    s_limits = [7, 14]   # Minimum and maximum values for travel time
    tx_remote = 0.10     # Proportion of remotely maneuverable switches

    # Problem dimension
    data = [

        # Small instances
        {
            'n': [6, 8, 10, 12],
            'm': [2, 3, 4],
            'precedence': [ ('S', [2, 3]), ('T', [2, 3]), ('I', [2, 3]), ('R', [10, 20]) ]
        },

        # Medium and large instances
        {
            'n': [50, 75, 100, 125],
            'm': [10, 15, 20],
            'precedence': [ ('S', [10, 20]), ('T', [10, 20]), ('I', [10, 20]), ('R', [10, 20]) ]
        }
    ]

    # Settings for tuning and Validation group of instances
    benchmark_group = Group("BENCHMARK", "ORCS", "", [1, 3, 6], p_limits, s_limits, tx_remote, data)

    # Create the output directories if they does not exist
    if not os.path.exists(args.path):
        os.makedirs(args.path)

    # Create instance files for each group
    create_instances(benchmark_group, args.path)


if __name__ == "__main__":
    main()

