grammar ifcc;

axiom : prog EOF ;

prog : funcDecl+ ;

funcDecl : funcType funcName=ID LPAR (type ID)? (COMMA type ID)* RPAR block ;

block : LCUR stmt* RCUR ;
stmt : decl_stmt SEMICOLON
     | assign_stmt SEMICOLON
     | expr_stmt SEMICOLON
     | return_stmt SEMICOLON
     | funcCall_stmt SEMICOLON
     | if_stmt
     | block_stmt
     ;
     
decl_stmt : type decl_element (COMMA decl_element)* ;
decl_element : ID (ASSIGN expr)? ;

funcCall : ID LPAR expr? (COMMA expr)* RPAR ;

assign_stmt : ID ASSIGN expr ;
expr_stmt : expr ;
return_stmt : RETURN expr ;
funcCall_stmt : funcCall ;
if_stmt : IF LPAR expr RPAR block (ELSE block)? ;
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
        | CONSTINT # intExpr
        | CONSTCHAR # charExpr
        | LPAR expr RPAR # parExpr
        | ID ASSIGN expr # assignExpr
        | funcCall # funcCallExpr
        ;

mOp : STAR | SLASH | MOD ;
aOp : PLUS | MINUS ;
eqOp : EQ | NEQ ;
compOp : LT | LE | GT | GE ;

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
COMMA : ',' ;
RETURN : 'return' ;
IF : 'if' ;
ELSE : 'else' ;

type : INT | CHAR ;
funcType : INT | CHAR | VOID ;
INT : 'int' ;
CHAR : 'char' ;
VOID : 'void' ;

CONSTINT : '0' | [1-9][0-9]* ;
CONSTCHAR : '\''.*?'\'' ;

ID : [a-zA-Z][a-zA-Z0-9_]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);