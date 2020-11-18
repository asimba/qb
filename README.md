# qb  
  
Cube (qb). This program provides simple compression of user data.  
  
--- 
Usage : ./bin/qb \<OPTIONS\>  
**Examples:**  
```  
qb --pack -i ./ > packed.qb  
qb --pack --input=./source.in --output=./packed.qb  
qb --pack -i ./src1 -i ./src2 -o ./packed.qb  
qb --unpack -i ./source.qb  
qb --unpack -i ./source.qb -o ./  
qb -p -i ./source.in -o ./packed.qb  
qb -u -i ./source.qb  
qb --list -i ./source.qb  
qb -l -i ./source.qb  
```  
---  
**Options**:  
Option  | Value
----- | ------ 
-p, --pack | pack input data  
-u, --unpack | unpack input data  
-u0,--unpack=0 | unpack input data, but without writing - a kind of check  
-l, --list | list contents of the qb-archive  
-l0, --list=0 | list contents of the qb-archive, but without printing - a kind of check  
-n, --ignore | ignore impossibility of change file ownership/permissions or access/modification times (ONLY for "unpack" MODE)  
-i NAME  , --input=NAME | input file or directory (wildcards accepted ONLY for "pack" MODE)<br>NOTE: when using wildcards - put them in double quotes  
-o NAME  , --output=NAME | output file or directory - ONLY ONE FILENAME or PATH  
-h, --help | display this help and exit  
-v, --version | output version information and exit  
  
---  
  
<p align="justify">This is free software. There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.</p>  
  
---  
  
Written by Alexey V. Simbarsky.  
