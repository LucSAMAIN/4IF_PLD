grammar ifcc;

axiom : prog EOF ;

prog : funcDecl+ ;

funcDecl : funcType funcName=ID LPAR (type ID)? (COMMA type ID)* RPAR block ;

block : LCUR stmt* RCUR ;
stmt : decl_stmt
     | assign_stmt
     | expr_stmt
     | return_stmt
     | funcCall_stmt
     | continue_stmt
     | break_stmt
     | if_stmt
     | while_stmt 
     | block_stmt
     ;
     
decl_stmt : type decl_element (COMMA decl_element)* SEMICOLON ;
decl_element : ID (ASSIGN expr)? ;

funcCall : ID LPAR expr? (COMMA expr)* RPAR ;

assign_stmt : ID ASSIGN expr SEMICOLON ;
expr_stmt : expr SEMICOLON ;
return_stmt : RETURN expr SEMICOLON ;
funcCall_stmt : funcCall SEMICOLON ;
break_stmt : BREAK SEMICOLON ;
continue_stmt : CONTINUE SEMICOLON ;
if_stmt : IF LPAR expr RPAR block (ELSE block)? ;
while_stmt : WHILE LPAR expr RPAR block ;
block_stmt : block ;


expr : primary                     # primaryExpr
     | <assoc=right>NOT primary    # notExpr
     | <assoc=right>MINUS primary  # unaryMinusExpr
     | left=expr mOp right=expr    # mulDivExpr
     | left=expr aOp right=expr    # addSubExpr
     | left=expr compOp right=expr # compExpr
     | left=expr eqOp right=expr   # eqExpr
     | left=expr AND right=expr    # andExpr
     | left=expr XOR right=expr    # xorExpr
     | left=expr OR right=expr     # orExpr
     ;

primary : ID             # idUse
        | CONSTINT       # intExpr
        | CONSTCHAR      # charExpr
        | CONSTDOUBLE    # doubleExpr
        | LPAR expr RPAR # parExpr
        | ID ASSIGN expr # assignExpr
        | funcCall       # funcCallExpr
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
WHILE : 'while' ;
CONTINUE : 'continue' ;
BREAK : 'break' ;

type : INT | CHAR | DOUBLE ;
funcType : INT | CHAR | DOUBLE | VOID ;
INT : 'int' ;
CHAR : 'char' ;
VOID : 'void' ;
DOUBLE : 'double' ;

CONSTINT : '0' | [1-9][0-9]* ;
CONSTCHAR : '\''.*?'\'' ;
CONSTDOUBLE : [0-9]* '.' [0-9]+ ;

ID : [a-zA-Z][a-zA-Z0-9_]* ;
MULTILINE_COMMENT : '/*' .*? '*/' -> skip ;
SINGLELINE_COMMENT : '//' .*? '\n' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);