/* 
CS220 Spring 2014 Lab 2: C Lab (Bonus w/ Hash)
Code by Christopher Zhang

   Functionality: 
     Grimoire:
     declares an array of an array of pointers to HASH number of dictionaries

     Dictionary:
     declares a char **array (a dynamically allocated array of pointers)
     mallocs the char **array in a function (passing it in via ***)
     passes the char **array to function for load and print
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define  WORD_LENGTH  50
#define  INITIAL_ARRAY_MAX 25
#define  INSERT_SUCCESS 1
#define  INSERT_DUP_FAIL 0
#define  HASH 33 // The larger, the faster generally (trade-off in space)

// Grimoire: (An array of dictionaries)
// A tome of magic that hastens the look-up and therefore deletion
// of words (the typedef saves time of counting *'s)
typedef char *** Grimoire;
// Grimoire
int loadGrimoire(char * inFileName, Grimoire * g, int * count, int * capacity);
unsigned hasher(char * word);
// Array Resize
int loadArray(char * word, char *** array, int * count, int * capacity);
void repackage(char *** array, int * capacity);
// Print
void printArray(Grimoire * g, int * count, FILE * stream);
void menu(Grimoire * g, int * count, char * outFileName);
// Count
void countModule(int * count);
// Search
void searchModule(Grimoire * g, int * count);
int binarySearch(char * word, char *** array, int count);
// Insert
void insertModule(Grimoire * g, int * count);
int insertWord(char *** array, int * wordCount, char word[]);
int charPtrGreaterThanCharPtr(char * s1, char * word);
// Delete
void removeModule(Grimoire * g, int * count);
int removeWord(char *** array, int location, int count);
void quitModule(Grimoire * g, int * count, char * outFileName);
int saveDictionary(Grimoire * g, int * count, char * outFileName);
void freeGrimoire(Grimoire * g, int * counts);
void freeDictionary(char ** array, int * count);

int main(int argc, char * argv[]) {
    // Declare the array of pointers to dictionaries (which are ptrs to ptrs to
    // ptrs))
    Grimoire g;
    // Have an array of counts, each position corresponds to the respective bin
    // where each bin is a dictionary
    int wordCounts[HASH];
    int wordCapacities[HASH];
    for (int i = 0; i < HASH; ++i) {
        wordCounts[i] = 0; // Set all the initial counts to 0
        wordCapacities[i] = INITIAL_ARRAY_MAX; // Recall nothing is allocated
    }
    // Check args
    if (argc != 3) {
        fprintf(stderr,"Usage: %s inputfile outputfile\n", argv[0]);
        return 1;
    }
    // Try reading in the file
    if (loadGrimoire(argv[1], &g, wordCounts, wordCapacities) != 0) {
        fprintf(stderr,"    loadGrimoire failed! Program terminating...\n");
        return 1;  
    }
    int sum = 0;
    for (int i = 0; i < HASH; ++i)
        sum += wordCounts[i];
    printf("\nFinished loading %d words.\n", sum);
    menu(&g, wordCounts, argv[2]);
    //freeDictionary(&wordArray, &wordCount);
    freeGrimoire(&g, wordCounts);
    return 0;
}

/*
Perform the file input, reading, and closing. Allocate an array for the 
array of dictionaries according to a global constant used for the hash.
Get each line and call the picker() to identify which dictionary to use
insertWord() with. 
*/
int loadGrimoire(char * inFileName, Grimoire * g, int * counts,
                    int * capacities) {
    // Open file
    FILE *inFile;
    char word[WORD_LENGTH]; 
    // Read it
    if ((inFile = fopen(inFileName, "r")) == NULL) {
        fprintf(stderr,"Error opening input file, %s\n", inFileName);
        return -1;
    }
    // Allocated the grimoire
    *g = (Grimoire)malloc(HASH * sizeof(char**));
    if (*g == NULL) {
        fprintf(stderr, "Malloc of grimoire failed!\n");
        return -1;
    }
    // Allocate a dictionary for each position in the grimoire
    for (int i = 0; i < HASH; ++i) {
        char ** tempDictionary = (char **)malloc(capacities[i] * sizeof(char*));
        if (tempDictionary == NULL) {
            fprintf(stderr, "Malloc of individual dictionary failed!\n");
            return -1;
        }
        (*g)[i] = tempDictionary;
    }
    if (*g == NULL) {
        fprintf(stderr, "Malloc of array in loadGrimoire failed!\n");
        return -1;
    }
    // Parse the word
    printf("Reading file %s (each . is 5000 words read)\n", inFileName);
    int overallCount = 0;
    while (fscanf(inFile, "%s", word) == 1) {    
        ++overallCount;
        if (0 == overallCount % 5000) {
            printf(".");
            fflush(stdout);
        }
        // Choose which dictionary to send it to with hasher()
        unsigned hash = hasher(word);
        loadArray(word, *g + hash, counts + hash, capacities + hash);    
    }
    fclose(inFile); // Close the file
    return 0;
}

