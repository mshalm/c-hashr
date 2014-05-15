#include "string_handler.h"
#include <string.h>

//initializes string_handler_t structure
void string_handler_init(string_handler_t * s)
{
    string_handler_clear(s);
    s->alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
}


/* string_handler_get_next function:
 * modifies .cur_pos to next string for processing
 */
void string_handler_get_next(string_handler_t * s){
    string_handler_inc_char(s,0);
}

/* string_handler_inc_char function:
 * recursive helper for string_handler_get_next
 */
void string_handler_inc_char(string_handler_t * s, int idx)
{
    if (idx == strlen(s->cur_pos))
    {
	//overflow; all characters set to last element of alphabet; add alphabet[0] to end
	s->cur_pos[idx] = s->alphabet[0];
	return;
    }
    if (s->cur_pos[idx] == s->alphabet[strlen(s->alphabet)-1])
    {
	//character is equal to the last element of alphabet; set to alphabet[0] and carry over to next index
	s->cur_pos[idx] = s->alphabet[0];
	string_handler_inc_char(s,idx+1);
	return;
    }
    //increment value of character
    int char_idx = string_handler_get_char_pos(s,s->cur_pos[idx]);
    s->cur_pos[idx] = s->alphabet[char_idx+1];
    
    
    
}

/* string_handler_get_char_pos function:
 * returns position of parameter character in alphabet;
 */
int string_handler_get_char_pos(string_handler_t * s, char character)
{
    int idx = 0;
    while (character != s->alphabet[idx])
	idx++;
    return idx;
    
}

/* string_handler_clear function:
 * resets value of cur_pos to ""
 */
void string_handler_clear(string_handler_t * s)
{
    int i;
    for (i = 0; i<CUR_POS_LEN; i++)
	s->cur_pos[i] = '\0'; 
}

/* string_handler_set_init_len function:
 * sets value of cur_pos to a string containing only alphabet[0] of length len
 */
void string_handler_set_init_len(string_handler_t * s, int len)
{
    int i;
    len = (len > (CUR_POS_LEN -1)) ? CUR_POS_LEN -1 : len;
    for (i = 0; i<len; i++)
	s->cur_pos[i] = s->alphabet[0]; 
    
}