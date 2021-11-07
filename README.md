Objective:
Given input file consist of lower case alphabets, count occurence of each letter and output, with concurrency

Flow:
1. create a datatype of index indicating the current reading character, occurrence counter and the pointer to the input string 
2. Load file
3. Get input
4. Initialize shared memory with the created datatype
5. Create threads
6. Iterate through the given string while comparing with all alphabets
7. Record occurrences
8. Output results
