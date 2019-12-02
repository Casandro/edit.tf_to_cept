#include "print_cept.h"
#include <stdio.h>


typedef struct {
	int lang;
	char c;
	char *str;
} translation_t;

const translation_t transl[]={
//	LangCod	code	translated code
	{1,	0x23,	"\x23"},	//#
	{1,	0x24,	"\x19\x34"}, 	//$
	{1,	0x40,	"\x19\x37"}, 	//§
	{1,	0x5b,	"\x19HA"},	//Ä
	{1,	0x5c,	"\x19HO"},	//Ö
	{1,	0x5d,	"\x19HU"},	//Ü
	{1,	0x5e,	"\x5e"},	//^
	{1,	0x5f,	"\x5f"},	//_
	{1,	0x60,	"\x19\x30"},	//°
	{1,	0x7b,	"\x19Ha"},	//ä
	{1,	0x7c,	"\x19Ho"},	//ö
	{1,	0x7d,	"\x19Hu"},	//ü
	{1,	0x7e,	"\x19\x7b"}	//ß
};


void print_char_int(FILE *f, const char c, const int lang, const int cnt)
{
	//if character should be printed 0 times, it should not be printed
	if (cnt<=0) return;
	//find Translation for national characters
	char *tr=NULL;
	int n;
	for (n=0; n<sizeof(transl)/sizeof(translation_t); n++) {
		if ( (c==transl[n].c) && ( (lang==transl[n].lang) || (transl[n].lang<0) ) ) tr=transl[n].str;
	}
	if (tr!=NULL) {
		fprintf(f, "%s", tr);
	} else fprintf(f, "%c", c);
	//Handle repeats
	if (cnt<2) return; //if only one character needs to printed, do nothing;
	//For less than 4 characters just print it 4 times
	if (cnt<4) return print_char_int(f, c, lang, cnt-1);
	fprintf(f, "\x12%c", (cnt-1)+0x41);
}

void print_char(FILE *f, const char c, const int lang, const int cnt, const int mosaik)
{
	if (cnt<=0) return;
	if (c<=0) return;
	if (mosaik==0) print_char_int(f, c, lang, cnt);
	else print_char_int(f, c, -1, cnt);
}

void print_control_character(FILE *f, const char c, int *mosaik)
{
	switch (c) {
		case 0x00: //Alpha Black
		case 0x01: //Alpha Red
		case 0x02: //Alpha Green
		case 0x03: //Alpha Yellow
		case 0x04: //Alpha Blue
		case 0x05: //Alpha Magenta
		case 0x06: //Alpha Cyan
		case 0x07: //Alpha White
			*mosaik=0;
			fprintf(f, "%c", c-0x00+0x80);
			break;
		case 0x08: //Flash
			fprintf(f, "\x88");
			break;
		case 0x09: //Steady
			fprintf(f, "\x89");
			break;
		case 0x0a: //End Box
		case 0x0b: //Start Box
			//Unsupported
			fprintf(f, " ");
			break;
		case 0x0c: //Normal Height
			fprintf(f,"\x8c");
			break;
		case 0x0d: //Double Height
			fprintf(f,"\x8D");
			break;
		case 0x0e: //SO
		case 0x0f: //SI
			//unsupported, perhaps later double width and height
			fprintf(f, " ");
			break;
		case 0x10: //Mosaik Black
		case 0x11: //Mosaik Red
		case 0x12: //Mosaik Green
		case 0x13: //Mosaik Yellow
		case 0x14: //Mosaik Blue
		case 0x15: //Mosaik Magenta
		case 0x16: //Mosaik Cyan
		case 0x17: //Mosaik White
			*mosaik=1;
			fprintf(f, "%c", c-0x10+0x90);
			break;
		case 0x18: //Conceal Display
			fprintf(f, "\x98");
			break;
		case 0x19: //Continuous Graphics
		case 0x1a: //Separated Graphics
		case 0x1b: //ESC
			//Unsupported
			fprintf(f, " ");
			break;
		case 0x1c: //Black Background
			fprintf(f, "\x9c");
			break;
		case 0x1d: //New Background
			fprintf(f, "\x9d");
			break;
		case 0x1e: //Hold Graphics
			fprintf(f, "\x9e");
			break;
		case 0x1f: //Release Graphics
			fprintf(f, "\x9f");
			break;
	}	
}

void print_line(FILE *f, const char c[], const int lang)
{
	fprintf(f, "\x87\x8c\x9c\x9f\x0d"); //reset all attributes and move to the left, bugfix for buggy terminals (xcept)
	int mosaik=0;
	char oc=-1; //previous character;
	int cnt=0; //number of identical characters

	int n;
	for (n=0; n<40; n++) {
		if (c[n]<0x20){
			print_char(f, oc, lang, cnt, mosaik);
			oc=-1;
			cnt=0;
			print_control_character(f, c[n], &mosaik);
		} else {
			if (c[n]!=oc) {
				print_char(f, oc, lang, cnt, mosaik);
				oc=c[n];
				cnt=1;
			} else {
				cnt=cnt+1;
			}
		}
	}
	print_char(f, oc, lang, cnt, mosaik);
}

void print_page(FILE *f, const char c[], const int lang)
{
	fprintf(f, "\x1f\x2d"); //screen size 24x40 wraparound
	fprintf(f, "\x1f\x2f\x41"); //serial attributes erase screen
	fprintf(f, "\x1e"); //Cursor to home position
	int line;
	for (line=0; line<24; line++) {
		print_line(f, &(c[line*40]), lang);
	}
}
