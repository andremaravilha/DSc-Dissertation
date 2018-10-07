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
This script runs the same experiments described in the PhD dissertation.
"""


import sys
import glob
import subprocess
from multiprocessing import RLock
from concurrent.futures import ThreadPoolExecutor


#
# Re-entrant lock object used to control the access to the output file.
#
lock = RLock()


#
# Initializes the CSV output file and writes the header.
#
def initialize_results_file(filename):
    with open(filename, "w") as f:
        f.write("INSTANCE," +
                "ALGORITHM," +
                "SEED," +
                "STATUS," +
                "OBJECTIVE," +
                "TIME.SEC," +
                "ITERATIONS," +
                "RELAXATION," +
                "OPT.GAP\n")


#
# Writes the results of an entry in the output file.
#
def write_results(filename, results):
    content = (results["instance"] + "," +
               results["algorithm"] + "," +
               results["seed"] + "," +
               results["status"] + "," +
               results["objective"] + "," +
               results["time"] + "," + 
               results["iterations"] + "," +
               results["relaxation"] + "," +
               results["gap"] +
               "\n")

    f = open(filename, "a")
    f.write(content)
    f.close()


#
# Runs an entry.
#
def run_entry(instance, algorithm, data, seed):

    # Keep the result
    results = dict()
    results["instance"] = instance
    results["algorithm"] = algorithm
    results["seed"] = seed
    results["status"] = "ERROR"
    results["objective"] = ""
    results["time"] = ""
    results["iterations"] = ""
    results["relaxation"] = ""
    results["gap"] = ""

    try:

        # Create the command to run
        command = ([data["command"]] +
                   data["algorithms"][algorithm]["params"] +
                   ["--seed"] + [seed] +
                   ["--file"] + [data["instances"][instance]])

        # Run the command
        output = subprocess.run(command,
                                stdout=subprocess.PIPE,
                                universal_newlines=True)

        # Check if the command run without errors
        if output.returncode == 0:

            # Get the output as a string
            str_output = str(output.stdout).strip('\n\t ').split("\n")
            str_output = str_output[0] if len(str_output) == 1 else str_output[-1]
            str_output = str_output.strip('\n\t ').split(" ")

            # Process the output
            results["status"] = str_output[0]
            results["objective"] = str_output[1] if str_output[1] != "?" else ""
            results["time"] = str_output[2] if str_output[2] != "?" else ""
            results["iterations"] = str_output[3] if str_output[3] != "?" else ""
            results["relaxation"] = str_output[4] if str_output[4] != "?" else ""
            results["gap"] = str_output[5] if str_output[5] != "?" else ""

    except:
        results["status"] = "ERROR"
        results["objective"] = ""
        results["time"] = ""
        results["iterations"] = ""
        results["relaxation"] = ""
        results["gap"] = ""

    # Update progress
    data["entries-done"] = data["entries-done"] + 1
    progress = (data["entries-done"] / data["entries-total"]) * 100

    # Write results in the output file
    log = ("[{0} of {1} ({2:.2f}%) completed] Instance: {3}, Algorithm: {4}, " +
           "Seed: {5}, Status: {6}").format(data["entries-done"],
                                            data["entries-total"],
                                            progress,
                                            instance,
                                            algorithm,
                                            str(seed),
                                            results["status"])

    with lock:
        write_results(data["output-file"], results)
        print(log)
        sys.stdout.flush()


#
# Main statements
#
if __name__ == "__main__":

    # Map used to store data necessary to run the experiments
    data = dict()
    data["command"] = "../build/schd"                        # program
    data["threads"] = 22                                     # entries to solve simultaneouly
    data["output-file"] = "results.csv"                      # file to write the results
    data["instances-path"] = "../instances/benchmark/files"  # base path to instances
    data["instances"] = dict()                               # path to each instance
    data["algorithms"] = dict()                              # settings of each algorithm
    data["seeds"] = None                                     # seeds
    data["entries-total"] = 0                                # num. of entries
    data["entries-done"] = 0                                 # num. of entries finished

    # Set the seeds used at each repetition of (instance, algorithm)
    data["seeds"] = "2 3 5 7 11 13 17 19 23 29".split(" ")

    # Load the list of instances
    instances = [f.split("/")[-1].split(".")[0] for f in glob.glob(data["instances-path"] + "/*.txt")]
    for instance in instances:
        data["instances"][instance] = data["instances-path"]+ "/" + instance + ".txt"

    # Algorithms' settings
    common_parameters = "--details 2 --time-limit 3600 --threads 1".split(" ")

    data["algorithms"]["mip-precedence"] = {
        "params": common_parameters + "--algorithm mip-precedence".split(" "),
        "run-once": True
    }

    data["algorithms"]["mip-linear-ordering"] = {
        "params": common_parameters + "--algorithm mip-linear-ordering".split(" "),
        "run-once": True
    }

    data["algorithms"]["mip-arc-time-indexed"] = {
        "params": common_parameters + "--algorithm mip-arc-time-indexed".split(" "),
        "run-once": True
    }

    data["algorithms"]["greedy"] = {
        "params": common_parameters + "--algorithm greedy".split(" "),
        "run-once": True
    }

    data["algorithms"]["neh"] = {
        "params": common_parameters + "--algorithm neh".split(" "),
        "run-once": True
    }

    data["algorithms"]["ils"] = {
        "params": common_parameters + "--algorithm ils --perturbation-passes-limit 5".split(" "),
        "run-once": False
    }

    # Compute the total number of entries to solve
    for alg_name, alg_data in data["algorithms"].items():
        if alg_data["run-once"]:
            data["entries-total"] = data["entries-total"] + len(instances)
        else:
            data["entries-total"] = data["entries-total"] + len(instances) * len(data["seeds"])
    
    # Create and initialize the output file
    initialize_results_file(data["output-file"])

    # Run each entry (instance, algorithm, seed)
    with ThreadPoolExecutor(max_workers=data["threads"]) as executor:

        for alg_name, alg_data in data["algorithms"].items():

            if alg_data["run-once"]:
                for instance in instances:
                    executor.submit(run_entry, instance, alg_name, data, "2")
            else:
                for seed in data["seeds"]:
                    for instance in instances:
                        executor.submit(run_entry, instance, alg_name, data, seed)

