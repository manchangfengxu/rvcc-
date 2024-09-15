# thought
## parse.c
### track
1，// program = stmt*
// stmt = exprStmt
// exprStmt = expr ";"
// expr = assign
// assign = equality ("=" assign)?
// equality = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add = mul ("+" mul | "-" mul)*
// mul = unary ("*" unary | "/" unary)*
// unary = ("+" | "-") unary | primary
// primary = "(" expr ")" | num
将优先级更高的节点和变量，数字...进行连接，并返回自己的节点。
2,按照以上顺序寻找，左数一定可以与节点相连，右数则继续按照以上顺序进行递归。
3,递归的返回是遇到了同级或更低级的节点或其它或无（此时要skip;），返回到上级节点后继续按序递归。
4,正负符号+-，+可忽略，-则建立节点，二者都继续unary递归，直到遇到primary返回
5，每个stmt都是一个小数跟（表达式结束;），每个stmt进行相连，Function的body节点指向最开始的树根。

### 
    用local指向本地变量链表头
## codegen.c
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
