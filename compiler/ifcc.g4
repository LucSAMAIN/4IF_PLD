grammar ifcc;

axiom : prog EOF ;

prog : INT MAIN LPAR RPAR block ;

block : LCUR stmt* RCUR ;
stmt : decl_stmt SEMICOLON
     | assign_stmt SEMICOLON
     | expr_stmt SEMICOLON
     | return_stmt SEMICOLON
     | block_stmt
     ;
     
decl_stmt : type ID (ASSIGN expr)? ;
assign_stmt : ID ASSIGN expr ;
expr_stmt : expr ;
return_stmt : RETURN expr ;
block_stmt : block ;

expr : primary # primaryExpr
     | <assoc=right>NOT primary # notExpr
     | <assoc=right>MINUS primary # unaryMinusExpr
     | left=expr mOp right=expr # mulDivExpr
     | left=expr aOp right=expr # addSubExpr
     | left=expr compOp right=expr # compExpr
     | left=expr eqOp right=expr # eqExpr
     | left=expr AND right=expr # andExpr
     | left=expr XOR right=expr # xorExpr
     | left=expr OR right=expr # orExpr
     ;

primary : ID # idUse
        | CONST # const
        | LPAR expr RPAR # parExpr
        ;

mOp : STAR | SLASH | MOD ;
aOp : PLUS | MINUS ;
eqOp : EQ | NEQ ;
compOp : LT | LE | GT | GE ;

MAIN : 'main' ;
LPAR : '(' ;
RPAR : ')' ;
LCUR : '{' ;
RCUR : '}' ;
PLUS : '+' ;
MINUS : '-' ;
XOR : '^' ;
AND : '&' ;
OR : '|' ;
EQ : '==' ;
NEQ : '!=' ;
GT : '>' ;
GE : '>=' ;
LT : '<' ;
LE : '<=' ;
NOT : '!' ;
STAR : '*' ;
SLASH : '/' ;
MOD : '%' ;
ASSIGN : '=' ;
SEMICOLON : ';' ;
RETURN : 'return' ;
type : INT | CHAR ;
INT : 'int' ;
CHAR : 'char' ;
CONST : '0' | [1-9][0-9]* | '\''.*?'\'' ;
ID : [a-zA-Z][a-zA-Z0-9_]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);