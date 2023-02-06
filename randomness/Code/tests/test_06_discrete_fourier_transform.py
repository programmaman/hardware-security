import numpy as np
import math
import scipy.fft as fft

# import scipy.special as ss
# from scipy.stats import chisquare
# import multiprocessing as mp
# from multiprocessing.dummy import Pool as ThreadPool

def discrete_fourier_transform_test(binary):
    """ 
        Tests the discrete fourier transform (DFT) of the sequence to determine if the number of peaks is 
        comparable to a random sequence.
    """

    # convert bits from {0,1} to {-1,1}
    bits = 2*binary.unpacked.astype(np.int8) - 1

    # compute fft of the sequence
    s = np.abs(fft.rfft(bits))[:len(bits) // 2]

    # T = the 95% peak height threshold value
    # 95% of the values from s should not exceed T
    T = math.sqrt(math.log(1.0/0.05)*len(bits))

    N0 = 0.95 * len(bits) / 2.0  # expected number of peaks
    N1 = len(np.where(s < T)[0]) # actual observed number of peaks

    # distance metric, p-value calculation
    d = (N1 - N0)/math.sqrt((len(bits)*0.95*0.05)/4)
    p = math.erfc(abs(d)/math.sqrt(2))

    success = (p >= 0.01)

    return [p, success]

# leaving this out for now, is an attempt to have a memory efficient DFT test
# the above does not work for 1B bits
# def discrete_fourier_transform_test_two_level(binary, m=1):

    # bits = 2*binary.unpacked.astype(np.int8) - 1

    # two-level test for bitstreams that are too large
    # if binary.n > 10_000_000:
    #     m = 1000
    
    # blockSize = binary.n // m
    # blocks = bits.reshape(m, blockSize)
    # T = math.sqrt(math.log(1.0/0.05)*blockSize)
    
    # with ThreadPool(mp.cpu_count()) as p:
    #     s = p.map(vectorized_fft, blocks)

    # N0 = 0.95 * blockSize / 2.0
    # N1 = np.array([len(np.where(si < T)[0]) for si in s])

    # d = (N1 - N0)/math.sqrt((blockSize*0.95*0.05)/4)

    # compute proportion of p-values that pass
    # p = np.array([math.erfc(abs(di)/math.sqrt(2)) for di in d])
    # alpha = 0.01
    # mean  = 1.0 - alpha
    # std   = math.sqrt((alpha*mean / m))

    # prop = len(np.where(p >= alpha)[0]) / m
    # cilow = mean - 3*std
    # cihigh = mean + 3*std

    # # check if passes proportion test
    # propPass = prop >= cilow and prop <= cihigh

    # perform chisq test on computed p-values
    # chsq, uniformityp = chisquare(p, ddof=10)
    # pt = ss.gammainc(10/2,chsq/2)


    # return [prop, propPass, pt, pt >= 0.0001]

# def vectorized_fft(x):
#     return np.abs(fft.rfft(x)[:len(x) // 2]).astype(np.float32)