flex lexical.l
gcc main.c -lfl lex.yy.c -o scanner
./scanner ../Test/test2.cmm
