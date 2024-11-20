// RagTag.cpp : Defines the entry point for the application.
//

#include "RagTag.h"
#include "tag_map.h"
#include <nlohmann/json.hpp>
#include <fstream>

using namespace std;

int main() {
  ragtag::TagMap tag_map;
  tag_map.registerTag(5, "Five");
  tag_map.registerTag(3, "Three");
  tag_map.registerTag(11, "Eleven");
  nlohmann::json j;
  j["tag_list"] = tag_map.toJson();
  std::ofstream o("test.json");
  o << std::setw(2) << j << std::endl;
}
