#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

typedef enum
{
    TK_PUNCT, // 操作符
    TK_NUM,   // 整数
    TK_EOF,   // 输入结束
} TokenKind;

typedef struct Token Token;
struct Token
{
    TokenKind Kind;
    Token *Next;
    int Val;   // value
    char *Loc; // location
    int Len;
};
// expr = mul ("+" mul | "-" mul)*表达式由乘数间加减得到
// mul = num ("*" primary | "/" primary)*乘数由基数乘除得到
// primary = num | "(" expr ")"基数由数字或者括号包裹的表达式得到

static char *CurrentInput;

static void error(const char *Fmt, ...);
static Token *newToken(TokenKind Kind, char *Start, char *End);
static Token *tokenize();         // 解析
static int getNumber(Token *Tok); // 获取数字
static bool equal(Token *Tok, char *Str);
static void verrorAt(char *Loc, char *Fmt, va_list VA);

static void error(const char *Fmt, ...)
{
    va_list VA;

    va_start(VA, Fmt);
    vfprintf(stderr, Fmt, VA);
    fprintf(stderr, "\n");
    va_end(VA);

    exit(1);
}

static void verrorAt(char *Loc, char *Fmt, va_list VA)
{
    fprintf(stderr, "%s\n", CurrentInput);

    int Pos = Loc - CurrentInput;
    fprintf(stderr, "%*s", Pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, Fmt, VA);
    fprintf(stderr, "\n");
    va_end(VA);
}
// 字符解析出错
static void errorAt(char *Loc, char *Fmt, ...)
{
    va_list VA;
    va_start(VA, Fmt);
    verrorAt(Loc, Fmt, VA);
    exit(1);
}

// Tok解析出错
static void errorTok(Token *Tok, char *Fmt, ...)
{
    va_list VA;
    va_start(VA, Fmt);
    verrorAt(Tok->Loc, Fmt, VA);
    exit(1);
}

static bool equal(Token *Tok, char *Str)
{
    return memcmp(Tok->Loc, Str, Tok->Len) ==
               0 &&
           Str[Tok->Len] == '\0';
}

// 跳过指定的Str
static Token *skip(Token *Tok, char *Str)
{
    if (!equal(Tok, Str))
    {
        errorTok(Tok, "expected '%s'", Str);
    }

    return Tok->Next;
}
static int getNumber(Token *Tok)
{
    if (Tok->Kind != TK_NUM)
        errorTok(Tok, "expected a number");
    return Tok->Val;
}

static Token *newToken(TokenKind Kind, char *Start, char *End)
{
    Token *Tok = calloc(1, sizeof(Token));
    // if (Tok == NULL)
    // {
    //     error("failed to allocate memory");
    // }

    Tok->Kind = Kind;
    Tok->Loc = Start;
    Tok->Len = End - Start;
    /*End - Start 表达式的结果是 ptrdiff_t 类型。
    ptrdiff_t 是一个用于表示指针差的标准类型，
    它是有符号的整数类型，通常用于指针运算的结果。*/
    return Tok;
}

// Token流构建

// 判断Str是否以SubStr开头
static bool startsWith(char *Str, char *SubStr)
{
    return strncmp(Str, SubStr, strlen(SubStr)) == 0;
}
// 读取操作符,数字代表操作符个数
static int readPunct(char *Ptr)
{
    if (startsWith(Ptr, "==") || startsWith(Ptr, "!=") ||
        startsWith(Ptr, "<=") || startsWith(Ptr, ">="))
    {
        return 2;
    }

    return ispunct(*Ptr) ? 1 : 0;
}
static Token *tokenize()
{
    char *P = CurrentInput;
    Token Head = {};
    Token *Cur = &Head;

    while (*P)
    {
        if (isspace(*P))
        {
            ++P;
            continue;
        }

        if (isdigit(*P))
        {
            Cur->Next = newToken(TK_NUM, P, P);
            Cur = Cur->Next;
            const char *OldPtr = P;
            Cur->Val = strtol(P, &P, 10);
            // strtol会保存解析p后的位置到p中
            Cur->Len = P - OldPtr;
            continue;
        }
        // 判断是否为标志符号
        int PunctLen = readPunct(P);
        if (PunctLen)
        {
            Cur->Next = newToken(TK_PUNCT, P, P + PunctLen);
            Cur = Cur->Next;
            P += PunctLen;
            continue;
        }
        // 处理无法识别的字符
        errorAt(P, "invalid token");
    }
    Cur->Next = newToken(TK_EOF, P, P);

    return Head.Next;
}

