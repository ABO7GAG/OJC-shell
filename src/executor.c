#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"
#include "parser.h"
#include "executor.h"
#include "builtins.h"

#define MAX_ARGS 64
#define MAX_COMMANDS 32

static int wait_child(pid_t pid,int *status) {
    pid_t result;
    do { result=waitpid(pid,status,0); } while (result<0 && errno==EINTR);
    return result<0 ? -1 : 0;
}

static void child_exec(char **args,int in_fd,int out_fd,
                       const char *in_file,const char *out_file,int append) {
    int fd;
    signal(SIGINT,SIG_DFL);
    signal(SIGQUIT,SIG_DFL);
    if (in_fd>=0 && dup2(in_fd,STDIN_FILENO)<0) { perror("ojc: dup2"); _exit(126); }
    if (out_fd>=0 && dup2(out_fd,STDOUT_FILENO)<0) { perror("ojc: dup2"); _exit(126); }
    if (in_file) {
        fd=open(in_file,O_RDONLY);
        if (fd<0) { perror(in_file); _exit(1); }
        if (dup2(fd,STDIN_FILENO)<0) { perror("ojc: dup2"); close(fd); _exit(126); }
        close(fd);
    }
    if (out_file) {
        fd=open(out_file,O_WRONLY|O_CREAT|(append?O_APPEND:O_TRUNC),0664);
        if (fd<0) { perror(out_file); _exit(1); }
        if (dup2(fd,STDOUT_FILENO)<0) { perror("ojc: dup2"); close(fd); _exit(126); }
        close(fd);
    }
    if (in_fd>=0) close(in_fd);
    if (out_fd>=0) close(out_fd);
    if (run_builtin(args)) { fflush(NULL); _exit(last_status); }
    execvp(args[0],args);
    fprintf(stderr,"ojc: %s: %s\n",args[0],strerror(errno));
    _exit(errno==ENOENT?127:126);
}

static void run_pipeline(char **commands[],int count,
                         const char *in_file,const char *out_file,int append) {
    pid_t pids[MAX_COMMANDS];
    int started=0,previous=-1,last_status_raw=0,wait_error=0,i;

    for (i=0;i<count;++i) {
        int fds[2]={-1,-1},out=-1;
        pid_t pid;
        if (i<count-1 && pipe(fds)<0) { perror("ojc: pipe"); goto fail; }
        if (i<count-1) out=fds[1];
        pid=fork();
        if (pid<0) {
            perror("ojc: fork");
            if (fds[0]>=0) close(fds[0]);
            if (fds[1]>=0) close(fds[1]);
            goto fail;
        }
        if (pid==0) {
            if (fds[0]>=0) close(fds[0]);
            child_exec(commands[i],previous,out,
                       i==0?in_file:NULL,
                       i==count-1?out_file:NULL,append);
        }
        pids[started++]=pid;
        if (previous>=0) close(previous);
        if (out>=0) close(out);
        previous=fds[0];
    }
    if (previous>=0) close(previous);
    for (i=0;i<started;++i) {
        int status;
        if (wait_child(pids[i],&status)<0) { perror("ojc: waitpid"); wait_error=1; }
        else if (i==started-1) last_status_raw=status;
    }
    if (wait_error) last_status=1;
    else if (WIFEXITED(last_status_raw)) last_status=WEXITSTATUS(last_status_raw);
    else if (WIFSIGNALED(last_status_raw)) last_status=128+WTERMSIG(last_status_raw);
    else last_status=1;
    return;

fail:
    if (previous>=0) close(previous);
    for (i=0;i<started;++i) (void)kill(pids[i],SIGTERM);
    for (i=0;i<started;++i) { int status; (void)wait_child(pids[i],&status); }
    last_status=1;
}

void run_command(char *command) {
    char *args[MAX_ARGS], **commands[MAX_COMMANDS];
    const char *in_file=NULL,*out_file=NULL;
    int append=0,argc,count=1,i;
    struct timespec begin,end;

    argc=parse_input(command,args,MAX_ARGS);
    if (argc<0) { last_status=2; return; }
    if (!argc || !args[0] || !*args[0]) return;

    for (i=0;i<argc;++i) {
        if (!args[i]) continue;
        if (!strcmp(args[i],"|")) {
            if (count>=MAX_COMMANDS || !args[i+1] || !*args[i+1]) {
                fprintf(stderr,"ojc: invalid pipeline\n"); last_status=2; return;
            }
            /* Pipe tokens are split in the command-vector pass below. */
        } else if (!strcmp(args[i],"<")) {
            if (in_file || !args[i+1] || !*args[i+1]) {
                fprintf(stderr,"ojc: invalid input redirection\n"); last_status=2; return;
            }
            in_file=args[i+1]; args[i]=NULL; args[i+1]=NULL; ++i;
        } else if (!strcmp(args[i],">") || !strcmp(args[i],">>")) {
            if (out_file || !args[i+1] || !*args[i+1]) {
                fprintf(stderr,"ojc: invalid output redirection\n"); last_status=2; return;
            }
            append=!strcmp(args[i],">>");
            out_file=args[i+1]; args[i]=NULL; args[i+1]=NULL; ++i;
            if (args[i+1]) {
                fprintf(stderr,"ojc: unexpected token after redirection\n"); last_status=2; return;
            }
        }
    }

    /* Rebuild command vector starts after operators have been nulled. */
    count=0;
    commands[count++]=args;
    for (i=0;i<argc;++i) {
        if (args[i] && !strcmp(args[i],"|")) {
            args[i]=NULL;
            if (count>=MAX_COMMANDS || !args[i+1]) {
                fprintf(stderr,"ojc: invalid pipeline\n"); last_status=2; return;
            }
            commands[count++]=&args[i+1];
        }
    }
    for (i=0;i<count;++i) {
        if (!commands[i][0]) { fprintf(stderr,"ojc: empty pipeline command\n"); last_status=2; return; }
    }

    if (count==1 && !in_file && !out_file && run_builtin(args)) return;
    {
        int timed=clock_gettime(CLOCK_MONOTONIC,&begin)==0;
        run_pipeline(commands,count,in_file,out_file,append);
        if (timed && clock_gettime(CLOCK_MONOTONIC,&end)==0) {
            double elapsed=(end.tv_sec-begin.tv_sec)+(end.tv_nsec-begin.tv_nsec)/1e9;
            snprintf(elapsed_str,sizeof(elapsed_str),"%.3fs",elapsed);
        }
    }
}
