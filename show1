#!/bin/sh

runoff1 "$@" | pr.pl -h "xv6/$@" | mpage -m50t50b -o -bLetter -T -t -2 -FLucidaSans-Typewriter83 -L60 >x.ps; gv --swap x.ps
