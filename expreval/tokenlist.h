#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H

#include "token.h"

#ifndef TOKEN_LIST_FREE
#define TOKEN_LIST_FREE free
#define TOKEN_LIST_MUST_INCLUDE_STDLIB
#endif

#ifndef TOKEN_LIST_MALLOC
#define TOKEN_LIST_MALLOC malloc
#define TOKEN_LIST_MUST_INCLUDE_STDLIB
#endif

#ifndef TOKEN_LIST_REALLOC
#define TOKEN_LIST_REALLOC realloc
#define TOKEN_LIST_MUST_INCLUDE_STDLIB
#endif

#ifndef TOKEN_LIST_INITIAL_CAPACITY
#define TOKEN_LIST_INITIAL_CAPACITY 8
#endif

#ifdef TOKEN_LIST_MUST_INCLUDE_STDLIB
#include <stdlib.h>
#endif

typedef struct {
    Token *data;
    int len, cap;
} TokenList;

void TokenList_Init(TokenList *self)
{
    self->data = (Token*)TOKEN_LIST_MALLOC(TOKEN_LIST_INITIAL_CAPACITY * sizeof(Token));
    self->len = 0;
    self->cap = TOKEN_LIST_INITIAL_CAPACITY;
}

void TokenList_Free(TokenList *self)
{
    if(self->data) TOKEN_LIST_FREE(self->data);
    self->len = 0;
    self->cap = 0;
}

Token TokenList_Get(TokenList *self, int idx)
{
    return self->data[idx];
}

void TokenList_Realloc(TokenList *self, int new_cap)
{
    Token *temp = (Token*)TOKEN_LIST_REALLOC(self->data, new_cap * sizeof(Token));
    if(temp)
    {
        self->data = temp;
        self->cap = new_cap;
    }
}

void TokenList_TryRealloc(TokenList *self)
{
    if(self->len >= self->cap) TokenList_Realloc(self, self->cap * 2);
}

void TokenList_Add(TokenList *self, Token token)
{
    TokenList_TryRealloc(self);
    self->data[self->len] = token;
    self->len += 1;
}

void TokenList_Clear(TokenList *self)
{
	self->len = 0; // Token has no destructor of its own, so we can just set the length to 0 and reuse the memory since Token is just a trivial POD type.
}

int TokenList_Length(TokenList *self)
{
	return self->len;
}

int TokenList_Capacity(TokenList *self)
{
	return self->cap;
}

#endif

/*
	NOTE:
	
	The scanner and parser implementations are so simple that we could theoretically just store the index at which the current and previous tokens
	are located within the source string, allowing us to have a 0 heap allocation expression evaluator, with the only downside of having to reparse a
	few times.
	
	To avoid that, we could also store the parsed previous and current tokens, allowing the peek previous calls to not need to reparse.
	
	The current implementation uses a token list with heap allocations instead simply for the sake of trying to make the code generic enough that it could
	fit anyone's needs.
	
	If someone wants to make it 0 heap allocation, all they need to do is change the token list's behaviour to just store 2 elements, the current and the previous, and that way all the storage is statically known. Then, modify the scanner to not parse all tokens in a look till the end, and modify the parser to request tokens on demand from the scanner, doing live token scanning during parsing. That way, rather than parsing till current is >= tokens_count, we would parse till we hit EOF, and the scanner can just return EOF always when requesting token reads past the end of the source string.
	
	Since this is a simple expression evaluator, like a calculator, that would make sense, but the code is more oriented as if it were going to be part of a larger program, something like a compiler or whatever, so it accommodates for usage with user defined token lists / buffers in case users want to pass a custom parsed list of tokens.
*/
