grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' statement* return_stmt '}' ;

return_stmt: RETURN expression ';' ;

statement : declaration ';'
          | assignment ';'
          | expression ';' 
          ;

declaration : type IDENTIFIER ('=' expression)?;
assignment : IDENTIFIER '=' expression;

expression : 
            '(' expression ')'
           | CONST
           | IDENTIFIER
           | unary_operation_prefixe expression
           | expression unary_operation_suffixe
           | expression binary_operation expression 
           ;


binary_operation : '+' | '-' | '*' | '/' | '%' | '==' | '!=' | '<' | '<=' | '>' | '>=' | '&&' | '||' ;
unary_operation_prefixe : '-' | '!' | '&' | '*' | 'sizeof' | '++' | '--' ;
unary_operation_suffixe : '++' | '--' ;
type: 'int' | 'char' | 'void' ;






RETURN : 'return' ;
CONST : [0-9]+ ;
IDENTIFIER : [a-zA-Z_][a-zA-Z0-9_]* ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
