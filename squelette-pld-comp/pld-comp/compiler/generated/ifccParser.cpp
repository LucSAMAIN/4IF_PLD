
// Generated from ifcc.g4 by ANTLR 4.7.2


#include "ifccVisitor.h"

#include "ifccParser.h"


using namespace antlrcpp;
using namespace antlr4;

ifccParser::ifccParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

ifccParser::~ifccParser() {
  delete _interpreter;
}

std::string ifccParser::getGrammarFileName() const {
  return "ifcc.g4";
}

const std::vector<std::string>& ifccParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& ifccParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- AxiomContext ------------------------------------------------------------------

ifccParser::AxiomContext::AxiomContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ifccParser::ProgContext* ifccParser::AxiomContext::prog() {
  return getRuleContext<ifccParser::ProgContext>(0);
}

tree::TerminalNode* ifccParser::AxiomContext::EOF() {
  return getToken(ifccParser::EOF, 0);
}


size_t ifccParser::AxiomContext::getRuleIndex() const {
  return ifccParser::RuleAxiom;
}

antlrcpp::Any ifccParser::AxiomContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitAxiom(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::AxiomContext* ifccParser::axiom() {
  AxiomContext *_localctx = _tracker.createInstance<AxiomContext>(_ctx, getState());
  enterRule(_localctx, 0, ifccParser::RuleAxiom);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(18);
    prog();
    setState(19);
    match(ifccParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ProgContext ------------------------------------------------------------------

ifccParser::ProgContext::ProgContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ifccParser::Return_stmtContext* ifccParser::ProgContext::return_stmt() {
  return getRuleContext<ifccParser::Return_stmtContext>(0);
}

std::vector<ifccParser::StatementContext *> ifccParser::ProgContext::statement() {
  return getRuleContexts<ifccParser::StatementContext>();
}

ifccParser::StatementContext* ifccParser::ProgContext::statement(size_t i) {
  return getRuleContext<ifccParser::StatementContext>(i);
}


size_t ifccParser::ProgContext::getRuleIndex() const {
  return ifccParser::RuleProg;
}

antlrcpp::Any ifccParser::ProgContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitProg(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::ProgContext* ifccParser::prog() {
  ProgContext *_localctx = _tracker.createInstance<ProgContext>(_ctx, getState());
  enterRule(_localctx, 2, ifccParser::RuleProg);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(21);
    match(ifccParser::T__0);
    setState(22);
    match(ifccParser::T__1);
    setState(23);
    match(ifccParser::T__2);
    setState(24);
    match(ifccParser::T__3);
    setState(25);
    match(ifccParser::T__4);
    setState(29);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << ifccParser::T__0)
      | (1ULL << ifccParser::T__2)
      | (1ULL << ifccParser::T__10)
      | (1ULL << ifccParser::T__11)
      | (1ULL << ifccParser::CONST)
      | (1ULL << ifccParser::IDENTIFIER))) != 0)) {
      setState(26);
      statement();
      setState(31);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(32);
    return_stmt();
    setState(33);
    match(ifccParser::T__5);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Return_stmtContext ------------------------------------------------------------------

ifccParser::Return_stmtContext::Return_stmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ifccParser::Return_stmtContext::RETURN() {
  return getToken(ifccParser::RETURN, 0);
}

ifccParser::ExpressionContext* ifccParser::Return_stmtContext::expression() {
  return getRuleContext<ifccParser::ExpressionContext>(0);
}


size_t ifccParser::Return_stmtContext::getRuleIndex() const {
  return ifccParser::RuleReturn_stmt;
}

antlrcpp::Any ifccParser::Return_stmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitReturn_stmt(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::Return_stmtContext* ifccParser::return_stmt() {
  Return_stmtContext *_localctx = _tracker.createInstance<Return_stmtContext>(_ctx, getState());
  enterRule(_localctx, 4, ifccParser::RuleReturn_stmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(35);
    match(ifccParser::RETURN);
    setState(36);
    expression(0);
    setState(37);
    match(ifccParser::T__6);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatementContext ------------------------------------------------------------------

ifccParser::StatementContext::StatementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ifccParser::DeclarationContext* ifccParser::StatementContext::declaration() {
  return getRuleContext<ifccParser::DeclarationContext>(0);
}

ifccParser::AssignmentContext* ifccParser::StatementContext::assignment() {
  return getRuleContext<ifccParser::AssignmentContext>(0);
}

ifccParser::ExpressionContext* ifccParser::StatementContext::expression() {
  return getRuleContext<ifccParser::ExpressionContext>(0);
}


size_t ifccParser::StatementContext::getRuleIndex() const {
  return ifccParser::RuleStatement;
}

antlrcpp::Any ifccParser::StatementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitStatement(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::StatementContext* ifccParser::statement() {
  StatementContext *_localctx = _tracker.createInstance<StatementContext>(_ctx, getState());
  enterRule(_localctx, 6, ifccParser::RuleStatement);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(48);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(39);
      declaration();
      setState(40);
      match(ifccParser::T__6);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(42);
      assignment();
      setState(43);
      match(ifccParser::T__6);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(45);
      expression(0);
      setState(46);
      match(ifccParser::T__6);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DeclarationContext ------------------------------------------------------------------

ifccParser::DeclarationContext::DeclarationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

ifccParser::TypeContext* ifccParser::DeclarationContext::type() {
  return getRuleContext<ifccParser::TypeContext>(0);
}

tree::TerminalNode* ifccParser::DeclarationContext::IDENTIFIER() {
  return getToken(ifccParser::IDENTIFIER, 0);
}

ifccParser::ExpressionContext* ifccParser::DeclarationContext::expression() {
  return getRuleContext<ifccParser::ExpressionContext>(0);
}


size_t ifccParser::DeclarationContext::getRuleIndex() const {
  return ifccParser::RuleDeclaration;
}

antlrcpp::Any ifccParser::DeclarationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitDeclaration(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::DeclarationContext* ifccParser::declaration() {
  DeclarationContext *_localctx = _tracker.createInstance<DeclarationContext>(_ctx, getState());
  enterRule(_localctx, 8, ifccParser::RuleDeclaration);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(50);
    type();
    setState(51);
    match(ifccParser::IDENTIFIER);
    setState(54);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == ifccParser::T__7) {
      setState(52);
      match(ifccParser::T__7);
      setState(53);
      expression(0);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignmentContext ------------------------------------------------------------------

ifccParser::AssignmentContext::AssignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ifccParser::AssignmentContext::IDENTIFIER() {
  return getToken(ifccParser::IDENTIFIER, 0);
}

ifccParser::ExpressionContext* ifccParser::AssignmentContext::expression() {
  return getRuleContext<ifccParser::ExpressionContext>(0);
}


size_t ifccParser::AssignmentContext::getRuleIndex() const {
  return ifccParser::RuleAssignment;
}

antlrcpp::Any ifccParser::AssignmentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitAssignment(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::AssignmentContext* ifccParser::assignment() {
  AssignmentContext *_localctx = _tracker.createInstance<AssignmentContext>(_ctx, getState());
  enterRule(_localctx, 10, ifccParser::RuleAssignment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(56);
    match(ifccParser::IDENTIFIER);
    setState(57);
    match(ifccParser::T__7);
    setState(58);
    expression(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

ifccParser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* ifccParser::ExpressionContext::CONST() {
  return getToken(ifccParser::CONST, 0);
}

tree::TerminalNode* ifccParser::ExpressionContext::IDENTIFIER() {
  return getToken(ifccParser::IDENTIFIER, 0);
}

std::vector<ifccParser::ExpressionContext *> ifccParser::ExpressionContext::expression() {
  return getRuleContexts<ifccParser::ExpressionContext>();
}

ifccParser::ExpressionContext* ifccParser::ExpressionContext::expression(size_t i) {
  return getRuleContext<ifccParser::ExpressionContext>(i);
}

ifccParser::Binary_operationContext* ifccParser::ExpressionContext::binary_operation() {
  return getRuleContext<ifccParser::Binary_operationContext>(0);
}


size_t ifccParser::ExpressionContext::getRuleIndex() const {
  return ifccParser::RuleExpression;
}

antlrcpp::Any ifccParser::ExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitExpression(this);
  else
    return visitor->visitChildren(this);
}


ifccParser::ExpressionContext* ifccParser::expression() {
   return expression(0);
}

ifccParser::ExpressionContext* ifccParser::expression(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  ifccParser::ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, parentState);
  ifccParser::ExpressionContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 12;
  enterRecursionRule(_localctx, 12, ifccParser::RuleExpression, precedence);

    

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(67);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case ifccParser::CONST: {
        setState(61);
        match(ifccParser::CONST);
        break;
      }

      case ifccParser::IDENTIFIER: {
        setState(62);
        match(ifccParser::IDENTIFIER);
        break;
      }

      case ifccParser::T__2: {
        setState(63);
        match(ifccParser::T__2);
        setState(64);
        expression(0);
        setState(65);
        match(ifccParser::T__3);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(75);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<ExpressionContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleExpression);
        setState(69);

        if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
        setState(70);
        binary_operation();
        setState(71);
        expression(3); 
      }
      setState(77);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- Binary_operationContext ------------------------------------------------------------------

ifccParser::Binary_operationContext::Binary_operationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t ifccParser::Binary_operationContext::getRuleIndex() const {
  return ifccParser::RuleBinary_operation;
}

antlrcpp::Any ifccParser::Binary_operationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitBinary_operation(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::Binary_operationContext* ifccParser::binary_operation() {
  Binary_operationContext *_localctx = _tracker.createInstance<Binary_operationContext>(_ctx, getState());
  enterRule(_localctx, 14, ifccParser::RuleBinary_operation);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(78);
    _la = _input->LA(1);
    if (!(_la == ifccParser::T__8

    || _la == ifccParser::T__9)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeContext ------------------------------------------------------------------

ifccParser::TypeContext::TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t ifccParser::TypeContext::getRuleIndex() const {
  return ifccParser::RuleType;
}

antlrcpp::Any ifccParser::TypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<ifccVisitor*>(visitor))
    return parserVisitor->visitType(this);
  else
    return visitor->visitChildren(this);
}

ifccParser::TypeContext* ifccParser::type() {
  TypeContext *_localctx = _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 16, ifccParser::RuleType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(80);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << ifccParser::T__0)
      | (1ULL << ifccParser::T__10)
      | (1ULL << ifccParser::T__11))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool ifccParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 6: return expressionSempred(dynamic_cast<ExpressionContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool ifccParser::expressionSempred(ExpressionContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 2);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> ifccParser::_decisionToDFA;
atn::PredictionContextCache ifccParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN ifccParser::_atn;
std::vector<uint16_t> ifccParser::_serializedATN;

std::vector<std::string> ifccParser::_ruleNames = {
  "axiom", "prog", "return_stmt", "statement", "declaration", "assignment", 
  "expression", "binary_operation", "type"
};

std::vector<std::string> ifccParser::_literalNames = {
  "", "'int'", "'main'", "'('", "')'", "'{'", "'}'", "';'", "'='", "'+'", 
  "'-'", "'char'", "'void'", "'return'"
};

std::vector<std::string> ifccParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "RETURN", "CONST", 
  "IDENTIFIER", "COMMENT", "DIRECTIVE", "WS"
};

dfa::Vocabulary ifccParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> ifccParser::_tokenNames;

ifccParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0x14, 0x55, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x3, 0x2, 0x3, 
    0x2, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x7, 0x3, 0x1e, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x21, 0xb, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 
    0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x5, 0x5, 0x33, 0xa, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x5, 0x6, 0x39, 0xa, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 
    0x7, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 
    0x8, 0x5, 0x8, 0x46, 0xa, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 
    0x7, 0x8, 0x4c, 0xa, 0x8, 0xc, 0x8, 0xe, 0x8, 0x4f, 0xb, 0x8, 0x3, 0x9, 
    0x3, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x2, 0x3, 0xe, 0xb, 0x2, 0x4, 
    0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x2, 0x4, 0x3, 0x2, 0xb, 0xc, 0x4, 
    0x2, 0x3, 0x3, 0xd, 0xe, 0x2, 0x52, 0x2, 0x14, 0x3, 0x2, 0x2, 0x2, 0x4, 
    0x17, 0x3, 0x2, 0x2, 0x2, 0x6, 0x25, 0x3, 0x2, 0x2, 0x2, 0x8, 0x32, 
    0x3, 0x2, 0x2, 0x2, 0xa, 0x34, 0x3, 0x2, 0x2, 0x2, 0xc, 0x3a, 0x3, 0x2, 
    0x2, 0x2, 0xe, 0x45, 0x3, 0x2, 0x2, 0x2, 0x10, 0x50, 0x3, 0x2, 0x2, 
    0x2, 0x12, 0x52, 0x3, 0x2, 0x2, 0x2, 0x14, 0x15, 0x5, 0x4, 0x3, 0x2, 
    0x15, 0x16, 0x7, 0x2, 0x2, 0x3, 0x16, 0x3, 0x3, 0x2, 0x2, 0x2, 0x17, 
    0x18, 0x7, 0x3, 0x2, 0x2, 0x18, 0x19, 0x7, 0x4, 0x2, 0x2, 0x19, 0x1a, 
    0x7, 0x5, 0x2, 0x2, 0x1a, 0x1b, 0x7, 0x6, 0x2, 0x2, 0x1b, 0x1f, 0x7, 
    0x7, 0x2, 0x2, 0x1c, 0x1e, 0x5, 0x8, 0x5, 0x2, 0x1d, 0x1c, 0x3, 0x2, 
    0x2, 0x2, 0x1e, 0x21, 0x3, 0x2, 0x2, 0x2, 0x1f, 0x1d, 0x3, 0x2, 0x2, 
    0x2, 0x1f, 0x20, 0x3, 0x2, 0x2, 0x2, 0x20, 0x22, 0x3, 0x2, 0x2, 0x2, 
    0x21, 0x1f, 0x3, 0x2, 0x2, 0x2, 0x22, 0x23, 0x5, 0x6, 0x4, 0x2, 0x23, 
    0x24, 0x7, 0x8, 0x2, 0x2, 0x24, 0x5, 0x3, 0x2, 0x2, 0x2, 0x25, 0x26, 
    0x7, 0xf, 0x2, 0x2, 0x26, 0x27, 0x5, 0xe, 0x8, 0x2, 0x27, 0x28, 0x7, 
    0x9, 0x2, 0x2, 0x28, 0x7, 0x3, 0x2, 0x2, 0x2, 0x29, 0x2a, 0x5, 0xa, 
    0x6, 0x2, 0x2a, 0x2b, 0x7, 0x9, 0x2, 0x2, 0x2b, 0x33, 0x3, 0x2, 0x2, 
    0x2, 0x2c, 0x2d, 0x5, 0xc, 0x7, 0x2, 0x2d, 0x2e, 0x7, 0x9, 0x2, 0x2, 
    0x2e, 0x33, 0x3, 0x2, 0x2, 0x2, 0x2f, 0x30, 0x5, 0xe, 0x8, 0x2, 0x30, 
    0x31, 0x7, 0x9, 0x2, 0x2, 0x31, 0x33, 0x3, 0x2, 0x2, 0x2, 0x32, 0x29, 
    0x3, 0x2, 0x2, 0x2, 0x32, 0x2c, 0x3, 0x2, 0x2, 0x2, 0x32, 0x2f, 0x3, 
    0x2, 0x2, 0x2, 0x33, 0x9, 0x3, 0x2, 0x2, 0x2, 0x34, 0x35, 0x5, 0x12, 
    0xa, 0x2, 0x35, 0x38, 0x7, 0x11, 0x2, 0x2, 0x36, 0x37, 0x7, 0xa, 0x2, 
    0x2, 0x37, 0x39, 0x5, 0xe, 0x8, 0x2, 0x38, 0x36, 0x3, 0x2, 0x2, 0x2, 
    0x38, 0x39, 0x3, 0x2, 0x2, 0x2, 0x39, 0xb, 0x3, 0x2, 0x2, 0x2, 0x3a, 
    0x3b, 0x7, 0x11, 0x2, 0x2, 0x3b, 0x3c, 0x7, 0xa, 0x2, 0x2, 0x3c, 0x3d, 
    0x5, 0xe, 0x8, 0x2, 0x3d, 0xd, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x3f, 0x8, 
    0x8, 0x1, 0x2, 0x3f, 0x46, 0x7, 0x10, 0x2, 0x2, 0x40, 0x46, 0x7, 0x11, 
    0x2, 0x2, 0x41, 0x42, 0x7, 0x5, 0x2, 0x2, 0x42, 0x43, 0x5, 0xe, 0x8, 
    0x2, 0x43, 0x44, 0x7, 0x6, 0x2, 0x2, 0x44, 0x46, 0x3, 0x2, 0x2, 0x2, 
    0x45, 0x3e, 0x3, 0x2, 0x2, 0x2, 0x45, 0x40, 0x3, 0x2, 0x2, 0x2, 0x45, 
    0x41, 0x3, 0x2, 0x2, 0x2, 0x46, 0x4d, 0x3, 0x2, 0x2, 0x2, 0x47, 0x48, 
    0xc, 0x4, 0x2, 0x2, 0x48, 0x49, 0x5, 0x10, 0x9, 0x2, 0x49, 0x4a, 0x5, 
    0xe, 0x8, 0x5, 0x4a, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x4b, 0x47, 0x3, 0x2, 
    0x2, 0x2, 0x4c, 0x4f, 0x3, 0x2, 0x2, 0x2, 0x4d, 0x4b, 0x3, 0x2, 0x2, 
    0x2, 0x4d, 0x4e, 0x3, 0x2, 0x2, 0x2, 0x4e, 0xf, 0x3, 0x2, 0x2, 0x2, 
    0x4f, 0x4d, 0x3, 0x2, 0x2, 0x2, 0x50, 0x51, 0x9, 0x2, 0x2, 0x2, 0x51, 
    0x11, 0x3, 0x2, 0x2, 0x2, 0x52, 0x53, 0x9, 0x3, 0x2, 0x2, 0x53, 0x13, 
    0x3, 0x2, 0x2, 0x2, 0x7, 0x1f, 0x32, 0x38, 0x45, 0x4d, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

ifccParser::Initializer ifccParser::_init;
