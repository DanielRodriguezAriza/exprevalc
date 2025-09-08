#ifndef SCANNER_H
#define SCANNER_H

// Includes from std
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Includes from project
#include "token.h"
#include "tokenlist.h"

// Defines
#define SCANNER_CHARS_WHITESPACE_BUF " \t\r\n\v"
#define SCANNER_CHARS_WHITESPACE_LEN ((sizeof(SCANNER_CHARS_WHITESPACE_BUF) / sizeof(SCANNER_CHARS_WHITESPACE_BUF[0])) - 1)

// Scanner struct
typedef struct {
	char const *source;
	int source_length;
	int current;
	int start;
	TokenList *tokens;
	bool has_failed;
} Scanner;

// Forward Declarations
void Scanner_Init(Scanner*, TokenList*, char const*);
void Scanner_Free(Scanner*);

void scanner_scan(Scanner*);
void scanner_scan_token(Scanner*);

bool scanner_is_whitespace(char);
bool scanner_is_number(char);
int scanner_get_number_from_source(char const*, int, int);
void scanner_scan_number(Scanner*);

bool scanner_is_at_end(Scanner*);
char scanner_advance(Scanner*);
char scanner_peek_at(Scanner*, int);
char scanner_peek(Scanner*);
char scanner_peek_previous(Scanner*);
char scanner_peek_next(Scanner*);

void scanner_add_token(Scanner*, int, int);

// Definitions and Implementation
void Scanner_Init(Scanner *self, TokenList *token_list, char const *src)
{
	self->source = src;
	self->source_length = strlen(src);
	self->current = 0;
	self->start = 0;
	self->tokens = token_list;
	self->has_failed = false;
}

void Scanner_Free(Scanner *self)
{
	self->source = NULL;
	self->source_length = 0;
	self->current = 0;
	self->start = 0;
	self->tokens = NULL;
	self->has_failed = false;
}

void scanner_add_token(Scanner *self, int type, int value)
{
    // printf("%s, %d\n", TokenTypeName[type], value);
	TokenList_Add(self->tokens, (Token){type, value});
}

bool scanner_is_at_end(Scanner *self)
{
    return self->current >= self->source_length;
	// return self->source[self->current] == '\0';
}

void scanner_scan(Scanner *self)
{
    while(!scanner_is_at_end(self))
    {
        self->start = self->current;
        scanner_scan_token(self);
    }
	// scanner_add_token(self, TOKEN_EOF, 0);
}

char scanner_peek_at(Scanner *self, int offset)
{
	return self->source[self->current + offset];
}

char scanner_advance(Scanner *self)
{
	if(scanner_is_at_end(self)) return '\0';
    char ans = scanner_peek(self);
	self->current++;
	return ans;
}

char scanner_peek(Scanner *self)
{
	return scanner_peek_at(self, 0);
}

char scanner_peek_previous(Scanner *self)
{
	return scanner_peek_at(self, -1);
}

char scanner_peek_next(Scanner *self)
{
	return scanner_peek_at(self, 1);
}

bool scanner_is_number(char c)
{
    return c >= '0' && c <= '9';
}

bool scanner_is_whitespace(char c)
{
    for(int i = 0; i < SCANNER_CHARS_WHITESPACE_LEN; ++i)
    {
        if(SCANNER_CHARS_WHITESPACE_BUF[i] == c)
        {
            return true;
        }
    }
    return false;
}

void scanner_scan_token(Scanner *self)
{
    char c = scanner_advance(self);
    switch(c)
    {
        case '(': scanner_add_token(self, TOKEN_PAREN_L, 0); break;
        case ')': scanner_add_token(self, TOKEN_PAREN_R, 0); break;
        case '+': scanner_add_token(self, TOKEN_OP_PLUS, 0); break;
        case '-': scanner_add_token(self, TOKEN_OP_MINUS, 0); break;
        case '*': scanner_add_token(self, TOKEN_OP_STAR, 0); break;
        case '/': scanner_add_token(self, TOKEN_OP_SLASH, 0); break;
        default:
            if(scanner_is_whitespace(c))
            {
                // Ignore it, but it's not an error to find whitespace!
                // printf("-----whitespace!\n");
            }
            else
            if(scanner_is_number(c))
            {
                scanner_scan_number(self);
            }
            else
            {
                fprintf(stderr, "Unknown char '%c' found in sequence!\n", c);
				self->has_failed = true;
            }
            break;
    }
}

int scanner_get_number_from_source(char const *source, int idx_start, int idx_end)
{
    // printf("scanning integer from %d to %d\n", idx_start, idx_end);
    int ans = 0;
    for(int i = idx_start; i <= idx_end; ++i)
    {
        ans *= 10;
        ans += source[i] - '0';
    }
    return ans;
}

void scanner_scan_number(Scanner *self)
{
    while(!scanner_is_at_end(self) && scanner_is_number(scanner_peek(self))){scanner_advance(self);}
    scanner_add_token(self, TOKEN_LITERAL_NUMBER, scanner_get_number_from_source(self->source, self->start, self->current - 1));
}

#endif
