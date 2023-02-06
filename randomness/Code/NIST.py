import random
import numpy as np
import time
import copy
import argparse
from tests import *

class TRNGtester:
    """
        Class container for the NIST SP 800-22r1a specification for random number generator (RNG) testing.

        The test suite contains 15 total tests, which are ran in their order of reference in the NIST documentation.

    """


    def __init__(self, binaryFile=None, bits=-1):
        """ Initializer for TRNGTester class.

        Args:
            binaryFile (str): Filename where the random data to test is.
            bits (int, optional): number of bits to read/test. Defaults to -1 (all bits).
        """

        if binaryFile != None:
            self.binary = self.BinaryData(binaryFile, bits=bits, binformat='bytes')
        else:
            self.binary = None

    class BinaryData:
        """ Class that holds arrays of packed/unpacked binary data.
            Constructor parses bin
        """
        def __init__(self, filename, bits=-1, binformat='bytes'):
            if filename == None:
                self.unpacked = None
                self.packed = None
                self.n = 0
                return

            self.packed = np.fromfile(filename, dtype=np.uint8, count=bits//8)

            self.unpacked = np.unpackbits(self.packed)
            self.n = len(self.packed) * 8

    def run_nist_tests(self):
        """ Runs all the NIST tests and returns p-values. """
        verbose=2
        nistFuncs = [
                    monobit_test, frequency_within_block_test, runs_test, longest_run_within_block_test,
                    binary_matrix_rank_test, discrete_fourier_transform_test, 
                    non_overlapping_template_matching_test,
                    overlapping_template_matching_test, maurers_universal_test, 
                    linear_complexity_test,
                    serial_test, approximate_entropy_test, cumulative_sums_test, random_excursion_test, random_excursion_variant_test
        ]

        allstart = time.time()

        pvalues = []

        # iterate through the nist tests, run each one and gather the results
        for func in nistFuncs:
            start = time.time()
            if verbose == 2:
                print("Starting: {}".format(func.__name__))

            try:
                ret = func(copy.deepcopy(self.binary))
                end = time.time() - start
                
                if verbose == 2:
                    print("\t\tResults (p-value(s), Pass/Fail) = \n\t\t\t{}".format(ret))
                    print("✔️ Test completed in {}s".format(end))
                    print("\n")
                    
                if len(ret) == 2 and type(ret[0]) != tuple:
                    ps = [ret[0]]
                else:
                    ps = [p for p,s in ret]

                pvalues.append(ps)
                    
            except Exception as e:
                print(f"Ran into exception: {repr(e)}")
                print("❌ Error running: {}\nVerify that you have at least 10000 bits of data.".format(func.__name__))
                return None

        if verbose == 2:
            print("Completed all tests in: {}s".format(time.time() - allstart))

        return pvalues
