#include <Parser/Lexer.h>

enum State
{
    State_IGNORE,
    State_S,
    State_WORD,
    State_INT,
    State_FLOAT0,
    State_FLOAT1,
    State_FLOAT2,
    State_FLOAT3,
    State_FLOAT4,
    State_STRING0,
    State_STRING1,
    State_STRING2,
    State_STRING3,
    State_ARROW,
    State_LT,
    State_GT,
    State_EQ,
    State_EXCLAMATION,
    State_CHAR0,
    State_CHAR1,
    State_CHAR2,
    State_CHAR3,
    State_CHAR4,
};

Map keywords;

void Lexer_init()
{
    keywords = Map_create(sizeof(int));

    String s_if = String_new("if", 2);
    int t_if = Tag_IF;
    Map_set(&keywords, s_if, &t_if);

    String s_else = String_new("else", 4);
    int t_else = Tag_ELSE;
    Map_set(&keywords, s_else, &t_else);

    String s_while = String_new("while", 5);
    int t_while = Tag_WHILE;
    Map_set(&keywords, s_while, &t_while);

    String s_return = String_new("return", 6);
    int t_return = Tag_RETURN;
    Map_set(&keywords, s_return, &t_return);

    String s_continue = String_new("continue", 8);
    int t_continue = Tag_CONTINUE;
    Map_set(&keywords, s_continue, &t_continue);

    String s_break = String_new("break", 5);
    int t_break = Tag_BREAK;
    Map_set(&keywords, s_break, &t_break);

    String s_var = String_new("var", 3);
    int t_var = Tag_VAR;
    Map_set(&keywords, s_var, &t_var);

    String s_const = String_new("const", 5);
    int t_const = Tag_CONST;
    Map_set(&keywords, s_const, &t_const);

    String s_type = String_new("type", 4);
    int t_type = Tag_TYPE;
    Map_set(&keywords, s_type, &t_type);

    String s_use = String_new("use", 3);
    int t_use = Tag_USE;
    Map_set(&keywords, s_use, &t_use);

    String s_as = String_new("as", 2);
    int t_as = Tag_AS;
    Map_set(&keywords, s_as, &t_as);

    String s_import = String_new("import", 6);
    int t_import = Tag_IMPORT;
    Map_set(&keywords, s_import, &t_import);

    String s_export = String_new("export", 6);
    int t_export = Tag_EXPORT;
    Map_set(&keywords, s_export, &t_export);

    String s_void = String_new("void", 4);
    int t_void = Tag_VOID;
    Map_set(&keywords, s_void, &t_void);

    String s_s_cast = String_new("$cast", 5);
    int t_s_cast = Tag_S_CAST;
    Map_set(&keywords, s_s_cast, &t_s_cast);

    String s_s_size = String_new("$size", 5);
    int t_s_size = Tag_S_SIZE;
    Map_set(&keywords, s_s_size, &t_s_size);

    String s_function = String_new("function", 8);
    int t_function = Tag_FUNCTION;
    Map_set(&keywords, s_function, &t_function);
}

void Lexer_free()
{
    Map_destroy(&keywords);
}

int is_keyword(String key)
{
    return Map_get(&keywords, key) ? 1 : 0;
}

int get_keyword(String key)
{
    return *(int*)Map_get(&keywords, key);
}

static U32 code_page_from_buffer(const char* buffer)
{
    U32 code_page;

    switch (buffer[0] & 0xF0)
    {
        case 0xC0:
        case 0xD0:
            code_page = ((buffer[0] & 0x1F) << 6) + (buffer[1] & 0x3F);
            break;

        case 0xE0:
            code_page = ((buffer[0] & 0x0F) << 12) + ((buffer[1] & 0x3F) << 6) +
                        (buffer[2] & 0x3F);
            break;

        case 0xF0:
            code_page = ((buffer[0] & 0x07) << 18) +
                        ((buffer[1] & 0x3F) << 12) + ((buffer[2] & 0x3F) << 6) +
                        (buffer[3] & 0x3F);
            break;

        default:
            code_page = buffer[0];
    }

    return code_page;
}

