import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

plt.figure(figsize=(10, 10))

# Load data
data = pd.read_csv('test_results.csv')
plt.axis([0, max(data['size']), 0, max(data['sha256'])])

plt.plot(data['size'], data['blake3'], label='Blake3')
plt.plot(data['size'], data['sha256'], label='SHA-256')
plt.plot(data['size'], data['sha512'], label='SHA-512')
plt.plot(data['size'], data['sha1'], label='SHA-1')

plt.xlabel('size (mb)', fontsize=14)
plt.ylabel('time (ms)', fontsize=14)
plt.grid(True)
plt.legend(loc='best', fontsize=12)
plt.savefig('diagram.png')
plt.show()

