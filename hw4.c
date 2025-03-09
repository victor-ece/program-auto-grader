#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>

/*
 * To "handle" the signal from the timer
 * Parameters: The signal
 * Returns: Nothing
*/
static void myhandler(int sig) {}

/*
 * Prints the score of the program
 * Parameters: The score for the compilation, termination, output and memory access
 * Returns: Nothing
*/
void scorePrint(int compilationScore, int terminationScore, int outputScore, int memoryScore)
{
    int maxScore, totalScores;

    totalScores = compilationScore + terminationScore + outputScore + memoryScore;
    if (totalScores < 0) {
        maxScore = 0;
    } else {
        maxScore = totalScores;
    }

    printf("\nCompilation: %d\n\nTermination: %d\n\nOutput: %d\n\nMemory access: %d\n\nScore: %d\n"
    ,compilationScore, terminationScore, outputScore, memoryScore, maxScore);
}

/* argv[1] = progname.c , argv[2] = progname.args , argv[3] = progname.in , argv[4] = progname.out
 * argv[5] = timeout value */
int main (int argc, char *argv[])
{
    int i, pid, pid2, prognameLength, errorfileDescriptor, returnValue, status = 0, 
    pipeDescriptors[2], prognameInFd, compilationScore = 0, terminationScore = 0, outputScore = 0,
    memoryScore = 0;
    char *progname, *executableName, *errorFileName, *arguementString = NULL, *tempWord,
    **args = NULL, *errorFileLine = NULL;
    FILE *argsFile, *errorFile;
    size_t cap = 0;

    if(argc != 6) {
        fprintf(stderr ,"Wrong amount of arguements.\n");
        return(1);
    }

	struct sigaction action = {{0}};
	struct itimerval t = {{0}}; 
	
    action.sa_handler = myhandler;
	sigaction(SIGALRM, &action, NULL);
    t.it_value.tv_sec = atoi(argv[5]);
	t.it_value.tv_usec = 0;

    // Gets the programs name and creates the names for the errorsfile and the executable
    prognameLength = strlen(argv[1]) - 2; // -2 to exclude '.c'. '\0' not included in the length
    progname = malloc(sizeof(char)*(prognameLength + 1));
    for (i=0; i < prognameLength; i++) {
        progname[i] = argv[1][i];
    }
    progname[i] = '\0';

    executableName = malloc(sizeof(char)*(prognameLength + 3));
    executableName[0] = '.';
    executableName[1] = '/';
    for (i=0; i < prognameLength; i++) {
        executableName[i + 2] = argv[1][i];
    }
    executableName[i + 2] = '\0';

    errorFileName = malloc(sizeof(char)*(prognameLength + 5));
    strcpy(errorFileName, progname);
    errorFileName[prognameLength] = '.';
    errorFileName[prognameLength + 1] = 'e';
    errorFileName[prognameLength + 2] = 'r';
    errorFileName[prognameLength + 3] = 'r';
    errorFileName[prognameLength + 4] = '\0';

    // P1 PROCESS
    pid = fork();
    if(pid == -1) {
        perror("Fork for P1 failed.\n");
        free(progname);
        free(executableName);
        free(errorFileName);
        return(255);
    }

    if (pid == 0) {
        errorfileDescriptor = open(errorFileName, O_RDWR | O_CREAT | O_TRUNC, 0644);
        if (errorfileDescriptor == -1) {
            free(errorFileName);
            perror("Opening error file failed.\n");
            exit(255);
        }

        returnValue = dup2(errorfileDescriptor, STDERR_FILENO);
        if (returnValue == -1) {
            perror("Error with dup2 of errorfileDescriptor to STDERR_FILENO.\n");
            close(errorfileDescriptor);
            exit(255);
        }

        returnValue = execl("/usr/bin/gcc", "gcc", "-Wall", argv[1], "-o", progname, NULL);
        if(returnValue == - 1) {
            perror("Exec of progname failed.\n");
            exit(255);
        }
    }    
    else {
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 255) {
                free(progname);
                free(executableName);
                free(errorFileName);
                return(255);
            } 
        } 
        else {
            perror("The child didn't terminate normally.");
            return(255);
        }   

        errorFile = fopen(errorFileName, "r");
        if (errorFile == NULL) {
            perror("Error opening arguements file");
            return(255);
        }
        free(errorFileName);

        returnValue = 0;
        while (returnValue != -1) {
            returnValue = getline(&errorFileLine, &cap, errorFile);
            if(returnValue != -1) {
                // to check whether that's the first line of a warning or error message
                if (!strncmp(errorFileLine, argv[1], prognameLength + 2)) {
                    // 1st strtok is used to read the first string of words and second to detect
                    tempWord = strtok(errorFileLine, " ");
                    tempWord = strtok(NULL, " ");

                    if(!strcmp(tempWord, "error:")) {
                        compilationScore = -100;
                        scorePrint(compilationScore, 0, 0, 0);
                        free(executableName);
                        free(progname);
                        free(errorFileLine);
                        return(255);
                    } 
                    else if(!strcmp(tempWord, "warning:")) {
                        compilationScore = compilationScore - 5;
                    }
                }

                free(errorFileLine);
                errorFileLine = NULL;
            }
        }
        free(errorFileLine);

        returnValue = pipe(pipeDescriptors);
        if(returnValue == -1) {
            perror("Pipe failed.\n");
            return(255);
        }

        // P2 PROCESS
        pid = fork();
        if(pid == -1) {
            perror("Fork for P2 failed.\n");
            free(progname);
            free(executableName);
            close(pipeDescriptors[0]);
            close(pipeDescriptors[1]);
            return(255);
        }

        if(pid == 0) {
            // p2 code
            close(pipeDescriptors[0]);

            prognameInFd = open(argv[3], O_RDWR | O_CREAT, 0644);
            if (prognameInFd == -1) {
                perror("Opening progname.in file failed.\n");
                exit(255);
            }

            returnValue = dup2(prognameInFd, STDIN_FILENO);
            if (returnValue == -1) {
                perror("Error with dup2 of prognameInFd to STDIN_FILENO.\n");
                close(prognameInFd);
                exit(255);
            }

            returnValue = dup2(pipeDescriptors[1], STDOUT_FILENO);
            if (returnValue == -1) {
                perror("Error with dup2 of pipeDescriptors[1] to STDOUT_FILENO.\n");
                close(pipeDescriptors[1]);
                exit(255);
            }

            close(pipeDescriptors[1]);

            argsFile = fopen(argv[2], "r");
            if (argsFile == NULL) {
                perror("Error opening arguements file");
                exit(255);
            }

            cap = 0;
            returnValue = getline(&arguementString, &cap, argsFile);
            
            if (returnValue == -1) { // when it's empty
                args = realloc(args, sizeof(char *)*(2));
                if (args == NULL) {
                    perror("Error reallocating memory");
                    exit(255);
                } 
                args[0] = progname;
                args[1] = NULL;
            } 
            else {
                // to seperate each arguement from the arguementString
                tempWord = strtok(arguementString, " ");
                i=0;
                while(tempWord != NULL) {
                    i++;
                    args = realloc(args, sizeof(char *)*(i+2));
                    if (args == NULL) {
                        perror("Error reallocating memory");
                        exit(255);
                    }
                    
                    returnValue = strlen(tempWord);
                    if (tempWord[returnValue - 1] == '\n') {
                        tempWord[returnValue - 1] = '\0';
                    }
                    
                    args[i] = tempWord;
                    tempWord = strtok(NULL, " ");
                }
                args[0] = progname;
                args[i+1] = NULL; 
            }                

            returnValue = execvp(executableName, args);
            if(returnValue == - 1) {
                perror("Exec of progname with progname.args failed.\n");
                exit(255);
            }
        }
        else {
            // P3 PROCESS
            pid2 = fork();
            if(pid2 == -1) {
                free(arguementString); 
                free(args);
                free(progname);
                free(executableName);
                perror("Fork for P3 failed.\n");
                return(255);
            }

            if(pid2 == 0) {
                close(pipeDescriptors[1]);

                returnValue = dup2(pipeDescriptors[0], STDIN_FILENO);
                if (returnValue == -1) {
                    perror("Error with dup2 of pipeDescriptors[0] to STDIN_FILENO.\n");
                    close(pipeDescriptors[0]);
                    exit(255);
                }

                close(pipeDescriptors[0]);

                returnValue = execl("./p4diff", "p4diff", argv[4], NULL);
                if(returnValue == - 1) {
                    free(arguementString); 
                    free(args);
                    free(progname);
                    free(executableName);
                    perror("Exec of p4diff failed.\n");
                    exit(255);
                }
            }
            else { 
                //parent proces

                setitimer(ITIMER_REAL,&t,NULL);          
                close(pipeDescriptors[1]);

                waitpid(pid, &status, 0);
                // kills the child if it didn't finish in time
                if((kill(pid, SIGKILL) == -1) && (errno == ESRCH)) {
                    // in case the child wasn't killed
                    action.sa_handler = SIG_IGN;
                }
                else {
                    terminationScore = -100;
                    waitpid(pid, &status, 0);
                }
                
                free(arguementString); 
                free(args);
                free(executableName);
                free(progname);

                if (WEXITSTATUS(status) == 255) {
                    if (kill(pid2, SIGKILL) != 0) {
                        perror("Failed to kill process");
                    }
                    return(255);
                }
                if ((WTERMSIG(status) == SIGSEGV) || (WTERMSIG(status) == SIGABRT) ||
                (WTERMSIG(status) == SIGBUS)) {
                    memoryScore = -15;
                }

                returnValue = waitpid(pid2, &status, 0);
                if (returnValue == -1) {
                    perror("Waitpid for P3 failed");
                }
                if (WEXITSTATUS(status) == 255) {
                    return(255);
                } else {
                    outputScore = WEXITSTATUS(status);
                }
            }
        }
    }

    scorePrint(compilationScore, terminationScore, outputScore, memoryScore);    

    return(0);
}