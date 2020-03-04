//
// Created by artyomd on 1/5/20.
//

#pragma once

#include <vector>
#include <string>
#include <ios>
#include <fstream>
#include <sstream>

static std::vector<char> ReadFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("failed to open the file!");
  }
  size_t file_size = (size_t) file.tellg();
  std::vector<char> buffer(file_size);
  file.seekg(0);
  file.read(buffer.data(), file_size);
  file.close();
  return buffer;
}

static std::string ParseFile(const std::string &path) {
  std::fstream stream(path);
  std::string line;

  std::stringstream string_stream;
  while (getline(stream, line)) {
    string_stream << line << "\n";
  }

  return string_stream.str();
}