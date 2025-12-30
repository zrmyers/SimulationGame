#include "String.hpp"

namespace Core {

    bool EndsWith(const std::string& fullstring, const std::string& suffix) {
        bool endsWith = false;
        if (suffix.length() <= fullstring.length()) {

            endsWith = (fullstring.compare(
                fullstring.length() - suffix.length(),
                suffix.length(),
                suffix) == 0);
        }
        return endsWith;
    }
}