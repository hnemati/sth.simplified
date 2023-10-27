/*
 * this file parses the file to be tested to check for dependencies
 * 
 * NOTE: this could have been implemented in sed, awk, perl or python too.
 * But we wanted to minimize toolchain dependecies hence did it in C
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAGIC_NEEDS "@NEED"
#define MAGIC_OPTION "@OPTION"


char *trim(char *src) 
{
    char *end;
    
    // trim start
    while( *src != '\0' && !isalnum(*src))
        src++;    
    
    // trim end
    end = src + strlen(src) - 1;    
    while( !isalnum(*end))
        *end-- = '\0';           
    
    return src;
    
}

char *find(char *src, char *what)
{
    char *tmp = strstr(src, what);
    char *end;
    if(!tmp) return 0;
    
    tmp += strlen(what);
        
    tmp = trim(tmp);           
    return (*tmp == '\0')  ? 0 : tmp;

}

int print(char **files, int count, char *name)
{
    int i, cnt = 0;
    for(i = 0; i < count; i++) {
        if( strstr(files[i], name)) {
            cnt++;
            printf(" %s", files[i]);
        }
    }
    return cnt;
}

// --------------------------------------------

void insert_files(char *line, int filec, char **filev)
{
    char *next;
    do {
        next = strchr(line, ',');                        
        if(next) *next++ = '\0';
        line = trim(line);
        if( *line == '\0') continue; /* what?? */
        
        if(!print(filev, filec, line)) {
            printf("  ERROR-unable-to-find-%s_", line);
            fprintf(stderr, "ERROR: unable to find '%s'!\n", line);
            exit(20);
        }
        
        if(next) line = next;
    } while(next);
}   


void insert_option(char *line)
{
    char *next;
    do {
        next = strchr(line, ',');                        
        if(next) *next++ = '\0';
        line = trim(line);
        if( *line == '\0') continue; /* what?? */
        
        printf(" -%s", line);
        if(next) line = next;
    } while(next);    
}

// --------------------------------------------
int main(int argc, char **argv) 
{
    
    FILE *fp;
    char line[1024], *tmp;
    
    if(argc > 2) {
                
        FILE *fp = fopen(argv[1], "r");
        if(fp) {
            
            while(fgets(line, 1024-1, fp)) {
                line[1024-1] = '\0';
                
                /* file dependency */
                tmp = find(line, MAGIC_NEEDS);                               
                if(tmp) insert_files(tmp, argc -2, argv + 2);
                
                /* compiler options */ 
                tmp = find(line, MAGIC_OPTION);                               
                if(tmp) insert_option(tmp);      
                
            }
            fclose(fp);
        } else {
            fprintf(stderr, "ERROR: enable to open '%s'\n", argv[1]);
            return 20;
        }    
        
        printf(" %s", argv[1]);    
        
    } else {
        fprintf(stderr, "ERROR: incorrect input\n");;        
        return 20;
    }
    
    
    return 0;
}
