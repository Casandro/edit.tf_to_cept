#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "decode_base64.h"
#include "print_cept.h"
#include "links.h"

#define LINELEN (2048)
#define PAGENUMLEN (32)


/* returns 1 when string is completely nummeric
 * returns 0 when there is a different character
 */
int is_all_digits(char *s)
{
	if (s==NULL) return 0;
	if (*s=='\0') return 1;
	if (*s<'0') return 0;
	if (*s>'9') return 0;
	return is_all_digits(&(s[1]));
}


/*
 * Handles the hash of a page
 * stores page into *page
 * stores language into *lang
 * return -1 on failure (try other formats then)
 */
int handle_hash(const char *hash, char *page, int *lang)
{
	if (hash==NULL) return -1;
	char *colon=strchr(hash, ':');
	if (colon==NULL) return -1; //No colon found, therefore cannot be a hash
	if (colon-hash!=2) return -1; //No character in between
	int l=hash[1]-'0'; //decode language
	if (l<0) return -1; //Invalid language
	if (l>9) return -1; //Invalid language
	char *colon2=strchr(&(colon[1]), ':'); //Is there a second colon
	if (colon2!=NULL) *colon2=0; //Cut of anything after second colon if it exists
	decode_base64_to_char(&(colon[1]), page); //Decode string
	*lang=l;
	return 0;
}


int handle_link(char *line, links_t **links)
{
	char *equal=strchr(line,'>');
	if (equal==NULL) return -1;
	char key[PAGENUMLEN];
	memset(key, 0, sizeof(key));
	strncpy(key, line, equal-line);
	char value[PAGENUMLEN];
	memset(value, 0, sizeof(value));
	strncpy(value, equal+1, sizeof(value)-1);
	printf("link: %s => %s\n", key, value);
	links_t *l=add_link(*links, key, value);
	if (l==NULL) return -1;
	*links=l;
	return 0;
}


int output_page(const char *pagenum, const char *page, const int lang, links_t *links, const char *prefix)
{
	if (strlen(pagenum)<1) return 0;
	char fn[1024];
	snprintf(fn, sizeof(fn), "%s.cept", pagenum);
	FILE *f=fopen(fn, "w");
	print_page(f, page, lang);
	fclose(f);

	snprintf(fn, sizeof(fn), "%s.meta", pagenum);
	f=fopen(fn, "w");
	fprintf(f, "{ \"clear_screen\": false,\n"); 
	print_links(f, links, prefix);
	fprintf(f, ", \"publisher_color\": 7\n}\n");
	fclose(f);

}

int handle_output_page(const char *line, const char *page, const int lang, links_t **links, const char *prefix)
{
	if (line[0]!='p') return -1;
	char *space=strchr(line, ' ');
	if (space==NULL) return -1;

	char pagenum[PAGENUMLEN];
	memset(pagenum, 0, sizeof(pagenum));
	strncpy(pagenum, space+1, PAGENUMLEN-1);

	int n;
	for (n=0; n<strlen(pagenum); n++) {
		char c=pagenum[n];
		if ( ( (c>='0') && (c<='9') ) || ( (c>='a') && (c<='z')) ) continue;
		pagenum[n]=0;
	}
	int len=strlen(pagenum);
	char c=pagenum[len-1];
	printf("pagenum: %s %d %x\n", pagenum, len, c);

	if ((c<'a') || (c>'z')) strcat(pagenum, "a");
	output_page(pagenum, page, lang, *links, prefix);
	*links=NULL;
	return 0;
}

int handle_line(char *line, char *page, int *lang, links_t **links, const char *prefix)
{
	char *hash=strchr(line, '#');
	int res=handle_hash(hash, page, lang);
	if (res>=0) return 0;
	res=handle_link(line, links);
	if (res>=0) return 0;
	res=handle_output_page(line,	 page, *lang, links, prefix);
	if (res>=0) return 0;
	return -1;
}


int main(int argc, char *argv[])
{

	char page[1000];
	int language=0;
	links_t *links=NULL;

	char line[LINELEN];
	while (fgets(line, sizeof(line)-1, stdin)!=NULL) {
		while ((strlen(line)>1) && (line[strlen(line)-1]<=' ')) {
			int len=strlen(line);
			line[len-1]=0; //Remove final character (should be newline)
		}
		int res=handle_line(line, page, &language, &links, "3603");
		if (res<0) printf("couldn't understand line: %s\n", line);
	}
	return 0;
}
