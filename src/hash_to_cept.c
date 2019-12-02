#include <stdio.h>
#include "decode_base64.h"
#include "print_cept.h"


int main(int argc, char *argv[])
{
	if (argc!=2) return 1;
	char screen[1000];
	decode_base64_to_char(argv[1], screen);
	print_page(stdout, screen, 1);
/*	int l;
	for (l=0; l<25; l++) {
		int c;
		for (c=0; c<40; c++) {
			int p=c+l*40;
			if ((screen[p]>32) && (screen[p]<127) ) printf("%c", screen[p]); else printf(".");
		}
		for (c=0; c<40; c++) {
			int p=c+l*40;
			printf("%02x ", screen[p]);
		}
		printf("\n");
	}*/
	return 0;
}
