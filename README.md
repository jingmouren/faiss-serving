# faiss-serving

![Run Test](https://github.com/scatterlab/faiss-serving/workflows/Run%20Test/badge.svg)

A lightweight Faiss HTTP Server 🚀

## Usage

### Using docker

You can use [our pre-built docker image. (scatterlab/faiss-serving)](https://hub.docker.com/r/scatterlab/faiss-serving)

```sh
$ docker run \
  -p $HOST_PORT:8080 \
  -v /path/to/faiss/index:/path2/to/faiss/index \
  scatterlab/faiss-serving --index-file /path2/to/faiss/index
[2020-10-28 02:24:08.743] [info] Configuration
[2020-10-28 02:24:08.743] [info]  - Host: 0.0.0.0
[2020-10-28 02:24:08.743] [info]  - Port: 8080
[2020-10-28 02:24:08.743] [info]  - Num of Http Listener Threads: 4
[2020-10-28 02:24:08.743] [info]  - IndexFile: /test.index
[2020-10-28 02:24:08.743] [info]  - Default numK: 800
[2020-10-28 02:24:08.743] [info]  - EfSearch Value: 0
...
...
```

### How to query

You can create a dummy index file using `create_test_faiss_embedding.py`.

```sh
$ curl localhost:8080/v1/search \
    -d '{"queries":[[1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]], "top_k": 1}'
{"distances":[[13.8764]],"indices":[[600]]}
$ # To use default numK,
$ curl localhost:8080/v1/search \
   -d '{"queries":[[1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]]}'
{"distances":[[13.8764,14.0086,14.1078,14.3203,14.3573,]],"indices":[[600,917,830,890,902,...]]}
$ # To query multiple vectors,
$ curl localhost:8080/v1/search \
  -d '{"queries":[[1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1],
  [0,1,2,1,3,1,1,5,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]], "top_k": 10}'
{"distances":[[13.8764,14.0086,14.1078,14.3203,14.3573,14.7239,14.7417,15.0456,15.1448,15.4105],[38.1983,38.2254,39.6167,39.7312,40.1475,40.5731,40.6985,41.1706,41.2871,41.476]],"indices":[[600,917,830,890,902,900,148,1143,479,1289],[148,900,917,890,41,50,584,376,1261,973]]}
```

## Build

### Build from source

```sh
$ git clone https://github.com/scatterlab/faiss-serving.git
$ cd faiss-serving
$ git submodule update --init --recursive
$ mkdir build && cd build
$ cmake .. && make
$ ./faiss-serving --help
A lightweight Faiss HTTP Server

Usage:
  ./faiss-serving [OPTION...]

      --host arg              Host (default: localhost)
  -p, --port arg              Port (default: 8080)
  -i, --index-file arg        Faiss index file path (default: )
  -t, --listener-threads arg  Num threads to use for http server (default: 4)
  -k, --num-k arg             Default num k (default: 800)
      --hnsw-ef-search arg    efSearch Value for hnsw index (default: 0)
  -h, --help                  Print usage
```

### Build docker image

```sh
$ docker build -t IMAGE_NAME .
Sending build context to Docker daemon  163.2MB
Step 1/13 : FROM alpine:3.12.0 AS builder
...
```
