#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
//#include <html.h>

#define BASE_URL "www.chitkara.edu.in"
#define urlLength 1000
char* returnSubUrl(char *url) {
    int i = 0;
    char *subUrl = (char*)malloc(sizeof(char) * urlLength);
    subUrl = url;
    while(url[i] != '/') {
        if(url[i] == '\0')
            break;
        i++;
    }
    subUrl[i] = '\0';
    return subUrl;
}
int checkArgueCount(int argc) {
    if(argc == 4) 
        return 1;
    printf("Arguement are not passed as specified");
    exit(0);
}
int checkDepth(char *depth) {
    int depthInInt;
    sscanf(depth, "%d", &depthInInt);
    if(depthInInt < 0) {
        printf("Depth is not allowed in negative");
        exit(0);
    } else if(depthInInt > 4) {
        printf("That much of deep depth is not allowed");
    }
    return 1;
}
int checkUrl(char *url) {
    char cmnd[urlLength] = "wget --spider ";
    strcat(cmnd, url);
    if((!strcmp(returnSubUrl(url), BASE_URL)) && !system(cmnd))
        printf("Valid Url");
    else {
        printf("Invalid Url");
        exit(0);
    }
}
void checkDir(char *dir) {

    struct stat dirInfo;
    if(stat(dir, &dirInfo) == -1) { 
    // this function will return -1 if the dir get not found and also this fnc will store all the dir information in the dirInfo structure
        printf("-----------------\n");
        printf("Invalid Directroy\n");  
        printf("-----------------\n");
        exit(1);
    }
    if(!S_ISDIR(dirInfo.st_mode)) {
    // this function will check that the inputed string is a directory or not
        printf("----------------------------\n");
        printf("You input is not a directory\n");
        printf("----------------------------\n");
        exit(1);
    }
    if((dirInfo.st_mode & S_IWUSR) != S_IWUSR) {
    //This function will check that wheather the dir is in writeable mode or not
        printf("-------------------------------------------\n");
        printf("Your inputed dir is not having write access\n");
        printf("-------------------------------------------\n");
        exit(1);
    }
}
void check(int argc, char *argv[]) {
    if(checkArgueCount(argc)) {
        if(checkDepth(argv[2])) {
            if(checkUrl(argv[1])) {
                checkDir(argv[3]);
                    printf("\nAll things are correct\n");
            }
        }
    }
}
void getPage(char *url, char *dir) {
    char urlBuffer[urlLength + 300] = {0};
    strcat(urlBuffer, "wget -O ");
    strcat(dir, "htmlIntxt.txt ");
    strcat(urlBuffer, dir);
    strcat(urlBuffer, url);
    system(urlBuffer);
}
void transferFile() {
    static int file_no = 1;
    char fileName[10], ch;
    sprintf(fileName, "%d", file_no);
    strcat(fileName, ".txt");
    FILE *oldFile = fopen("htmlIntxt.txt", "r");
    FILE *newFile = fopen(fileName, "w");
    ch = getc(oldFile);
    while(ch != EOF) {
        putc(ch, newFile);
        ch = getc(oldFile);
    }
    file_no++;
    printf("New file created\n");
    fclose(oldFile);
    fclose(newFile);
}
void removeWhiteSpace(char* html) {
  int i;
  char *buffer = malloc(strlen(html)+1), *p=malloc (sizeof(char)+1);
  memset(buffer,0,strlen(html)+1);
  for (i=0;html[i];i++) 
  {
    if(html[i]>32)
    {
      sprintf(p,"%c",html[i]);
      strcat(buffer,p);
    }
  }
  strcpy(html,buffer);
  free(buffer); free(p);
}

