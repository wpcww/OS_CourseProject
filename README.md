Objective:
Given input file consist of lower case alphabets, count occurence of each letter and output, with concurrency

Flow:
0. create a datatype of index indicating the current reading character, occurrence counter and the pointer to the input string 
1. Load file
2. Get input
3. Initialize shared memory with the created datatype
4. Create threads
5. Iterate through the given string while comparing with all alphabets
6. Record occurrences
7. Output results
