# thought
1，每次进行节点与数字相连的时候，都按照expr，equality, relational,mul，unary，primary的顺序寻找节点，
以便将优先级更高的节点和数字进行连接，并返回自己的节点。
2,按照以上顺序寻找，左数一定可以与节点相连，右数则继续按照以上顺序进行递归。
3,递归的返回是遇到了同级或更低级的节点或其它，返回到上级节点后继续按序递归。
4,符号后是+-，+可忽略，-则建立节点，二者都继续unary递归，直到遇到primary返回

# error
return放在while外，哭
```c
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

        //*Rest = Tok;
        //return Nd;
}
