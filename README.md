# Pancake
Pancake is the first system designed to protect key-value stores from access pattern leakage attacks with small constant factor bandwidth overhead.

Requirements:
libbzp2-dev
zlib1g-dev
cmake-3.5+
gcc-5
redis-server
ssdb-rocks - https://github.com/anuragkh/ssdb-rocks

Building:
After installing the requirements run ./build.sh at the top-level directory

Running:

Running pancake requires three machines
1. Client with a CLIENT_IP and CLIENT_PORT
2. Proxy with a PROXY_IP 
3. Storage server with STORAGE_SERVER_IP and STORAGE_PORT

To run the benchmark code with a particular
batch size - BATCH_SIZE
number of clients - NUM_CLIENTS
workload file - WORKLOAD_FILE
number of proxy threads - NUM_PROXY_THREADS

Make sure to place the workload file in the root of the pancake directory.

First start the storage server

Then start the proxy:
./bin/proxy_server -h <STORAGE_SERVER_IP> -p <STORAGE_PORT> -b <BATCH_SIZE> -t <NUM_PROXY_THREADS> -l <WORKLOAD_FILE>

After the proxy says its reachable launch the benchmark code:
./bin/proxy_benchmark -n <NUM_CLIENTS> -h <PROXY_IP> -t <WORKLOAD_FILE>

After completion the benchmark will display the throughput during the run. There will be a new folder in the data folder that contains one file for each client displaying the latency of each operation in nanoseconds.