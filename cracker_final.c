//  gcc -Wall crackerABC.c -fopenmp -lm -o crackerABC
// ./hasher X      ||   ./crackerABC -s Xs X

#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>

#define SIZE 16
#define MIN 4
#define MAX 4

#define ALPHA "qwertyuiopñlkjhgfdsazxcvbnm"
#define SECRET "2090335593" //mueble



unsigned long compute_hash(char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = (*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

long get_keyspace(char *alpha, int max)
{
    long size = 1;
    for (int j = 0; j < max; j++)
    {
        size = size * strlen(alpha);
    }
    return size;
}

int cracker_by_frute_force(char *alpha, int max, char *secret)
{
    int found = 0;
    int index[max];
    char candidate[max];
    long keyspace = get_keyspace(alpha, max);
    char str[25];
    int i, j;
    //crackeando por fuerza bruta
    for (int i = 0; i < max; ++i)
    {
        index[i] = 0;
    }
    //TODO: gestionar numeros threads
        omp_set_num_threads(4);

#pragma omp parallel for private(i, j, candidate, str) shared(found)
    for (i = 0; i < keyspace; ++i)
    {

        if (found == 0)
        {
            strcpy(candidate, "");
            for (j = max - 1; j >= 0; --j)
            {
                index[j] = (i / (int)pow(strlen(alpha), j)) % strlen(alpha);
                sprintf(candidate, "%s%c", candidate, alpha[index[j]]);
            }
            sprintf(str, "%lu", compute_hash(candidate));
            if (strcmp(str, secret) == 0)
            {
                printf("Secret found: **%s**\n", candidate);
                found = 1;
            }
        }
    }
    if (found == 0)
        printf("Password not found by frute force :(\n");

       int nthreads = omp_get_num_threads();
        printf("There are %d threads\n",nthreads);
    return found;
}

int cracker_by_dictionary(char *alpha, int max, char *secret)
{
    int found = 0;

    char candidate[max];
    char str[25];
    //crackeando por diccionario
    FILE *dictionary = fopen(alpha, "r");
    int number_lines_file = 0;
    int i;
    //check if file exists
    if (dictionary == NULL)
    {
        printf("file does not exists %s", alpha);
        return 0;
    }

    strcpy(candidate, "");

    //count lines of the file
    char c; 
    for (c = getc(dictionary); c != EOF; c = getc(dictionary))
        if (c == '\n')
            number_lines_file = number_lines_file + 1;

    printf("The file %s has %d lines\n ", alpha, number_lines_file);

    fclose(dictionary);
    dictionary = fopen(alpha, "r");
    //read line by line
    const size_t line_size = 300;
    char *line = malloc(line_size);

#pragma omp parallel for private(i, candidate, str) shared(found, line)
    //while (fgets(line, line_size, dictionary) != NULL)
    for (i = 0; i < number_lines_file; i++)
    {

        if (fgets(line, line_size, dictionary) != NULL)
        {

            size_t indexOfNullTerminator = strlen(line);
            line[indexOfNullTerminator - 1] = '\0';
            strcpy(candidate, line);
           // printf("Line:  -%s-    |    ", candidate);

            // compara line del dict con el hash
            sprintf(str, "%lu", compute_hash(candidate));
            //printf("Hash:  %s \n", str);

            if (strcmp(str, secret) == 0)
            {
                printf("Secret found by dictionary: **%s**\n", candidate);
                found = 1;
            }
        }
    }
    free(line);

    // }
    if (found == 0)
        printf("Password not found by dictionary:(\n");

    return found;
}

int main(int argc, char *argv[])
{

    char *secret;
    char *alpha;
    char *avalue = NULL;
    char *svalue = NULL;
    int letra;
    //parsear argv
    while ((letra = getopt(argc, argv, "s:a:")) != -1)
        switch (letra)
        {
        case 's': //tamano de las claves candidatas
            svalue = optarg;
            break;
        case 'a':
            avalue = optarg;
            break;
        case '?':
            if (optopt == 's' || optopt == 'a')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            return 1;
        default:
            abort();
        }

    printf("He recibido en -s %s\n", svalue);
    printf("He recibido en -a %s\n", avalue);

    int max;
    if (svalue != NULL)
    {
        max = atoi(svalue);
    }
    else
    {
        max = MAX;
        printf("Default size (-s) %d\n", MAX);

    }
    printf("max = %d \n", max);

    if (optind < argc)
    {
        secret = strdup(argv[optind]);
    }
    else
    {
        secret = strdup(SECRET);
    }
    printf("secret = %s \n", secret);

    if (avalue != NULL)
    {
        alpha = strdup(avalue);
        printf("alpha = %s \n", alpha);
        cracker_by_dictionary(alpha, max, secret);
    }
    else
    {
        alpha = strdup(ALPHA);
        printf("alpha = %s \n", alpha);
        cracker_by_frute_force(alpha, max, secret);
    }

    return 0;
}
