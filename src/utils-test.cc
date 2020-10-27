#include "utils.hh"
#include <cstring>
#include <utility>
#include <vector>
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class CLIArgumentTest : public testing::TestWithParam<std::pair<std::vector<std::string>, fs::CLIArguments>> {};

TEST_P(CLIArgumentTest, test_parse_cli_arguments) {
  std::pair<std::vector<std::string>, fs::CLIArguments> param = GetParam();

  std::vector<char*> argv;
  for (auto str : param.first) {
    char* a = new char[str.length() + 1];
    std::strcpy(a, str.c_str());
    argv.emplace_back(a);
  }

  auto args = fs::parseCLIArgs(argv.size(), argv.data());

  ASSERT_EQ(args.host, param.second.host);

  for (auto str : argv) {
    delete[] str;
  }
}

INSTANTIATE_TEST_SUITE_P(
    DummyArguments,
    CLIArgumentTest,
    testing::Values(std::make_pair<std::vector<std::string>, fs::CLIArguments>({"run-test", "--host", "0.0.0.0"},
                                                                               {"0.0.0.0", "", 8080, 4, false}),
                    std::make_pair<std::vector<std::string>, fs::CLIArguments>({"run-test", "-i", "test"},
                                                                               {"localhost", "test", 8080, 4, false})));

TEST(server_util_test, parse_json__root_object_test) {
  std::vector<float> v;
  int64_t numK;
  ASSERT_THAT([&]() { fs::parseJsonPayload("[[0]]", 128, v, numK); },
              testing::ThrowsMessage<std::runtime_error>(testing::HasSubstr("Cannot parse json object.")));

  ASSERT_THAT([&]() { fs::parseJsonPayload("0", 128, v, numK); },
              testing::ThrowsMessage<std::runtime_error>(testing::HasSubstr("Cannot parse json object.")));

  ASSERT_THAT([&]() { fs::parseJsonPayload("null", 128, v, numK); },
              testing::ThrowsMessage<std::runtime_error>(testing::HasSubstr("Cannot parse json object.")));

  ASSERT_THAT([&]() { fs::parseJsonPayload("null", 128, v, numK); },
              testing::ThrowsMessage<std::runtime_error>(testing::HasSubstr("Cannot parse json object.")));
}

TEST(server_util_test, parse_json__parsing_error) {
  std::vector<float> v;
  int64_t numK;

  ASSERT_THAT([&]() { fs::parseJsonPayload("{}", 128, v, numK); },
              testing::ThrowsMessage<std::runtime_error>(
                  testing::HasSubstr("The JSON field referenced does not exist in this object.")));

  ASSERT_THAT([&]() { fs::parseJsonPayload("{\"queries\": [0, 1, 2]}", 128, v, numK); },
              testing::ThrowsMessage<std::runtime_error>(
                  testing::HasSubstr("The JSON element does not have the requested type.")));

  ASSERT_THAT([&]() { fs::parseJsonPayload("{\"queries\": [[0, 1, 2]]}", 2, v, numK); },
              testing::ThrowsMessage<std::runtime_error>(testing::HasSubstr("Dimension mismatch.")));
}

TEST(server_util_test, parse_json) {
  std::vector<float> outputVector;
  int64_t numK = -1;

  fs::parseJsonPayload("{\"queries\": [[1, 2]]}", 2, outputVector, numK);
  ASSERT_THAT(outputVector, testing::ElementsAre(1, 2));
  ASSERT_EQ(numK, -1);

  outputVector.clear();
  fs::parseJsonPayload("{\"queries\": [[1, 2,3,4,5], [6,7,8,9,0]], \"top_k\": 10}", 5, outputVector, numK);
  ASSERT_THAT(outputVector, testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9, 0));
  ASSERT_EQ(numK, 10);
}
