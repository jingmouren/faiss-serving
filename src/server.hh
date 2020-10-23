#pragma once

#include <sstream>
#include <string>
#include "cxxopts.hpp"
#include "faiss/Index.h"
#include "simdjson.h"

namespace fs {

struct CLIArguments {
  const std::string host;
  const std::string indexFile;
  const size_t port;
  const size_t listenerThreads;
  const size_t numK;
  const bool debug;
};

inline const CLIArguments parseCLIArgs(int argc, char** argv) {
  cxxopts::Options options(argv[0], "Embedding Search HTTP Server");

  // clang-format off
  options.add_options()
    ("host", "Host", cxxopts::value<std::string>()->default_value("localhost"))
    ("i,index-file", "Faiss index file path", cxxopts::value<std::string>()->default_value(""))
    ("p,port", "Port", cxxopts::value<size_t>()->default_value("8080"))
    ("t,listener-threads", "Num Threads to use for http server", cxxopts::value<size_t>()->default_value("4"))
    ("k,num-k", "Num K for Faiss", cxxopts::value<size_t>()->default_value("800"))
    ("d,debug", "Logging all debug logs")
    ("h,help", "Print usage");
  // clang-format on

  auto args = options.parse(argc, argv);

  if (args.count("help")) {
    std::cout << options.help() << std::endl;
    std::exit(0);
  }

  return {args["host"].as<std::string>(),        args["index-file"].as<std::string>(), args["port"].as<size_t>(),
          args["listener-threads"].as<size_t>(), args["num-k"].as<size_t>(),           args["debug"].as<bool>()};
}

inline size_t parseJsonPayload(const std::string& payload,
                               size_t dimension,
                               std::vector<float>& outputVector,
                               int64_t& numK) {
  try {
    simdjson::dom::parser parser;
    simdjson::dom::element element = parser.parse(payload.c_str(), payload.length());

    if (element.type() != simdjson::dom::element_type::OBJECT)
      throw std::runtime_error("Cannot parse json object. Root object must be json object.");

    simdjson::dom::array queries;
    simdjson::error_code error = element["queries"].get(queries);
    if (error)
      throw std::runtime_error(simdjson::error_message(error));

    error = element["numK"].get(numK);  // ignore error

    outputVector.reserve(queries.size() * dimension);

    for (simdjson::dom::array query : queries) {
      if (query.size() != dimension)
        throw std::runtime_error("Dimension mismatch.");

      for (double item : query) {
        outputVector.push_back(item);
      }
    }

    return queries.size();
  } catch (simdjson::simdjson_error error) {
    // change simdjson_error to std::runtime_error
    throw std::runtime_error(error.what());
  }
}

inline std::string constructJson(const faiss::Index::idx_t* labels,
                                 const float* distances,
                                 int64_t numK,
                                 size_t numQueries) {
  std::stringstream outputJson;

  outputJson << "{\"distances\":[";
  for (int i = 0; i < numQueries; i++) {
    outputJson << "[";
    for (int j = 0; j < numK; j++) {
      outputJson << distances[numK * i + j];
      if (j != numK - 1)
        outputJson << ",";
    }
    outputJson << "]";
  }
  outputJson << "],\"indices\":[";
  for (int i = 0; i < numQueries; i++) {
    outputJson << "[";
    for (int j = 0; j < numK; j++) {
      outputJson << labels[numK * i + j];
      if (j != numK - 1)
        outputJson << ",";
    }
    outputJson << "]";
  }
  outputJson << "]}";

  return outputJson.str();
}

}  // namespace fs