// AST的节点种类
typedef enum
{
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NEG, // 负号-
    ND_EQ,  // ==
    ND_NE,  // !=
    ND_LT,  // <
    ND_LE,  // <=
    ND_NUM, // 整形
} NodeKind;

// AST中二叉树节点
typedef struct Node Node;
struct Node
{
    NodeKind Kind; // 节点种类
    Node *LHS;     // 左部，left-hand side
    Node *RHS;     // 右部，right-hand side
    int Val;       // 存储ND_NUM种类的值
};
static Node *newNode(NodeKind Kind)
{
    Node *Nd = calloc(1, sizeof(Node));
    Nd->Kind = Kind;
    return Nd;
}

static Node *newUnary(NodeKind Kind, Node *Expr)
{
    Node *Nd = newNode(Kind);
    Nd->LHS = Expr;
    return Nd;
}

static Node *newBinary(NodeKind Kind, Node *LHS, Node *RHS)
{
    Node *Nd = newNode(Kind);
    Nd->LHS = LHS;
    Nd->RHS = RHS;
    return Nd;
}

static Node *newNum(int Val)
{
    Node *Nd = newNode(ND_NUM);
    Nd->Val = Val;
    return Nd;
}

// expr = equality
// equality = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add = mul ("+" mul | "-" mul)*
// mul = unary ("*" unary | "/" unary)*
// unary = ("+" | "-") unary | primary
// primary = "(" expr ")" | num
static Node *expr(Token **Rest, Token *Tok);
static Node *equality(Token **Rest, Token *Tok);
static Node *relational(Token **Rest, Token *Tok);
static Node *add(Token **Rest, Token *Tok);
static Node *mul(Token **Rest, Token *Tok);
static Node *unary(Token **Rest, Token *Tok);
static Node *primary(Token **Rest, Token *Tok);

// Rest改变Tok指向，Tok为当前要解析的Token

/*
1，每次进行节点与数字相连的时候，都按照expr，mul，unary，primary的顺序寻找节点，
以便将优先级更高的节点和数字进行连接，并返回自己的节点。
2,按照以上顺序寻找，左数一定可以与节点相连，右数则继续按照以上顺序进行递归。
3,递归的返回是遇到了同级或更低级的节点或其它，返回到上级节点后继续按序递归。
4,符号后是+-，+可忽略，-则建立节点，二者都继续unary递归，直到遇到primary返回
*/

static Node *expr(Token **Rest, Token *Tok)
{
    return equality(Rest, Tok);
}

static Node *equality(Token **Rest, Token *Tok)
{
    // relational
    Node *Nd = relational(&Tok, Tok);

    // ("==" relational | "!=" relational)*
    while (true)
    {
        // "==" relational
        if (equal(Tok, "=="))
        {
            Nd = newBinary(ND_EQ, Nd, relational(&Tok, Tok->Next));
            continue;
        }

        // "!=" relational
        if (equal(Tok, "!="))
        {
            Nd = newBinary(ND_NE, Nd, relational(&Tok, Tok->Next));
            continue;
        }

        *Rest = Tok;
        return Nd;
    }
}

static Node *relational(Token **Rest, Token *Tok)
{
    Node *Nd = add(&Tok, Tok);

    while (true)
    {
        if (equal(Tok, "<"))
        {
            Nd = newBinary(ND_LT, Nd, add(&Tok, Tok->Next));
            continue;
        }

        if (equal(Tok, "<="))
        {
            Nd = newBinary(ND_LE, Nd, add(&Tok, Tok->Next));
            continue;
        }

        if (equal(Tok, ">"))
        {
            Nd = newBinary(ND_LT, add(&Tok, Tok->Next), Nd);
            continue;
        }

        if (equal(Tok, ">="))
        {
            Nd = newBinary(ND_LE, add(&Tok, Tok->Next), Nd);
            continue;
        }
        *Rest = Tok;
        return Nd;
    }
}

