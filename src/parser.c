#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "shell.h"

#define TOKEN_POOL_SIZE 8192
static char token_pool[TOKEN_POOL_SIZE];

static int put_char(size_t *used, char c) {
    if (*used >= sizeof(token_pool)-1) return -1;
    token_pool[(*used)++] = c;
    return 0;
}
static int put_text(size_t *used, const char *s) {
    if (!s) return 0;
    while (*s) if (put_char(used,*s++) != 0) return -1;
    return 0;
}
static int var_start(unsigned char c) {
    return c=='_' || (c>='A'&&c<='Z') || (c>='a'&&c<='z');
}
static int var_char(unsigned char c) {
    return var_start(c) || (c>='0'&&c<='9');
}
static int expand_var(const char **cursor,size_t *used) {
    const char *p=*cursor,*start;
    char name[256],status[16];
    const char *value=NULL;
    size_t n;
    ++p;
    if (*p=='?') {
        snprintf(status,sizeof(status),"%d",last_status);
        value=status; ++p;
    } else if (*p=='{') {
        start=++p;
        while (*p && *p!='}') ++p;
        if (*p!='}') {
            if (put_char(used,'$') || put_char(used,'{')) return -1;
            *cursor=start; return 0;
        }
        n=(size_t)(p-start);
        if (!n || n>=sizeof(name)) { ++p; *cursor=p; return 0; }
        memcpy(name,start,n); name[n]='\0';
        value=getenv(name); ++p;
    } else if (var_start((unsigned char)*p)) {
        start=p;
        while (var_char((unsigned char)*p)) ++p;
        n=(size_t)(p-start);
        if (n>=sizeof(name)) return -1;
        memcpy(name,start,n); name[n]='\0';
        value=getenv(name);
    } else {
        if (put_char(used,'$')) return -1;
        *cursor=p; return 0;
    }
    if (put_text(used,value)) return -1;
    *cursor=p; return 0;
}

/* Supported grammar: whitespace, single/double quotes, backslash escapes,
   $NAME, ${NAME}, and $?. Expansion remains in the same argument. */
int parse_input(const char *input,char **args,int max_args) {
    enum { NONE, SINGLE, DOUBLE } quote=NONE;
    size_t used=0;
    int argc=0,started=0;
    const char *p;
    if (!input || !args || max_args<1) return -1;
    for (p=input; *p;) {
        unsigned char c=(unsigned char)*p;
        if (quote==NONE && (c==' '||c=='\t'||c=='\n')) {
            if (started) {
                if (put_char(&used,'\0')) goto overflow;
                started=0;
            }
            ++p; continue;
        }
        if (quote==NONE && !started) {
            if (argc>=max_args-1) goto too_many;
            args[argc++]=token_pool+used;
            started=1;
        }
        if (quote==NONE && c=='\'') { quote=SINGLE; ++p; continue; }
        if (quote==SINGLE && c=='\'') { quote=NONE; ++p; continue; }
        if (quote==NONE && c=='"') { quote=DOUBLE; ++p; continue; }
        if (quote==DOUBLE && c=='"') { quote=NONE; ++p; continue; }
        if (quote!=SINGLE && c=='$') {
            if (expand_var(&p,&used)) goto overflow;
            continue;
        }
        if (quote==NONE && c=='\\') {
            ++p;
            if (*p && put_char(&used,*p++)) goto overflow;
            continue;
        }
        if (quote==DOUBLE && c=='\\') {
            unsigned char next=(unsigned char)p[1];
            if (next=='"'||next=='\\'||next=='$'||next==96) ++p;
            if (put_char(&used,*p++)) goto overflow;
            continue;
        }
        if (put_char(&used,*p++)) goto overflow;
    }
    if (quote!=NONE) {
        fprintf(stderr,"ojc: syntax error: unmatched quote\n");
        return -1;
    }
    if (started && put_char(&used,'\0')) goto overflow;
    args[argc]=NULL;
    return argc;
too_many:
    fprintf(stderr,"ojc: too many arguments (limit: %d)\n",max_args-1);
    return -1;
overflow:
    fprintf(stderr,"ojc: command is too large\n");
    return -1;
}

int split_on_delim(char *input,char **parts,int max_parts) {
    char *p,*start;
    int count=0,escaped=0,single=0,dbl=0;
    if (!input||!parts||max_parts<1) return -1;
    start=input;
    for (p=input;*p;++p) {
        if (escaped) { escaped=0; continue; }
        if (*p=='\\'&&!single) { escaped=1; continue; }
        if (*p=='\''&&!dbl) { single=!single; continue; }
        if (*p=='"'&&!single) { dbl=!dbl; continue; }
        if (*p==';'&&!single&&!dbl) {
            if (count>=max_parts-1) {
                fprintf(stderr,"ojc: too many commands on one line\n");
                return -1;
            }
            *p='\0'; parts[count++]=start; start=p+1;
        }
    }
    if (single||dbl) {
        fprintf(stderr,"ojc: syntax error: unmatched quote\n");
        return -1;
    }
    parts[count++]=start;
    if (count<max_parts) parts[count]=NULL;
    return count;
}