/*
Here is the the djb2 hash function with a mod HASH functionality.
*/
unsigned hasher(char * word) {
    unsigned hashAddress = 5381;
    for (int i = 0; word[i] != '\0'; ++i) {
        hashAddress = ((hashAddress << 5) + hashAddress) + word[i];
    }
    return hashAddress % HASH;
}

int loadArray(char * word, char *** array, int * count, int * capacity) {
    // Go the appropriate dictionary array
    // Check to see if the count is not the capacity
    if (*count >= *capacity) {
        // Double the size of array and copy contents
        repackage(array, capacity);
    }
    // Attempt to insert the word
    int insertOK = 0;
    if ((insertOK = insertWord(array, count, word)) == -1) {
        fprintf(stderr, "    Insert returned an error!\n");
        return 1;
    }
    else if (INSERT_DUP_FAIL == insertOK) {
        printf("%s ignored (duplicate)\n", word);
    }
    return 0;
}

/*
After reaching the maximum capacity of the array, allocate a new array, copy
over all old values. Re-assign the main array. Update the capacity as well.
*/
void repackage(char *** array, int * capacity) {
    // Allocate a new chunk of memory that is twice the capacity
    char ** temp = (char **)malloc((2 * (*capacity)) * sizeof(char*));
    if (*array == NULL) {
        fprintf(stderr, "Malloc of bigger array in re-sizing failed!\n");
        return;
    }
    // Copy the pointers to each word over
    for (int i = 0; i < *capacity; ++i) {
        temp[i] = (*array)[i];
    }
    // Free the memory that was allocated to the old array
    free(*array);
    *array = temp;
    (*capacity) *= 2;
}

/*
Given a current word, add it into the dictionary. Maintain the sorted-ness of
the dictionary by finding where it belongs with a binary search over the
elements from 0 to index count - 1, inserting it into that position and
shifting successors down by one position.
*/
int insertWord(char *** array, int * count, char word[]) {
    // Find the position this word belongs using a lexical scheme
    // Since the dictionary is sorted in-place, assume the items in indices
    // 0 to count - 1 are already in lexical order
    // Lexical order is a char by char comparison
    int j = (*count) - 1;
    int k = 0;
    while (j >= 0 && (k = charPtrGreaterThanCharPtr((*array)[j], word))) {
        if (-1 == k) { // A duplicate, halt this insert call
            /*
            Must now undo the shifts done so far.
            An enticing alternative is to call search on each word in insert.
            While this is trivial to implement, it performs worse under the 
            assumption that there are few duplicate attempts. Because a lg(n)
            search would be executed as well as potentially n shifts for every
            insert, the result for one file with n lines has approximately
            (n^2)(lgn) performance overall.
            */
            // From everything after the current position to *count-1
            for (++j; j < (*count); ++j) {
                (*array)[j] = (*array)[j+1];
            }
            return INSERT_DUP_FAIL;
        }
        (*array)[j+1] = (*array)[j];
        --j;
    }
    // Now that the destination of the word is known, do allocation
    char *wordPtr;
    wordPtr = (char *)malloc((strlen(word) + 1) * sizeof(char));
    if (wordPtr == NULL) {
    	fprintf(stderr,"    Malloc of array[%d] failed!\n", *count);
	    return -1;
    }
    /* Memory for this word has been allocated, so copy characters
       and insert into array */
    strcpy(wordPtr, word);
    // Set the emptied location to the new word pointer
    (*array)[j+1] = wordPtr;
    // Update count
    (*count)++;
    return INSERT_SUCCESS;
}

/*
Comparison function when passed two pointers to strings.
Returns whether first argument should come after, in lexical order, the second.
*/
int charPtrGreaterThanCharPtr(char * s1, char * word) {
    // Lexical order considers two strings of the same length using padding
    // Blanks come before any other letter
    while ((*s1) == (*word)) {
        if ((*s1) == '\0') // If the two strings are the same
            return -1; // No duplicates allowed, return -1
        ++s1;
        ++word;
    }
    // If either is the null terminating sequence, that holds precedence
   if ('\0' == *s1)
        return 0;
    else if ('\0' == *word)
        return 1;
    return ((*s1) > (*word));
}

