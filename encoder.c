#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BIT     16      /* required for accommodating precision 4 decimal */

#define ERROR_FLOAT -1.0

void process_file(FILE *);
float get_number(FILE *);
char *bin_enc(float);
char *spacer(char *);

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
    while((U = get_number(fp)) != ERROR_FLOAT) {
        if(debug) {
            s = bin_enc(U);
            printf("\t%3d. %+6.4f = %c %.3s %.4s %.4s %.4s\n", count, U, s[0], &s[1], &s[4], &s[8], &s[12]);
        } else {
            printf("%s", spacer(bin_enc(U)));
        }
    }
    if(debug) printf("Number of Points: %d\n", count);
    return;
}

float get_number(FILE *fp)
{
    register int c = 0;
    float f;
    while(!feof(fp) && isspace(c = fgetc(fp))); /* Skip spaces */
    if(!feof(fp))
        ungetc(c, fp);
    else
        return ERROR_FLOAT;
    if(isdigit(c) || c == '+' || c == '-' || c == '.') {
        c = fscanf(fp, "%f", &f);
        if(feof(fp) || c == 0 || f < -0.5 || f > 0.5) return ERROR_FLOAT;
    } else return ERROR_FLOAT;
    count++;
    return f;
}

char *bin_enc(float f)
{
    int i;
    short unsigned int n;
    char *b = (char *) malloc((MAX_BIT + 1) * sizeof(char));
    b[0]  = f < 0? '1': '0';    /* Store the sign bit */
    b[MAX_BIT] = '\0';          /* NULL terminated binary string */
    f = f < 0? -f: f;           /* take the absolute value */
    n = (int) (10000 * f);
    for(i = MAX_BIT - 1; i > 0; i--) {
        b[i] = '0' + (n & 0x1);
        n >>= 1;
    }
    return b;
}

char *spacer(char *s)
{
	int i, len;
	char *ss;
	if(!s) return (char *)NULL;
	len = strlen(s);
	ss = (char *) malloc((2 * len + 1) * sizeof(char));
	ss[2*len] = '\0';
	for(i = 0; i < len; i++) {
		ss[2*i] = s[i];
		ss[2*i + 1] = ' ';
	}
	return ss;
}
