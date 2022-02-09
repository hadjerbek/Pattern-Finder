#!/usr/bin/bash

PROG="./pattern-finder"

TESTS=(
    "Lorem 0\n"
    "lorem 271\n1235\n"
    "lor 14\n271\n490\n861\n1235\n1632\n"
    "vitae 149\n315\n1330\n1389\n1725\n1812\n2004\n2396\n2729\n3427\n"
)

for nbt in `seq 1 10`; do
  for tst in "${TESTS[@]}"; do
      pattern=$(echo $tst | cut -d' ' -f1 -)
      results=$(echo -e $tst | cut -d' ' -f2 -)
      echo -n "$pattern ($nbt threads): "
      res=$($PROG 1 $pattern lorem.txt)
      if [ "$res" == "$results" ]; then
          echo "OK!"
      else
          echo "KO."
      fi
  done
  echo -e "\n"
done
