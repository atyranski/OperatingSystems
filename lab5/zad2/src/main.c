// Napisać program przyjmujący jeden (nadawca lub data) lub trzy argumenty (<adresEmail> <tytuł> <treść>):
// W przypadku wywołania z jednym argumentem uruchamiany jest (za pomocą popen()) program mail. Program użytkownika ma wypisywać listę e-maili posortowaną alfabetycznie wg. adresów e-mail (argument nadawca) lub wg. daty otrzymania e-maili (argument data)
// Jeżeli program zostanie wywołany z trzema argumentami, to (za pomocą popen()) uruchamiany jest program mail i za jego pomocą wysyłany jest e-mail do określonego nadawcy z określonym tematem i treścią

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "printutils.h"

// ---- Return codes
#define RETURN_SUCCESS 0
#define RETURN_INCORRECT_ARGUMENT_AMOUNT -1

// ---- Main program
int main(int argc, char **argv){

    FILE* pipeFile;
    char buffer[1000];

    if(argc != 2 && argc != 4){
        error("INCORRECT_ARGUMENT_AMOUNT", "provide argument(s) according to chosen mode:\none argument: <nadawca | data>\nthree arguments: <emailAddress> <title> <content>");
        return RETURN_INCORRECT_ARGUMENT_AMOUNT;
    }

    // Reading mails and sortiny by sender
    if(argc == 2 && strcmp(argv[1], "nadawca") == 0){
        // To co robi nadawca

        pipeFile = popen("mail | sort -k 3", "w");
        fgets(buffer, 1000, pipeFile);

        printf("%s\n", buffer);

        return RETURN_SUCCESS;
    }

    // Reading mails and sortiny by date
    if(argc == 2 && strcmp(argv[1], "data") == 0){
        // To co robi data

        pipeFile = popen("mail | sort -k 2 ", "w");
        fgets(buffer, 1000, pipeFile);

        printf("%s\n", buffer);

        return RETURN_SUCCESS;
    }

    // Sending message
    if(argc == 4){
        char command[1000];
        char* message = argv[3];
        char* subject = argv[2];
        char* to = argv[1];
        char* from = "atyranski@sender.pl";

        sprintf(command, "echo %s | mail -s %s %s -aFrom: %s", message, subject, to, from);
        pipeFile = popen(command, "w");

        printInfo("Success", "mail has been successfully sent");

        return RETURN_SUCCESS;
    }

    pclose(pipeFile);
    return RETURN_SUCCESS;
}