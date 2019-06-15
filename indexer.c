#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
//global declarations
FILE *file;
//structure for page
struct PageList {
    char *fileName;
    char *title;
    char *desc;
    int count;
    struct PageList *next;
};
struct WordList {
    char *word;
    int count;
    struct WordList *next;
    struct PageList *page;
};
struct Hash {
    struct WordList *wordHashHead;
}hash[123];
// forward declarations
void countWords(char*, int);
void putInList(char*, int);
//void checkCopy(char*);
void insertNewPageNode(int, int);
char* convertDataInStr(char *fileName) {
 
    struct stat st;
    stat(fileName, &st);
    int fileSize = st.st_size, i = 0;
    char *fileContent = (char*)calloc(fileSize, sizeof(char)), ch;
    file = fopen(fileName, "r");
    ch = getc(file);
    while(ch != EOF) {
        fileContent[i] = ch;
        ch = getc(file);
        i++;
    } 
    fileContent[i] = '\0';
    fclose(file);
    return fileContent;
}

void findTitle(char *fileData, int fileNo) {

    char *titleStart, *titleEnd, *subStr;
    int noOfChars;
    for(int i = 0;i<strlen(fileData);i++) {
        if(fileData[i] == '<' && fileData[i+1] == 't') {
            titleStart = &fileData[i];
            titleStart = strpbrk(titleStart, "\'\">");
            titleStart++;
            break;
        }
    }
    titleEnd = strpbrk(titleStart, "<");
    noOfChars = (titleEnd-titleStart);
    subStr = (char*)calloc(noOfChars, sizeof(char));
    memcpy(subStr, titleStart, noOfChars);
    *(subStr + noOfChars) = '\0';
    countWords(subStr, fileNo);
    free(subStr);
}
void findMeta(char *fileData, int fileNo) {
    char *metaStart, *metaEnd, *subStr;
    int noOfChars;
    for(int i = 0;i<strlen(fileData);i++) {
        if(fileData[i] == '<' && fileData[i+1] == 'm') {
            metaStart = (strpbrk(&fileData[i], "\"\'") + 1);
            if(metaStart) {
                metaEnd = strpbrk(metaStart, "/>");
                if(*metaStart == 'd' || *metaStart == 'a' || *metaStart == 'k') {
                    metaStart = strpbrk(metaStart, "=");
                    metaStart += 2;
                    noOfChars = (metaEnd-metaStart);
                    subStr = (char*)calloc(noOfChars, sizeof(char));
                    memcpy(subStr, metaStart, noOfChars);
                    countWords(subStr, fileNo);
                    free(subStr);
                }
            }
        }
    }
  
}
void findHeadings(char *fileData, int fileNo) {
    char *headingStart, *headingEnd, *subStr;
    int noOfChars;
    for(int i = 0;i<strlen(fileData);i++) {
        if(fileData[i] == '<' && fileData[i+1] == 'h') {
            headingStart = &fileData[i];
            headingStart = strpbrk(headingStart, ">");
            headingStart++;
            headingEnd = strpbrk(headingStart, "<");
            noOfChars = (headingEnd-headingStart);
            subStr = (char*)calloc(noOfChars, sizeof(char));
            memcpy(subStr, headingStart, noOfChars);
            *(subStr + noOfChars) = '\0';
            countWords(subStr, fileNo);
            free(subStr);
        }
    }
}
void countWords(char *withCompare, int fileNo) {
    int inWord = 0, startIndex, endIndex, noOfChars;
    char *word;
    for(int i = 0;i<=strlen(withCompare);i++ ) {
        if(withCompare[i] < 64 || withCompare[i] == '\0') {
            if(inWord == 1) {
                endIndex = i;
                noOfChars = (endIndex - startIndex);
                word = (char*)calloc(noOfChars, sizeof(char));
                memcpy(word, &withCompare[startIndex], noOfChars);
                *(word + noOfChars) = '\0';
                if(strlen(word) > 2)
                    putInList(word, fileNo);
                free(word);
            }
            inWord = 0;
        } else {
            if(inWord == 0) {
                startIndex = i;
            }
            inWord = 1;
        }
    }
}
void putInList(char *word, int i) {
    int key = (int)word[0];
        char name[10];
        sprintf(name, "%d", i);
        strcat(name, ".txt");
    if(hash[key].wordHashHead == 0) {
        hash[key].wordHashHead = (struct WordList*)calloc(1, sizeof(struct WordList));
        hash[key].wordHashHead->word = (char*) calloc(strlen(word), sizeof(char));
        hash[key].wordHashHead->page = (struct PageList*) calloc(1, sizeof(struct PageList));
        hash[key].wordHashHead->page->count = 1;
        hash[key].wordHashHead->page->fileName = (char*) calloc(7, sizeof(char));
        strcpy(hash[key].wordHashHead->page->fileName, name);
        strcpy(hash[key].wordHashHead->word, word);
        hash[key].wordHashHead->next = 0;
    } else {
        struct WordList *ptr = hash[key].wordHashHead;
        while(ptr != 0) {
            if(!strcmp(ptr->word, word)) {
                ptr->count++;
                if(((ptr->page->fileName[0]) - '0') == i) {
                    ptr->page->count++;
                    return;
                }
                else {
                    struct PageList *ptr1 = ptr->page;
                    while(ptr1 != 0) {
                        if(((ptr1->fileName[0]) - '0') == i) {
                            ptr1->count++;
                            return ;
                        }
                        ptr1 = ptr1->next;
                    }
                    ptr1 = (struct PageList*) calloc(1, sizeof(struct PageList));
                    ptr1->fileName = (char*) calloc(7, sizeof(char));
                    strcpy(ptr1->fileName, name);
                    ptr1->count = 1;
                    return;
                }
            }
            ptr = ptr->next;
        }
        ptr = (struct WordList*)calloc(1 ,sizeof(struct WordList));
        ptr->word = (char*) calloc(strlen(word), sizeof(char));
        ptr->next = 0;
        ptr->page = (struct PageList*) calloc(1, sizeof(struct PageList));
        ptr->count = 1;
        ptr->page->count = 1;
        ptr->page->fileName = (char*) calloc(7, sizeof(char));
        strcpy(ptr->page->fileName, name);
        strcpy(ptr->word, word);
    }
}
void travList() {
    for(int i = 0;i<123;i++) {
        struct WordList *ptr = hash[i].wordHashHead;
        while(ptr != 0) {
            printf("%s    %d\n", ptr->word,ptr->count);
            ptr = ptr->next;
        }
    }
}
void putInFile() {

    FILE *newFile = fopen("content.txt" ,"w");
    for(int i = 0;i<123;i++) {
       struct WordList *ptr = hash[i].wordHashHead;
       while(ptr != 0) {
           printf("%s    %d\n", ptr->word,ptr->count);
           fprintf(newFile, "Word: %s     ", ptr->word);
           fprintf(newFile, "Total Count: %d\n", ptr->count);
           while(ptr->page != 0) {
                printf("Hello there\n");
                fprintf(newFile, "Found in: %s\n", ptr->page->fileName);
                fprintf(newFile, "Count is: %d\n", ptr->page->count);
                ptr->page = ptr->page->next;
           }
           ptr = ptr->next;
       }
    }

}
int main() {
    char fileName[10];
    struct stat st;
    int fileSize;
    for(int i = 1;i<5;i++) {
        sprintf(fileName, "%d", i);
        strcat(fileName, ".txt");
        stat(fileName, &st);
        fileSize = st.st_size;
        char *fileData;
        fileData = convertDataInStr(fileName);
        findTitle(fileData, i);
        findMeta(fileData, i);
        findHeadings(fileData, i);
        free(fileData);
    }
    travList();
    putInFile();
}
