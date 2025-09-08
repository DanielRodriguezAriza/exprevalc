#ifndef TOKEN_H
#define TOKEN_H

enum TokenType
{
    TOKEN_NONE = 0,
    TOKEN_PAREN_L, TOKEN_PAREN_R,
    TOKEN_OP_PLUS, TOKEN_OP_MINUS, TOKEN_OP_STAR, TOKEN_OP_SLASH,
    TOKEN_LITERAL_NUMBER,
	TOKEN_EOF,
    TOKEN_COUNT,
};

static char const * const TokenTypeName[] = {
    "TOKEN_NONE",
    "TOKEN_PAREN_L", "TOKEN_PAREN_R",
    "TOKEN_OP_PLUS", "TOKEN_OP_MINUS", "TOKEN_OP_STAR", "TOKEN_OP_SLASH",
    "TOKEN_LITERAL_NUMBER",
	"TOKEN_EOF",
    "TOKEN_COUNT",
};

typedef struct {
    int type;
    int value;
} Token;

#endif
