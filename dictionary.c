// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <cs50.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>


#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Choose number of buckets in hash table
// NOTE: MUST be multiple of 26
// 1st multiple = 26; 2nd mult = 676; 3rd = 17576; 4th = 456976; 5th = 11881376; 6th = 308915776
// NOTE: 7th = 8031810176. Doesn't work because max size of unsigned int is 4.3 billion!!
// Best performer is the 5th multiple (time tradeoffs of search vs load,check,etc)
const unsigned int N = 11881376;

// Hash table
node *table[N];

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // hash the word
    unsigned int index = hash(word);

    // iterate through the linked list returning true if strcasecmp finds match
    node *p = table[index];
    while(p != NULL)
    {
        // check if words match
        if(strcasecmp(word, p->word) == 0)
        {
            // match found
            return true;
        }

        // words don't match, so incriment
        p = p->next;
    }

    return false;
}

// Hashes word to a number (between 0 and N-1 inclusive)
unsigned int hash(const char *word)
{
    //take N and divide by 26 (assuming N must be multiple of 26) to get multiple
    int multiple = multiplier(N);

    //while multiplier > 0
    int counter = 0;
    unsigned int hash = 0;
    //continue until end of word or end of multiplier
    while(multiple > 0 && word[counter] != '\0')
    {
        //magnified score
        //check not an apostrophe
        if(word[counter] != '\'')
        {
            hash += (toupper(word[counter]) - 'A') * multiple;

            //decrement mult
            multiple--;
        }

        //incriment counter
        counter++;

    }

    //yield value
    return hash;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    //gives a name of a dictionary to find as a file and then load into memory
    //file open dictionary with that name
    FILE *file = fopen(dictionary, "r");

    //check if there was a problem; if so, return false
    if(file == NULL)
    {
        return false;
    }

    //create hash table from the opened file by:

    //iterate through the dictionary by word (although it looks like one char at a time)
    char c;
    char nWord[LENGTH + 1];
    int count = 0;
    int index = 0;

    while(fread(&c, sizeof(char), 1, file))
    {
        //construct a word one char at a time
        nWord[count] = c;

        //if is complete word
        if(c == '\n')
        {
            //cap the cstring so the hash function knows what the word is (avoid garbage)
            nWord[count] = '\0';

            //reset counter for next word
            count = 0;

            //put word into hash function to get number label of linked list
            index = hash(nWord);

            //add the word to that linked list
                //malloc node
            node *fresh = malloc(sizeof(node));
                //initialize node
            fresh->word[0] = '\0';
            fresh->next = NULL;

                //copy word in
            strcpy(fresh->word, nWord);


            //check if any linked list off of table at index yet
            if(table[index] == NULL)
            {
                //initialize key pointer to a piece of memory
                table[index] = fresh;
            }
            else
            {
                //have the new node point to the first node (a bit of redundancy)
                fresh->next = table[index];

                //then have the node in the array change its pointer to the new node
                table[index] = fresh;
            }
        }
        else
        {
            count++;
        }

    }

    //close the dictionary file
    fclose(file);

    //load complete
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    unsigned int counter = 0;
    node *origin;
    node *destination;
    // iterate through N-1 inclusive of table
    for(size_t i = 0; i < N; i++)
    {
        //check if node's pointer is null
        if(table[i] != NULL)
        {

            //if not, follow pointer until it is null
            origin = table[i]; //0th
            destination = origin->next; //1st
            counter++; //accounts for first word (destination)

            while(destination != NULL)
            {
                //incriment a counter for each pointer traversed beyond the key
                counter++;
                origin = destination;
                destination = origin->next;
            }
        }

    }
    //return the counter number (it will be 0 if dictionary wasn't loaded)
    return counter;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{

    //Free the hash table from memory

    //iterate through table from 0 to N-1 inclusive
    node *p;
    node *temp;
    for(size_t i = 0; i < N; i++)
    {
        //create pointer initialized to table[i] and temp pointer
        p = table[i];

        //follow linked list until end
        while(p != NULL)
        {
            //secure next pointer location in list
            temp = p->next;
            //free current pointer in list
            free(p);
            //reassign to next location in list
            p = temp;
        }

    }

    //FIXME: when would I ever return false?

    return true;
}

int multiplier(int M)
{
    //NOTE: Assumes that N is a multiple of 26 in order to find correct exponent

    int multiple = 1;

    while(M > 26)
    {
        M = M/26;
        multiple++;
    }

    return multiple;
}