static int is_start(const char* buffer)
{
    return UCD[code_page_from_buffer(buffer)].xid_start;
}

static int is_continue(const char* buffer)
{
    return UCD[code_page_from_buffer(buffer)].xid_continue;
}

static int is_decimal(const char* buffer)
{
    return UCD[code_page_from_buffer(buffer)].numeric_type == Numeric_Type_De;
}

static char numeric_value_to_ascii(const char* buffer)
{
    return '0' + UCD[code_page_from_buffer(buffer)].numeric_value;
}

Lexer Lexer_create(const char* buffer, String path)
{
    Lexer self;
    self.buffer = buffer;
    self.path = path;
    self.at = 0;
    self.ignore = 0;
    self.lexeme = Stack_create(sizeof(char));
    return self;
}

void Lexer_destroy(Lexer* self)
{
    Stack_destroy(&self->lexeme);
}

void Lexer_get(Lexer* self)
{
    if (self->ignore)
    {
        self->ignore = 0;
        return;
    }

    switch (self->buffer[self->at] & 0xF0)
    {
        case 0xF0:
            self->at += 1;
        case 0xE0:
            self->at += 1;
        case 0xC0:
        case 0xD0:
            self->at += 1;
        default:
            self->at += 1;
    }
}

void Lexer_unget(Lexer* self)
{
    if (self->at == 0)
    {
        self->ignore = 1;
        return;
    }

    self->at -= 1;
    for (;;)
    {
        switch (self->buffer[self->at] & 0xC0)
        {
            case 0x80:
                self->at -= 1;
                break;
            default:
                return;
        }
    }
}

