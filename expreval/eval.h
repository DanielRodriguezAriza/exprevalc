#ifndef EVAL_H
#define EVAL_H

#include <stdio.h>
#include <stdbool.h>

#include "token.h"
#include "tokenlist.h"
#include "scanner.h"
#include "parser.h"

static inline bool is_quit_message(char const *buf)
{
	return ((buf[0] == 'q' || buf[0] == 'Q') && buf[1] == 0);
}

static inline int eval_loop(void)
{
	int ans = 0;
	char buf[1024] = {0};
	char dummy = 0;
	bool has_to_quit = false;
	
	TokenList tokens;
	TokenList_Init(&tokens);
	
	while(!has_to_quit)
	{
		buf[0] = 0;
		TokenList_Clear(&tokens);
		
		printf("\n> ");
		scanf("%1024[^\n]", buf);
		scanf("%c", &dummy);
		
		// printf("the user has input: '%s'\n", buf);
		
		if(is_quit_message(buf))
		{
			has_to_quit = true;
			continue;
		}
		
		Scanner scanner;
		Scanner_Init(&scanner, &tokens, buf);
		scanner_scan(&scanner);
		if(scanner.has_failed)
		{
			// fprintf(stderr, "Unknown token found in sequence!\n");
			continue;
		}
		Scanner_Free(&scanner);
		
		Parser parser;
		Parser_Init(&parser, &tokens);
		ans = parser_parse_expr(&parser);
		if(parser.has_failed)
		{
			// fprintf(stderr, "Failed to parse expression!\n");
			continue;
		}
		Parser_Free(&parser);
		
		// printf("%s = %d\n", buf, ans);
		printf("%d\n", ans);
	}

	TokenList_Free(&tokens);
    
	return ans;
}

#endif
