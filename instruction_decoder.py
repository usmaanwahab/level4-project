#!/usr/bin/env python3
import subprocess
import glob
import os
import csv
import json
from collections import defaultdict, Counter

# EXE = "/workspace/build/jacobian-1d-nopie"
# EXE = "/workspace/build/hotspot_openmp-nopie"
EXE = "/workspace/build/sparselu_single-nopie"
MEMTRACE_PATTERN = "memtrace.out.*"

# executable .text range
# filter out instructions that aren't part of the original source
# JAC nopie
# EXE_START = int("0x401090", 16)
# EXE_END   = int("0x4018c4", 16)

# Hotspot nopie
# EXE_START = int("0x401170", 16)
# EXE_END = int("0x402564", 16)

# SparseLU
EXE_START = int("0x4011d0", 16)
EXE_END   = int("0x404efc", 16)

CACHE_LINE_SIZE = 64

addr2line_cache = {}

def addr2line(ip: str):
    if ip in addr2line_cache:
        return addr2line_cache[ip]

    try:
        ip_int = int(ip, 16)
        if ip_int < EXE_START or ip_int >= EXE_END:
            addr2line_cache[ip] = None
            return None

        res = subprocess.check_output(
            ["addr2line", "-f", "-e", EXE, ip],
            stderr=subprocess.DEVNULL
        ).decode().strip().split("\n")

        if len(res) != 2 or res[1] == "??:0":
            addr2line_cache[ip] = None
            return None

        result = {
            "function": res[0],
            "location": res[1]
        }

        addr2line_cache[ip] = result
        return result

    except subprocess.CalledProcessError:
        addr2line_cache[ip] = None
        return None
    

def parse_memtrace_file(filename):
    thread_id = os.path.basename(filename).split(".")[-1]
    entries = []

    with open(filename, "r") as f:
        for line in f:
            parts = line.strip().split()

            if len(parts) == 2:
                ip, mem = parts
                access_type = "U"
            elif len(parts) == 3:
                ip, mem, access_type = parts
            else:
                continue

            src = addr2line(ip)
            if src is None:
                continue

            entries.append({
                "thread": thread_id,
                "ip": ip,
                "memory": mem,
                "access_type": access_type,
                "function": src["function"],
                "location": src["location"]
            })

    return entries

def classify_sharing(stats):
    threads = len(stats["threads"])
    reads = stats["reads"]
    writes = stats["writes"]

    if threads == 1:
        return "Private"

    if writes == 0:
        return "Read-only sharing"

    if writes > 1:
        return "WAW"

    if writes > 0 and reads > 0:
        return "RAW/WAR"

    return "Other"


def aggregate(entries):

    memory_stats = defaultdict(lambda: {
        "count": 0,
        "reads": 0,
        "writes": 0,
        "threads": set()
    })

    ip_stats = defaultdict(lambda: {
        "count": 0,
        "reads": 0,
        "writes": 0,
        "threads": set(),
        "function": None,
        "location": None
    })

    thread_stats = Counter()

    for e in entries:

        thread = e["thread"]
        ip = e["ip"]
        mem = e["memory"]
        access = e["access_type"]
        func = e["function"]
        loc = e["location"]

        thread_stats[thread] += 1

        m = memory_stats[mem]
        m["count"] += 1
        m["threads"].add(thread)

        if access == "R":
            m["reads"] += 1
        elif access == "W":
            m["writes"] += 1

        i = ip_stats[ip]
        i["count"] += 1
        i["threads"].add(thread)
        i["function"] = func
        i["location"] = loc

        if access == "R":
            i["reads"] += 1
        elif access == "W":
            i["writes"] += 1

    return memory_stats, ip_stats, thread_stats


def detect_false_sharing(memory_stats):
    cacheline_map = defaultdict(lambda: {
        "addresses": [],
        "threads": set(),
        "writes": 0
    })

    for mem, stats in memory_stats.items():
        addr = int(mem, 16)
        line = addr // CACHE_LINE_SIZE

        entry = cacheline_map[line]
        entry["addresses"].append(mem)
        entry["threads"].update(stats["threads"])
        entry["writes"] += stats["writes"]

    false_sharing_lines = {}

    for line, data in cacheline_map.items():
        if len(data["threads"]) > 1 and data["writes"] > 0:
            false_sharing_lines[line] = data

    return false_sharing_lines


def write_memory_csv(memory_stats, filename="memory_summary.csv"):

    with open(filename, "w", newline="") as f:
        writer = csv.writer(f)

        writer.writerow([
            "memory_address",
            "total_accesses",
            "reads",
            "writes",
            "num_threads",
            "threads",
            "sharing_type"
        ])

        for mem, stats in sorted(memory_stats.items(),
                                 key=lambda x: x[1]["count"],
                                 reverse=True):

            sharing = classify_sharing(stats)

            writer.writerow([
                mem,
                stats["count"],
                stats["reads"],
                stats["writes"],
                len(stats["threads"]),
                ",".join(sorted(stats["threads"])),
                sharing
            ])


def write_ip_csv(ip_stats, filename="ip_summary.csv"):

    with open(filename, "w", newline="") as f:
        writer = csv.writer(f)

        writer.writerow([
            "instruction_pointer",
            "total_accesses",
            "reads",
            "writes",
            "num_threads",
            "function",
            "location"
        ])

        for ip, stats in sorted(ip_stats.items(),
                                key=lambda x: x[1]["count"],
                                reverse=True):

            writer.writerow([
                ip,
                stats["count"],
                stats["reads"],
                stats["writes"],
                len(stats["threads"]),
                stats["function"],
                stats["location"]
            ])


def write_thread_csv(thread_stats, filename="thread_summary.csv"):

    with open(filename, "w", newline="") as f:
        writer = csv.writer(f)

        writer.writerow(["thread", "total_accesses"])

        for thread, count in sorted(thread_stats.items()):
            writer.writerow([thread, count])


def write_false_sharing_csv(false_sharing, filename="false_sharing.csv"):

    with open(filename, "w", newline="") as f:
        writer = csv.writer(f)

        writer.writerow([
            "cache_line",
            "num_addresses",
            "num_threads",
            "threads"
        ])

        for line, data in false_sharing.items():
            writer.writerow([
                line,
                len(data["addresses"]),
                len(data["threads"]),
                ",".join(sorted(data["threads"]))
            ])

def main():

    files = glob.glob(MEMTRACE_PATTERN)

    print(f"Found {len(files)} memtrace files")

    all_entries = []

    for f in files:
        entries = parse_memtrace_file(f)
        all_entries.extend(entries)

    print(f"Parsed {len(all_entries)} total memory accesses")

    memory_stats, ip_stats, thread_stats = aggregate(all_entries)

    false_sharing = detect_false_sharing(memory_stats)

    write_memory_csv(memory_stats)
    write_ip_csv(ip_stats)
    write_thread_csv(thread_stats)
    write_false_sharing_csv(false_sharing)

    print("\nOutput files generated:")
    print("  memory_summary.csv")
    print("  ip_summary.csv")
    print("  thread_summary.csv")
    print("  false_sharing.csv")

if __name__ == "__main__":
    main()