// Generated from ifcc.g4 by ANTLR 4.7.2
import org.antlr.v4.runtime.tree.ParseTreeListener;

/**
 * This interface defines a complete listener for a parse tree produced by
 * {@link ifccParser}.
 */
public interface ifccListener extends ParseTreeListener {
	/**
	 * Enter a parse tree produced by {@link ifccParser#axiom}.
	 * @param ctx the parse tree
	 */
	void enterAxiom(ifccParser.AxiomContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#axiom}.
	 * @param ctx the parse tree
	 */
	void exitAxiom(ifccParser.AxiomContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#prog}.
	 * @param ctx the parse tree
	 */
	void enterProg(ifccParser.ProgContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#prog}.
	 * @param ctx the parse tree
	 */
	void exitProg(ifccParser.ProgContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#return_stmt}.
	 * @param ctx the parse tree
	 */
	void enterReturn_stmt(ifccParser.Return_stmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#return_stmt}.
	 * @param ctx the parse tree
	 */
	void exitReturn_stmt(ifccParser.Return_stmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#statement}.
	 * @param ctx the parse tree
	 */
	void enterStatement(ifccParser.StatementContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#statement}.
	 * @param ctx the parse tree
	 */
	void exitStatement(ifccParser.StatementContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#declaration}.
	 * @param ctx the parse tree
	 */
	void enterDeclaration(ifccParser.DeclarationContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#declaration}.
	 * @param ctx the parse tree
	 */
	void exitDeclaration(ifccParser.DeclarationContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#assignment}.
	 * @param ctx the parse tree
	 */
	void enterAssignment(ifccParser.AssignmentContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#assignment}.
	 * @param ctx the parse tree
	 */
	void exitAssignment(ifccParser.AssignmentContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void enterExpression(ifccParser.ExpressionContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void exitExpression(ifccParser.ExpressionContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#binary_operation}.
	 * @param ctx the parse tree
	 */
	void enterBinary_operation(ifccParser.Binary_operationContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#binary_operation}.
	 * @param ctx the parse tree
	 */
	void exitBinary_operation(ifccParser.Binary_operationContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#unary_operation}.
	 * @param ctx the parse tree
	 */
	void enterUnary_operation(ifccParser.Unary_operationContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#unary_operation}.
	 * @param ctx the parse tree
	 */
	void exitUnary_operation(ifccParser.Unary_operationContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#type}.
	 * @param ctx the parse tree
	 */
	void enterType(ifccParser.TypeContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#type}.
	 * @param ctx the parse tree
	 */
	void exitType(ifccParser.TypeContext ctx);
}