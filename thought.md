# thought
## parse.c
### track
1,// program = "{" compoundStmt
// compoundStmt = stmt* "}"
// stmt = "return" expr ";"
//        | "if" "(" expr ")" stmt ("else" stmt)?
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
### 以a = 3; a;举例
``` s
      .globl main
main:
  addi sp, sp, -8
  sd fp, 0(sp)      #在sp-8的地址存入fp地址
  mv fp, sp         #fp指向sp-8
  addi sp, sp, -208 #sp越过变量表指向栈顶（此处默认变量表大小为208）
  addi a0, fp, -8
  addi sp, sp, -8
  sd a0, 0(sp)      #fp-8计算变量地址，将其压栈
  li a0, 3          #立即数存入a0
  ld a1, 0(sp)  
  addi sp, sp, 8    #变量地址出栈
  sd a0, 0(a1)      #a0赋值到a1存的地址
  addi a0, fp, -8   #a0存入fp-8的地址，变量地址
  ld a0, 0(a0)      #取出变量的值
  mv sp, fp         
  ld fp, 0(sp)
  addi sp, sp, 8    #sp指向fp，fp指向sp(fp)内存的地址，sp指向sp+8
  ret
```
###
通过本地变量数在栈中分配变量表大小（进行内存对齐）

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
