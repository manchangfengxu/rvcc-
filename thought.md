# thought
## parse.c
### track
1,
// program = "{" compoundStmt
// compoundStmt = (declaration | stmt)* "}"
// declaration =
// program = (functionDefinition | globalVariable)*
// functionDefinition = declspec declarator "{" compoundStmt*
// declspec = "char" | "int"
// declarator = "*"* ident typeSuffix
// typeSuffix = "(" funcParams | "[" num "]" typeSuffix | ε
// funcParams = (param ("," param)*)? ")"
// param = declspec declarator

// compoundStmt = (declaration | stmt)* "}"
// declaration =
//    declspec (declarator ("=" expr)? ("," declarator ("=" expr)?)*)? ";"
// stmt = "return" expr ";"
//        | "if" "(" expr ")" stmt ("else" stmt)?
//        | "for" "(" exprStmt expr? ";" expr? ")" stmt
//        | "while" "(" expr ")" stmt
//        | "{" compoundStmt
//        | exprStmt
// exprStmt = expr? ";"
// expr = assign ("," expr)?
// assign = equality ("=" assign)?
// equality = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add = mul ("+" mul | "-" mul)*
// mul = unary ("*" unary | "/" unary)*
// unary = ("+" | "-" | "*" | "&") unary | postfix
// postfix = primary ("[" expr "]")*
// primary = "(" "{" stmt+ "}" ")"
//         | "(" expr ")"
//         | "sizeof" unary
//         | ident funcArgs?
//         | str
//         | num

// funcall = ident "(" (assign ("," assign)*)? ")"

根据语法解析，就是让上一层语法关键节点指向下一层，而下一层语法解析的返回应该是无法继续解析，equal(Tok,"?");没有对应，而不是检测到某个Tok为？

2,
### declaration
声明语句与stmt等价。声明可以声明时赋值。对多重指针进行解析。
例如int a,b=3;

### FUNCTION
ident的Token后面是"(",则为FUNCTION.
Ty->Name, Fn->Name, Node->Name
要为函数返回地址保留栈空间，ra寄存器。

分析后先存入Ty，（returnTy，param等）再分析Ty存入Function
### PRIMARY
primary函数可识别标识符，如函数调用，变量

### GLOBAL
全局变量等价函数
### Scope
Scope链表，Scp指针指向当前域
通过头插法建立和Next操作进入和出域（改变Scp指向）（新域在链表最前）
将Var放入当前Scope的VarScope里。
findVar会遍历Scp及其之后的表，找到Var。
{int x = 2; {x = 3;} return x;}得到的结果是3。
当进入二级域时{{}}，会遍历到一级域声明的x，并将其赋值为3。
{int x = 2;{int x = 3;}return x;}结果为2
二级域{{}}的x会

Next操作回到上一级Scope。

### ","运算
","运算，先左遍历再右遍历。","运算可和"="连用,在genAdder时加入","运算

## codegen.c

# error
## test.sh
``` sh
    assert(){
    expected="$1"
    input="$2"
    ./rvcc "$input" > tmp.s || exit 
    #我在input参数上加了“”，防止测试空格，意外使Argv！=2报错
    #.....
    }
```
## Rest和&Tok
###
``` c
static Node *compoundStmt(Token **Rest, Token *Tok);
static Node *exprStmt(Token **Rest, Token *Tok);
static Node *expr(Token **Rest, Token *Tok);

Token *Tok;

  compoundStmt(&Tok,Tok){

    stmt(&Tok, Tok){
      return exprStmt(Rest, Tok);
    }
  }

//exprStmt函数中的Rest指向的是compondStmt函数的Tok。
//当以&Tok传入，指向的是上一级函数的Tok；
/*当以Rest传入，传入的是上一级函数的Rest指向的Tok。指向上上一级的Tok.
一直以Rest传入，就可以一直指向最上层的Tok。
*/
```
###
``` c
static Node *stmt(Token **Rest, Token *Tok)
{
  // "return" expr ";"
  if (equal(Tok, "return"))
  {
    Node *Nd = newUnary(ND_RETURN, expr(&Tok, Tok->Next));
    *Rest = skip(Tok, ";");
    return Nd;
  }

  // 解析if语句
  // "if" "(" expr ")" stmt ("else" stmt)?  
  if(equal(Tok, "if")){
    //......
    
    // ("else" stmt)?，不符合条件后的语句
    if(equal(Tok, "else")){
      Nd->Els = stmt(&Tok, Tok->Next);
    }
    *Rest = Tok;
    return Nd;
  }

  if(equal(Tok,"for")){
    //......

    // stmt
    Nd->Then = stmt(Rest, Tok);
    return Nd;
  }

  // "{" compoundStmt
  if (equal(Tok, "{"))
  {
    return compoundStmt(Rest, Tok->Next);
  }

  // exprStmt
  return exprStmt(Rest, Tok);
}

/*有些传入&Tok,有些传入Rest。
1，传入&Tok意味着，这个函数返回后，我还要继续进行Tok解析。
2，而传入Rest则是，我直接return我的下一级函数返回的Nd了，我不给上一级函数更新Tokl，那么我就需要我的下一级函数帮我改变我的上一级函数的Tok。如此递归，知道某一级函数需要更新上一级Tok时，直接让它更新最上层函数，因为及时它return后它的所有上级函数也不更新Tok，而是一直return。
3，if语句中有*Rest = Tok；有可能没有else，所以我不能直接return我最后一个需要解析的。else中的stmt中也需要传值&Tok，因为接下来的*Rest = Tok；中的Tok需要更新，然后传给Rest给上一级更新。
*/
```

# errorThought
任何变量都要在声明时赋值！！！