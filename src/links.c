#include "links.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>


links_t *create_link(const char *key, const char *value)
{
	if (key==NULL) return NULL;
	if (value==NULL) return NULL;
	//reject long key and value strings
	if (strlen(key)>256) return NULL;
	if (strlen(value)>256) return NULL;

	links_t *t=malloc(sizeof(links_t));
	if (t==NULL) return NULL;

	t->next=NULL;
	t->key=malloc(strlen(key)+1);
	t->value=malloc(strlen(value)+1);
	strcpy(t->key, key);
	strcpy(t->value, value);
	return t;
}


links_t *add_link(links_t *o, const char *key, const char *value)
{
	links_t *t=create_link(key, value);
	if (t==NULL) return o;
	t->next=o;
	return t;
}

void delete_link(links_t *l)
{
	if (l==NULL) return;
	if (l->key!=NULL) free(l->key);
	if (l->value!=NULL) free(l->value);
	free(l);
}

links_t *remove_links(links_t *o)
{
	if (o==NULL) return NULL;
	links_t *l=o->next;
	delete_link(o);
	return l;
}



void print_links(FILE *f, links_t *start, const char *prefix)
{
	fprintf(f, "\"links\":{");
	links_t *l=start;
	while (l!=NULL) {
		if  ( (l->key[0]!=0) && (l->value[0]!=0) ) {
			if (l->value[0]=='*') {
				fprintf(f, "\t\"%s\": \"%s\"", l->key, &(l->value[1]));
			} else{
				fprintf(f, "\t\"%s\": \"%s%s\"", l->key, prefix, l->value);
			}
			if (l->next!=NULL) fprintf(f,",");
		}
		fprintf(f,"\n");
		l=l->next;
	}
	fprintf(f, "}");
	remove_links(start);
}
