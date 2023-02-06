import numpy as np
import math

def runs_test(binary):
    """
        This tests the "oscillation" of the sequence,
        i.e. how much it switches from 1->0 and 0->1.
    """

    # proportion of 1s in the binary string
    if len(binary.packed) % 8 == 0:
        repacked = binary.packed
    else:
        repacked = np.pad(binary.packed, (8 - len(binary.packed) % 8, 0))

    ones = repacked.view(np.uint64)
    ones -= ((ones >> 1) & 0x5555555555555555)
    ones = (ones & 0x3333333333333333) + (ones >> 2 & 0x3333333333333333)
    ones = (((ones + (ones >> 4)) & 0xf0f0f0f0f0f0f0f) * 0x101010101010101 >> 56) & 0xff
    prop = np.sum(ones) / binary.n

    # test statistic
    vobs = 1 + np.sum(binary.unpacked[:-1] ^ binary.unpacked[1:])
    p = math.erfc(abs(vobs-(2.0*binary.n*prop*(1.0-prop)))/(2.0*math.sqrt(2.0*binary.n)*prop*(1-prop)))

    success = (p >= 0.01)

    return [p, success]