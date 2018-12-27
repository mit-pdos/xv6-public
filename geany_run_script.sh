#!/bin/sh

rm$0

"./sh"

echo "

---------------------------
(program exited with code: $?)"

echo"Press return to continue"

#to be more compatible with shells like dasj

dummy_var=""

read dummy_var
