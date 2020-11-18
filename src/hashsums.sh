#!/bin/bash

find . -name \* -not -path "*/.git/*" -type f | sort | \
while read f_name
do
  sha1sum -b "$f_name" >> ./sha1sums
done
