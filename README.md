# Pancake

`Pancake` is the first system designed to protect key-value stores from access pattern leakage attacks with small constant factor bandwidth overhead.

## Table of Contents

- [Requirements](#requirements)
- [Building](#building)
- [Running](#running)

## Requirements

- libbzp2-dev
- zlib1g-dev
- cmake-3.5+
- gcc-5
- libssl-dev
- redis-server
- ssdb-rocks - <https://github.com/anuragkh/ssdb-rocks>

## Building

After installing the requirements run

```bash
./build.sh
```

at the top-level directory.

Download one of our workload traces from s3 and place it into the root of pancake's directory.

```bash
aws s3 --no-sign-request cp s3://pancaketranscripts/ycsb-c-1m .
```

## Running

Running `Pancake` requires three machines

1. Client with a CLIENT_IP and CLIENT_PORT
2. Proxy with a PROXY_IP
3. `Redis` or `Rocksdb` backing storage server with STORAGE_SERVER_IP and STORAGE_PORT

To run the benchmark code with a particular:

- batch size - BATCH_SIZE
- number of clients - NUM_CLIENTS
- workload file - WORKLOAD_FILE
- number of proxy threads - NUM_PROXY_THREADS

First start the storage server

Then start the proxy:

```bash
./bin/proxy_server -h <STORAGE_SERVER_IP> -p <STORAGE_PORT> -b <BATCH_SIZE> -t <NUM_PROXY_THREADS> -l <WORKLOAD_FILE>
```

`Pancake` will now initialize - it will take longer with larger keysets. After the proxy says it's reachable launch the benchmark code:

```bash
./bin/proxy_benchmark -n <NUM_CLIENTS> -h <PROXY_IP> -t <WORKLOAD_FILE>
```

After completion the benchmark will display the throughput during the run. There will be a new folder in the data folder that contains one file for each client displaying the latency of each operation in nanoseconds.
