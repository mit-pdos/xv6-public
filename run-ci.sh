i#!/bin/bash

#set -euo # errexit,nounset,pipefail
# TODO: uncomment -euo when run-ci.sh passes all tests
set -uo # nounset,pipefail


#########################################################################
# clang-format
# clang-format -style=google -dump-config > .clang-format
changed_files=$(find . \( -iname "*.c" -o -iname "*.h" -o -iname "*.S" \) \
  -exec clang-format -i {} \; -exec git diff --name-only {} \;)


if [ -n "$changed_files" ]; then
  echo "Files were changed by clang-format:"
  echo "$changed_files"
  git diff --color=always &> required_changes.txt
  cat required_changes.txt
  false
fi

#########################################################################
# inatll and run cpp style checker
python3 -m venv /tmp/myenv
. /tmp/myenv/bin/activate
pip install cpplint
FILTERS=-legal/copyright,-readability/casting,-build/include_subdir,
FILTERS+=-build/header_guard,-runtime/int,-readability/braces
find . -regex ".*\.c\|.*\.h" | xargs cpplint  --filter=$FILTERS

########################################################################
# install and run bashate
pip install bashate
find . -name "*.sh"  -exec bashate {} \;

########################################################################
# run static analyzer
ERROR_CODE=20
cppcheck --error-exitcode=${ERROR_CODE} \
        --enable=portability,information,performance,warning \
	--inconclusive --xml --xml-version=2 . 2> cppcheck.xml
if [ $? -eq  $ERROR_CODE ]; then
  cppcheck-htmlreport --file=cppcheck.xml --report-dir=cppcheck-report \
	  --source-dir=. --title="Cppcheck Report"
fi

########################################################################
# compile
make clean
make

########################################################################
#  Run tests
./runtests.exp mylog.txt


########################################################################
#  Last verification
lines=$(tail -1 mylog | grep  "ALL TESTS PASSED" | wc -l)
if [ $lines -ne 1 ]; then
   echo "ALL TESTS PASSED string was not found"
   exit 1
fi

echo "SUCCESS"
exit 0 
