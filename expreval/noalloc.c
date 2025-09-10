// Simple no alloc implementation. Just a simple showcase.

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static inline int powi(int a, int b)
{
    int ans = 1;
    for(int i = 0; i < b; ++i)
    {
        ans *= a;
    }
    return ans;
}

enum TokenType {
    TOKEN_NONE,
    TOKEN_PAREN_L, TOKEN_PAREN_R,
    TOKEN_OP_PLUS, TOKEN_OP_MINUS, TOKEN_OP_STAR, TOKEN_OP_SLASH, TOKEN_OP_POW,
    TOKEN_LIT_NUMBER,
    TOKEN_EOF,
    TOKEN_COUNT,
};

static char const *TokenTypeName[] = {
    "TOKEN_NONE",
    "TOKEN_PAREN_L", "TOKEN_PAREN_R",
    "TOKEN_OP_PLUS", "TOKEN_OP_MINUS", "TOKEN_OP_STAR", "TOKEN_OP_SLASH", "TOKE_OP_POW",
    "TOKEN_LIT_NUMBER",
    "TOKEN_EOF",
    "TOKEN_COUNT",
};

typedef struct {
    int type;
    int value;
} Token;

bool has_failed = false;

char const *scanner_source = "";
int scanner_source_length = 0;
int scanner_current = 0;
int scanner_start = 0;

Token parser_tokens[1024]; // Kind of shitty, but I just wanted to make a quick noalloc test as a demo. It would be better to use an on-demmand scanner with a previous, current and next tokens allocated to allow arbitrary length scanning, but since the CLI on most systems has a limited input length, this was good enough as a showcase of what could be done as a hacky workaround in a system with limited memory and no dynamic allocations. As long as the scanned input string is within the limits, this is ok.
int parser_tokens_length = 0;
int parser_current = 0;

static inline bool scanner_is_at_end(void)
{
    return scanner_current >= scanner_source_length || has_failed; // return scanner_source[current] == 0;
}

static inline char scanner_peek_at(int offset)
{
    return scanner_source[scanner_current + offset];
}

static inline char scanner_peek(void)
{
    return scanner_peek_at(0);
}

static inline char scanner_peek_previous(void)
{
    return scanner_peek_at(-1);
}

static inline char scanner_peek_next(void)
{
    return scanner_peek_at(1);
}

static inline char scanner_advance(void)
{
    char ans = scanner_peek();
    ++scanner_current;
    return ans;
}

static inline bool scanner_match(char c)
{
    if(scanner_peek() == c)
    {
        ++scanner_current;
        return true;
    }
    return false;
}

static inline bool scanner_is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v';
}

static inline bool scanner_is_number(char c)
{
    return c >= '0' && c <= '9';
}

static inline void scanner_add_token_with_value(int type, int value)
{
    // printf("token : %s, %d\n", TokenTypeName[type], value);
    parser_tokens[parser_tokens_length++] = (Token){type, value};
}

static inline void scanner_add_token(int type)
{
    scanner_add_token_with_value(type, 0);
}

static inline int scanner_scan_number(void)
{
    int ans = scanner_peek_previous() - '0';
    while(!scanner_is_at_end() && scanner_is_number(scanner_peek()))
    {
        ans *= 10;
        char c = scanner_advance();
        ans += c - '0';
    }
    return ans;
}

static inline void scanner_scan_token(void)
{
    char c = scanner_advance();
    switch(c)
    {
        case '+': scanner_add_token(TOKEN_OP_PLUS); break;
        case '-': scanner_add_token(TOKEN_OP_MINUS); break;
        case '*': scanner_add_token(TOKEN_OP_STAR); break;
        case '/': scanner_add_token(TOKEN_OP_SLASH); break;
        case '(': scanner_add_token(TOKEN_PAREN_L); break;
        case ')': scanner_add_token(TOKEN_PAREN_R); break;
        case '^': scanner_add_token(TOKEN_OP_POW); break;
        default: {
            if(scanner_is_number(c))
            {
                int x = scanner_scan_number();
                scanner_add_token_with_value(TOKEN_LIT_NUMBER, x);
            }
            else
            if(scanner_is_whitespace(c))
            {
                // do nothing...
            }
            else
            {
                fprintf(stderr, "Scan error: Unknown symbol found '%c'.\n", c);
                has_failed = true;
            }
        } break;
    }
}

static inline void scanner_scan(void)
{
    while(!scanner_is_at_end())
    {
        scanner_start = scanner_current;
        scanner_scan_token();
    }
    scanner_add_token(TOKEN_EOF);
}

static inline bool parser_is_at_end(void)
{
    return parser_tokens[1].type == TOKEN_EOF; // return parser_tokens[parser_current] == TOKEN_EOF;
}

