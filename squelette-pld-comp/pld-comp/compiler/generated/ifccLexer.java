// Generated from ifcc.g4 by ANTLR 4.7.2
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class ifccLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.7.2", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		T__0=1, T__1=2, T__2=3, T__3=4, T__4=5, T__5=6, T__6=7, T__7=8, T__8=9, 
		T__9=10, T__10=11, T__11=12, RETURN=13, CONST=14, IDENTIFIER=15, COMMENT=16, 
		DIRECTIVE=17, WS=18;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "T__6", "T__7", "T__8", 
			"T__9", "T__10", "T__11", "RETURN", "CONST", "IDENTIFIER", "COMMENT", 
			"DIRECTIVE", "WS"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "'int'", "'main'", "'('", "')'", "'{'", "'}'", "';'", "'='", "'+'", 
			"'-'", "'char'", "'void'", "'return'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, "RETURN", "CONST", "IDENTIFIER", "COMMENT", "DIRECTIVE", "WS"
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


	public ifccLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "ifcc.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\2\24z\b\1\4\2\t\2\4"+
		"\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4\13\t"+
		"\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22\t\22"+
		"\4\23\t\23\3\2\3\2\3\2\3\2\3\3\3\3\3\3\3\3\3\3\3\4\3\4\3\5\3\5\3\6\3\6"+
		"\3\7\3\7\3\b\3\b\3\t\3\t\3\n\3\n\3\13\3\13\3\f\3\f\3\f\3\f\3\f\3\r\3\r"+
		"\3\r\3\r\3\r\3\16\3\16\3\16\3\16\3\16\3\16\3\16\3\17\6\17S\n\17\r\17\16"+
		"\17T\3\20\3\20\7\20Y\n\20\f\20\16\20\\\13\20\3\21\3\21\3\21\3\21\7\21"+
		"b\n\21\f\21\16\21e\13\21\3\21\3\21\3\21\3\21\3\21\3\22\3\22\7\22n\n\22"+
		"\f\22\16\22q\13\22\3\22\3\22\3\22\3\22\3\23\3\23\3\23\3\23\4co\2\24\3"+
		"\3\5\4\7\5\t\6\13\7\r\b\17\t\21\n\23\13\25\f\27\r\31\16\33\17\35\20\37"+
		"\21!\22#\23%\24\3\2\6\3\2\62;\5\2C\\aac|\6\2\62;C\\aac|\5\2\13\f\17\17"+
		"\"\"\2}\2\3\3\2\2\2\2\5\3\2\2\2\2\7\3\2\2\2\2\t\3\2\2\2\2\13\3\2\2\2\2"+
		"\r\3\2\2\2\2\17\3\2\2\2\2\21\3\2\2\2\2\23\3\2\2\2\2\25\3\2\2\2\2\27\3"+
		"\2\2\2\2\31\3\2\2\2\2\33\3\2\2\2\2\35\3\2\2\2\2\37\3\2\2\2\2!\3\2\2\2"+
		"\2#\3\2\2\2\2%\3\2\2\2\3\'\3\2\2\2\5+\3\2\2\2\7\60\3\2\2\2\t\62\3\2\2"+
		"\2\13\64\3\2\2\2\r\66\3\2\2\2\178\3\2\2\2\21:\3\2\2\2\23<\3\2\2\2\25>"+
		"\3\2\2\2\27@\3\2\2\2\31E\3\2\2\2\33J\3\2\2\2\35R\3\2\2\2\37V\3\2\2\2!"+
		"]\3\2\2\2#k\3\2\2\2%v\3\2\2\2\'(\7k\2\2()\7p\2\2)*\7v\2\2*\4\3\2\2\2+"+
		",\7o\2\2,-\7c\2\2-.\7k\2\2./\7p\2\2/\6\3\2\2\2\60\61\7*\2\2\61\b\3\2\2"+
		"\2\62\63\7+\2\2\63\n\3\2\2\2\64\65\7}\2\2\65\f\3\2\2\2\66\67\7\177\2\2"+
		"\67\16\3\2\2\289\7=\2\29\20\3\2\2\2:;\7?\2\2;\22\3\2\2\2<=\7-\2\2=\24"+
		"\3\2\2\2>?\7/\2\2?\26\3\2\2\2@A\7e\2\2AB\7j\2\2BC\7c\2\2CD\7t\2\2D\30"+
		"\3\2\2\2EF\7x\2\2FG\7q\2\2GH\7k\2\2HI\7f\2\2I\32\3\2\2\2JK\7t\2\2KL\7"+
		"g\2\2LM\7v\2\2MN\7w\2\2NO\7t\2\2OP\7p\2\2P\34\3\2\2\2QS\t\2\2\2RQ\3\2"+
		"\2\2ST\3\2\2\2TR\3\2\2\2TU\3\2\2\2U\36\3\2\2\2VZ\t\3\2\2WY\t\4\2\2XW\3"+
		"\2\2\2Y\\\3\2\2\2ZX\3\2\2\2Z[\3\2\2\2[ \3\2\2\2\\Z\3\2\2\2]^\7\61\2\2"+
		"^_\7,\2\2_c\3\2\2\2`b\13\2\2\2a`\3\2\2\2be\3\2\2\2cd\3\2\2\2ca\3\2\2\2"+
		"df\3\2\2\2ec\3\2\2\2fg\7,\2\2gh\7\61\2\2hi\3\2\2\2ij\b\21\2\2j\"\3\2\2"+
		"\2ko\7%\2\2ln\13\2\2\2ml\3\2\2\2nq\3\2\2\2op\3\2\2\2om\3\2\2\2pr\3\2\2"+
		"\2qo\3\2\2\2rs\7\f\2\2st\3\2\2\2tu\b\22\2\2u$\3\2\2\2vw\t\5\2\2wx\3\2"+
		"\2\2xy\b\23\3\2y&\3\2\2\2\7\2TZco\4\b\2\2\2\3\2";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}