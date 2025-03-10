grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' statement* return_stmt '}' ;

return_stmt: RETURN expression ';' ;

statement : declaration ';'
          | assignment ';'
          ;

declaration : type IDENTIFIER ('=' expression)?;
assignment : IDENTIFIER '=' expression;

expression : CONST
           | IDENTIFIER
           ;


type: 'int' | 'char' | 'void' ;






RETURN : 'return' ;
CONST : [0-9]+ ;
IDENTIFIER : [a-zA-Z_][a-zA-Z0-9_]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
