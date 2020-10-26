#include <memory>
#include <sstream>
#include <string>

#include "faiss/Index.h"
#include "faiss/IndexHNSW.h"
#include "faiss/MetaIndexes.h"
#include "faiss/index_io.h"
#include "httplib.h"
#include "spdlog/spdlog.h"

#include "server.hh"

int main(int argc, char** argv) {
  auto config = fs::parseCLIArgs(argc, argv);

  if (config.debug)
    spdlog::set_level(spdlog::level::debug);

  spdlog::debug("Configuration");
  spdlog::debug(" - Host: {}", config.host);
  spdlog::debug(" - Port: {}", config.port);
  spdlog::debug(" - Num of Http Listener Threads: {}", config.listenerThreads);
  spdlog::debug(" - IndexFile: {}", config.indexFile);
  spdlog::debug(" - Default numK: {}", config.numK);
  spdlog::debug(" - EfSearch Value: {}", config.efSearch);
  spdlog::debug(" - debug: {}", config.debug);

  httplib::Server server;
  server.new_task_queue = [config] { return new httplib::ThreadPool(config.listenerThreads); };

  spdlog::debug("Start loading {}", config.indexFile);
  faiss::Index* index = faiss::read_index(config.indexFile.c_str(), faiss::IO_FLAG_READ_ONLY);

  if (config.efSearch != 0) {
    auto indexHNSW = dynamic_cast<faiss::IndexHNSWPQ*>(dynamic_cast<faiss::IndexIDMap*>(index)->index);
    indexHNSW->hnsw.efSearch = config.efSearch;
    spdlog::debug("Set efsearch as {}", config.efSearch);
  }

  server.set_logger([](const httplib::Request& req, const httplib::Response& res) {
    spdlog::debug("{} {} HTTP/{} {} - from {}", req.method, req.path, req.version, res.status, req.remote_addr);
  });

  server.Post("/search", [index, config](const httplib::Request& req, httplib::Response& res) {
    std::vector<float> queryVector;
    int64_t numK = -1;
    size_t numQueries = fs::parseJsonPayload(req.body, index->d, queryVector, numK);

    float* distances = new float[numK * numQueries];
    faiss::Index::idx_t* labels = new faiss::Index::idx_t[numK * numQueries];

    index->search(numQueries, queryVector.data(), (faiss::Index::idx_t)(numK == -1 ? config.numK : numK), distances,
                  labels);

    res.set_content(fs::constructJson(labels, distances, numK, numQueries), "application/json");
    delete[] distances;
    delete[] labels;
  });

  spdlog::info("Running server on {}:{}", config.host, config.port);
  server.listen(config.host.c_str(), config.port);

  return 0;
}
