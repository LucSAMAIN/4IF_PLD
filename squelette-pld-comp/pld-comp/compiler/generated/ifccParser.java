// Generated from ifcc.g4 by ANTLR 4.7.2
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class ifccParser extends Parser {
	static { RuntimeMetaData.checkVersion("4.7.2", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		T__0=1, T__1=2, T__2=3, T__3=4, T__4=5, T__5=6, T__6=7, T__7=8, T__8=9, 
		T__9=10, RETURN=11, CONST=12, IDENTIFIER=13, COMMENT=14, DIRECTIVE=15, 
		WS=16;
	public static final int
		RULE_axiom = 0, RULE_prog = 1, RULE_return_stmt = 2, RULE_statement = 3, 
		RULE_declaration = 4, RULE_assignment = 5, RULE_expression = 6, RULE_type = 7;
	private static String[] makeRuleNames() {
		return new String[] {
			"axiom", "prog", "return_stmt", "statement", "declaration", "assignment", 
			"expression", "type"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "'int'", "'main'", "'('", "')'", "'{'", "'}'", "';'", "'='", "'char'", 
			"'void'", "'return'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, null, null, null, null, null, null, null, null, null, null, "RETURN", 
			"CONST", "IDENTIFIER", "COMMENT", "DIRECTIVE", "WS"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}

	@Override
	public String getGrammarFileName() { return "ifcc.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }

	public ifccParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}
	public static class AxiomContext extends ParserRuleContext {
		public ProgContext prog() {
			return getRuleContext(ProgContext.class,0);
		}
		public TerminalNode EOF() { return getToken(ifccParser.EOF, 0); }
		public AxiomContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_axiom; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).enterAxiom(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).exitAxiom(this);
		}
	}

	public final AxiomContext axiom() throws RecognitionException {
		AxiomContext _localctx = new AxiomContext(_ctx, getState());
		enterRule(_localctx, 0, RULE_axiom);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(16);
			prog();
			setState(17);
			match(EOF);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ProgContext extends ParserRuleContext {
		public Return_stmtContext return_stmt() {
			return getRuleContext(Return_stmtContext.class,0);
		}
		public List<StatementContext> statement() {
			return getRuleContexts(StatementContext.class);
		}
		public StatementContext statement(int i) {
			return getRuleContext(StatementContext.class,i);
		}
		public ProgContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_prog; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).enterProg(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).exitProg(this);
		}
	}

	public final ProgContext prog() throws RecognitionException {
		ProgContext _localctx = new ProgContext(_ctx, getState());
		enterRule(_localctx, 2, RULE_prog);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(19);
			match(T__0);
			setState(20);
			match(T__1);
			setState(21);
			match(T__2);
			setState(22);
			match(T__3);
			setState(23);
			match(T__4);
			setState(27);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << T__0) | (1L << T__8) | (1L << T__9) | (1L << CONST) | (1L << IDENTIFIER))) != 0)) {
				{
				{
				setState(24);
				statement();
				}
				}
				setState(29);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(30);
			return_stmt();
			setState(31);
			match(T__5);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class Return_stmtContext extends ParserRuleContext {
		public TerminalNode RETURN() { return getToken(ifccParser.RETURN, 0); }
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public Return_stmtContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_return_stmt; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).enterReturn_stmt(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).exitReturn_stmt(this);
		}
	}

	public final Return_stmtContext return_stmt() throws RecognitionException {
		Return_stmtContext _localctx = new Return_stmtContext(_ctx, getState());
		enterRule(_localctx, 4, RULE_return_stmt);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(33);
			match(RETURN);
			setState(34);
			expression();
			setState(35);
			match(T__6);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class StatementContext extends ParserRuleContext {
		public DeclarationContext declaration() {
			return getRuleContext(DeclarationContext.class,0);
		}
		public AssignmentContext assignment() {
			return getRuleContext(AssignmentContext.class,0);
		}
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public StatementContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_statement; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).enterStatement(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).exitStatement(this);
		}
	}

	public final StatementContext statement() throws RecognitionException {
		StatementContext _localctx = new StatementContext(_ctx, getState());
		enterRule(_localctx, 6, RULE_statement);
		try {
			setState(46);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,1,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(37);
				declaration();
				setState(38);
				match(T__6);
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(40);
				assignment();
				setState(41);
				match(T__6);
				}
				break;
			case 3:
				enterOuterAlt(_localctx, 3);
				{
				setState(43);
				expression();
				setState(44);
				match(T__6);
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class DeclarationContext extends ParserRuleContext {
		public TypeContext type() {
			return getRuleContext(TypeContext.class,0);
		}
		public TerminalNode IDENTIFIER() { return getToken(ifccParser.IDENTIFIER, 0); }
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public DeclarationContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_declaration; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).enterDeclaration(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).exitDeclaration(this);
		}
	}

	public final DeclarationContext declaration() throws RecognitionException {
		DeclarationContext _localctx = new DeclarationContext(_ctx, getState());
		enterRule(_localctx, 8, RULE_declaration);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(48);
			type();
			setState(49);
			match(IDENTIFIER);
			setState(52);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==T__7) {
				{
				setState(50);
				match(T__7);
				setState(51);
				expression();
				}
			}

			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class AssignmentContext extends ParserRuleContext {
		public TerminalNode IDENTIFIER() { return getToken(ifccParser.IDENTIFIER, 0); }
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public AssignmentContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_assignment; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).enterAssignment(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).exitAssignment(this);
		}
	}

	public final AssignmentContext assignment() throws RecognitionException {
		AssignmentContext _localctx = new AssignmentContext(_ctx, getState());
		enterRule(_localctx, 10, RULE_assignment);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(54);
			match(IDENTIFIER);
			setState(55);
			match(T__7);
			setState(56);
			expression();
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ExpressionContext extends ParserRuleContext {
		public TerminalNode CONST() { return getToken(ifccParser.CONST, 0); }
		public TerminalNode IDENTIFIER() { return getToken(ifccParser.IDENTIFIER, 0); }
		public ExpressionContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_expression; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).enterExpression(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).exitExpression(this);
		}
	}

	public final ExpressionContext expression() throws RecognitionException {
		ExpressionContext _localctx = new ExpressionContext(_ctx, getState());
		enterRule(_localctx, 12, RULE_expression);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(58);
			_la = _input.LA(1);
			if ( !(_la==CONST || _la==IDENTIFIER) ) {
			_errHandler.recoverInline(this);
			}
			else {
				if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
				_errHandler.reportMatch(this);
				consume();
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class TypeContext extends ParserRuleContext {
		public TypeContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_type; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).enterType(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof ifccListener ) ((ifccListener)listener).exitType(this);
		}
	}

	public final TypeContext type() throws RecognitionException {
		TypeContext _localctx = new TypeContext(_ctx, getState());
		enterRule(_localctx, 14, RULE_type);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(60);
			_la = _input.LA(1);
			if ( !((((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << T__0) | (1L << T__8) | (1L << T__9))) != 0)) ) {
			_errHandler.recoverInline(this);
			}
			else {
				if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
				_errHandler.reportMatch(this);
				consume();
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\3\22A\4\2\t\2\4\3\t"+
		"\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\3\2\3\2\3\2\3\3\3\3"+
		"\3\3\3\3\3\3\3\3\7\3\34\n\3\f\3\16\3\37\13\3\3\3\3\3\3\3\3\4\3\4\3\4\3"+
		"\4\3\5\3\5\3\5\3\5\3\5\3\5\3\5\3\5\3\5\5\5\61\n\5\3\6\3\6\3\6\3\6\5\6"+
		"\67\n\6\3\7\3\7\3\7\3\7\3\b\3\b\3\t\3\t\3\t\2\2\n\2\4\6\b\n\f\16\20\2"+
		"\4\3\2\16\17\4\2\3\3\13\f\2<\2\22\3\2\2\2\4\25\3\2\2\2\6#\3\2\2\2\b\60"+
		"\3\2\2\2\n\62\3\2\2\2\f8\3\2\2\2\16<\3\2\2\2\20>\3\2\2\2\22\23\5\4\3\2"+
		"\23\24\7\2\2\3\24\3\3\2\2\2\25\26\7\3\2\2\26\27\7\4\2\2\27\30\7\5\2\2"+
		"\30\31\7\6\2\2\31\35\7\7\2\2\32\34\5\b\5\2\33\32\3\2\2\2\34\37\3\2\2\2"+
		"\35\33\3\2\2\2\35\36\3\2\2\2\36 \3\2\2\2\37\35\3\2\2\2 !\5\6\4\2!\"\7"+
		"\b\2\2\"\5\3\2\2\2#$\7\r\2\2$%\5\16\b\2%&\7\t\2\2&\7\3\2\2\2\'(\5\n\6"+
		"\2()\7\t\2\2)\61\3\2\2\2*+\5\f\7\2+,\7\t\2\2,\61\3\2\2\2-.\5\16\b\2./"+
		"\7\t\2\2/\61\3\2\2\2\60\'\3\2\2\2\60*\3\2\2\2\60-\3\2\2\2\61\t\3\2\2\2"+
		"\62\63\5\20\t\2\63\66\7\17\2\2\64\65\7\n\2\2\65\67\5\16\b\2\66\64\3\2"+
		"\2\2\66\67\3\2\2\2\67\13\3\2\2\289\7\17\2\29:\7\n\2\2:;\5\16\b\2;\r\3"+
		"\2\2\2<=\t\2\2\2=\17\3\2\2\2>?\t\3\2\2?\21\3\2\2\2\5\35\60\66";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}