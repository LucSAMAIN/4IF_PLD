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
decl_element : decl_var
             | decl_pointer
             | decl_array
             ;
decl_var : ID (ASSIGN expr)? ;
decl_pointer : STAR ID (ASSIGN expr)? ;
decl_array : ID LBRA CONSTINT RBRA ;

funcCall : ID LPAR expr? (COMMA expr)* RPAR ;

assign_stmt : lValue ASSIGN value=expr SEMICOLON ;
expr_stmt : expr SEMICOLON ;
return_stmt : RETURN expr SEMICOLON ;
funcCall_stmt : funcCall SEMICOLON ;
break_stmt : BREAK SEMICOLON ;
continue_stmt : CONTINUE SEMICOLON ;
if_stmt : IF LPAR expr RPAR block (ELSE block)? ;
while_stmt : WHILE LPAR expr RPAR block ;
block_stmt : block ;

lValue : ID                # lIdUse
       | ID LBRA expr RBRA # lArrayAccess
       | STAR expr         # lDereference
       ;

expr : LPAR expr RPAR                        # parExpr
     | CONSTINT                              # intExpr
     | CONSTCHAR                             # charExpr
     | CONSTDOUBLE                           # doubleExpr
     | ID                                    # idUse
     | funcCall                              # funcCallExpr
     | ID LBRA expr RBRA                     # arrayAccess
     | <assoc=right>unaryOp expr             # unaryExpr
     | left=expr mOp right=expr              # mulDivExpr
     | left=expr aOp right=expr              # addSubExpr
     | left=expr compOp right=expr           # compExpr
     | left=expr eqOp right=expr             # eqExpr
     | left=expr BITAND right=expr           # andExpr
     | left=expr XOR right=expr              # xorExpr
     | left=expr BITOR right=expr            # orExpr
     | left=expr LOGAND right=expr           # logAndExpr
     | left=expr LOGOR right=expr            # logOrExpr
     | <assoc=right>lValue ASSIGN value=expr # assignExpr
     ;

unaryOp : NOT | MINUS | BITAND | STAR ;
mOp : STAR | SLASH | MOD ;
aOp : PLUS | MINUS ;
eqOp : EQ | NEQ ;
compOp : LT | LE | GT | GE ;

LPAR : '(' ;
RPAR : ')' ;
LBRA : '[' ;
RBRA : ']' ;
LCUR : '{' ;
RCUR : '}' ;
PLUS : '+' ;
MINUS : '-' ;
XOR : '^' ;
LOGAND : '&&' ;
LOGOR : '||' ;
BITAND : '&' ;
BITOR : '|' ;
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