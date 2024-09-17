# thought
## parse.c
### track
1,
// program = "{" compoundStmt
// compoundStmt = stmt* "}"
// stmt = "return" expr ";"
//        | "if" "(" expr ")" stmt ("else" stmt)?
//        | "for" "(" exprStmt expr? ";" expr? ")" stmt
//        | "while" "(" expr ")" stmt
//        | "{" compoundStmt
//        | exprStmt
// exprStmt = expr? ";"
// expr = assign
// assign = equality ("=" assign)?
// equality = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add = mul ("+" mul | "-" mul)*
// mul = unary ("*" unary | "/" unary)*
// unary = ("+" | "-") unary | primary
// primary = "(" expr ")" | ident | num

2,
### 
    用local指向本地变量链表头Z
## codegen.c
### 以{a = 3; a;}举例
``` s
.globl main          # 声明 main 函数为全局，其他文件可见
main:                # main 函数的标签
  addi sp, sp, -8    # 为栈指针 (sp) 减去 8 个字节，腾出空间保存旧的帧指针
  sd fp, 0(sp)       # 将当前帧指针 (fp) 的值存储到栈顶位置，即保存旧帧指针
  mv fp, sp          # 将当前栈指针的值复制到帧指针 (fp)，设置新的栈帧
  addi sp, sp, -16   # 为栈指针 (sp) 减去 16 个字节，腾出空间用于局部变量
  addi a0, fp, -8    # 将帧指针减去 8，将局部变量的地址存入寄存器 a0
  addi sp, sp, -8    # 为栈指针 (sp) 减去 8 个字节，腾出空间用于保存 a0
  sd a0, 0(sp)       # 将寄存器 a0 的值（局部变量地址）存储到栈顶
  li a0, 3           # 将立即数 3 加载到寄存器 a0，准备将值 3 写入局部变量
  ld a1, 0(sp)       # 从栈顶加载局部变量地址到寄存器 a1
  addi sp, sp, 8     # 栈指针增加 8 字节，释放用于保存局部变量地址的空间
  sd a0, 0(a1)       # 将寄存器 a0（值为 3）存储到局部变量地址中（即将 3 写入局部变量）
  addi a0, fp, -8    # 将帧指针减去 8，再次将局部变量的地址存入a0
  ld a0, 0(a0)       # 从局部变量地址中加载值到 a0（a0 此时将会3）
.L.return:           # return 标签，表示函数返回部分的起始
  mv sp, fp          # 将帧指针的值复制回栈指针，恢复之前的栈状态
  ld fp, 0(sp)       # 从栈中加载旧帧指针的值并恢复帧指针
  addi sp, sp, 8     # 栈指针增加 8 字节，释放保存旧帧指针的空间
  ret                # 返回，结束函数

```

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

