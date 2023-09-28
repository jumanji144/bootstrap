#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace util {

    bool string_ends_with_one_of(const std::string_view &string, const std::vector<std::string_view> &suffixes) {
        return std::ranges::any_of(suffixes, [&string](const std::string_view &suffix) {
            return string.ends_with(suffix);
        });
    }

}