int GetNextURL(char* html, char* urlofthispage, char* result, int pos) {
  char c;
  int len, i, j;
  char* linkStart;  //!< pointer pointed to the start of a new-founded URL.
  char* linkEnd;  //!< pointer pointed to the end of a new-founded URL.

  // Clean up \n chars
  if(pos == 0) {
    removeWhiteSpace(html);
  }
  // Find the <a> <A> HTML tag.
  while (0 != (c = html[pos])) {
    if ((c=='<') && ((html[pos+1] == 'a') || (html[pos+1] == 'A'))) {
      break;
    }
    pos++;
  }
  //! Find the URL it the HTML tag. They usually look like <a href="www.abc.com">
  //! We try to find the quote mark in order to find the URL inside the quote mark.
  if (c) {  
    // check for equals first... some HTML tags don't have quotes...or use single quotes instead
    //Here we are finding = because the link will start after it
    linkStart = strchr(&(html[pos+1]), '=');
    
    if ((!linkStart) || (*(linkStart-1) == 'e') || ((linkStart - html - pos) > 10)) 
    {
      // keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    //this condition is checking double or single quotes
    if (*(linkStart+1) == '\"' || *(linkStart+1) == '\'')
      linkStart++;
    //so the link will be after quotes that's why we increamented by one
    linkStart++;    
    //strpbrk will find the occurence of character from second string in first string
    linkEnd = strpbrk(linkStart, "\'\">");
    //with the help of this we will get the end of the link
    if(!linkEnd) {
      // keep going... if we didn't find the end of the link
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (*linkStart == '#') { 
        // Why bother returning anything here....recursively keep going...
      return GetNextURL(html,urlofthispage,result,pos+1);
    }
    if (!strncmp(linkStart, "mailto:",7))
      return GetNextURL(html, urlofthispage, result, pos+1);
    if (!strncmp(linkStart, "http", 4) || !strncmp(linkStart, "HTTP", 4)) {
      //! Nice! The URL we found is in absolute path.
      strncpy(result, linkStart, (linkEnd-linkStart));
      return  (int)(linkEnd - html + 1);
    } else {
      //! We find a URL. HTML is a terrible standard. So there are many ways to present a URL.
      if (linkStart[0] == '.') {
        //! Some URLs are like <a href="../../../a.txt"> I cannot handle this. 
    	// again...probably good to recursively keep going..    
        return GetNextURL(html,urlofthispage,result,pos+1);
      }
      if (linkStart[0] == '/') {
        //! this means the URL is the relative path
        for (i = 7; i < strlen(urlofthispage); i++)
          if (urlofthispage[i] == '/')
            break;
        strcpy(result, urlofthispage);
        result[i] = 0;
        strncat(result, linkStart, (linkEnd - linkStart));
        return (int)(linkEnd - html + 1);        
      } else {
        //! the URL is a absolute path.
        len = strlen(urlofthispage);
        for (i = (len - 1); i >= 0; i--)
          if (urlofthispage[i] == '/')
            break;
        for (j = (len - 1); j >= 0; j--)
          if (urlofthispage[j] == '.')
              break;
        if (i == (len -1)) {
          //! urlofthis page is like http://www.abc.com/
            strcpy(result, urlofthispage);
            result[i + 1] = 0;
            strncat(result, linkStart, linkEnd - linkStart);
            return (int)(linkEnd - html + 1);
        }
        if ((i <= 6) || (i > j)) {
          //! urlofthis page is like http://www.abc.com/~xyz
          //! or http://www.abc.com
          strcpy(result, urlofthispage);
          result[len] = '/';
          strncat(result, linkStart, linkEnd - linkStart);
          return (int)(linkEnd - html + 1);
        }
        strcpy(result, urlofthispage);
        result[i + 1] = 0;
        strncat(result, linkStart, linkEnd - linkStart);
        return (int)(linkEnd - html + 1);
      }
    }
  }    
  return -1;
}
char* convertDataInStr(char *fileName) {
 
    struct stat st;
    stat(fileName, &st);
    int fileSize = st.st_size, i = 0;
    char *fileContent = (char*)malloc(sizeof(char) * fileSize), ch;
    FILE *file = fopen(fileName, "r");
    ch = getc(file);
    while(ch != EOF) {
        fileContent[i] = ch;
        ch = getc(file);
        i++;
    }
    fileContent[i] = '\0';
//    fclose(file);
    return fileContent;
}
int main(int argc, char* argv[]) {

    check(argc, argv);
    getPage(argv[1], argv[3]);
    //transferFile();
    char *fileContent = convertDataInStr("htmlIntxt.txt");
    int pos = 0;
    char **links = (char**) malloc(sizeof(char*) * 100);
    for(int i = 0;i<100;i++) {
        links[i] = (char*)malloc(sizeof(char)*100);
        pos = GetNextURL(fileContent, argv[1], links[i], pos);
    }
    int i = 0;
    while(i<100) {
        printf("%s\n", links[i]);
        i++;
    }
    return 0;
}
