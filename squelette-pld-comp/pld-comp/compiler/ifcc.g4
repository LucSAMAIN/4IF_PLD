grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' statement* return_stmt '}' ;

return_stmt: RETURN CONST ';' ;

statement : declaration ';'
          | assignment ';'
          ;

declaration : TYPE IDENTIFIER ('=' expression)?;
assignment : IDENTIFIER '=' expression;

expression : CONST
           | IDENTIFIER
           ;


TYPE : 'int' | 'char' | 'void' ;
RETURN : 'return' ;
CONST : [0-9]+ ;
IDENTIFIER : [a-zA-Z_][a-zA-Z0-9_]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
