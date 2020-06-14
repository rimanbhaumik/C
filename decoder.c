#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BIT     16      /* required for accommodating precision 4 decimal */

#define ERROR_FLOAT -1.0

void process_file(FILE *);
char *get_binary_stream(FILE *);
float bin_dec(char *);

int debug;
int count;
char *prog;

int main(int argc, char *argv[])
{
    prog = argv[0];
	debug = getenv("DEBUG")? 1: 0;
    if(argc == 1) {
        process_file(stdin);
    } else {
        FILE *fp;
        while(1) {
			if(argc == 1) break;
			argc--; argv++; /* Skip argv[0] */
	        if(debug) printf("Processing File %d: %s\n", argc, *argv);
            if(!(fp = fopen(*argv, "r"))) {
                fprintf(stderr, "%s: error: %s: ", prog, *argv);
                perror("");
                return 1;
            }
            count = 0;
            process_file(fp);
            fclose(fp);
        }
    }
    printf("\n");
    return 0;
}

void process_file(FILE *fp)
{
    float U;
    char *s;
    if(debug) printf("Binary Points:\n");
    while(1) {
		s = get_binary_stream(fp);
		if(!s) break;
        if(debug) {
            U = bin_dec(s);
            printf("\t%3d. %+6.4f = %c %.3s %.4s %.4s %.4s\n", count, U, s[0], &s[1], &s[4], &s[8], &s[12]);
        } else {
            printf("%+6.4f\n", bin_dec(s));
        }
    }
    if(debug) printf("Number of Points: %d\n", count);
    return;
}

char *get_binary_stream(FILE *fp)
{
	int i;
    char *bs = (char *) malloc((2 * MAX_BIT + 1) * sizeof(char));
    bs[2 * MAX_BIT] = '\0';
    if((fread((void *)bs, sizeof(char), 2 * MAX_BIT, fp)) != (2 * MAX_BIT)) {
		free(bs);
        bs = (char *)NULL;
	} else {
		count++;
		for(i = 0; i < MAX_BIT; i++) {
			bs[i] = bs[2*i];
		}
		bs[MAX_BIT] = '\0';
	}
    return bs;
}

float bin_dec(char *bs)
{
	int i;
	short unsigned int n;

	if(!bs || strlen(bs) != MAX_BIT) return ERROR_FLOAT;
	for(i = 0; i < MAX_BIT; i++)
		if(bs[i] != '0' && bs[i] != '1') return ERROR_FLOAT;

	for(i = MAX_BIT - 1, n = 0; i > 0; i--) {
		n |= (bs[i] - '0')<<(MAX_BIT - i - 1);
	}

	return (bs[0] == '1'? -1: 1) * ((float)n / 10000);
}
