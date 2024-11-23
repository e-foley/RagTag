// RagTag.cpp : Defines the entry point for the application.
//

#include "RagTag.h"
#include "tag_map.h"
#include <nlohmann/json.hpp>
#include <wx/wx.h>
#include <fstream>

using namespace std;

int main() {
  ragtag::TagMap tag_map;
  tag_map.registerTag("Banana");
  tag_map.registerTag("Cantaloupe");
  tag_map.registerTag("Apple");
  nlohmann::json j = tag_map.toJson();
  std::ofstream o("test.json");
  o << std::setw(2) << j << std::endl;
}
