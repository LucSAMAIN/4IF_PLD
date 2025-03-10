grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' statement* return_stmt '}' ;

return_stmt: RETURN CONST ';' ;

statement : declaration ';'
          | assignment ';'
          ;

declaration : 'int' IDENTIFIER ('=' expression)?;
assignment : IDENTIFIER '=' expression;

expression : CONST
           | IDENTIFIER
           ;



RETURN : 'return' ;
CONST : [0-9]+ ;
IDENTIFIER : [a-zA-Z][a-zA-Z0-9]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
