#ifndef __STRING_HANDLER_H__
#define __STRING_HANDLER_H__
#define CUR_POS_LEN 20
/**
 * String Handler Data Structure
 */
typedef struct{
	char cur_pos[CUR_POS_LEN];
	char * alphabet;
} string_handler_t;

void string_handler_init(string_handler_t * s);
void string_handler_set_init_len(string_handler_t * s, int len);
void string_handler_get_next(string_handler_t * s);
void string_handler_inc_char(string_handler_t * s,int idx);
int string_handler_get_char_pos(string_handler_t * s,char character);
void string_handler_clear(string_handler_t * s);
#endif
 
