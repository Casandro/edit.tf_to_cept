

/*
 * decode_base64_to_char
 * *s: Hash-Part of the URL
 * *out: array of 1000 chars where the result will be stored
 * s[] will be base64 decoded into bits, then groups of 7 bits will be stored in a char of out[]
 */
void decode_base64_to_char(const char *s, char out[]);
