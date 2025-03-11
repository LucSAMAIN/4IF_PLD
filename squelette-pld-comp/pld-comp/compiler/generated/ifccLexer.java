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
		T__9=10, T__10=11, T__11=12, T__12=13, T__13=14, T__14=15, T__15=16, T__16=17, 
		T__17=18, T__18=19, T__19=20, T__20=21, T__21=22, T__22=23, RETURN=24, 
		CONST=25, IDENTIFIER=26, COMMENT=27, DIRECTIVE=28, WS=29;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "T__6", "T__7", "T__8", 
			"T__9", "T__10", "T__11", "T__12", "T__13", "T__14", "T__15", "T__16", 
			"T__17", "T__18", "T__19", "T__20", "T__21", "T__22", "RETURN", "CONST", 
			"IDENTIFIER", "COMMENT", "DIRECTIVE", "WS"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "'int'", "'main'", "'('", "')'", "'{'", "'}'", "';'", "'='", "'+'", 
			"'-'", "'*'", "'/'", "'%'", "'=='", "'!='", "'<'", "'<='", "'>'", "'>='", 
			"'&&'", "'||'", "'char'", "'void'", "'return'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, null, 
			"RETURN", "CONST", "IDENTIFIER", "COMMENT", "DIRECTIVE", "WS"
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
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\2\37\u00ac\b\1\4\2"+
		"\t\2\4\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4"+
		"\13\t\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22"+
		"\t\22\4\23\t\23\4\24\t\24\4\25\t\25\4\26\t\26\4\27\t\27\4\30\t\30\4\31"+
		"\t\31\4\32\t\32\4\33\t\33\4\34\t\34\4\35\t\35\4\36\t\36\3\2\3\2\3\2\3"+
		"\2\3\3\3\3\3\3\3\3\3\3\3\4\3\4\3\5\3\5\3\6\3\6\3\7\3\7\3\b\3\b\3\t\3\t"+
		"\3\n\3\n\3\13\3\13\3\f\3\f\3\r\3\r\3\16\3\16\3\17\3\17\3\17\3\20\3\20"+
		"\3\20\3\21\3\21\3\22\3\22\3\22\3\23\3\23\3\24\3\24\3\24\3\25\3\25\3\25"+
		"\3\26\3\26\3\26\3\27\3\27\3\27\3\27\3\27\3\30\3\30\3\30\3\30\3\30\3\31"+
		"\3\31\3\31\3\31\3\31\3\31\3\31\3\32\6\32\u0085\n\32\r\32\16\32\u0086\3"+
		"\33\3\33\7\33\u008b\n\33\f\33\16\33\u008e\13\33\3\34\3\34\3\34\3\34\7"+
		"\34\u0094\n\34\f\34\16\34\u0097\13\34\3\34\3\34\3\34\3\34\3\34\3\35\3"+
		"\35\7\35\u00a0\n\35\f\35\16\35\u00a3\13\35\3\35\3\35\3\35\3\35\3\36\3"+
		"\36\3\36\3\36\4\u0095\u00a1\2\37\3\3\5\4\7\5\t\6\13\7\r\b\17\t\21\n\23"+
		"\13\25\f\27\r\31\16\33\17\35\20\37\21!\22#\23%\24\'\25)\26+\27-\30/\31"+
		"\61\32\63\33\65\34\67\359\36;\37\3\2\6\3\2\62;\5\2C\\aac|\6\2\62;C\\a"+
		"ac|\5\2\13\f\17\17\"\"\2\u00af\2\3\3\2\2\2\2\5\3\2\2\2\2\7\3\2\2\2\2\t"+
		"\3\2\2\2\2\13\3\2\2\2\2\r\3\2\2\2\2\17\3\2\2\2\2\21\3\2\2\2\2\23\3\2\2"+
		"\2\2\25\3\2\2\2\2\27\3\2\2\2\2\31\3\2\2\2\2\33\3\2\2\2\2\35\3\2\2\2\2"+
		"\37\3\2\2\2\2!\3\2\2\2\2#\3\2\2\2\2%\3\2\2\2\2\'\3\2\2\2\2)\3\2\2\2\2"+
		"+\3\2\2\2\2-\3\2\2\2\2/\3\2\2\2\2\61\3\2\2\2\2\63\3\2\2\2\2\65\3\2\2\2"+
		"\2\67\3\2\2\2\29\3\2\2\2\2;\3\2\2\2\3=\3\2\2\2\5A\3\2\2\2\7F\3\2\2\2\t"+
		"H\3\2\2\2\13J\3\2\2\2\rL\3\2\2\2\17N\3\2\2\2\21P\3\2\2\2\23R\3\2\2\2\25"+
		"T\3\2\2\2\27V\3\2\2\2\31X\3\2\2\2\33Z\3\2\2\2\35\\\3\2\2\2\37_\3\2\2\2"+
		"!b\3\2\2\2#d\3\2\2\2%g\3\2\2\2\'i\3\2\2\2)l\3\2\2\2+o\3\2\2\2-r\3\2\2"+
		"\2/w\3\2\2\2\61|\3\2\2\2\63\u0084\3\2\2\2\65\u0088\3\2\2\2\67\u008f\3"+
		"\2\2\29\u009d\3\2\2\2;\u00a8\3\2\2\2=>\7k\2\2>?\7p\2\2?@\7v\2\2@\4\3\2"+
		"\2\2AB\7o\2\2BC\7c\2\2CD\7k\2\2DE\7p\2\2E\6\3\2\2\2FG\7*\2\2G\b\3\2\2"+
		"\2HI\7+\2\2I\n\3\2\2\2JK\7}\2\2K\f\3\2\2\2LM\7\177\2\2M\16\3\2\2\2NO\7"+
		"=\2\2O\20\3\2\2\2PQ\7?\2\2Q\22\3\2\2\2RS\7-\2\2S\24\3\2\2\2TU\7/\2\2U"+
		"\26\3\2\2\2VW\7,\2\2W\30\3\2\2\2XY\7\61\2\2Y\32\3\2\2\2Z[\7\'\2\2[\34"+
		"\3\2\2\2\\]\7?\2\2]^\7?\2\2^\36\3\2\2\2_`\7#\2\2`a\7?\2\2a \3\2\2\2bc"+
		"\7>\2\2c\"\3\2\2\2de\7>\2\2ef\7?\2\2f$\3\2\2\2gh\7@\2\2h&\3\2\2\2ij\7"+
		"@\2\2jk\7?\2\2k(\3\2\2\2lm\7(\2\2mn\7(\2\2n*\3\2\2\2op\7~\2\2pq\7~\2\2"+
		"q,\3\2\2\2rs\7e\2\2st\7j\2\2tu\7c\2\2uv\7t\2\2v.\3\2\2\2wx\7x\2\2xy\7"+
		"q\2\2yz\7k\2\2z{\7f\2\2{\60\3\2\2\2|}\7t\2\2}~\7g\2\2~\177\7v\2\2\177"+
		"\u0080\7w\2\2\u0080\u0081\7t\2\2\u0081\u0082\7p\2\2\u0082\62\3\2\2\2\u0083"+
		"\u0085\t\2\2\2\u0084\u0083\3\2\2\2\u0085\u0086\3\2\2\2\u0086\u0084\3\2"+
		"\2\2\u0086\u0087\3\2\2\2\u0087\64\3\2\2\2\u0088\u008c\t\3\2\2\u0089\u008b"+
		"\t\4\2\2\u008a\u0089\3\2\2\2\u008b\u008e\3\2\2\2\u008c\u008a\3\2\2\2\u008c"+
		"\u008d\3\2\2\2\u008d\66\3\2\2\2\u008e\u008c\3\2\2\2\u008f\u0090\7\61\2"+
		"\2\u0090\u0091\7,\2\2\u0091\u0095\3\2\2\2\u0092\u0094\13\2\2\2\u0093\u0092"+
		"\3\2\2\2\u0094\u0097\3\2\2\2\u0095\u0096\3\2\2\2\u0095\u0093\3\2\2\2\u0096"+
		"\u0098\3\2\2\2\u0097\u0095\3\2\2\2\u0098\u0099\7,\2\2\u0099\u009a\7\61"+
		"\2\2\u009a\u009b\3\2\2\2\u009b\u009c\b\34\2\2\u009c8\3\2\2\2\u009d\u00a1"+
		"\7%\2\2\u009e\u00a0\13\2\2\2\u009f\u009e\3\2\2\2\u00a0\u00a3\3\2\2\2\u00a1"+
		"\u00a2\3\2\2\2\u00a1\u009f\3\2\2\2\u00a2\u00a4\3\2\2\2\u00a3\u00a1\3\2"+
		"\2\2\u00a4\u00a5\7\f\2\2\u00a5\u00a6\3\2\2\2\u00a6\u00a7\b\35\2\2\u00a7"+
		":\3\2\2\2\u00a8\u00a9\t\5\2\2\u00a9\u00aa\3\2\2\2\u00aa\u00ab\b\36\3\2"+
		"\u00ab<\3\2\2\2\7\2\u0086\u008c\u0095\u00a1\4\b\2\2\2\3\2";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}