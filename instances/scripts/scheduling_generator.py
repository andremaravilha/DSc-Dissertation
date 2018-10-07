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
Python script to create instances for the maneuver scheduling problem for the 
restoration of electric power distribution networks.
"""

import argparse
import copy
import random
import math
import os
from igraph import *


class Instance:
    """
    Instance of maneuver scheduling problem for the restoration of electric power distribution 
    networks. The switches are numbered from 1 to n (value 0 is used to indicate the initial 
    location of the teams), and the teams are numbered from 0 to m - 1.

    :param n: Number of switches.
    :param m: Number of teams available.
    :param p: Processing times, in which p[i] is the time required to maneuver switch i.
    :param s: Setup times, in which s[k][i][j] is the time required by team k to move to switch j
           location from switch i location. The values s[k][0][j] is the time required by team k to 
           move from its initial location to switch j location.
    :param technology: Technology of swiches, in which technology[i] is the technology used on 
           switch i. The values are 'M' for manually maneuverable switch, 'R' for remotely 
           maneuverable switches, and '' for inapplicable (e.g., technology[0]).
    :param precedence: List of precedence constraints, in which each precedence is represented by a 
           tuple of two elements (i,j) that means switch j cannot be maneuvered before switch i is 
           maneuvered.
    :param density: number of precedence relations (including the transitive ones) divided by the 
           theoretical maximum number of precedence relations n(n-1)/2, which is sometimes referred 
           to as order strength (OS).
    """

    def __init__(self, n, m, p, s, technology, precedence, density):
        self.n = n
        self.m = m
        self.p = p
        self.s = s
        self.technology = technology
        self.precedence = precedence
        self.density = density


def create_instance(n, m, p_limits, s_limits, tx_remote, type_precedence, desired_density=None, n_stages=None, triangular=True, integer_only=True, seed=None):
    """
    Create a random instance of maneuver scheduling problem for the restoration of electric power 
    distribution networks.

    :param n: Number or maneuvers.
    :param m: Number of teams available.
    :param p_limits: A list as [LB, UB] that limits the maneuver times, in which LB and UB are, 
           respectively, the lower and upper bounds for maneuver times.
    :param s_limits: A list as [LB, UB] that limits the travel times, in which LB and UB are, 
           respectively, the lower and upper bounds for travel times.
    :param tx_remote: Proportion of remotely maneuverable switches.
    :param type_precedence: Type of precedence graph (independent, intree, sequential, mixed, random).
    :param desired_density: The desired density (i.e., order strength, OS) for the graph of 
           precedence constraints. It is used for random precedence graphs only.
    :param n_stages: Number of stages (each stage is defined by a sequence of zero or more 
           opening operations followed by a closing one. It is used for generating precedence 
           graphs (except for the random type).
    :param triangular: If set as True, it makes sure the triangular inequality is met by the travel 
           times (i.e. the values[k][i][j], for a fixed k). If set as False, the triangular 
           inequality is not assured.
    :param integer_only: Whether the maneuver time and travel time should be integer values only.
    :param seed: Seed used to initialize the random number generator.
    :return: An instance of the maneuver scheduling problem for the restoration of electric power 
             distribution networks.
    """

    # Initialize the seed of the random number generator
    rand = random.Random()
    rand.seed(seed)

    # Number of decimal digits for float numbers
    precision = 5

    # Function used to create random numbers
    def rvalue(lb, ub):
        if integer_only:
            return rand.randint(math.ceil(lb), math.floor(ub))
        else:
            return round(rand.uniform(lb, ub), precision)

    # Switches technology
    technology = ["M"] * (n + 1);
    technology[0] = ""
    for i in rand.sample(range(1, n + 1), math.ceil(n * tx_remote)):
        technology[i] = "R"

    # Maneuver times
    p = [0] * (n + 1)
    for i in range(1, n + 1):
        if technology[i] == "R":
            p[i] = rvalue(1, 1)
        else:
            p[i] = rvalue(p_limits[0], p_limits[1])

    # Travel times
    s = [[[rvalue(s_limits[0], s_limits[1]) if i != j else 0 for j in range(0, n + 1)] for i in range(0, n + 1)] for k in range(0, m)]

    # Correct values if triangular inequality is required
    if triangular:
        for k in range(0, m):
            for l in range(0, n + 1):
                for i in range(0, n + 1):
                    for j in range(0, n + 1):
                        if l == 0:
                            s[k][i][j] = min(s[k][i][j], s[k][i][l] + s[k][l][j])
                        else:
                            s[k][i][j] = min(s[k][i][j], s[k][i][l] + p[l] + s[k][l][j])

    # Precedence constraints
    if type_precedence == "random":
        precedence, density = __create_random_precedence_graph(n, desired_density, rand)
    elif type_precedence == "independent":
        precedence, density = __create_independent_precedence_graph(n, n_stages, rand)
    elif type_precedence == "intree":
        precedence, density = __create_intree_precedence_graph(n, n_stages, rand)
    elif type_precedence == "sequential":
        precedence, density = __create_sequential_precedence_graph(n, n_stages, rand)
    elif type_precedence == "mixed":
        precedence, density = __create_mixed_precedence_graph(n, n_stages, rand)

    # Create and return the instance of the problem
    return Instance(n, m, p, s, technology, precedence, density)


def __create_stages(n: int, n_stages: float, rand: random.Random):

    # Operations (open or close)
    operation = ["O"] * (n + 1)
    operation[0] = None
    operation[-1] = "C"
    for i in rand.sample(range(1, n), n_stages - 1):
        operation[i] = "C"

    # Stage
    stage = [0] * (n + 1)
    current_stage = 1
    for i in range(1, n + 1):
        stage[i] = current_stage
        if operation[i] == "C":
            current_stage = current_stage + 1

    # Split switches by operation
    to_open = [[] for i in range(0, n_stages + 1)]
    to_close = [[] for i in range(0, n_stages + 1)]
    for i in range(1, n + 1):
        if operation[i] == "C":
            to_close[stage[i]].append(i)
        elif operation[i] == "O":
            to_open[stage[i]].append(i)

    return stage, operation, to_open, to_close


def __calculate_density(n: int, precedence):

    # Arc of the precedence graph
    arcs = copy.deepcopy(precedence)

    # Create a precedence matrix
    A = [[False for j in range(0, n + 1)] for i in range(0, n + 1)]

    while len(arcs) > 0:

        # Get an arc
        i, j = arcs[0]

        # Update precedence matrix (including transitive relations)
        predecessors = [i] + [l for l in range(1, n + 1) if A[l][i]]
        successors = [j] + [k for k in range(1, n + 1) if A[j][k]]

        for l in predecessors:
            for k in successors:
                A[l][k] = True
                if arcs.count((l, k)) > 0:
                    arcs.remove((l, k))

    # Calculate density
    count = 0
    for i in range(1, n + 1):
        for j in range(1, n + 1):
            if A[i][j]:
                count += 1

    total_arcs = (n * (n - 1)) / 2
    current_density = count / total_arcs

    return current_density


def __create_independent_precedence_graph(n: int, n_stages: float, rand: random.Random):

    # Divide switche operations into stages
    stage, operation, to_open, to_close = __create_stages(n, n_stages, rand)
    
    # Build the precedence constraints
    precedence = []
    for j in range(1, n + 1):
        if operation[j] == "C":
            for i in to_open[stage[j]]:
                precedence.append((i, j))

    # Calculate the density of the precedence graph
    density = __calculate_density(n, precedence)

    return precedence, density


def __create_intree_precedence_graph(n: int, n_stages: float, rand: random.Random):

    # Divide switche operations into stages
    stage, operation, to_open, to_close = __create_stages(n, n_stages, rand)
    
    # Build the precedence constraints
    precedence = []
    for j in range(1, n + 1):
        if operation[j] == "C":
            for i in to_open[stage[j]]:
                precedence.append((i, j))
            for i in to_close[stage[j] - 1]:
                precedence.append((i,j))

    # Calculate the density of the precedence graph
    density = __calculate_density(n, precedence)

    return precedence, density


def __create_sequential_precedence_graph(n: int, n_stages: float, rand: random.Random):

    # Divide switche operations into stages
    stage, operation, to_open, to_close = __create_stages(n, n_stages, rand)
    
    # Build the precedence constraints
    precedence = []
    for j in range(1, n + 1):
        if operation[j] == "O":
            for i in to_close[stage[j] - 1]:
                precedence.append((i, j))
        elif operation[j] == "C":
            if len(to_open[stage[j]]) == 0:
                for i in to_close[stage[j] - 1]:
                    precedence.append((i, j))
            else:
                for i in to_open[stage[j]]:
                    precedence.append((i, j))

    # Calculate the density of the precedence graph
    density = __calculate_density(n, precedence)

    return precedence, density


def __create_mixed_precedence_graph(n: int, n_stages: float, rand: random.Random):

    # Divide switche operations into stages
    stage, operation, to_open, to_close = __create_stages(n, n_stages, rand)
    precedence = []
    
    # First stage
    j = to_close[1][0]
    for i in to_open[stage[j]]:
        precedence.append((i, j))

    # Next stages
    for current_stage in range(2, n_stages + 1):
        type_precedence = random.choice(["independent", "intree", "sequential"])

        if type_precedence == "independent":
            j = to_close[current_stage][0]
            for i in to_open[stage[j]]:
                precedence.append((i, j))

        elif type_precedence == "intree":
            j = to_close[current_stage][0]
            for i in to_open[stage[j]]:
                precedence.append((i, j))
            for i in to_close[stage[j] - 1]:
                precedence.append((i, j))

        elif type_precedence == "sequential":
            for j in to_open[current_stage]:
                for i in to_close[stage[j] - 1]:
                    precedence.append((i, j))
            j = to_close[current_stage][0]
            if len(to_open[stage[j]]) == 0:
                for i in to_close[stage[j] - 1]:
                    precedence.append((i, j))
            else:
                for i in to_open[stage[j]]:
                    precedence.append((i, j))

    # Calculate the density of the precedence graph
    density = __calculate_density(n, precedence)

    return precedence, density


def __create_random_precedence_graph(n: int, density: float, rand: random.Random):

    # Create a precedence matrix
    A = [[False for j in range(0, n + 1)] for i in range(0, n + 1)]
    precedence = []

    # Set of nodes
    nodes = [i for i in range(1, n + 1)]
    rand.shuffle(nodes)

    # Set of possible precedende relations
    arcs = []
    for i in range(0, len(nodes)):
        for j in range(i + 1, len(nodes)):
            arcs.append((nodes[i], nodes[j]))

    total_arcs = (n * (n - 1)) / 2
    current_density = 0.0

    while len(arcs) > 0 and current_density < density:

        # Get an arc
        i, j = rand.choice(arcs)

        # Add precedence relation
        precedence.append((i, j))

        # Update precedence matrix (including transitive relations)
        predecessors = [i] + [l for l in range(1, n + 1) if A[l][i]]
        successors = [j] + [k for k in range(1, n + 1) if A[j][k]]

        for l in predecessors:
            for k in successors:
                A[l][k] = True
                if arcs.count((l, k)) > 0:
                    arcs.remove((l, k))

        # Update the density
        count = 0
        for i in range(1, n + 1):
            for j in range(1, n + 1):
                if A[i][j]:
                    count += 1

        current_density = count / total_arcs

    return precedence, current_density


def plot_precedence_graph(instance: Instance, filename=None, extensions=[]):
    """
    Plot the precedence graph.
    :param instance: Instance to plot.
    :param filename: List with names of file in which the plot will be saved. If not set, then the 
           figure is not saved to any file.
    """

    graph = Graph(directed=True)
    graph.add_vertices(instance.n)
    graph.add_edges([(i - 1, j - 1) for i, j in instance.precedence])
    graph.degree(mode="in")

    count_predecessors = [0] * (instance.n + 1)
    for i, j in instance.precedence:
        count_predecessors[j] += 1
    
    roots = [j - 1 for j in range(1, instance.n + 1) if count_predecessors[j] == 0]

    #layout = graph.layout_auto()
    layout = graph.layout_reingold_tilford(root=roots)

    p_nodes = graph.vs.select()
    p_nodes['shape'] = 'circle'
    p_nodes['color'] = 'white'
    p_nodes['size'] = 35
    p_nodes['frame_width'] = 2

    p_edges = graph.es.select(_source_in=list(range(0, instance.n)),
                              _target_in=list(range(0, instance.n)))
    p_edges['color'] = 'black'
    p_edges['width'] = 2
    p_edges['curved'] = False
    #p_edges['curved'] = True

    labels = [i for i in range(1, instance.n + 1)]

    if filename is None or len(extensions) == 0:
        plot(graph, layout=layout, vertex_label=labels, bbox=(0, 0, 1536, 900), keep_aspect_ratio=True)
    else:
        for extension in extensions:
            plot(graph, filename + "." + extension, layout=layout, vertex_label=labels, bbox=(0, 0, 1536, 900), keep_aspect_ratio=True)
        


def write_instance(filename: str, instance: Instance):
    """
    Write the instance data into a plain text file.

    :param filename: Path and name of the file in which the instance will be written.
    :param instance: The instance that will be saved to file.
    """

    with open(filename, "w") as f:

        # Instance size
        f.write("{} {} {}\n".format(instance.n, instance.m, round(instance.density, 3)))

        # Switches' data: i, technology[i], p[i]
        for i in range(1, instance.n + 1):
            f.write("{} {} {}".format(i, instance.technology[i], instance.p[i]))
            f.write("\n")

        # Precedence constraints for each switch: i, num of predecessors, list of precedence
        for i in range(1, instance.n + 1):
            predecessors = [j for j, l in instance.precedence if l == i]
            f.write("{} {} ".format(i, len(predecessors)))
            for j in predecessors:
                f.write("{} ".format(j))
            f.write("\n")

        # Travel times for each team
        for k in range(0, instance.m):
            for i in range(0, instance.n + 1):
                for j in range(0, instance.n + 1):
                    f.write("{} ".format(instance.s[k][i][j]))
                f.write("\n")


def __create_instance(args):
    return create_instance(args.switches, args.teams, [args.p_min, args.p_max], [args.s_min, args.s_max],
                           args.tx_remote, args.precedence, args.density, args.stages, args.triangular, 
                           args.integer_only, args.seed)


def __create_cli():
    """
    Create the Command-Line Interface of the application.

    :return: The command-line interface for parsing the user input.
    """

    parser = argparse.ArgumentParser(description="Instance generator for the maneuver scheduling "
                                                 "problem for the restoration of electric power "
                                                 "distribution networks.")
    parser.add_argument("--filename",
                        help="Path and name of the file in which the instance will be saved.",
                        type=str,
                        required=True)
    parser.add_argument("--switches",
                        help="Number of switches.",
                        type=int,
                        required=True)
    parser.add_argument("--teams",
                        help="Number of teams.",
                        type=int,
                        required=True)
    parser.add_argument("--seed",
                        help="Seed used to initialize the random number generator.",
                        type=int,
                        default=0)
    parser.add_argument("--triangular",
                        help="Whether the triangular inequality must be satisfied.",
                        action="store_true")
    parser.add_argument("--integer-only",
                        help="Whether the values generated for this instances should be truncated "
                             "to integer values.",
                        action="store_true",
                        dest="integer_only")
    parser.add_argument("--precedence",
                        help="Type of precedence graph.",
                        type=str,
                        choices=["random", "independent", "intree", "sequential", "mixed"],
                        default="random",
                        dest="precedence")
    parser.add_argument("--stages",
                        help="Number of stages (except for random precedence graphs)",
                        type=int,
                        default=None,
                        dest="stages")
    parser.add_argument("--density",
                        help="Desired density (i.e., order strength) for random precedence graph.",
                        type=float,
                        default=None,
                        dest="density")
    parser.add_argument("--tx-remote",
                        help="Proportion of remote switches.",
                        type=float,
                        default=0.10,
                        dest="tx_remote")
    parser.add_argument("--p-min",
                        help="Minimum value for maneuver times.",
                        type=float,
                        default=1,
                        dest="p_min")
    parser.add_argument("--p-max",
                        help="Maximum value for maneuver times.",
                        type=float,
                        default=3,
                        dest="p_max")
    parser.add_argument("--s-min",
                        help="Minimum value for travel times.",
                        type=float,
                        default=5,
                        dest="s_min")
    parser.add_argument("--s-max",
                        help="Maximum value for travel times.",
                        type=float,
                        default=10,
                        dest="s_max")

    return parser


if __name__ == "__main__":

    # Parse user arguments
    cli = __create_cli()
    args = cli.parse_args()

    # Create instance
    instance = __create_instance(args)

    # Write file with instance data
    write_instance(args.filename + ".txt", instance)

    # Save the graph of precedence as image
    if not os.path.exists("figs"):
        os.makedirs("figs")

    plot_precedence_graph(instance, os.path.join("figs", args.filename), ["png", "eps", "pdf"])

