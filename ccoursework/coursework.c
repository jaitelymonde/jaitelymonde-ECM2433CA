//include packages
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "msString.h"

/* msString = pointer -> memory address that stores long int that is length of string,
and then characters that make up the string*/

//declare task 1 function
void printBytes(void *ptr, int numBytes);

//declare task 2 functions
void reverseString(char *String);
void writeFile(char *name, char *writtenString);

int main(int argc, char **argv)
{
    //task 1 tests
    unsigned char memoryBytes[] = {10, 20, 30, 145, 255};
    void *somePointer = &memoryBytes[0];

    printBytes(somePointer, 4);

    //task 3 tests
    msString ms = msSetString ("Hello");
    msString ms2 = msSetString (" World!");
    msString mscopy = NULL;

    printf(" String |%s| is %ld characters long (%p).\n " ,
        msGetString(ms) , msLength(ms) , ms);
    msCopy(&mscopy, ms);
    printf("Copied string |%s| is %ld characters long (%p).\n " ,
        msGetString ( mscopy ) , msLength ( mscopy ) , mscopy );

    printf ("Compare ms with mscopy : %d \n " , msCompare ( ms , mscopy ));
    printf ("Compare ms with ms2 : %d \n " , msCompare ( ms , ms2 ));
    printf ("Compare ms with Hello : %d \n " , msCompareString ( ms ,"Hello"));
    printf ("Compare ms with HelloX : %d \n " , msCompareString ( ms ,"HelloX"));
    printf ("Compare ms with Hella : %d \n " , msCompareString ( ms ,"Hella"));

    msConcatenate(&mscopy, ms2);
    printf(" Concatenated string |%s| is %ld characters long (%p).\n " ,
        msGetString(mscopy) , msLength(mscopy), mscopy);   


    //task 2 tests
    //initialize file pointer and buffer
    if(argc != 3){
        printf("improper use: %s <inputfile> <outputfile>", argv[0]);
        return 1;
    }

    //open file and buffer
    FILE *fp;
    fp = fopen(argv[1], "r");
    char *buffer;

    //check if pointer is null or not
    if(fp == NULL){
        //error message
        fprintf(stderr, "error %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    if(!(buffer = (char *)malloc(255 * sizeof(char)))){
        fprintf(stderr, "failed to allocate memory\n");
        exit(1);
    }

    //reverse string and write to file
    fgets(buffer, 255, fp);
    reverseString(buffer);
    writeFile(argv[2], buffer);

    fclose(fp);

     

    
    return 0;
}


//task 1
//print bytes and memory address
void printBytes(void *ptr, int numBytes)
{
    //intial memory address
    printf("Starting at memory address %p:\n", ptr);
    unsigned char *bytePtr = (unsigned char *)ptr;

    //loop to print bytes
    int i;
    for (i = 0; i < numBytes; i++) {
        printf("%03d: %d\n", i + 1, *bytePtr);
        bytePtr++;
    }
}

//task 2
//function to reverse string
void reverseString(char *string)
{
    //check if string is null
    if(string == NULL){
        printf("string invalid");
    }
    else{
        //algorithm to swap characters from beginning and end
        int length = strlen(string);
        int middle = length/2;
        char temp;
        int i;
        for(i = 0; i < middle; i++)
        {
            temp = string[i];
            string[i] = string[length - i - 1];
            string[length - i - 1] = temp;
        }
    }
}

//function to write string to file
void writeFile(char *name, char *writtenString)
{
    //create file pointer
    FILE *file_pointer;

    //open file in write
    file_pointer = fopen(name, "w");

    // check if file is created properly
    if (file_pointer == NULL) {
        printf("Unable to open file for read access.\n");
        fprintf(stderr, "error %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    //write string to file
    fprintf(file_pointer, "%s\n", writtenString);

    //close file
    fclose(file_pointer);
}


//task 3
//data structure to allocate memory contiguously
struct msData
{
    long int length;
    char *characters;
};

//convert string -> msString
extern msString msSetString(char *str)
{
    struct msData *data1;
    //trap fact that memory was not allocated
    if (!(data1 = (struct msData *)malloc(sizeof(struct msData))) ){
        printf("Out of memory\n");
    }

    //set length and char variables of struct
    data1->length = strlen(str);
    //trap fact that memory was not allocated
    if (!(data1->characters = (char *)malloc(data1->length + 1))) {
        free(data1);
        msError(str);
    }

    //copy string to struct
    strcpy(data1->characters, str);
    //point msString variable to struct
    msString ms = data1;

    //return msString
    return ms;
}

//convert msString -> string
extern char* msGetString(msString ms)
{
    //check if msString is null
    if (ms == NULL) {
        printf("msString pointer is null\n");
        return NULL;
    }

    //retrieve string from struct cast
    struct msData *msPointer = (struct msData *)ms;
    char *finalString = msPointer->characters;

    //return string
    return finalString;
}

//copy msString to pointer
extern void msCopy(msString *destination, msString source)
{
    //allocate memory if destination is null
    if (*destination == NULL) {
        *destination = malloc(sizeof(struct msData));
    }

    //cast pointers
    struct msData *destData = (struct msData *)*destination;
    struct msData *sourceData = (struct msData *)source;

    //allocate memory
    destData->characters = malloc((sourceData->length + 1) * sizeof(char));
    if (destData->characters == NULL) {
        printf("memory allocation failed\n");
        free(*destination);
        *destination = NULL;
        return;
    }

    //copy length to destination
    destData->length = sourceData->length;

    //copy chars to destination
    strcpy(destData->characters, sourceData->characters);
}

//concatenates characters and length of msString onto destination msString
extern void msConcatenate(msString *destination, msString source)
{
    //allocate memory if destination is null
    if (*destination == NULL) {
        *destination = malloc(sizeof(struct msData));
    }

    //cast pointers
    struct msData *destData = (struct msData *)*destination;
    struct msData *sourceData = (struct msData *)source;

    //set new length after concatenation
    long int newLength = destData->length + sourceData->length;

    //reallocate memory for destination
    destData->characters = realloc(destData->characters, (newLength + 1) * sizeof(char));
    if (destData->characters == NULL) {
        printf("memory reallocation failed\n");
        return;
    }

    //set new length
    destData->length = newLength;

    //concatenate source and destination strings
    strcat(destData->characters, sourceData->characters);
}

//returns long int value of length of chars in string of msString
long int msLength(msString ms)
{
    struct msData *msPointer = (struct msData *)ms;
    long int finalLength = msPointer->length;
    return finalLength;
}

//compare two msStrings to see if values are equal
extern int msCompare(msString ms1, msString ms2)
{
    //cast msStrings to get data
    struct msData *msPointer1 = (struct msData *)ms1;
    struct msData *msPointer2 = (struct msData *)ms2;

    //retreive string
    char *ms1String = msPointer1->characters;
    char *ms2String = msPointer2->characters;

    //retreive length
    long int ms1Length = msPointer1->length;
    long int ms2Length = msPointer2->length;

    //compare vals
    if(strcmp(ms1String, ms2String) == 0 && ms1Length == ms2Length){
        return 0; //return 0 if equal msStrings
    } else{
        return 1; //return 1 if unequal msStrings
    }
}

//compares an msString with standard string to check if chars match
extern int msCompareString(msString ms, char *str)
{
    //get the string from msString
    char *msChars = msGetString(ms);
    char *mStr;

    //trap memory if not allocated
    if (!(mStr = (char *)malloc(strlen(str) + 1))) {
        free(mStr);
        msError(str);
    }

    //intialize comparison value with strcmp
    int compareVal = strcmp(msChars, str);

    //check compare val
    if(compareVal == 0){ //equal strings
        return 0;
    } else{ //not equal strings
        return 1;
    }
}   

//outputs standard string input as an error
static void msError(char *str)
{
    printf("%s is causing an error\n",str);
    fprintf(stderr, "error %d: %s\n", errno, strerror(errno));
}