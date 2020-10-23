Quine-McCluskey Method

Description:
    An implementation of two-level logic optimizer based on Quine-McCluskey method. The first step is to
    generate all prime implicants for the given function. The second step is to choose a minimum-cost 
    cover based on column covering method. The target is to generate a sum-of-products expression of this 
    function with minimum number of literals.

Compile:
    $ make

Usage:
    $ ./hw1 [input file name] [output file name] 

Implementation steps:
    1. Read input from file using `fstream` and `getline`.
    2. Grouping the on set numbers and don't care set numbers. First I create a map that calculate number of 
       bit 1 of all possible numbers under 2^(number of variables). Then I can optain the corresponding number
       of bit 1 of the on set numbers and don't care set numbers and they are grouped with their number of 
       bit 1.
    3. For finding primary implicants I compare numbers in two adjacent groups. If two number has only one bit
       different, they will be merge in new group. If a number has no such matching pair, it is a primary
       implicant.
    3. For column covering I create a table with its row the primary implicants and its column on-set elements.
       1 means the element is covered by the primary implicant, 0 means not. First I find the column covered by
       only one primary implicant. Here I recorded the number of 1s in row and column so that I can easily find
       columns with only one "1". Then for the correspoding primary implicant, set every row of the column of 
       all the numbers it covered to 0 on the table.
    4. Now the rows remaining 1s on the table means it is not the essential primary implicants. I find the row
       with the most 1s remaining for optimized solution (but I'm not sure if this will result in optimized 
       result). Repeat what I've done in step 3 with the chosen row.
    5. Finally the table will be all 0s. And rows (primary implicants) selected in step 3 and 4 will be the
       minimum sum-of-products expression. Next calculate the cost and write the result to file.
