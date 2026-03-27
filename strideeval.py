import os
import sys
from collections import defaultdict

BINARY_TEXT_MIN = int("0x401090", 16)
BINARY_TEXT_MAX = int("0x4018c4", 16)

def parse_trace(filepath):
    accesses = defaultdict(list)
    with open(filepath) as f:
        for line in f:
            parts = line.strip().split()
            if len(parts) < 3:
                continue
            ip, addr, rw = parts[0], parts[1], parts[2]
            if not (BINARY_TEXT_MIN <= int(ip, 16) <= BINARY_TEXT_MAX):
                continue
            accesses[ip].append(int(addr, 16))
    return accesses



def compute_strides(accesses):
    strides = {}
    for ip, addrs in accesses.items():
        if len(addrs) < 2:
            continue
        deltas = [abs(addrs[i+1] - addrs[i]) for i in range(len(addrs)-1)]
        # filter out resets - ignore deltas larger than a threshold e.g. 256 bytes
        deltas = [d for d in deltas if 0 < d <= 256]
        if not deltas:
            continue
        dominant = max(set(deltas), key=deltas.count)
        consistency = deltas.count(dominant) / len(deltas)

        strides[ip] = (dominant, consistency)
    return strides

trace_dir = sys.argv[1]
files = [f for f in os.listdir(trace_dir) if "memtrace.out.944117.0" in f]

ip_counts = defaultdict(int)
all_strides = defaultdict(list)
for fname in files:
    path = os.path.join(trace_dir, fname)
    accesses = parse_trace(path)
    for ip, addrs in accesses.items():
        ip_counts[ip] += len(addrs)
    strides = compute_strides(accesses)
    for ip, (stride, consistency) in strides.items():
        all_strides[ip].append((stride, consistency, fname))


print(f"{'IP':<20} {'count':>10} {'Stride':>10} {'Consistency':>11.1} {'File'}")
print("-" * 60)
for ip, entries in sorted(all_strides.items(), key=lambda x: -ip_counts[x[0]]):
    for stride, consistency, fname in entries:
        print(f"{ip:<20} {ip_counts[ip]:>10} {stride:>10} {consistency:>11.1%} {fname}")
