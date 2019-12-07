#pragma once

#include <stdio.h>

typedef struct links_s {
	struct links_s *next;
	char *key;
	char *value;
} links_t;


links_t *add_link(links_t *o, const char *key, const char *value);
void print_links(FILE *f, links_t *start, const char *prefix);
