#pragma once
#include <optional>
#include <string>
#include <vector>

std::vector<std::string> split(const std::string &s, char delim);

void rebootInto(const char* target);

// Atomically create a marker file at path `file` with `content`
void createMarkerFile(std::string file, std::optional<std::string> content = std::nullopt);