Token Lexer_next(Lexer* self)
{
    char c;
    char hex = 0;
    size_t begin = 0;
    Token token;
    char p;

    token.lexeme = String_new("", 0);

    int state = State_IGNORE;

    Stack_clear(&self->lexeme);

    for (int step = 1; step; Lexer_get(self))
    {
        switch (state)
        {
            case State_IGNORE:
            {
                switch (c = self->buffer[self->at])
                {
                    case '\n':
                    case ' ':
                    case '\t':
                    case '\v':
                    case '\r':
                    case '\f':
                        break;

                    default:
                        begin = self->at;
                        Lexer_unget(self);
                        state = State_S;
                }

                break;
            }

            case State_S:
            {
                switch (c = self->buffer[self->at])
                {
                    case 0:
                        Lexer_unget(self);
                        token.tag = Tag_END;
                        step = 0;
                        break;

                    case '.':
                        state = State_FLOAT0;
                        p = '.';
                        Stack_push(&self->lexeme, &p);
                        break;

                    case '"':
                        state = State_STRING0;
                        break;

                    case '\'':
                        state = State_CHAR0;
                        break;

                    case '-':
                        state = State_ARROW;
                        break;

                    case '=':
                        state = State_EQ;
                        break;

                    case '<':
                        state = State_LT;
                        break;

                    case '>':
                        state = State_GT;
                        break;

                    case '!':
                        state = State_EXCLAMATION;
                        break;

                    default:
                        if (c == '_' || c == '$' ||
                            is_start(self->buffer + self->at))
                        {
                            state = State_WORD;
                        }
                        else if (is_decimal(self->buffer + self->at))
                        {
                            p = numeric_value_to_ascii(self->buffer + self->at);
                            Stack_push(&self->lexeme, &p);
                            state = State_INT;
                        }
                        else
                        {
                            int width = 0;

                            switch (self->buffer[self->at] & 0xF0)
                            {
                                case 0xF0:
                                    width += 1;
                                case 0xE0:
                                    width += 1;
                                case 0xC0:
                                case 0xD0:
                                    width += 1;
                                default:
                                    width += 1;
                            }

                            if (width > 1)
                            {
                                token.tag = Tag_UNKNOW;
                                token.lexeme =
                                    String_new(self->buffer + begin,
                                               self->at + width - begin);
                            }
                            else
                            {
                                token.tag = c;
                            }

                            step = 0;
                        }
                }

                break;
            }

            case State_WORD:
            {
                if (self->buffer[self->at] != '_' &&
                    self->buffer[self->at] != '$' &&
                    !is_continue(self->buffer + self->at))
                {
                    String string =
                        String_new(self->buffer + begin, self->at - begin);

                    if (is_keyword(string))
                    {
                        token.tag = get_keyword(string);
                    }
                    else
                    {
                        token.tag = Tag_ID;
                    }

                    token.lexeme = string;
                    Lexer_unget(self);
                    step = 0;
                }

                break;
            }

            case State_INT:
            {
                if (is_decimal(self->buffer + self->at))
                {
                    p = numeric_value_to_ascii(self->buffer + self->at);
                    Stack_push(&self->lexeme, &p);
                    break;
                }

                switch (c = self->buffer[self->at])
                {
                    case '.':
                        p = '.';
                        Stack_push(&self->lexeme, &p);
                        state = State_FLOAT1;
                        break;

                    case 'e':
                    case 'E':
                        p = 'E';
                        Stack_push(&self->lexeme, &p);
                        state = State_FLOAT2;
                        break;

                    default:
                        token.tag = Tag_LITERAL_INTEGER;
                        token.lexeme =
                            String_new(self->lexeme.buffer, self->lexeme.size);
                        Lexer_unget(self);
                        step = 0;
                }

                break;
            }

            case State_FLOAT0:
            {
                if (is_decimal(self->buffer + self->at))
                {
                    p = numeric_value_to_ascii(self->buffer + self->at);
                    Stack_push(&self->lexeme, &p);
                    state = State_FLOAT1;
                }
                else
                {
                    Lexer_unget(self);
                    token.tag = '.';
                    step = 0;
                }

                break;
            }

            case State_FLOAT1:
            {
                if (is_decimal(self->buffer + self->at))
                {
                    p = numeric_value_to_ascii(self->buffer + self->at);
                    Stack_push(&self->lexeme, &p);
                    break;
                }

                switch (c = self->buffer[self->at])
                {
                    case 'e':
                    case 'E':
                        p = 'E';
                        Stack_push(&self->lexeme, &p);
                        state = State_FLOAT2;
                        break;

                    default:
                        token.tag = Tag_LITERAL_REAL;
                        token.lexeme =
                            String_new(self->lexeme.buffer, self->lexeme.size);
                        Lexer_unget(self);
                        step = 0;
                }

                break;
            }

            case State_FLOAT2:
            {
                if (is_decimal(self->buffer + self->at))
                {
                    p = numeric_value_to_ascii(self->buffer + self->at);
                    Stack_push(&self->lexeme, &p);
                    state = State_FLOAT4;
                }
                else
                {
                    switch (c = self->buffer[self->at])
                    {
                        case '+':
                            p = '+';
                            Stack_push(&self->lexeme, &p);
                            state = State_FLOAT3;
                            break;

                        case '-':
                            p = '-';
                            Stack_push(&self->lexeme, &p);
                            state = State_FLOAT3;
                            break;

                        default:
                            token.tag = Tag_UNKNOW;
                            token.lexeme = String_new(self->buffer + begin,
                                                      self->at - begin);
                            Lexer_unget(self);
                            step = 0;
                    }
                }

                break;
            }

            case State_FLOAT3:
            {
                if (is_decimal(self->buffer + self->at))
                {
                    p = numeric_value_to_ascii(self->buffer + self->at);
                    Stack_push(&self->lexeme, &p);
                    state = State_FLOAT4;
                }
                else
                {
                    token.tag = Tag_UNKNOW;
                    token.lexeme =
                        String_new(self->buffer + begin, self->at - begin);
                    Lexer_unget(self);
                    step = 0;
                }

                break;
            }

            case State_FLOAT4:
            {
                if (is_decimal(self->buffer + self->at))
                {
                    p = numeric_value_to_ascii(self->buffer + self->at);
                    Stack_push(&self->lexeme, &p);
                }
                else
                {
                    token.tag = Tag_LITERAL_REAL;
                    token.lexeme =
                        String_new(self->lexeme.buffer, self->lexeme.size);
                    Lexer_unget(self);
                    step = 0;
                }

                break;
            }

            case State_CHAR0:
            {
                switch (c = self->buffer[self->at])
                {
                    case '\\':
                        state = State_CHAR1;
                        break;

                    default:
                        Stack_push(&self->lexeme, &c);
                        state = State_CHAR4;
                }

                break;
            }

            case State_CHAR1:
            {
                switch (c = self->buffer[self->at])
                {
                    case 'v':
                        p = '\v';
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case 't':
                        p = '\t';
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case 'n':
                        p = '\n';
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case 'r':
                        p = '\r';
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case 'f':
                        p = '\f';
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case '\\':
                        p = '\\';
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case '\'':
                        p = '\'';
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case '"':
                        p = '"';
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case 'x':
                        state = State_CHAR2;
                        break;

                    default:
                        token.tag = Tag_UNKNOW;
                        token.lexeme =
                            String_new(self->buffer + begin, self->at - begin);
                        Lexer_unget(self);
                        step = 0;
                }

                break;
            }

            case State_CHAR2:
            {
                switch (c = self->buffer[self->at])
                {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        hex = (c - '0') << 4;
                        state = State_CHAR3;
                        break;

                    case 'a':
                    case 'A':
                        hex = 0xA0;
                        state = State_CHAR3;
                        break;

                    case 'b':
                    case 'B':
                        hex = 0xB0;
                        state = State_CHAR3;
                        break;

                    case 'c':
                    case 'C':
                        hex = 0xC0;
                        state = State_CHAR3;
                        break;

                    case 'd':
                    case 'D':
                        hex = 0xD0;
                        state = State_CHAR3;
                        break;

                    case 'e':
                    case 'E':
                        hex = 0xE0;
                        state = State_CHAR3;
                        break;

                    case 'f':
                    case 'F':
                        hex = 0xF0;
                        state = State_CHAR3;
                        break;

                    default:
                        token.tag = Tag_UNKNOW;
                        token.lexeme =
                            String_new(self->buffer + begin, self->at - begin);
                        Lexer_unget(self);
                        step = 0;
                }

                break;
            }

            case State_CHAR3:
            {
                switch (c = self->buffer[self->at])
                {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        p = hex + (c - '0');
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case 'a':
                    case 'A':
                        p = hex + 0x0A;
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case 'b':
                    case 'B':
                        p = hex + 0x0B;
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case 'c':
                    case 'C':
                        p = hex + 0x0C;
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case 'd':
                    case 'D':
                        p = hex + 0x0D;
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case 'e':
                    case 'E':
                        p = hex + 0x0E;
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    case 'f':
                    case 'F':
                        p = hex + 0x0F;
                        Stack_push(&self->lexeme, &p);
                        state = State_CHAR4;
                        break;

                    default:
                        token.tag = Tag_UNKNOW;
                        token.lexeme =
                            String_new(self->buffer + begin, self->at - begin);
                        Lexer_unget(self);
                        step = 0;
                }

                break;
            }

            case State_CHAR4:
            {
                switch (c = self->buffer[self->at])
                {
                    case '\'':
                        token.tag = Tag_LITERAL_CHAR;
                        token.lexeme =
                            String_new(self->lexeme.buffer, self->lexeme.size);
                        step = 0;
                        break;

                    default:
                        token.tag = Tag_UNKNOW;
                        token.lexeme =
                            String_new(self->buffer + begin, self->at - begin);
                        Lexer_unget(self);
                        step = 0;
                }

                break;
            }

            case State_STRING0:
            {
                switch (c = self->buffer[self->at])
                {
                    case '\\':
                        state = State_STRING1;
                        break;

                    case '"':
                        token.tag = Tag_LITERAL_STRING;
                        token.lexeme =
                            String_new(self->lexeme.buffer, self->lexeme.size);
                        step = 0;
                        break;

                    case '\n':
                    default:
                        Stack_push(&self->lexeme, &c);
                }

                break;
            }

            case State_STRING1:
            {
                switch (c = self->buffer[self->at])
                {
                    case 'v':
                        p = '\v';
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case 't':
                        p = '\t';
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case 'n':
                        p = '\n';
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case 'r':
                        p = '\r';
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case 'f':
                        p = '\f';
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case '\\':
                        p = '\\';
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case '\'':
                        p = '\'';
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case '"':
                        p = '\"';
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case 'x':
                        state = State_STRING2;
                        break;

                    default:
                        token.tag = Tag_UNKNOW;
                        token.lexeme =
                            String_new(self->buffer + begin, self->at - begin);
                        Lexer_unget(self);
                        step = 0;
                }

                break;
            }

            case State_STRING2:
            {
                switch (c = self->buffer[self->at])
                {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        hex = (c - '0') << 4;
                        state = State_STRING3;
                        break;

                    case 'a':
                    case 'A':
                        hex = 0xA0;
                        state = State_STRING3;
                        break;

                    case 'b':
                    case 'B':
                        hex = 0xB0;
                        state = State_STRING3;
                        break;

                    case 'c':
                    case 'C':
                        hex = 0xC0;
                        state = State_STRING3;
                        break;

                    case 'd':
                    case 'D':
                        hex = 0xD0;
                        state = State_STRING3;
                        break;

                    case 'e':
                    case 'E':
                        hex = 0xE0;
                        state = State_STRING3;
                        break;

                    case 'f':
                    case 'F':
                        hex = 0xF0;
                        state = State_STRING3;
                        break;

                    default:
                        token.tag = Tag_UNKNOW;
                        token.lexeme =
                            String_new(self->buffer + begin, self->at - begin);
                        Lexer_unget(self);
                        step = 0;
                }

                break;
            }

            case State_STRING3:
            {
                switch (c = self->buffer[self->at])
                {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        p = hex + (c - '0');
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case 'a':
                    case 'A':
                        p = hex + 0x0A;
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case 'b':
                    case 'B':
                        p = hex + 0x0B;
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case 'c':
                    case 'C':
                        p = hex + 0x0C;
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case 'd':
                    case 'D':
                        p = hex + 0x0D;
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case 'e':
                    case 'E':
                        p = hex + 0x0E;
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    case 'f':
                    case 'F':
                        p = hex + 0x0F;
                        Stack_push(&self->lexeme, &p);
                        state = State_STRING0;
                        break;

                    default:
                        token.tag = Tag_UNKNOW;
                        token.lexeme =
                            String_new(self->buffer + begin, self->at - begin);
                        Lexer_unget(self);
                        step = 0;
                }

                break;
            }

            case State_ARROW:
            {
                switch (c = self->buffer[self->at])
                {
                    case '>':
                        token.tag = Tag_ARROW;
                        step = 0;
                        break;

                    default:
                        Lexer_unget(self);
                        token.tag = '-';
                        step = 0;
                }

                break;
            }

            case State_LT:
            {
                switch (c = self->buffer[self->at])
                {
                    case '<':
                        token.tag = Tag_LSHIFT;
                        step = 0;
                        break;

                    case '=':
                        token.tag = Tag_LE;
                        step = 0;
                        break;

                    default:
                        Lexer_unget(self);
                        token.tag = '<';
                        step = 0;
                }

                break;
            }

            case State_GT:
            {
                switch (c = self->buffer[self->at])
                {
                    case '>':
                        token.tag = Tag_RSHIFT;
                        step = 0;
                        break;

                    case '=':
                        token.tag = Tag_GE;
                        step = 0;
                        break;

                    default:
                        Lexer_unget(self);
                        token.tag = '>';
                        step = 0;
                }

                break;
            }

            case State_EQ:
            {
                switch (c = self->buffer[self->at])
                {
                    case '=':
                        token.tag = Tag_EQ;
                        step = 0;
                        break;

                    default:
                        Lexer_unget(self);
                        token.tag = '=';
                        step = 0;
                }

                break;
            }

            case State_EXCLAMATION:
            {
                switch (c = self->buffer[self->at])
                {
                    case '=':
                        token.tag = Tag_NE;
                        step = 0;
                        break;

                    default:
                        Lexer_unget(self);
                        token.tag = '!';
                        step = 0;
                }

                break;
            }
        }
    }

    token.path = self->path;
    token.begin = begin;
    token.end = token.tag == Tag_END ? self->at : self->at - 1;

    return token;
}
