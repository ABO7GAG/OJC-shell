#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser.h"

int last_status=7;

int main(void) {
    char *args[16];
    char line[]="echo one; echo 'two;three'; echo four\\;five";
    char *parts[8];
    int n;
    if (setenv("OJ_TEST_VAR","two words",1)) return 1;
    n=parse_input("echo 'a b' \"$OJ_TEST_VAR\" '$OJ_TEST_VAR' ${OJ_TEST_VAR} $?",args,16);
    if (n!=6 || strcmp(args[0],"echo") || strcmp(args[1],"a b") ||
        strcmp(args[2],"two words") || strcmp(args[3],"$OJ_TEST_VAR") ||
        strcmp(args[4],"two words") || strcmp(args[5],"7")) return 2;
    if (parse_input("echo \"unfinished",args,16)!=-1) return 3;
    if (parse_input("echo one two three",args,3)!=-1) return 4;
    n=split_on_delim(line,parts,8);
    if (n!=3 || strcmp(parts[0],"echo one") ||
        strcmp(parts[1]," echo 'two;three'") ||
        strcmp(parts[2]," echo four\\;five")) return 5;
    puts("OJC parser tests passed.");
    return 0;
}