static inline Token parser_peek_at(int offset)
{
    return parser_tokens[parser_current + offset];
}

static inline Token parser_peek(void)
{
    return parser_peek_at(0);
}

static inline Token parser_peek_previous(void)
{
    return parser_peek_at(-1);
}

static inline Token parser_peek_next(void)
{
    return parser_peek_at(1);
}

static inline Token parser_advance(void)
{
    Token ans = parser_peek();
    ++parser_current;
    return ans;
}

static inline bool parser_match(int type)
{
    if(parser_peek().type == type)
    {
        parser_advance();
        return true;
    }
    return false;
}

static inline int parser_parse_expr(void);

static inline int parser_parse_expr_primary(void)
{
    Token token = parser_advance();
    int ans = 0;
    switch(token.type)
    {
        case TOKEN_NONE:
        case TOKEN_EOF:
            break;
        
        case TOKEN_LIT_NUMBER: {
            ans = token.value;
        } break;
        case TOKEN_PAREN_L: {
            int value = parser_parse_expr();
            if(parser_match(TOKEN_PAREN_R))
            {
                ans = value;
            }
            else
            {
                has_failed = true;
                fprintf(stderr, "Expected ')' at end of grouping expression.\n");
            }
        } break;
        default: {
            has_failed = true;
            fprintf(stderr, "Unknown token found in primary expression '%s' (%d, %d).\n", TokenTypeName[token.type], token.type, token.value);
        } break;
    }
    return ans;
}

static inline int parser_parse_expr_unary(void)
{
    // NOTE : In these 2 op cases we could actually return parse unary so that
    // we can support "- - 10", which is not the same as "--10", but it is
    // the same as "(-(-10))".
    
    if(parser_match(TOKEN_OP_PLUS))
    {
        return parser_parse_expr_primary();
    }
    
    if(parser_match(TOKEN_OP_MINUS))
    {
        return -parser_parse_expr_primary();
    }
    
    return parser_parse_expr_primary();
}

static inline int parser_parse_expr_pow(void)
{
    int l = 0, r = 0;
    l = parser_parse_expr_unary();
    while(parser_match(TOKEN_OP_POW))
    {
        Token op = parser_peek_previous();
        r = parser_parse_expr_unary();
        if(op.type == TOKEN_OP_POW) {
            l = powi(l, r);
        } else {
            has_failed = true;
            fprintf(stderr, "Wrong operator found in pow expr.\n");
        }
    }
    return l;
}

static inline int parser_parse_expr_muldiv(void)
{
    int l = 0, r = 0;
    l = parser_parse_expr_pow();
    while(parser_match(TOKEN_OP_STAR) || parser_match(TOKEN_OP_SLASH))
    {
        Token op = parser_peek_previous();
        r = parser_parse_expr_pow();
        switch(op.type)
        {
            case TOKEN_OP_STAR: {
                l = l * r;
            } break;
            case TOKEN_OP_SLASH: {
                l = l / r;
            } break;
            default: {
                has_failed = true;
                fprintf(stderr, "Wrong operator found in muldiv expr.\n");
            } break;
        }
    }
    return l;
}

static inline int parser_parse_expr_addsub(void)
{
    int l = 0, r = 0;
    l = parser_parse_expr_muldiv();
    while(parser_match(TOKEN_OP_PLUS) || parser_match(TOKEN_OP_MINUS))
    {
        Token op = parser_peek_previous();
        r = parser_parse_expr_muldiv();
        switch(op.type)
        {
            case TOKEN_OP_PLUS: {
                l = l + r;
            } break;
            case TOKEN_OP_MINUS: {
                l = l - r;
            } break;
            default: {
                has_failed = true;
                fprintf(stderr, "Wrong operator found in addsub expr.\n");
            } break;
        }
    }
    return l;
}

static inline int parser_parse_expr(void)
{
    return parser_parse_expr_addsub();
}

static inline int parser_parse(void)
{
    return parser_parse_expr();
}

int eval(char const *str)
{
    has_failed = false;
    
    scanner_source = NULL;
    scanner_source_length = 0;
    scanner_current = 0;
    scanner_start = 0;
    
    parser_tokens_length = 0;
    parser_current = 0;
    
    scanner_source = str;
    scanner_source_length = strlen(str);
    scanner_scan();
    
    int ans = parser_parse();
    
    return ans;
}

int main()
{
    char buf[1024];
    char d;
    while(1)
    {
        printf("\n> ");
        scanf("%1024[^\n]", buf);
        scanf("%c", &d);
        
        int ans = eval(buf);
        printf("%d\n", ans);
    }
    return 0;
}
