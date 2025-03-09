#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BLOCK_SIZE 64

/*
 * Calculates how similar the two strings given are
 * Parameters: The two strings
 * Returns: The amount of similar bytes
*/
int byteSimilarity(char fileBlock[BLOCK_SIZE], char inputBlock[BLOCK_SIZE], int filesBytes,
int inputsBytes)
{
    int i, similarBytes = 0, comparableBytes;

    // to know how many bytes to compare 
    comparableBytes = filesBytes;
    if (inputsBytes < filesBytes) {
        comparableBytes = inputsBytes;
    }

    for (i=0; i < comparableBytes; i++) {
        if(fileBlock[i] == inputBlock[i]) {
            similarBytes++;
        }
    }

    return(similarBytes);
}

int main (int argc, char *argv[])
{
    char *filename, *bufferPosition, fileBlock[BLOCK_SIZE + 1], inputBlock[BLOCK_SIZE + 1];
    int fileDescriptor, bytesRead, remainingBytes, totalSimilarBytes = 0, loopEnd = 0, i, 
    totalInputBytesRead = 0, totalFileBytesRead = 0, fileBytes, inputBytes;

    filename = argv[1];

    fileDescriptor = open(filename, O_RDONLY);
    if (fileDescriptor == -1) {
        close(fileDescriptor);
        perror("Error opening file\n");
        return(0);
    }

    while(loopEnd < 2) {
        for(i=0; i < BLOCK_SIZE; i++) {
            fileBlock[i] = '\0';
            inputBlock[i] = '\0';
        }
        loopEnd = 0;

        remainingBytes = BLOCK_SIZE;
        bufferPosition = inputBlock;
        inputBytes = 0;
        while(remainingBytes > 0) {
            bytesRead = read(STDIN_FILENO, bufferPosition, remainingBytes);
            
            if (bytesRead <= 0) {
                if(bytesRead == -1) {
                    perror("Error while trying to read.\n");
                    return(0);
                }
                break; // Exit loop if it reached EOF
            }

            inputBytes = inputBytes + bytesRead;            
            totalInputBytesRead = totalInputBytesRead +  bytesRead;
            remainingBytes = remainingBytes - bytesRead;
            bufferPosition = bufferPosition + bytesRead;        
        }
        if(bytesRead == 0) {
            loopEnd++;
        }
        
        remainingBytes = BLOCK_SIZE;
        bufferPosition = fileBlock;
        fileBytes = 0;
        while(remainingBytes > 0) {
            bytesRead = read(fileDescriptor, bufferPosition, remainingBytes);
            
            if (bytesRead <= 0) {
                if(bytesRead == -1) {
                    perror("Error while trying to read.\n");
                    return(0);
                }
                break; // Exit loop if it reached EOF
            }

            fileBytes = fileBytes + bytesRead; 
            totalFileBytesRead = totalFileBytesRead + bytesRead;
            remainingBytes = remainingBytes - bytesRead;
            bufferPosition = bufferPosition + bytesRead;
        }

        totalSimilarBytes = totalSimilarBytes + byteSimilarity(fileBlock, inputBlock, fileBytes,
        inputBytes);

        if(bytesRead == 0) {
            loopEnd++;
        }
    }
    
    // to set the biggest number of bytes read and divide by that
    if (totalFileBytesRead > totalInputBytesRead) {
        totalInputBytesRead = totalFileBytesRead;
    }

    if ((totalFileBytesRead == 0) && (totalInputBytesRead == 0)) {
        return(100);
    }

    return((100*totalSimilarBytes)/totalInputBytesRead);
}