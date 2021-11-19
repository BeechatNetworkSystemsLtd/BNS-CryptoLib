import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

plt.figure(figsize=(8, 10))

# Load data
data = pd.read_csv('test_results.csv')

blake3 = data['size'] / data['blake3'] * 1000 / 1048576
sha256 = data['size'] / data['sha256'] * 1000 / 1048576
sha512 = data['size'] / data['sha512'] * 1000 / 1048576
sha1 = data['size'] / data['sha1'] * 1000 / 1048576

allgr = [max(blake3), max(sha512), max(sha256), max(sha1)]
plt.axis([0, max(data['size']), 0, max(allgr) + 0.13 * max(allgr)])

plt.plot(data['size'], blake3, label='Blake3')
plt.plot(data['size'], sha256, label='SHA-256')
plt.plot(data['size'], sha512, label='SHA-512')
plt.plot(data['size'], sha1, label='SHA-1')

plt.xlabel('size (Mb)', fontsize=14)
plt.ylabel('hashing force (Mb/s)', fontsize=14)
plt.grid(True)
plt.legend(loc='best', fontsize=12)
plt.savefig('diagram.png')
plt.show()

