# thought
## parse
1，每次进行节点与数字相连的时候，都按照expr，equality, relational,mul，unary，primary的顺序寻找节点，
以便将优先级更高的节点和数字进行连接，并返回自己的节点。
2,按照以上顺序寻找，左数一定可以与节点相连，右数则继续按照以上顺序进行递归。
3,递归的返回是遇到了同级或更低级的节点或其它，返回到上级节点后继续按序递归。
4,符号后是+-，+可忽略，-则建立节点，二者都继续unary递归，直到遇到primary返回

## codegen
    以a=3;a;为例
``` s
      .globl main
main:
  addi sp, sp, -8
  sd fp, 0(sp)      #在sp-8的地址存入fp地址
  mv fp, sp         #fp指向sp-8
  addi sp, sp, -208 #sp越过字母指向栈顶
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

# error
