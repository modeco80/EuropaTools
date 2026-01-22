#!/bin/bash
# Reformats the entire EuropaTools source tree.
find include/ src/ -type f | rg -e "(.cpp|.h|.hpp)" | xargs -i -- clang-format -i "{}"
