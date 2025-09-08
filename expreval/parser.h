#ifndef PARSER_H
#define PARSER_H

// Includes from std
#include <stdio.h>
#include <stdbool.h>

// Includes from project
#include "token.h"
#include "tokenlist.h"

typedef struct {
	TokenList *tokens;
	int current;
	bool has_failed;
} Parser;

// Forward declarations
int parser_parse_expr(Parser*);
int parser_parse_expr_addsub(Parser*);
int parser_parse_expr_muldiv(Parser*);
int parser_parse_expr_unary(Parser*);
int parser_parse_expr_primary(Parser*);

Token parser_peek_at(Parser*, int);
Token parser_peek(Parser*);
Token parser_peek_previous(Parser*);
Token parser_peek_next(Parser*);

Token parser_advance(Parser*);
bool parser_match(Parser*, int);

bool parser_is_at_end(Parser*);

// Implementation

void Parser_Init(Parser *self, TokenList *token_list)
{
	self->tokens = token_list;
	self->current = 0;
	self->has_failed = false;
}

void Parser_Free(Parser *self)
{
	self->tokens = NULL;
	self->current = 0;
	self->has_failed = false;
}

Token parser_peek_at(Parser *self, int offset)
{
    return TokenList_Get(self->tokens, self->current + offset);
}

Token parser_peek(Parser *self)
{
	if(parser_is_at_end(self)) return (Token){TOKEN_EOF, 0};
    return parser_peek_at(self, 0);
}

Token parser_peek_previous(Parser *self)
{
	return parser_peek_at(self, -1);
}

Token parser_peek_next(Parser *self)
{
	return parser_peek_at(self, 1);
}

Token parser_advance(Parser *self)
{
    Token ans = parser_peek(self);
	self->current += 1;
    // printf("current: %d -> %d, with caught value (%s, %d)\n", self->current - 1, self->current, TokenTypeName[ans.type], ans.value);
	return ans;
}

bool parser_match(Parser *self, int type)
{
    Token token = parser_peek(self);
    if(token.type == type)
    {
        parser_advance(self);
        // printf("token matched: %s\n", TokenTypeName[token.type]);
        return 1;
    }
    return 0;
}

bool parser_is_at_end(Parser *self)
{
	// return parser_peek_at(self, 0).type == TOKEN_EOF || self->has_failed;
	return self->current >= TokenList_Length(self->tokens) || self->has_failed;
	// if we fail, we act as if we were at the end so that we can quit early. That's because this is a simple expression evaluator and not a full language parser, so once we fail, there's nothing left for us to do.
}

int parser_parse_expr(Parser *self)
{
    return parser_parse_expr_addsub(self);
}

int parser_parse_expr_addsub(Parser *self)
{
    int l = 0, r = 0;
    l = parser_parse_expr_muldiv(self);
    while(!parser_is_at_end(self) && (parser_match(self, TOKEN_OP_PLUS) || parser_match(self, TOKEN_OP_MINUS)))
    {
        Token tok = parser_peek_previous(self);
        r = parser_parse_expr_muldiv(self);
        switch(tok.type)
        {
            case TOKEN_OP_PLUS: l = l + r; break;
            case TOKEN_OP_MINUS: l = l - r; break;
            default: self->has_failed = true; l = 0; fprintf(stderr, "WRONG OP, EXPECTED + OR - (%d, %d)\n", tok.type, tok.value); break;
        }
    }
    // printf("l = %d, r = %d\n", l, r);
    return l;
}

int parser_parse_expr_muldiv(Parser *self)
{
    int l = 0, r = 0;
    l = parser_parse_expr_unary(self);
    while(!parser_is_at_end(self) && (parser_match(self, TOKEN_OP_STAR) || parser_match(self, TOKEN_OP_SLASH)))
    {
        Token tok = parser_peek_previous(self);
        r = parser_parse_expr_unary(self);
        switch(tok.type)
        {
            case TOKEN_OP_STAR: l = l * r; break;
            case TOKEN_OP_SLASH: l = l / r; break;
            default: self->has_failed = true; l = 0; fprintf(stderr, "WRONG OP, EXPECTED * OR / (%d, %d)\n", tok.type, tok.value); break;
        }
    }
    // printf("l = %d, r = %d\n", l, r);
    return l;
}

int parser_parse_expr_unary(Parser *self)
{
    if(parser_match(self, TOKEN_OP_PLUS))
    {
        int v = parser_parse_expr_primary(self);
        return v;
    }
    
    if(parser_match(self, TOKEN_OP_MINUS))
    {
        int v = parser_parse_expr_primary(self);
        return -v;
    }
    
    return parser_parse_expr_primary(self);
}

int parser_parse_expr_primary(Parser *self)
{
    Token token = parser_advance(self);
	int ans = 0;
	// printf("primary expr : %s\n", TokenTypeName[token.type]);
    switch(token.type)
    {
		case TOKEN_EOF:
			{
				// Do nothing, or maybe report "reached end of sequence" or something like that, idk...
			}
			break;
        case TOKEN_LITERAL_NUMBER:
			{
				// printf("%d\n", token.value);
				ans = token.value;
			}
			break;
        case TOKEN_PAREN_L:
            {
                int v = parser_parse_expr(self);
				// this part right here where we do the if-else is what is usually implemented as a "consume(TOKEN_TYPE, 'error message')" type of function, but we do it inline because we're cool af.
                if(parser_match(self, TOKEN_PAREN_R))
                {
                    return v;
                }
                else
                {
					self->has_failed = true;
                    fprintf(stderr, "Expected ')' at end of grouping expression\n");
                }
            }
            break;
        default:
			{
				// printf("parse_expr_litnum()\n");
				self->has_failed = true;
				fprintf(stderr, "Unknown primary expression found (%s)\n", TokenTypeName[token.type]);
			}
			break;
    }
    return ans;
}

#endif