/*
Offer options to the user. The input that is read in is handled with very
primitive, homemade flushing (no libraries, flush methods). For the
arbitrariness that is user-input, fgets(...) proved handier than scanf. 
For a more robust program with allowable library dependencies, using curses for
Unix and <conio.h> for DOS systems seems better-fitting for the single char
automatic input (no need to hit that ENTER).
*/
void menu(Grimoire * g, int * count, char * outFileName) {
    static char searchInformation[] = "S_EARCH:\tTest if word exists.\n";
    static char insertInformation[] = "I_NSERT:\tInsert a word.\n";
    static char removeInformation[] = "R_EMOVE:\tRemove a word.\n";
    static char countInformation[] = "C_OUNT:  \tReturn the number of words.\n";
    static char quitInformation[] = "Q_UIT:    \tExit program.\n";
    char string[256];
    char c;
    for(;;) {
        printf("Choose: 'P'rint, 'S'earch, 'I'nsert, 'R'emove, 'C'ount,"
               " Q'uit or 'N' for more information :");
        if (NULL == fgets(string, 256, stdin)) {
            fprintf(stderr, "The user-input was invalid. Aborting.\n");
            return;
        }
        c = string[0];
        if (string[1] != '\n') {
            printf("Enter exactly one character with the above options.\n");
            printf("Hit <Enter> to continue.\n");
            // Flush the input by repeatedly calling getchar()
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        switch(c) { // Spare pinkies (if human) and allow lower-case
            // To do this, utilize the fact that switch-case(s) "falls through"
            case 'P':
            case 'p':
                printArray(g, count, stdout); // No new module needed
                break;
            case 'S':
            case 's':
                // Call the search interface module
                searchModule(g, count); 
                break;
            case 'I':
            case 'i':
                // Call the insert word module
                insertModule(g, count); // count will be modified, pass ptr
                break;
            case 'R':
            case 'r':
                // Call the remove word module
                removeModule(g, count); // count will be modified, pass ptr 
                break;
            case 'C':
            case 'c':
                // Return the current count
                countModule(count);
                break;
            case 'Q':
            case 'q':
                quitModule(g, count, outFileName);
                return; // Quit the program
            case 'N':
            case 'n':
                printf("\n\t%s", searchInformation);
                printf("\t%s", insertInformation);
                printf("\t%s", removeInformation);
                printf("\t%s", countInformation);
                printf("\t%s\n", quitInformation);
                break;
            default:
                printf("Invalid input of %c.\n", c);
                break;
       }
    }
}

/*
Display the contents of the array that serves as the grimoire.
*/
void printArray(Grimoire * g, int * count, FILE * stream) {
    int isTerminal = 0;
    if (stream == stdout)
        isTerminal = 1;
    // A more complex version of this is needed for the hash since each bin
    // is individually sorted. Pluck the least top element of each bin. Repeat.
    if (isTerminal) printf("==========PRINT==========\n");
    int tops[HASH] = {0};
    int smallestDic = 0;
    char * smallestWord = "|";
    int currentCount, totalCount;
    currentCount = totalCount = 0;
    for (int i = 0; i < HASH; ++i)
        totalCount += count[i];
    if (isTerminal) printf("\n");
    while (currentCount < totalCount) {
        for (int i = 0; i < HASH; ++i) {
            if (tops[i] >= count[i]) {
                continue;
            }
            if (!charPtrGreaterThanCharPtr((*(*g+i))[tops[i]], smallestWord)) {
                smallestWord = (*(*g+i))[tops[i]];
                smallestDic = i;
            }
        }
        tops[smallestDic] += 1;
        if (isTerminal) printf("\t%s\n", smallestWord);
        else fprintf(stream, "%s\n", smallestWord);
        ++currentCount;
        smallestWord = "|";
    }
    if (isTerminal) printf("\n=========================\n");
}

/*
Wrapper search interface module. It will call search() once input is deemed
valid.
*/
void searchModule(Grimoire * g, int * count) {
    printf("==========SEARCH==========\n");
    for (;;) {
        printf("Enter a word (<Enter> to exit):\t");
        char query[256];
        if (NULL == fgets(query, 256, stdin)) {
            fprintf(stderr, "Unable to receive input. Aborting.\n");
            return;
        }
        if ('\n' == query[0]) { // User hit <Enter>, exit
            printf("==========================\n");
            break;
        }
        // Replace the ending newline with the null-terminating sequence
        for (int i = 0; i < 256; ++i) {
            if ('\n' == query[i]) {
                query[i] = '\0';
                break;
            }
        }
        int numBin = -1;
        numBin = hasher(query);
        int exists = binarySearch(query, *g + numBin, count[numBin]);
        if (-1 != exists)
            printf("\n\t%s found.\n\n", query);
        else
            printf("\n\t%s not found.\n\n", query);
    }
}

/*
Search for a word that will be prompted to the user. Return whether
it has been found. Binary search implementation can be used because
the dictionary is never un-sorted.
*/
int binarySearch(char * word, char *** array, int count) {
    const int NOT_FOUND = -1;
    int first = 0;
    int last = count - 1;
    int mid;
    int relation;
    while (first <= last) {
        mid = (last + first) / 2;
        relation = charPtrGreaterThanCharPtr((*array)[mid], word);
        if (-1 == relation) { // Match
            return mid;
        }
        else if (1 == relation) {
            last = mid - 1;
        }
        else {
            first = mid + 1;
        }
    }
    return NOT_FOUND; // Not found
}

/*
Insert word interface module. It will call insertWord() once input is deemed
valid.
*/
void insertModule(Grimoire * g, int * count) {
    printf("==========INSERT==========\n");
    for (;;) {
        printf("Enter a word (<Enter> to exit):\t");
        char query[256];
        if (NULL == fgets(query, 256, stdin)) {
            fprintf(stderr, "Unable to receive input. Aborting.\n");
            return;
        }
        if ('\n' == query[0]) { // User hit <Enter>, exit
            printf("==========================\n");
            break;
        }
        // Replace the ending newline with the null-terminating sequence
        for (int i = 0; i < 256; ++i) {
            if ('\n' == query[i]) {
                query[i] = '\0';
                break;
            }
        }
        int insertStatus;
        int binNum = hasher(query);
        insertStatus = insertWord(*g + binNum, count + binNum, query);
        if (INSERT_SUCCESS == insertStatus)
            printf("\n\t%s inserted\n\n", query);
        else if (INSERT_DUP_FAIL == insertStatus)
            printf("\n\t%s ignored (duplicate)\n\n", query);
    }
}

/*
Remove word interface module. It will call search() once input is deemed valid.
Valid input has an added condition: the word must already exist in data.
*/
void removeModule(Grimoire * g, int * count) { // Will need to modify count
    printf("==========REMOVE==========\n");
    for (;;) {
        printf("Enter a word(<Enter> to exit):\t");
        char query[256];
        if (NULL == fgets(query, 256, stdin)) {
            fprintf(stderr, "Unable to receive input. Aborting.\n");
            return;
        }
        if ('\n' == query[0]) { // User hit <Enter>, exit
            printf("=========================\n");
            break;
        }
        // Replace the ending newline with the null-terminating sequence
        for (int i = 0; i < 256; ++i) {
            if ('\n' == query[i]) {
                query[i] = '\0';
                break;
            }
        }
        int binNum = hasher(query);
        int location = binarySearch(query, *g + binNum, count[binNum]);
        if (-1 == location) {
            printf("\n\t%s ignored (not found)\n\n", query);
            continue;
        }
        if (removeWord(*g + binNum, location, count[binNum])) {
            count[binNum] -= 1;
            printf("\n\t%s removed\n\n", query);
        }
    }
}

/*
Using the validation to find the word before (already know location), go to
the location and shift all successors down one position. Return 1 if 
successful (then have removeModule decrement count accordingly).
*/
int removeWord(char *** array, int location, int count) {
    // Free the word currently at the location
    free((*array)[location]);
    while (location < (count - 1)) {
        (*array)[location] = (*array)[location + 1];
        ++location;
    }
    (*array)[location] = NULL;
    return 1;
}

/*
Module for the count operation
*/
void countModule(int * count) {
    printf("==========COUNT==========\n");
    int totalSum = 0;
    for (int i = 0; i < HASH; ++i)
        totalSum += count[i];
    printf("\n\t%d\n\n", totalSum);
    printf("=========================\n");
}

/*
Module for the quit program operation
*/
void quitModule(Grimoire * g, int * count, char * outFileName) {
    printf("==========QUIT==========\n");
    printf("The program is exiting. Writing data to %s\n", outFileName);
    if (saveDictionary(g, count, outFileName))
        printf("Quit was successful.\n");
    else
        printf("Quit failed.\n");
}

/*
Perform the file output of writing the dictionary.
*/
int saveDictionary(Grimoire * g, int * count, char * outFileName) {
    FILE *outFile;
    if ((outFile = fopen(outFileName, "w")) == NULL) {
        fprintf(stderr, "Error creating output file, %s\n", outFileName);
        return 0;
    }
    printArray(g, count, outFile);
    fclose(outFile);
    return 1;
}

/*
Called to free memory after processing with the dictionary in current memory is
done.
*/
void freeDictionary(char ** array, int * count) {
    // Go through the array for the count and free each word
    for (int i = 0; i < *count; ++i) {
        free((array)[i]);
    }
}

/*
Caled to free memory of allocating pointers to the grimoire.
*/
void freeGrimoire(Grimoire * g, int * counts) {
    for (int i = 0; i < HASH; ++i) {
        // Call free dictionary for that array and its count
        freeDictionary((*g)[i], counts+i);
        free((*g)[i]);
    }
    // Free the original grimoire itself
    free(*g);
}