static Node *add(Token **Rest, Token *Tok)
{
    Node *Nd = mul(&Tok, Tok);

    while (true)
    {
        if (equal(Tok, "+"))
        {
            Nd = newBinary(ND_ADD, Nd, mul(&Tok, Tok->Next));
            continue;
        }

        if (equal(Tok, "-"))
        {
            Nd = newBinary(ND_SUB, Nd, mul(&Tok, Tok->Next));
            continue;
        }
        *Rest = Tok;
        return Nd;
    }
}

static Node *mul(Token **Rest, Token *Tok)
{
    Node *Nd = unary(&Tok, Tok);

    while (true)
    {
        if (equal(Tok, "*"))
        {
            Nd = newBinary(ND_MUL, Nd, unary(&Tok, Tok->Next));
            continue;
        }

        if (equal(Tok, "/"))
        {
            Nd = newBinary(ND_DIV, Nd, unary(&Tok, Tok->Next));
            continue;
        }

        *Rest = Tok;
        return Nd;
    }
}

static Node *unary(Token **Rest, Token *Tok)
{
    if (equal(Tok, "+"))
    {
        return unary(Rest, Tok->Next);
    }

    if (equal(Tok, "-"))
    {
        return newUnary(ND_NEG, unary(Rest, Tok->Next));
    }

    return primary(Rest, Tok);
}

static Node *primary(Token **Rest, Token *Tok)
{
    if (equal(Tok, "("))
    {
        Node *Nd = expr(&Tok, Tok->Next);
        *Rest = skip(Tok, ")");
        return Nd;
    }

    if (Tok->Kind == TK_NUM)
    {
        Node *Nd = newNum(Tok->Val);
        *Rest = Tok->Next;
        return Nd;
    }

    errorTok(Tok, "expected expression");
    return NULL;
}

static int Depth;

static void push()
{
    printf("    addi sp, sp, -8\n");
    printf("    sd a0, 0(sp)\n");
    Depth++;
}

static void pop(char *Reg)
{
    printf("    ld %s, 0(sp)\n", Reg);
    printf("    addi sp, sp, 8\n");
    Depth--;
}

static void genExper(Node *Nd)
{
    switch (Nd->Kind)
    {
    case ND_NUM:
        printf("    li a0, %d\n", Nd->Val);
        return;
    case ND_NEG:
        genExper(Nd->LHS);
        printf("    neg a0, a0\n");
        return;
    default:
        break;
    }

    genExper(Nd->RHS);
    push();
    genExper(Nd->LHS);
    pop("a1");

    switch (Nd->Kind)
    {
    case ND_ADD:
        printf("    add a0, a0, a1\n");
        return;
    case ND_SUB:
        printf("    sub a0, a0, a1\n");
        return;
    case ND_MUL:
        printf("    mul a0, a0, a1\n");
        return;
    case ND_DIV:
        printf("    div a0, a0, a1\n");
        return;
    case ND_EQ:
    case ND_NE:
        // a0=a0^a1，异或指令,相等为0
        printf("    xor a0, a0, a1\n");
        if (Nd->Kind == ND_EQ)
        { // 置反为1
            printf("    seqz a0, a0\n");
        }
        else
        {
            printf("    snez a0, a0\n");
        }
        return;
    case ND_LT:
        printf("    slt a0, a0, a1\n");
        return;
    case ND_LE:
        printf("  slt a0, a1, a0\n");
        printf("  xori a0, a0, 1\n");
        return;
    default:
        break;
    }

    error("invalid expression");
}

int main(int Argc, char **Argv)
{
    if (Argc != 2)
    {
        error("%s: invalid number of arguments", Argv[0]);
    }

    CurrentInput = Argv[1];
    // error("Input");
    Token *Tok = tokenize();
    // error("tokeniz");
    Node *Nd = expr(&Tok, Tok);

    if (Tok->Kind != TK_EOF)
    {
        errorTok(Tok, "extra token");
    }

    // 声明一个全局main段，同时也是程序入口段
    printf("  .globl main\n");
    // main段标签
    printf("main:\n");

    genExper(Nd);
    printf("    ret\n");

    assert(Depth == 0);

    return 0;
}