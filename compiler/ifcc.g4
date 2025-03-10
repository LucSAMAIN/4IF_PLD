grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' block '}' ;

block : stmt* ;
stmt : type lValue ('=' expr)? ';' # decl_stmt
       | lValue '=' expr ';' # assign_stmt
       | RETURN expr ';' # return_stmt
       ;
type : 'int'
     ;

RETURN : 'return' ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);

lValue : ID ;
ID : [a-zA-Z][a-zA-Z0-9_]* ;

/** expressions */
expr : expr '*' expr # mulDivExpr
     | expr AOP expr # addSubExpr
     | '-' expr # unarySubExpr
     | ID # idUse
     | INT # int
     | '(' expr ')' #parExpr
     ;
AOP : '+'
    | '-'
    ;

INT : [0] | [1-9][0-9]* ;
