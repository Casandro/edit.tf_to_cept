#include "decode_base64.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>

int decode_b64_char(const char c)
{
	if ( (c>='A') && (c<='Z') ) return c-'A';
	if ( (c>='a') && (c<='z') ) return c-'a'+26;
	if ( (c>='0') && (c<='9') ) return c-'0'+52;
	if (c=='+') return 62;
	if (c=='-') return 62;
	if (c=='/') return 63;
	if (c=='_') return 63;
	return -1;
}


void decode_base64_string(const char *s, char bits[], size_t space)
{
	if (space<6) return;
	if (s==NULL) return;
	int b=decode_b64_char(*s);
	if (b<0) return;
	int n;
	for (n=0; n<6; n++) {
		bits[n]=(b>>(5-n))&1;
	}
	return decode_base64_string(&(s[1]), &(bits[6]), space-6);
}

void decode_base64_to_char(const char *s, char out[])
{
	char bits[7000];
	memset(bits, 0, sizeof(bits));
	char page[1000];
	memset(page, 0, sizeof(page));
	decode_base64_string(s, bits, sizeof(bits));
	int line=0;
	int column=0;
	for (line=0; line<25; line++)
	for (column=0; column<40; column++)
	{
		int p=column+line*40;
		char t=0;
		int n;
		for (n=0; n<7; n++) 
		{
			t=(t<<1)|(bits[p*7+n]&0x01);
		}
		out[p]=t;
	}
}
