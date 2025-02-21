#include <iostream>
#include "Parser.hpp"
#include "CST.hpp"
#include "CSTNode.hpp"
#include "stack"
#include "SymbolTable.h"
#include <tuple>


/** **************************************************************************************
parse function which creates our CST
@pre:vector of tokens from declaration
@post: returns a concrete syntax tree.
 *****************************************************************************************/
CST* Parser::parse() {

    //calls our over arching function to recurse through our token vector
    program();
    return cst;
}

/** **************************************************************************************
Checks program meets BNF requirements
@pre:uses the token vector in our parse data object.
@post: recusively calls one of our functions based on the given token
 *****************************************************************************************/
void Parser::program() {

    while(!tokenVector[index].isEOF()){

        ////std::cout<<"+++block start token++++++++++++++++++++++++++++++++++++++++++ "<<std::endl;
        //tokenVector[index].print();
        ////std::cout<<"+++block start token++++++++++++++++++++++++++++++++++++++++++ "<<std::endl;
        if (tokenVector[index].getTokenString() == "function") {
            ////std::cout<<" found function declaration" <<std::endl;
            inDeclaration = true;
            function_declaration();
            inDeclaration = false;
        } else if (tokenVector[index].getTokenString() == "procedure" && tokenVector[index + 1].getTokenString() != "main") {
            ////std::cout<<" found  procedure declaration " <<std::endl;
            procedure_declaration();
        } else if ( datatype_specifier() ) {
            ////std::cout<<" found  declaration statement " <<std::endl;
            int temp = scope;
            scope = 0;
            inDeclaration = true;
            declaration_statement();
            inDeclaration = false;
            scope = temp;
        } else if (tokenVector[index].getTokenString() == "procedure" && tokenVector[index + 1].getTokenString() == "main") { //needs  to be fixed to distinguish between procedure and procedure main
            ////std::cout<<" found main procedure" <<std::endl;
            main_procedure();
        } else{
            //something wrong
            ////std::cout<<"looked for declaration of function or procedure, but none found?"<<std::endl;
       //     //std::cout<<"offending token:"<<std::endl;
            tokenVector[index].print();
            throw;
        }
    }

    //need to add our function params as param lists at the end of our parsing whem making the Symbol table.
    //nested loops which push the param lists onto the symboltable proper.
//    //std::cout<< "adding params to symbol table"<<std::endl;
    for(int i=0; i < paramLists.size(); i++){
        for(int j= 0; j < paramLists[i].size(); j++){
       //     //std::cout<< "found param: " << paramLists[i][j].identifier_type <<std::endl;

            symbol_table_list.insertSymbol(paramLists[i][j], "");
        }
    }

}

/** **************************************************************************************
Checks main-procedure meets BNF requirements
@pre:uses the token vector in our parse data object.
@post:parses through the function headers and main procedures.
 *****************************************************************************************/
void Parser::main_procedure(){
    scope++;
    //SymbolTable new_symbol_table;
    new_symbol_table.scope = scope;
    new_symbol_table.identifier_type = tokenVector[index].getTokenString();
    expect("procedure");

    SymbolLocation = "main";
    new_symbol_table.identifier_name = tokenVector[index].getTokenString();
    expect("main");
 //   //std::cout<<"inserting symbol: "<<new_symbol_table.identifier_name <<std::endl;
    symbol_table_list.insertSymbol(new_symbol_table,"main");


    //clear the temp symbol table element in our parser class.
    SymbolTable empty_symbol_table;
    new_symbol_table = empty_symbol_table;

    expect("(");
    expect("void");
    expect(")");
    block_statement();
}

/** **************************************************************************************
Checks function_declaration meets BNF requirements
@pre:is called when we find a function declaration
@post: handles generation of cst function block
 *****************************************************************************************/
void Parser::function_declaration(){
    inFunction = true;
    scope++;
    //SymbolTable new_symbol_table;
    new_symbol_table.scope = scope;
    //so when we find a function we expect the token to be a identifier with the string function
    new_symbol_table.identifier_type = tokenVector[index].getTokenString();
    expect("function");

    //then we expect some data type specifier, and if it is then
    if ( datatype_specifier() ) {
        //get that datatype name and call our expect function
        new_symbol_table.datatype = tokenVector[index].getTokenString();
        expect(tokenVector[index].getTokenString());
    }

    //then we expect an identifier/ function name
    if (tokenVector[index].isIdentifier() ) {
        if (datatype_specifier() || reserved_word()){
            throw std::runtime_error("Syntax error on line " + std::to_string(tokenVector[index].getLineNum()) + ": reserved word \"" +
                                     tokenVector[index].getTokenString() + "\" cannot be used for the name of a function.");
        }
        tokenVector[index].setIsFuncName();
        new_symbol_table.identifier_name = tokenVector[index].getTokenString();
        tempFunctionName = tokenVector[index].getTokenString();

        //new set the symbollocation here
        SymbolLocation = tokenVector[index].getTokenString();

        expect(tokenVector[index].getTokenString());
    }

    //tokenVector[index].setIfFuncName();
    // then a open paren
    expect( "(");
    //may need a loop to handle, multiple instances of parameters being declared.
    //if we see the keyword void, in our current token.
    if (tokenVector[index].getTokenString() == "void") {
        //expect the void token.
        expect("void");
    } else {
        // add the new symbol to our table
   //     //std::cout<<"inserting symbol: "<<new_symbol_table.identifier_name <<std::endl;
        symbol_table_list.insertSymbol( new_symbol_table, SymbolLocation);
        //clear the temp symbol table element in our parser class.
        SymbolTable empty_symbol_table;
        new_symbol_table = empty_symbol_table;
        //otherwise we call our paramlist, until we reach a )
        parameter_list();
    }
    //now that we have eaten all our params and put them in paramlists
    paramLists.push_back(tempParamList);

    // after the params are added we want to clear our temp vector storing our params.
    tempParamList.clear();
    //tokenVector[index].setIfFuncName();
    expect( ")");
    expect( "{" );
    // calls compound statement to start the interior of our function block.
    compound_statement();
    expect( "}" );
    inFunction = false;

}

/** **************************************************************************************
Checks procedure_declaration meets BNF requirements
@pre:
@post:
 *****************************************************************************************/
void Parser::procedure_declaration(){
//    SymbolTable empty_symbol_table;
//    new_symbol_table = empty_symbol_table;
    scope++;
    //SymbolTable new_symbol_table;
    new_symbol_table.scope = scope;

    new_symbol_table.identifier_type = tokenVector[index].getTokenString();
    expect( "procedure");
    if ( tokenVector[index].isIdentifier() ) {
        new_symbol_table.identifier_name = tokenVector[index].getTokenString();
        tempFunctionName = tokenVector[index].getTokenString();
        // add the new symbol to our table
    //    //std::cout<<"inserting symbol: "<<new_symbol_table.identifier_name <<std::endl;
        symbol_table_list.insertSymbol( new_symbol_table, SymbolLocation);
        //clear the temp symbol table element in our parser class.
//        //SymbolTable empty_symbol_table;
//        new_symbol_table = empty_symbol_table;
        SymbolLocation = tokenVector[index].getTokenString();
        expect(tokenVector[index].getTokenString());
    }
    expect( "(" );
    if (tokenVector[index].getTokenString() == "void") {
        expect("void");
    } else {

        parameter_list();
        //now that we have eaten all our params and put them in paramlists
        paramLists.push_back(tempParamList);

        // after the params are added we want to clear our temp vector storing our params.
        tempParamList.clear();
    }
    // add the new symbol to our table
//    //std::cout<<"inserting symbol: "<<new_symbol_table.identifier_name <<std::endl;
//    symbol_table_list.insertSymbol( new_symbol_table );
    //clear the temp symbol table element in our parser class.
    SymbolTable empty_symbol_table;
    new_symbol_table = empty_symbol_table;
    expect( ")" );
    expect( "{" );
    compound_statement();
    expect( "}" );
}

/** **************************************************************************************
checks that the parameter lsit  follows BNF rules
@pre: is called when we find parameter for a function
@post:generates cst for parameters
 *****************************************************************************************/
void Parser::parameter_list(){
    inParamList = true;
    //used to revert to incoming value of inDeclaration
    bool revertTo = inDeclaration;
    //if we find a data type
    if ( datatype_specifier() ){
        inDeclaration = true;
        //set data type of paramlist symboltable to this data type.
        new_symbol_table.datatype = tokenVector[index].getTokenString();
        //call expect with that data type
        expect(tokenVector[index].getTokenString() );
    }

    //define the parameters variable name.
    new_symbol_table.identifier_type = tokenVector[index].getTokenString();

    //then expect variable name
    identifier();

    //then an array, can be valid syntax for a param list. so if we find a bracket
    if(tokenVector[index].isLBracket()){
        //call our function to handle an array [i]
        identifier_and_identifier_array_list();
    }

    // then we check to see if a comma is present
    if ( tokenVector[index].isComma()) {
        //if it is then add it
        expect(",");

        //set scope of variable
        new_symbol_table.scope = scope;

        //set the paramfunction identifier for param symbol.
        new_symbol_table.identifier_name = tempFunctionName;

        //set the paramfunction identifier for param symbol.
        new_symbol_table.isParamList = true;

        //save symbol parameter to our vector
        tempParamList.push_back(new_symbol_table);

        //clear the vector
        SymbolTable empty_symbol_table;
        new_symbol_table = empty_symbol_table;
        // and recuse until we are out of parameters to add.
        parameter_list();
    }else if(tokenVector[index].isRParen()){
        //set scope of variable
        new_symbol_table.scope = scope;

        //set the paramfunction identifier for param symbol.
        new_symbol_table.identifier_name = tempFunctionName;

        //set the paramfunction identifier for param symbol.
        new_symbol_table.isParamList = true;

        //save symbol parameter to our vector
        tempParamList.push_back(new_symbol_table);

        //clear the vector
        SymbolTable empty_symbol_table;
        new_symbol_table = empty_symbol_table;
        inParamList = false;
    }

    inDeclaration = revertTo;
}

/** **************************************************************************************
checks that the follwoing statement is a block statement that follows BNF rules
@pre:
@post:
 *****************************************************************************************/
void Parser::block_statement(){
    expect( "{");
    if( !tokenVector[index].isRParen()) {
        compound_statement();
    }
    expect( "}");
}

/** **************************************************************************************
checks that the follwoing statement is a compound statementthat follows BNF rules
@pre: called at the beginning of a scope block
@post:recurses through the scope of that block
 *****************************************************************************************/
void Parser::compound_statement() {

    //unless we find a closing brace
    if ( !tokenVector[index].isRBrace() ) {
        //check for statements and recurse this function.
        statement();
        compound_statement();
    }
}

/** **************************************************************************************
checks that the follwoing statement makes sure its a statemtn defined in BNF
@pre:is called to find a statement
@post: recurses though tokens to create a statement.
 *****************************************************************************************/
void Parser::statement(){

    //make a temp token?
    Token token = tokenVector[index];

    ////std::cout<<"statement called test: ("<< token.getTokenString()<<")"<<std::endl;

    //cases to recurse through a variety of different statements.
    if (token.getTokenString() == "return"){
//        //std::cout<<"return found"<<std::endl;
        return_statement();
    }else if ( token.getTokenString() == "if"){
//        //std::cout<<"if found"<<std::endl;
        selection_statement();
    }else if (token.getTokenString() == "else"){
//        //std::cout<<"else found"<<std::endl;
        selection_statement();
    }else if (token.getTokenString() == "printf"){
 //       //std::cout<<"printf found"<<std::endl;
        printf_statement();
    }else if (token.getTokenString() == "for" || token.getTokenString() == "while"){
//        //std::cout<<"for/while found"<<std::endl;
        iteration_statement();
    } else if ((token.isIdentifier() && tokenVector[index + 1].isAssignmentOperator()) ||
               (token.isIdentifier() && tokenVector[index + 1].isLBracket()) ||
               (token.isIdentifier() && tokenVector[index + 1].isLParen())){
        if (tokenVector[index + 1].isAssignmentOperator() || token.isIdentifier() && tokenVector[index + 1].isLBracket()) {
//            //std::cout << "assignment found" << std::endl;
            assignment_statement();
        }else if (tokenVector[index + 1].isLParen()){
            ////std::cout << "user defined func found" << std::endl;
            user_defined_function();
            expect(";");
        }

        //case which handles variable declarizations and assignments.
    } else if ( datatype_specifier() || token.isIdentifier() ) {
//        //std::cout<<"declaration found: "<< token.getTokenString()<<std::endl;
        if(datatype_specifier()){
            token.print();
            inDeclaration = true;
        }
        declaration_statement();
        inDeclaration = false;
    }else{
 //       //std::cout<<"error?"<<std::endl;
    }
}

/** **************************************************************************************
checks that the follwoing statement is a return statement that follows BNF rules
@pre:
@post:
 *****************************************************************************************/
void Parser::return_statement(){
    expect("return");
    if (tokenVector[index].isSingleQuote() || tokenVector[index].isDoubleQuote()) {
        expect(tokenVector[index].getTokenString());
    }
    else {
        expression();
    }
    expect(";");
}

/** **************************************************************************************
Checks declaration_statement meets BNF requirements
if scope is 0 and name is already taken then error for being defined globally
if scope is greater than 0 and name is already taken then error for being defined locally
@pre:
@post:
 *****************************************************************************************/
void Parser::declaration_statement(){
 //   //std::cout<<"in declaration_statement"<<std::endl;
    //if we recieve a specifier(data type)
    //SymbolTable new_symbol_table;
    new_symbol_table.scope = scope;
    if ( datatype_specifier() ){
        new_symbol_table.datatype = tokenVector[index].getTokenString();
        new_symbol_table.identifier_type = "datatype";

        expect( tokenVector[index].getTokenString());

        //then we expect an identifier after it.
        if (tokenVector[index].isIdentifier()) {
            new_symbol_table.identifier_name = tokenVector[index].getTokenString();
    //        //std::cout<<"found identifier after a data specifier."<<std::endl;
            identifier();
            //double check this
            //expect(tokenVector[index].getTokenString());
        } else {
            throw;
        }
    }

    //if we see a left bracket we are declaring an array
    if ( tokenVector[index].isLBracket()){
        identifier_and_identifier_array_list();
        if (inDeclaration) {
            new_symbol_table.DATATYPE_IS_ARRAY = "yes";
        }
    }

    // add the symbol but dont clear the temp data, as we may have more to add if there is a comma, but dont want to clear the values being declared.
    if(inDeclaration == true){
//        //std::cout<<"inserting symbol: "<<new_symbol_table.identifier_name <<std::endl;
        symbol_table_list.insertSymbol(new_symbol_table,SymbolLocation);
    }else{
 //       //std::cout<<"found symbol but not inserting: "<<new_symbol_table.identifier_name <<std::endl;
    }

    //if we see a comma after ward, that means that multiple things are being declared
    if ( tokenVector[index].isComma()){
        expect(",");
 //       //std::cout<<"found a comma now going into identifier_and_identifier_array_list."<<std::endl;
        identifier_and_identifier_array_list();
    }
    //expect statement to end with a semicolon.
    if(tokenVector[index].isRParen()){
        return;
    }else{

        //clear the vectorafter we are done with declaration
        SymbolTable empty_symbol_table;
        new_symbol_table = empty_symbol_table;
        inDeclaration = false;
        expect(";");
    }
}

/** **************************************************************************************
checks that the follwoing function is a user_defined_function  that follows BNF rules
@pre:
@post:
 *****************************************************************************************/
void Parser::user_defined_function(){
    if (tokenVector[index].isIdentifier()) {
        expect( tokenVector[index].getTokenString() );
    }
    tokenVector[index].setImportant();
    expect("(");
    if (tokenVector[index].isIdentifier()) {
        identifier_and_identifier_array_list();
    } else{
        expression();
    }
    tokenVector[index].setImportant();
    expect(")");

}

/** **************************************************************************************
checks that the follwoing function is a getchar function that follows BNF rules
@pre:
@post:
 *****************************************************************************************/
void Parser::getchar_function(){
    expect("getchar");
    expect("(");
    if (tokenVector[index].isIdentifier()) {
        expect( tokenVector[index].getTokenString() );
    }
    expect(")");

}

/** **************************************************************************************
checks that the follwoing statement is a printf statement that follows BNF rules
@pre:
@post:
 *****************************************************************************************/
void Parser::printf_statement(){

    expect("printf");
    expect("(");
    //after the first double quote
    if (tokenVector[index].isDoubleQuote()){
        //eat the string
        double_quoted_string();
        //if there is a comma, variables may be involved, so acount for those here
        if ( tokenVector[index].isComma() ){
            expect(",");
            identifier_and_identifier_array_list();
        }
    }else if (tokenVector[index].isSingleQuote()){
        single_quoted_string();
        //if there is a comma, variables may be involved, so acount for those here
        if ( tokenVector[index].isComma() ){
            expect(",");
            identifier_and_identifier_array_list();
        }
    }
    expect(")");
    expect(";");

}

/** **************************************************************************************
checks that the follwoing statement is a assignment statement that follows BNF rules
@pre:
@post:
 *****************************************************************************************/
void Parser::assignment_statement(){
    inImportantExp = true;
    expect( tokenVector[index].getTokenString() );
    if (tokenVector[index].isLBracket()){
        tokenVector[index].setImportant();
        expect("[");
        expect( tokenVector[index].getTokenString() );
        tokenVector[index].setImportant();
        expect("]");
    }
    expect("=");

    if (tokenVector[index].isSingleQuote()){
        single_quoted_string();
    }else if (tokenVector[index].isDoubleQuote()) {
        double_quoted_string();
    }else {
        expression();
    }
    expect(";");
    inImportantExp = false;
}

/** **************************************************************************************
checks that the follwoing statement is an iteration statement that follows BNF rules
@pre:
@post:
 *****************************************************************************************/
void Parser::iteration_statement() {

    //if identifier is a for loop then
    if (tokenVector[index].getTokenString() == "for") {

        //set for loop to true, so that we handle semicolons correctly within for statement
        inForLoop = true;
        expect(tokenVector[index].getTokenString());
        expect("(");
        ////std::cout<<"entering initilization statement for for loop"<<std::endl;
        initialization_statement();
        if (tokenVector[index].isSemicolon()) {
            expect(tokenVector[index].getTokenString());
        }
        ////std::cout<<"entering boolean statement for for loop"<<std::endl;
        boolean_expression();
        if (tokenVector[index].isSemicolon()) {
            expect(tokenVector[index].getTokenString());
        }
        ////std::cout<<"entering increment expression statement for for loop"<<std::endl;
        expression();
        expect(")");
        inForLoop = false;
        if (tokenVector[index].isLBrace()) {
            block_statement();
        } else {
            statement();
        }
        //set for loop to false, so we handle semicolons normally after this.
        //  inForLoop = false;
    } else if (tokenVector[index].getTokenString() == "while" ){
        expect(tokenVector[index].getTokenString());
        expect("(");
        boolean_expression();
        expect(")");
        if (tokenVector[index].isLBrace()) {
            block_statement();
        } else {
            statement();
        }
    }
}

/** **************************************************************************************
checks that the follwoing statement is a selection statement that follows BNF rules
@pre: when token starts a if
@post: generate if cst statement
 *****************************************************************************************/
void Parser::selection_statement(){
    inImportantExp = true;
    //first token should be a if statement.
    if (tokenVector[index].getTokenString() == "if"){
        expect(tokenVector[index].getTokenString());
    }

    //then a open paren
    expect("(");
    //bool expression
    boolean_expression();
    //closed paren
    expect(")");
    inImportantExp = false;
    // then a block statement
    if (tokenVector[index].isLBrace()) {
        //recurse througho ur block function.
        block_statement();
        if (tokenVector[index].getTokenString() == "else"){
            expect(tokenVector[index].getTokenString());
            if (tokenVector[index].isLBrace()) {
                block_statement();
            } else {
                statement();
            }
        }
    } else {
        statement();
        if (tokenVector[index].getTokenString() == "else "){
            expect(tokenVector[index].getTokenString());
            if (tokenVector[index].isLBrace()) {
                block_statement();
            } else {
                statement();
            }
        }
    }
}

/** **************************************************************************************
checks that the follwoing statement is a initialization expression that follows BNF rules
@pre:
@post:
 *****************************************************************************************/
void Parser::expression(){
//    //std::cout<<"in expression"<<std::endl;
    //if we find a token to be true or false boolean then
    if (tokenVector[index].isBoolTrue() || tokenVector[index].isBoolFalse() ) {
        //call our boolean expression code
        boolean_expression();
        //if we find a int
    } else if (tokenVector[index].isInt()) {
        //call numeric expression
        numerical_expression();
        //if param is integer then set it.
        //new_symbol_table.identifier_type =
        //if we find a identifier and a left paren
    } else if (tokenVector[index].isIdentifier() && ( ( tokenVector[index + 1].isLParen()) )) {

        //sets paramlist with given function name.
        //new_symbol_table.identifier_name = tokenVector[index].getTokenString();

        //built in function to search our symbol table to see if the function exists and extracts its data type for the param list.
        //new_symbol_table.datatype = symbol_table_list.findFunctionDataType(tokenVector[index].getTokenString());

        ////std::cout<<"in expression, expecting function with paren"<<std::endl;
        //then we found a function and want to encase the next expression in parenthesis
        expect( tokenVector[index].getTokenString() );


        //set the symbol table struct to take a paramlist
        //new_symbol_table.isParamList = true;
        // set scope of paramlist, is it always one for function calls or is it dependant on other factors?
        //new_symbol_table.scope = 1;



        //note: how do we set the data type? has to match the function definition right? maybe search for it in our symbol table?
        tokenVector[index].setImportant();
        expect("(");
        //set new symbol table object
        //new_symbol_table.identifier_type = tokenVector[index].getTokenString();
        expression();
        tokenVector[index].setImportant();
        expect(")");
        //need to return symbol here for the function call.
        //symbol_table_list.insertSymbol( new_symbol_table );
        //clear the temp symbol table element in our parser class.
        //SymbolTable empty_symbol_table;
        //new_symbol_table = empty_symbol_table;

        return;
        //if we find a identifier and therei s a demicolon agter it
    }else if (tokenVector[index].isIdentifier() && ( ( tokenVector[index + 1].isSemicolon() ) )) {
//        //std::cout<<"in expression, expecting function with paren"<<std::endl;
        //expect the identifier and then return
        expect( tokenVector[index].getTokenString() );
        return;
        //if we find a identifier and a bracket after it.
    }else if (tokenVector[index].isIdentifier() && ( ( tokenVector[index + 1].isLBracket()))) {
        //then we found an array and expect it to close properly
 //       //std::cout<<"in expression, expecting function with bracket so array"<<std::endl;
        //eat the identifier
        expect( tokenVector[index].getTokenString() );

        //then handle array with out declaration statement code.
        declaration_statement();
        return;
    }else if (tokenVector[index].isIdentifier()) {
//        //std::cout<<"in expression, expecting variable"<<std::endl;

        //if we see a assignment operator after a identifier
        if(tokenVector[index + 1].isAssignmentOperator()){
            //eat identifier and then assignment operator
            expect(tokenVector[index].getTokenString());
            expect("=");

            //then call our numerical expression function
            numerical_expression();

        }else if ( relational_expression() || relational_expression() ) {
            boolean_expression();
        }
        numerical_expression();
    }
    if ( tokenVector[index].isLParen() ) {
        expect("(");
        expression();
        expect(")");
    }
}


/** **************************************************************************************
checks that the follwoing statement is a initialization expression that follows BNF rules
@pre:
@post:
 *****************************************************************************************/
void Parser::initialization_statement(){

    if (tokenVector[index].isIdentifier() && ( tokenVector[index + 1].isAssignmentOperator() ) ) {
        expect( tokenVector[index].getTokenString() );
        expect( tokenVector[index].getTokenString() );
        if  (tokenVector[index].isSingleQuote() ){
            single_quoted_string();
        } else if ( tokenVector[index].isDoubleQuote() ){
            double_quoted_string();
        } else {
            expression();
        }
    }
}







/** **************************************************************************************
checks that the follwoing expression is a boolean expression that follows BNF rules
@pre:recieves start to boolean expression
@post: creates cst expression for boolean
 *****************************************************************************************/
void Parser::boolean_expression(){
//    //std::cout<<"in boolean_expression"<<std::endl;
    // if we find keyword true or false
    if (tokenVector[index].isBoolTrue()){
        expect( "TRUE");

        // then check to see if a operator follows it.
        if(relational_expression()){
            //then another expresion should follow it
            boolean_expression();
        }
    }else if (tokenVector[index].isBoolFalse()){
        expect( "FALSE");

        // then check to see if a operator follows it.
        if(relational_expression()){
            //then another expresion should follow it
            boolean_expression();
        }

        //if we find a identifier
    }else if(tokenVector[index].isBoolNot()){

        //eat !
        expect("!");

        //recursively call our boolean expression
        boolean_expression();

        //if we find a open paren
    }else if(tokenVector[index].isIdentifier()){

        //could be a variable so eat up input
        expect(tokenVector[index].getTokenString());

        //could be an array refrence or a function call
        if(tokenVector[index].isLBracket() || tokenVector[index].isLParen()){
            identifier_and_identifier_array_list();
        }

        //after we figure out what we are working with then check if next symbol is a bool operator
        if(relational_expression()){
            //then another expresion should follow it
            boolean_expression();
            //it could also be a numerical operator which in that case
        }else if(numerical_operator()){
            //then call numericla expression after we eat the numerical operator.
            numerical_expression();
        }

        //if we find a open paren
    }else if(tokenVector[index].isDoubleQuote()){

        // note, do we need to acount for ' (char) ' as well?
        //eat ""
        expect("\"");
        //we expect a string after the first quote
        if(tokenVector[index].isString()){
            //eat string, then quote
            expect(tokenVector[index].getTokenString());
            expect("\"");
        }else{
            //std::cout << "in boolean expression expected a string after a quote but recieved: " << tokenVector[index].getTokenString() << std::endl;
        }

        //recursively call our boolean expression
        boolean_expression();

        //if we find a open paren
    }else if (tokenVector[index].isLParen() ){
        //eat paren
        expect("(");

        if(tokenVector[index].isBoolNot()){
            expect("!");

            //recursively call our boolean expression
            boolean_expression();
            //if we find a open paren
        }

        // if we find a identifier
        if (tokenVector[index].isIdentifier()) {
            //could just receive an identifier by its self so recursive call so it's caught by our
            //case above where the identifier could be a array for function call.
            boolean_expression();

        }
        expect(")");

        if(relational_expression()){
            //then another expression should follow it
            boolean_expression();
            //it could also be a numerical operator which in that case
        }


    }else if (tokenVector[index].isSingleQuote() ){
        //eat paren
        single_quoted_string();

        boolean_expression();

    }else{

        ////std::cout<<"in else case!"<<std::endl;
        numerical_expression();
        if (  relational_expression() ){
            expect( tokenVector[index].getTokenString() );
        }
        numerical_expression();

        //after we figure out what we are working with then check if next symbol is a operator
        if(relational_expression()){
            //then another expression should follow it
            boolean_expression();
        }
    }
    ////std::cout<<"end boolean_expression"<<std::endl;
}



/** **************************************************************************************
checks that the follwoing expression is a numerical expression that follows BNF rules
@pre: is called when we find a neumeric expression for a bool operation
@post:
 *****************************************************************************************/
void Parser::numerical_expression() {
//    //std::cout<<"in numerical_expression"<<std::endl;

    bool eat = true;
    while(eat){

        //loop is empty, but the heavy lifting is done in the bool checks
        if(tokenVector[index].isLParen()){
            ////std::cout<<"begin paren inside of numerical expression"<<std::endl;
            //eat up the parens
            expect("(");
            //we then need to recurse into a new numeric expression call.
            numerical_expression();
            //then we expect a close paren
            expect(")");

            ////std::cout<<"end paren inside of numerical expression"<<std::endl;

        }else{
            eat = numerical_operand();
            //attempt to eat, but not an operand

            if(eat == false && tokenVector[index].isIdentifier()){
                //call expression function to make function call?
                expression();
            }


        }

        //then if there is a operator afterward, need to eat that before going to next loop
        eat = numerical_operator();

        if(relational_expression()){
            boolean_expression();
        }

    }


}


/** **************************************************************************************
checks if token is a relational_expression
@pre:
@post:
 *****************************************************************************************/
bool Parser::relational_expression(){
//    //std::cout<<"in relational_expression/boolean_operator"<<std::endl;
    if (tokenVector[index].isBoolE()){
        expect("==");
        return true;
    }else if (tokenVector[index].isBoolNE()){
        expect("!=");
        return true;
    }else if (tokenVector[index].isBoolGT()){
        expect(">");
        return true;
    }else if (tokenVector[index].isBoolLT()){
        expect("<");
        return true;
    }else if (tokenVector[index].isBoolGTE()){
        expect(">=");
        return true;
    }else if (tokenVector[index].isBoolLTE()){
        expect("<=");
        return true;
    }else if (tokenVector[index].isBoolOr()){
        expect("||");
        return true;
    }else if (tokenVector[index].isBoolAnd()){
        expect("&&");
        return true;
    }else{
        ////std::cout<<"token is not currently a boolean operator"<<std::endl;
        return false;
    }
}





/** **************************************************************************************
checks if token is a boolean operator
@pre:
@post:
 *****************************************************************************************/
bool Parser::numerical_operator(){
    ////std::cout<<"in numerical_operator"<<std::endl;
    if (tokenVector[index].isPlus()){
        expect("+");
        return true;
    }else if (tokenVector[index].isMinus()){
        expect("-");
        return true;
    }else if (tokenVector[index].isAsterisk()){
        expect("*");
        return true;
    }else if (tokenVector[index].isDivide()){
        expect("/");
        return true;
    }else if (tokenVector[index].isModulo()){
        expect("%");
        return true;
    }else if (tokenVector[index].isCarot()){
        expect("^");
        return true;
    }else{
        ////std::cout<<"token is not currently a numerical operator"<<std::endl;
        return false;
    }
}

/** **************************************************************************************
checks if numerical_operand follows BNF
@pre:
@post:
 *****************************************************************************************/
bool Parser::numerical_operand(){
    ////std::cout<<"in numerical_operand"<<std::endl;
    //if we find a int
    if (tokenVector[index].isIdentifier()) {
        if (tokenVector[index + 1].isLParen()) {
            user_defined_function();
        } else {
            expect( tokenVector[index].getTokenString() );
        }
        return true;
        // if we find a identifier
    } else if (tokenVector[index].isInt()){
        expect( tokenVector[index].getTokenString() );

        return true;
        //if we find a char
    }else if (tokenVector[index].getTokenString() == "getchar"){
        getchar_function();

        return true;
        //if we find a single quote
    }else if (tokenVector[index].isSingleQuote()){
        if(inImportantExp){
            tokenVector[index].setImportant();
        }
        expect("'");
        if(tokenVector[index].isChar()){
            expect( tokenVector[index].getTokenString() );
        }else if (tokenVector[index].isString()){
            expect( tokenVector[index].getTokenString());
        }else if (tokenVector[index].isEscChar()){
            expect( tokenVector[index].getTokenString() );
        }else{
            //ERROR ERROR ERROR ERROR ERROR ERROR
        }
        if(inImportantExp){
            tokenVector[index].setImportant();
        }
        expect("'");

        return true;
        //if we find a double quote
    }else if (tokenVector[index].isDoubleQuote()){
        if(inImportantExp){
            tokenVector[index].setImportant();
        }
        expect("\"");
        if(tokenVector[index].isChar()){
            expect( tokenVector[index].getTokenString() );
        }else if (tokenVector[index].isEscChar()){
            expect( tokenVector[index].getTokenString() );
        }else{
            //ERROR ERROR ERROR ERROR ERROR ERROR
        }
        if(inImportantExp){
            tokenVector[index].setImportant();
        }
        expect("\"");
        return true;
    } else {
        //ERROR ERROR ERROR ERROR ERROR ERROR ERROR
        return false;
    }
}






/** **************************************************************************************
returns if token is a datatype specifier
@pre:
@post:
 *****************************************************************************************/
bool Parser::datatype_specifier(){
    return (tokenVector[index].getTokenString() == "int" || tokenVector[index].getTokenString() == "char" ||
            tokenVector[index].getTokenString() == "bool");
}

/** **************************************************************************************
returns if token is a datatype specifier
@pre:
@post:
 *****************************************************************************************/
bool Parser::reserved_word(){
    return (tokenVector[index].getTokenString() == "if" || tokenVector[index].getTokenString() == "else" ||
            tokenVector[index].getTokenString() == "void" || tokenVector[index].getTokenString() == "main" ||
            tokenVector[index].getTokenString() == "procedure" || tokenVector[index].getTokenString() == "for" ||
            tokenVector[index].getTokenString() == "while" || tokenVector[index].getTokenString() == "printf" ||
            tokenVector[index].getTokenString() == "getchar" || tokenVector[index].getTokenString() == "return" ||
            tokenVector[index].getTokenString() == "function");
}





/** **************************************************************************************
checks if token is followed by identifier list or array lsit
@pre:
@post:
 *****************************************************************************************/
void Parser::identifier_and_identifier_array_list() {
//    //std::cout<<"in identifier_and_identifier_array_list"<<std::endl;
    if (tokenVector[index].isInt()){
        index -= 2;
    }

    //special case, if we end up finding a identifier here then
    if(tokenVector[index].isIdentifier()){
 //       //std::cout<<"now eating identifier: "<<tokenVector[index].getTokenString()<<std::endl;

 //       //std::cout<<"new_symbol_table.identifier_type: "<<new_symbol_table.identifier_type<<std::endl;

        //if there are multiple values being declared on one line this should cover it
        //ex int 1,j,k;
        new_symbol_table.identifier_name = tokenVector[index].getTokenString();
        if(inDeclaration == true){
//            //std::cout<<"inserting symbol: "<<new_symbol_table.identifier_name <<std::endl;
            symbol_table_list.insertSymbol(new_symbol_table,SymbolLocation);
        }else{
  //          //std::cout<<"not in declaration so didnt add symbol: "<<new_symbol_table.identifier_name <<std::endl;
        }

        expect(tokenVector[index].getTokenString());
    }

    //after that identifier is there was one, or if we dont hav to worry about one
    //do we have a bracket, if so then we have a array, go to that function logic
    if ( tokenVector[index].isLBracket()) {
        if (inDeclaration) {
            new_symbol_table.DATATYPE_IS_ARRAY = "yes";
        }
        identifier_array_list();

        //otherwise if its a open paren then we have a function call
    }else if (tokenVector[index].isLParen()) {
        if (inImportantExp){
            tokenVector[index].setImportant();
        }
        expect("(");
        identifier_list();
        if (inImportantExp){
            tokenVector[index].setImportant();
        }
        expect(")");

        //if we see a comma by its self then call identifier list. mostelikely called after identifier list was finished with a function call or array[a]
    }else if (tokenVector[index].isComma()) {
        expect(",");
        identifier_list();
    } else if (tokenVector[index].isIdentifier() /*&& tokenVector[index + 1].isSemicolon()*/){
        identifier();

    }else{
        //std::cout<<"flagging to find comma, esaping identifier_and_identifier_array_list: "<<std::endl;
        //throw std::runtime_error("Expected an identifier, but got '" + tokenVector[index].getTokenString() + "'");
    }

    ////std::cout<<"end identifier_and_identifier_array_list"<<std::endl;
}





/** **************************************************************************************
checks identifier  list follows BNF rules
@pre:
@post:
 *****************************************************************************************/
void Parser::identifier_list() {
//    //std::cout<<"in identifier list"<<std::endl;

    //if we find, a identifier, or a data type
    if(tokenVector[index].isIdentifier()){

        new_symbol_table.identifier_name = tokenVector[index].getTokenString();
        //std::cout<<"inserting symbol: "<<new_symbol_table.identifier_name <<std::endl;

        if(inDeclaration == true){
            symbol_table_list.insertSymbol(new_symbol_table,SymbolLocation);
        }

        //eat that value
        expect(tokenVector[index].getTokenString());

        //then if we find a comma after that we recurse on this function
        if(tokenVector[index].isComma()){
            //std::cout<<"recursively calling identifier list"<<std::endl;
            //eat the comma
            expect(",");
            //recurse to get the next identifier in our list
            identifier_list();
            //found a function call or a array in identifier list we call our identifier_and_identifier_array_list() function
        }else if(tokenVector[index].isLParen() || tokenVector[index].isLBracket()){
            identifier_and_identifier_array_list();
        }
    }else{
        //throw error, potentially called after we found a comma but no proper identifier or data type was found.
    }

}





/** **************************************************************************************
checks identifier array list follows BNF rules
@pre:
@post:
 *****************************************************************************************/
void Parser::identifier_array_list() {
    ////std::cout<<" in identifier_array_list"<<std::endl;
    ////std::cout<<"token is: "<<tokenVector[index].getTokenString()<<std::endl;

    if(inImportantExp){
        tokenVector[index].setImportant();
    }
    // ok so we found an array call, we expect to be at the open lbracket
//    cst->getRoot()->getToken()
    expect( "[" );

    if (tokenVector[index].getTokenString().find("-") != std::string::npos){
        throw std::runtime_error("Syntax error on line " + std::to_string(tokenVector[index].getLineNum()) + ": array declaration must be a positive integer.");
    }
        //then we expect either an int which we eat
    else if (tokenVector[index].isInt()) {
        if (inDeclaration) {
            new_symbol_table.datatype_array_size = tokenVector[index].getTokenString();
        }
        expect(tokenVector[index].getTokenString());
        //or an identifier that we eat
    }else if(tokenVector[index].isIdentifier()){
        if (inDeclaration) {
            new_symbol_table.datatype_array_size = tokenVector[index].getTokenString();
        }
        expect(tokenVector[index].getTokenString());
    }
    if(inImportantExp){
        tokenVector[index].setImportant();
    }
    //then we expect a closing bracket
    expect( "]" );

    //recursively call our array if there is an extra dimension to our array.
    if ( tokenVector[index].isComma() ) {
        if (inParamList){
            return;
        }else {
            identifier_array_list();
        }
    }
    ////std::cout<<" ending identifier_array_list"<<std::endl;
}





/** **************************************************************************************
checks if token is identifier
@pre:
@post:
 *****************************************************************************************/
void Parser::identifier(){
    if (!tokenVector[index].isIdentifier()){
        throw std::runtime_error("Expected an identifier, but got '" + tokenVector[index].getTokenString() + "'");
    }
    if (datatype_specifier() || reserved_word()){
        throw std::runtime_error("Syntax error on line " + std::to_string(tokenVector[index].getLineNum()) + ": reserved word \"" +
                                 tokenVector[index].getTokenString() + "\" cannot be used for the name of a variable.");
    }
    //loop through takenNames to see if name has been used try to also determine if
    //defined locally or globally
    for(int i = 0; i < takenNames.size(); i++){
        if(takenNames[i].first == tokenVector[index].getTokenString() && takenNames[i].second == 0){
            std::cerr << "Error on line " << std::to_string(tokenVector[index].getLineNum())
                      << ": variable \"" << tokenVector[index].getTokenString() << "\" is already defined globally" << std::endl;
            exit(1);
        }
        else if(takenNames[i].first == tokenVector[index].getTokenString() && takenNames[i].second == scope){
            std::cerr << "Error on line " << std::to_string(tokenVector[index].getLineNum())
                      << ": variable \"" << tokenVector[index].getTokenString() << "\" is already defined locally" << std::endl;
            exit(1);
        }
    }
    //add name and scope to vector for later checks
    takenNames.push_back(std::make_pair(tokenVector[index].getTokenString(), scope));
    expect( tokenVector[index].getTokenString() );
}


/** **************************************************************************************
checks if single quoted string
@pre:
@post:
 *****************************************************************************************/
void Parser::single_quoted_string(){
 //   //std::cout<<"in single quote"<<std::endl;
    if (!tokenVector[index].isSingleQuote()){
        throw std::runtime_error("Expected a single quote, but got '" + tokenVector[index].getTokenString() + "'");
    }
    if (inImportantExp) {
        tokenVector[index].setImportant();
    }
    expect( "\'");
    expect(tokenVector[index].getTokenString());
    if (inImportantExp) {
        tokenVector[index].setImportant();
    }
    expect("\'");
}

/** **************************************************************************************
checks if double quoted string
@pre:
@post:
 *****************************************************************************************/
void Parser::double_quoted_string(){
    if (!tokenVector[index].isDoubleQuote()){
        throw std::runtime_error("Expected a double quote, but got '" + tokenVector[index].getTokenString() + "'");
    }
    if (inImportantExp) {
        tokenVector[index].setImportant();
    }
    expect( "\"");
    expect(tokenVector[index].getTokenString());
    if (tokenVector[index].isEOF()){
        throw std::runtime_error("Syntax error on line " + std::to_string(tokenVector[index].getLineNum()) + ": unterminated string quote.");
    }
    if (inImportantExp) {
        tokenVector[index].setImportant();
    }
    expect("\"");
}

/** **************************************************************************************
If the token string matches expected_value then it adds it to the CST tree
@pre:
@post:
 *****************************************************************************************/
void Parser::expect(const std::string& expected_value) {
    //protective if to ensure we are within out vector of tokens.
    if (index < tokenVector.size()) {
        //make a token defined with the current token in our vector at our index position.
        Token token = tokenVector[index];
        //tokenVector[index].print();        //Use this to debug and know which token youre at
        if (token.getTokenString() != expected_value) {
            throw std::runtime_error("Expected '" + expected_value + "' but got: '" + tokenVector[index].getTokenString() + "' on line " + std::to_string(tokenVector[index].getLineNum()));
        }

        //if index is zero, then make new root
        if (index == 0){
            cst->setRoot(new CSTNode(tokenVector[index]));
            //if we find a left or right brace then make a right child of that token
        }else if (token.isLBrace()|| token.isRBrace() ){
            newStatement = true;
            cst->addChild( cst->getRoot(), token );
            //sibling for semicolon, but only if we arnt in a for loop
        }else if ( token.isSemicolon() && inForLoop == false){
            newStatement = true;
            cst->addSibling( cst->getRoot(), token );

            //sibling if we are in a for loop then we havent entered a new statement.
        }else if ( token.isSemicolon() && inForLoop == true){
            newStatement = false;
            cst->addSibling( cst->getRoot(), token );

        }else if (newStatement){
            cst->addChild( cst->getRoot(), token );
            newStatement = false;
        } else {
            if (tokenVector[index + 1].isLBracket()) {
                token.setArray();
            }
            cst->addSibling( cst->getRoot(), token ) ;
        }
        index++;
    } else {
        throw std::runtime_error("Unexpected end of input");
    }


}

void Parser::PrintSymbolTableLL( ){
    symbol_table_list.PrintSymbolTableList();
}

//converts cst to ast, then returns the ast in cst
CST* Parser::convertToAST(){
    cst->cstToAst();
    return cst;
}

void Parser::assignAddress(){
    //std::cout<<"starting adressing of ast"<<std::endl;
    assignAddressHelper(cst->getRoot(), 0);
}
//function applies a address to every node in our ast
void Parser::assignAddressHelper(CSTNode *root, int address) {

    //if nullptr then return
    if ( root == nullptr ) {
        return;
    }
    //if right is not null, then traverse to the right sibling
    if (root->getRight() != nullptr ){

        root->setLocation(address);
        //std::cout<<"address: "<<address<<" token: "<<root->getToken().getTokenString()<<std::endl;
        assignAddressHelper(root->getRight(), address+1);

        //if left is not nullptr traverse left sibling
    }else if ( root->getLeft() != nullptr ){

        //set address of the token in the node to the current address
        root->setLocation(address);
        //std::cout<<"address: "<<address<<" token: "<<root->getToken().getTokenString()<<std::endl;

        //if the token is a function wee need to look it up
        if(root->getToken().isFunction()){

            //if the token is main thenwe need to look up
            if (root->getToken().isMain()){

                //std::cout<<"address to main: "<< root->getToken().getFunctionName() <<std::endl;
                symbol_table_list.setAddress( symbol_table_list.lookupSymbol( root->getToken().getFunctionName() ), address);

                SymbolLocation = "main";
                //check to see if address is set properly.
                addFunction("main",address);

                //push the main function onto the stack to get us started when interpreting.
                callStack.push_back(address);

            }else{
                // add the function to our function table
                addFunction(root->getToken().getFunctionName(),address);

                //std::cout<<"address to function: "<< root->getToken().getFunctionName() <<std::endl;
                symbol_table_list.setAddress( symbol_table_list.lookupSymbol( root->getToken().getFunctionName() ), address);
            }

        }
        address++;

        //So we know how many addresses/elements we have
        highestAddress = address - 1;

        assignAddressHelper(root->getLeft(), address);
    }
    else{
        root->setLocation(address);
        //std::cout << "address: " << address << " token: " << root->getToken().getTokenString() << std::endl;
        assignAddressHelper(root->getRight(), address + 1);
        return;
    }
}

//look up function that checks our list of stored functions and there memory addresses.
int Parser::lookUpFunction(std::string functionName){
    
    //search our addres vector
    for (int i = 0; i < functionAddresses.size(); i++) {
        ////std::cout<< std::get<0>(functionAddresses[i]) <<std::endl;

        //if the function name matches the identifier
        if(std::get<0>(functionAddresses[i]) == functionName){
            
//            //std::cout<< "found function in address book: "<<std::get<0>(functionAddresses[i])<<std::endl;

            //return the memory location of that function.
            return std::get<1>(functionAddresses[i]);

        }
    }

    //fail case, if it could not be found return -1
    return -1;
}


std::string Parser::lookUpFunctionFromAddress(int addressloc){

    //search our addres vector
    for (int i = 0; i < functionAddresses.size(); i++) {
        ////std::cout<< std::get<0>(functionAddresses[i]) <<std::endl;

        //if the function name matches the identifier
        if(std::get<1>(functionAddresses[i]) == addressloc){
            
    //        //std::cout<< "found function in address book: "<<std::get<1>(functionAddresses[i])<<std::endl;

            //return the memory location of that function.
            return std::get<0>(functionAddresses[i]);

        }
    }

        //fail case, if it could not be found return -1
    //    //std::cout<< "couldnot find function at address: "<<addressloc<<std::endl;
        throw;
        return "";
}

//adds a function name and memory location
void Parser::addFunction(std::string functionName, int addressLoc){

 //   //std::cout<< "adding: "<<functionName<<" to table with memory address of: "<<addressLoc<<std::endl;
    //make a tuple with the new address and mem location
    functionAddresses.push_back(std::make_tuple(functionName, addressLoc));

}

void Parser::postFixEval(std::vector<Token> postfix,int callStartAddress){

    //stack to store operations
    std::stack<int> evalStack;
    //std::cout<<"Tokens in Operation"<<std::endl;
    for (int i = 0; i < postfix.size(); i++) {
        //std::cout<<postfix[i].getTokenString()<<" ";
        if (postfix[i].isIdentifier()){

        }
    }
  //  //std::cout<<""<<std::endl;

    //loop through vector of tokens
    for (int i = 1; i < postfix.size(); i++) {

        //If it's an operand, push its value to the stack
        if(postfix[i].isIdentifier() || postfix[i].isInt() || postfix[i].isDouble() || postfix[i].isString()) {
            if(postfix[i].isString()){
                int charToAscii = static_cast<int>(postfix[i].getTokenString().at(0));
                //std::cout << "charToAscii = " << charToAscii << std::endl;
                evalStack.push(charToAscii);
            }else if (postfix[i].isInt() || postfix[i].isDouble()) {
                int value = std::stoi(postfix[i].getTokenString());
                evalStack.push(value);
            }else{
                SymbolNode* variableToDigit = symbol_table_list.lookupSymbolAtLocation(postfix[i].getTokenString(),
                                                                                       lookUpFunctionFromAddress(callStartAddress));
                //std::cout << " var name = " << variableToDigit->symbolTable.identifier_type << "--var value = " << variableToDigit->variableVal << "--------------------------" << std::endl;
                int varToInt = variableToDigit->variableVal;
                //symbol_table_list.printTable(variableToDigit);
                evalStack.push(varToInt);
            }
        }
            //If it's an operator, evaluate the expression
        else if (postfix[i].isMinus() || postfix[i].isPlus() ||
                 postfix[i].isModulo() || postfix[i].isAsterisk() ||
                 postfix[i].isDivide()) {

            // Perform the operation based on the operator
            int operand2 = evalStack.top();
            evalStack.pop();
            int operand1 = evalStack.top();
            evalStack.pop();

            if (postfix[i].isPlus()) {
                evalStack.push(operand1 + operand2);
            }
            else if (postfix[i].isMinus()) {
                evalStack.push(operand1 - operand2);
            }
            else if (postfix[i].isAsterisk()) {
                evalStack.push(operand1 * operand2);
            }
            else if (postfix[i].isDivide()) {
                if (operand2 == 0) throw std::runtime_error("Division by zero error!");
                evalStack.push(operand1 / operand2);
            }
        } else if (postfix[i].isAssignmentOperator()){
            //Need to finish
            //test example as to how to set the value in our symbol table
            //searches for the symbol, using
            //postfix[i].getTokenString() = name of variable
            //lookUpFunctionFromAddress(callStartAddress) = gets name of function where call was made.
            //->variable val. get the value from the symbol table object. can set like below.

            SymbolNode* nodeEval = symbol_table_list.lookupSymbolAtLocation(postfix[0].getTokenString(),lookUpFunctionFromAddress(callStartAddress));
            symbol_table_list.setVarVal(nodeEval, evalStack.top());
            //std::cout<<"************************************************************************************"<<std::endl;
            //std::cout<<"Assigning the variable: "<<postfix[0].getTokenString()<<" with the value: "<<nodeEval->variableVal<<std::endl;

            evalStack.pop();
            //symbol_table_list.printTable(nodeEval);

        }

    }

}

void Parser::postFixEvalChar(std::vector<Token> postfix,int callStartAddress){
    //stack to store operations
    std::stack<int> evalStack;
    //std::cout<<"Tokens in Operation"<<std::endl;
    for (int i = 0; i < postfix.size(); i++) {
        //std::cout<<postfix[i].getTokenString()<<" ";
    }
    //std::cout<<""<<std::endl;
    //loop through vector of tokens
    for (int i = 1; i < postfix.size(); i++) {
        //If its a double quote then we evaluate the string by separating into chars and skip quotes on either side
        if(postfix[i].isDoubleQuote()) {
            if (postfix[i+1].isAssignmentOperator()){
                i++;
                continue;
            }
            std::string stringToChar = postfix[++i].getTokenString();
            for(int j = 0; j < stringToChar.size(); j++){
                if (stringToChar[j] == '\\'){
                    j++;
                    if (stringToChar[j] == 'x'){
                        j++;
                        if (stringToChar[j] == '0'){
                            if (arrayMapChar.find(postfix[0].getTokenString()) != arrayMapChar.end()) {
                                arrayMapChar[postfix[0].getTokenString()].push_back(48);
                            }
                        }
                    }
                }else{
                    if (arrayMapChar.find(postfix[0].getTokenString()) != arrayMapChar.end()) {
                        arrayMapChar[postfix[0].getTokenString()].push_back(stringToChar[j]);
                    }
                }
            }
            //If its a single quote we evaluate the char and skip the quotes on either side
            //We also add the values to the vector where needed
        }else if (postfix[i].isSingleQuote()){
            //DO STUFF
            //If its an operator, evaluate the expression
        }else if (postfix[i].isLBracket()){
        }else if (postfix[i].isMinus() || postfix[i].isPlus() ||
                  postfix[i].isModulo() || postfix[i].isAsterisk() ||
                  postfix[i].isDivide()) {
            // Perform the operation based on the operator
            int operand2 = evalStack.top();
            evalStack.pop();
            int operand1 = evalStack.top();
            evalStack.pop();
            if (postfix[i].isPlus()) {
                evalStack.push(operand1 + operand2);
            }
            else if (postfix[i].isMinus()) {
                evalStack.push(operand1 - operand2);
            }
            else if (postfix[i].isAsterisk()) {
                evalStack.push(operand1 * operand2);
            }
            else if (postfix[i].isDivide()) {
                if (operand2 == 0) throw std::runtime_error("Division by zero error!");
                evalStack.push(operand1 / operand2);
            }
        } else if (postfix[i].isAssignmentOperator()){
            //Need to finish
            //test example as to how to set the value in our symbol table
            //searches for the symbol, using
            //postfix[i].getTokenString() = name of variable
            //lookUpFunctionFromAddress(callStartAddress) = gets name of function where call was made.
            //->variable val. get the value from the symbol table object. can set like below.
            evalStack.pop();
//            SymbolNode* nodeEval = symbol_table_list.lookupSymbolAtLocation(postfix[0].getTokenString(),lookUpFunctionFromAddress(callStartAddress));
//            symbol_table_list.setVarVal(nodeEval, evalStack.top());
//            evalStack.pop();
//            symbol_table_list.printTable(nodeEval);
        }
    }
}


bool Parser::postFixEvalBool(std::vector<Token> postfix,int callStartAddress){

    //stack to store operations
    //in (integer value, boolean value, boolean value to determine if its a int or a boolean. false = int, true = boolean.)
    std::stack<std::tuple<int,bool,bool>> evalStack;

    //std::cout<<"Tokens in Operation"<<std::endl;
    for (int i = 0; i < postfix.size(); i++) {
        //std::cout<<postfix[i].getTokenString()<<" ";
    }
    //std::cout<<""<<std::endl;

    //loop through vector of tokens
    for (int i = 0; i < postfix.size(); i++) {

        //If it's an operand, push its value to the stack
        if(postfix[i].isIdentifier() || postfix[i].isInt() || postfix[i].isDouble() || postfix[i].isString()) {
            if(postfix[i].isString()){
                int charToAscii = static_cast<int>(postfix[i].getTokenString().at(0));
                //std::cout << "charToAscii = " << charToAscii << std::endl;
                evalStack.push(std::make_tuple(charToAscii,false,false));
            }else if (postfix[i].isInt() || postfix[i].isDouble()) {
                int value = std::stoi(postfix[i].getTokenString());

                evalStack.push(std::make_tuple(value,false,false));
            }else {
                SymbolNode* variableToDigit = symbol_table_list.lookupSymbolAtLocation(postfix[i].getTokenString(),
                                                                                       lookUpFunctionFromAddress(callStartAddress));
                int varToInt = variableToDigit->variableVal;
                //symbol_table_list.printTable(variableToDigit);
                evalStack.push(std::make_tuple(varToInt,false,false));
            }
        }
            //If it's an operator, evaluate the expression
        else if (postfix[i].isBoolE() || postfix[i].isModulo() ||
                 postfix[i].isBoolNE() || postfix[i].isBoolGT() ||
                 postfix[i].isBoolLT()|| postfix[i].isBoolGT() ||
                 postfix[i].isBoolGTE() || postfix[i].isBoolLTE()||
                 postfix[i].isBoolAnd() || postfix[i].isBoolOr() ||
                 postfix[i].isBoolTrue() ||postfix[i].isBoolFalse()) {

            // Perform the operation based on the operator
            std::tuple<int,bool,bool> operand2;
            std::tuple<int,bool,bool> operand1;

            //if we have a int value in our bool expression that set op2 correctly
            if(std::get<2>(evalStack.top()) == false ){

                //return tuple with a int
                operand2 = std::make_tuple(std::get<0>(evalStack.top()), false,false);
                //else if we have a boolean
            }if(std::get<2>(evalStack.top()) == true){

                //return a tuple with the boolean
                operand2 = std::make_tuple(0,std::get<1>(evalStack.top()),true);

            }
            //if tuple formed correctly then pop stack
            evalStack.pop();

            //if we have a int value in our bool expression that set op2 correctly
            if(std::get<2>(evalStack.top()) == false ){

                //return tuple with a int
                operand1 = std::make_tuple(std::get<0>(evalStack.top()), false,false);
                //else if we have a boolean
            }if(std::get<2>(evalStack.top()) == true){

                //return a tuple with the boolean
                operand1 = std::make_tuple(0,std::get<1>(evalStack.top()),true);

            }
            //if tuple formed correctly then pop stack
            evalStack.pop();

            //now to decide if we are working on two booleans, or two ints

            // if we are working with 2 ints.
            if(std::get<2>(operand1) == false && std::get<2>(operand2) == false){

                //and bool operater of two ints produces a boolean
                if (postfix[i].isBoolE()) {
                    bool results = (std::get<0>(operand1) == std::get<0>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolNE()) {
                    bool results = (std::get<0>(operand1) != std::get<0>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolGT()) {
                    bool results = (std::get<0>(operand1) > std::get<0>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolLT()) {
                    bool results = (std::get<0>(operand1) < std::get<0>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolGTE()) {
                    bool results = (std::get<0>(operand1) >= std::get<0>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolLTE()) {
                    bool results = (std::get<0>(operand1) <= std::get<0>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolAnd()) {
                    bool results = (std::get<0>(operand1) && std::get<0>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolOr()) {
                    bool results = (std::get<0>(operand1) || std::get<0>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isPlus()) {
                    int results = (std::get<0>(operand1) + std::get<0>(operand2));
                    evalStack.push(std::make_tuple(results,false,false));
                }
                else if (postfix[i].isMinus()) {
                    int results = (std::get<0>(operand1) - std::get<0>(operand2));
                    evalStack.push(std::make_tuple(results,false,false));
                }
                else if (postfix[i].isAsterisk()) {
                    int results = (std::get<0>(operand1) * std::get<0>(operand2));
                    evalStack.push(std::make_tuple(results,false,false));
                }
                else if (postfix[i].isDivide()) {
                    if (std::get<0>(operand2) == 0) throw std::runtime_error("Division by zero error IN bool!");

                    int results = (std::get<0>(operand1) / std::get<0>(operand2));
                    evalStack.push(std::make_tuple(results,false,false));
                }else if (postfix[i].isModulo()) {
                    int results = (std::get<0>(operand1) % std::get<0>(operand2));
                    evalStack.push(std::make_tuple(results,false,false));
                }

                //otherwise handle if we have two booleans.
            }else if(std::get<2>(operand1) == true && std::get<2>(operand2) == true){

                if (postfix[i].isBoolE()) {
                    bool results = (std::get<1>(operand1) == std::get<1>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolNE()) {
                    bool results = (std::get<1>(operand1) != std::get<1>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolGT()) {
                    bool results = (std::get<1>(operand1) > std::get<1>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolLT()) {
                    bool results = (std::get<1>(operand1) < std::get<1>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolGTE()) {
                    bool results = (std::get<1>(operand1) >= std::get<1>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolLTE()) {
                    bool results = (std::get<1>(operand1) <= std::get<1>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolAnd()) {
                    bool results = (std::get<1>(operand1) && std::get<1>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }
                else if (postfix[i].isBoolOr()) {
                    bool results = (std::get<1>(operand1) || std::get<1>(operand2));
                    evalStack.push(std::make_tuple(0, results,true));
                }

            }else{

                //std::cout<<"opperator expression has malformed stack input. "<<std::endl;
                //std::cout<<" opperator2: ( "<<std::get<0>(operand2)<<" , "<<std::get<1>(operand2)<<" , "<<std::get<2>(operand2)<<" )"<<std::endl;
                //std::cout<<" opperator1: ( "<<std::get<0>(operand1)<<" , "<<std::get<1>(operand1)<<" , "<<std::get<2>(operand1)<<" )"<<std::endl;
                throw;
            }
            //std::cout<<" opperator2: ( "<<std::get<0>(operand2)<<" , "<<std::get<1>(operand2)<<" , "<<std::get<2>(operand2)<<" )"<<std::endl;
            //std::cout<<" opperator1: ( "<<std::get<0>(operand1)<<" , "<<std::get<1>(operand1)<<" , "<<std::get<2>(operand1)<<" )"<<std::endl;
        }

    }
    if(std::get<1>(evalStack.top())){
//        //std::cout<<"bool Expression Returning true"<<std::endl;
    }else{
//        //std::cout<<"bool Expression Returning false"<<std::endl;
    }

    return std::get<1>(evalStack.top());
}

void Parser::interpret() {

    //gets the root of the cst
    CSTNode* root = cst->getRoot();

    //important variable to keep track of scope.
    int blockScope = 0;

    //define the node of main to start interpret. address should be on stack so set node to the search of that address
    CSTNode* mainNode = cst->getNodeAtAddress(callStack.back());

    //save the address where this call started.
    int callStartAddress = callStack.back();

    //important for figuring out when to stop interpreting. useful to keep track of function call interps as well.
    int callStackScope = callStack.size();

    //tester to see where we currently are interpreting.
    if(callStack.size()< 2){
        //std::cout << "main Address is: " << callStack.back() << std::endl;
    }else{
        //std::cout << "function Address is: " << callStack.back() << std::endl;
    }

    

    //we want to loop until we hit a return, or fall out of scope. that will happen when we pop off the current callstack.back()
    while (callStackScope == callStack.size()){

        //saftey net incase we hit a nullptr?
        if (mainNode != nullptr) {

            //the current node we are checking
            CSTNode *currentNode = cst->getNodeAtAddress(callStack.back());

            //for testing purposes to see what token we are looking at.
            std::string statement = currentNode->getToken().getTokenString();

            

            //if we find a begin block want to increase our blockScope by one
            if (statement == "BEGIN BLOCK") {

                blockScope++;
                //std::cout << "BEGIN BLOCK ADDRESS: " << callStack.back() <<" statement: "<<statement<<" blockScope: " << blockScope << std::endl;
                callStack.back()++;

            //like wise if we find a end block then we de increment the blockScope
            }else if (statement == "END BLOCK") {

                blockScope--;
                //std::cout << "END BLOCK ADDRESS: " << callStack.back() <<" statement: "<<statement<<" blockScope: " << blockScope << std::endl;
                //std::cout<<"forLoopExpress2Start: "<<forLoopExpress2Start<<" forLoopScope: "<<forLoopScope<<" forLoopCallLoc: "<<forLoopCallLoc<<std::endl;
                
                // case to see if we fallout of scope. this case usually happens when the function doesn't have a return call.
                if(blockScope == 0){
                    //std::cout<<"exiting runtime"<<std::endl;
                    callStack.pop_back();
                // otherwise we want to continue as normal
                }else{
                    callStack.back()++;
                }

                //however if we are in a for loop, aka 
                //forLoopExpress2Start is not -1 so it has a valid memory location to jump to when the loop reached the end of its scope
                //and the block scope is equal to the forloops scope, meaning we have escaped the bound of the loops scope
                //and we are in the correct interp call as the forloopcall needs to mats the start call address.
                if(forLoopExpress3Start > 0 && blockScope == forLoopScope && forLoopCallLoc == callStartAddress){
                    //std::cout<<"end of for loop, jumping back to for loop start position"<<std::endl;
                    forLoopEnd = callStack.back();
                    // jump to 3 to evaluate the condition.
                    callStack.back() = forLoopExpress3Start;

                }else if(forLoopExpress2Start > 0 && blockScope == forLoopScope && forLoopCallLoc == callStartAddress){
                    //std::cout<<"end of for loop, jumping back to for loop start position"<<std::endl;
                    forLoopEnd = callStack.back();
                    // jump to 3 to evaluate the condition.
                    callStack.back() = forLoopExpress2Start;

                }

            //for a declaration, just keep moving?
            }else if (statement == "DECLARATION") {

                //std::cout << "DECLARATION ADDRESS: " << callStack.back() << std::endl;
                if (currentNode->getToken().isArray()){
                    SymbolNode* arrayNode = symbol_table_list.lookupSymbolAtLocation(currentNode->getToken().getVarName(),lookUpFunctionFromAddress(callStartAddress));
                    if (arrayNode->symbolTable.datatype == "char") {
                        arrayMapChar[currentNode->getToken().getVarName()] = std::vector<char>();
                    }else if (arrayNode->symbolTable.datatype == "int"){
                        arrayMapInt[currentNode->getToken().getVarName()] = std::vector<int>();
                    }
                }
                callStack.back()++;

                //assignment of a variable.
            }else if (statement == "ASSIGNMENT") {
                //used as bool; if inAssignment >= 0 we are in ASSIGNMENT
                inAssignment++;

                //print out token with label assignment
                //std::cout << "ASSIGNMENT ADDRESS: " << callStack.back() << std::endl;
                callStack.back()++;

                //grab the node at the next position to operate on.
                currentNode = cst->getNodeAtAddress(callStack.back());

                //vector to store the postfix tokens
                std::vector<Token> postFix;

                //loop on the cst node until it no longer has any left siblings. this loop get is all the componnents of the assignment operation.
                while(currentNode->getRight() != nullptr){

                    //print out components of the of the assignment operation
                    statement = currentNode->getToken().getTokenString();
                    //std::cout << "ASSIGNMENT PART ADDRESS: " << callStack.back() <<" statement: "<<statement<<std::endl;

                    //if we find a function call in a assignment, then we need to perform some recursion
                    //call our look up function to find out if the given token is a function in our function list
                    int foundFunction = lookUpFunction(currentNode->getToken().getTokenString());

                    //if the value does not return -1 then we have found a function call.
                    if(foundFunction != -1){

                        //move past the token that resembles the function call
                        callStack.back()++;
                        //then move past the token that resembles the open paren
                        callStack.back()++;

                        //grab next node that is the first function call parameter.
                        currentNode = cst->getNodeAtAddress(callStack.back());

                        //then loop through the params, so long as we dont get a close paren
                        while(currentNode->getToken().getTokenString() != ")"){

                            //print out the function call parameters
                           //std::cout<<"found param for function call: "<<currentNode->getToken().getTokenString()<<std::endl;

                            //Initialize the variable with a value by using the variable inside the []
                            if(currentNode->getRight()->getToken().isLBracket()){

                                SymbolNode* symbolParam = symbol_table_list.lookupSymbolParam(lookUpFunctionFromAddress(foundFunction));
                                //We need the value of the variable inside [] so we look for its node and get its value
                                SymbolNode* equalToSymbol = symbol_table_list.lookupSymbolAtLocation(currentNode->getRight()->getRight()->getToken().getTokenString(),lookUpFunctionFromAddress(callStartAddress));
                                int varVal = arrayMapChar.find(currentNode->getToken().getTokenString())->second[equalToSymbol->variableVal];
                                //std::cout<<"setting param value to array value : "<<varVal<<std::endl;
                                symbol_table_list.setVarVal(symbolParam,varVal);
                                //skip the array variable
                                callStack.back()++;
                                //skip the l bracket
                                callStack.back()++;
                                //skip the inside variable
                                callStack.back()++;
                            }else{
                                //set params of function with the correct value matching that param.
                                //use
                                //test to see if setting address works correctly.
                                //symbol_table_list.lookupSymbolAtLocation(currentNode->getToken().getTokenString(),lookUpFunctionFromAddress(callStartAddress))->variableVal = 37;
                                //searches the symbol, by sending it the function name of the function called to set the value of the parameter in our function
                                //to the value of the symboltable value of the param passed in.
                                //note, if we have a function call that isnt a identifier, then we likely need cases for this
                                SymbolNode *symbolParam = symbol_table_list.lookupSymbolParam(
                                        lookUpFunctionFromAddress(foundFunction));
                                SymbolNode *equalToSymbol = symbol_table_list.lookupSymbolAtLocation(
                                        currentNode->getToken().getTokenString(),
                                        lookUpFunctionFromAddress(callStartAddress));
                                symbol_table_list.setVarVal(symbolParam, equalToSymbol->variableVal);
                            }

                            //std::cout<<"param value set to:"<<symbol_table_list.lookupSymbolParam(lookUpFunctionFromAddress(foundFunction))->variableVal<<"VALUE HERE VALUE HERE VALUE HERE"<<std::endl;

                            //move to the next element in memory
                            callStack.back()++;
                            currentNode = cst->getNodeAtAddress(callStack.back());

                            //set the paramlist to the variable. if a variable is a param then lookup that variable
                            //and set it to the param at this position.
                        }

                        //so we want to push the function memory address to the top of the callstack.
                        callStack.push_back(foundFunction);

                        //then we can recursively call interpret to interpret the function call.
                        interpret();
                        //after the function call we are able to resume at the same place in memory regardless of where the funtion is called.
                        //in a assignment.

                        //store the result of the function call in a token (done in RETURN)
                        //add that token to the vector
                        postFix.push_back(tempToken);



                    }else if(currentNode->getToken().isSingleQuote()){
                        //increment the programming counter. skip the single quote
                        callStack.back()++;

                        //move to next left sibling
                        currentNode = cst->getNodeAtAddress(callStack.back());
                        //currentNode->getToken().setChar();
                        //if not a function call push token to vector.
                        postFix.push_back(currentNode->getToken());
                        //increment the programming counter. skip the second single quote
                        callStack.back()++;

                        //move to next left sibling
                        currentNode = cst->getNodeAtAddress(callStack.back());


                    }else{

                        //if not a function call push token to vector.
                        postFix.push_back(currentNode->getToken());
                    }



                    //increment the programming counter.
                    callStack.back()++;

                    //move to next left sibling
                    currentNode = cst->getNodeAtAddress(callStack.back());
                }

                //print out components of the of the assignment operation.
                statement = currentNode->getToken().getTokenString();
                //std::cout << "ASSIGNMENT PART ADDRESS: " << callStack.back() <<" statement: "<<statement<<std::endl;

                postFix.push_back(currentNode->getToken());
                //UNFINISHED HERE!
                //the components above should make up some list which an arithmatic operation can be performed on
                //likely you will need to use the symbol table to keep track of the values above, atlest in the case of the function call.
                //then put them in some container that can operate on the components to set the result in the correct variable in our symbol table

                //call postfixeval to do operations and set value to the variable in the symbol table
                if(arrayMapChar.find(postFix[0].getTokenString()) != arrayMapChar.end()
                   || postFix[0].isChar()){
                    postFixEvalChar(postFix, callStartAddress);
                }else {
                    ////std::cout << "---------------ENDEDNEDNEDNNEDNEDN------------------"<<std::endl;
                    postFixEval(postFix, callStartAddress);
                }

                //increment the call stack
                callStack.back()++;

                //used as bool; if inAssignment >= 0 we are in ASSIGNMENT
                inAssignment--;
                //handles the logi of a iff statement.
            }else if (statement == "IF") {

                statement = currentNode->getToken().getTokenString();
                //std::cout << "IF ADDRESS: " << callStack.back() <<" statement: "<<statement<<std::endl;

                //move past the if identifier.
                callStack.back()++;

                //grab the node at the next position to operate on.
                currentNode = cst->getNodeAtAddress(callStack.back());

                //vector to store the postfix tokens
                std::vector<Token> postFix;

                //loop on the cst node until it no longer has any left siblings
                while(currentNode->getRight() != nullptr){

                    statement = currentNode->getToken().getTokenString();
                    //std::cout << "IF PART ADDRESS: " << callStack.back() <<" statement: "<<statement<<std::endl;

                    if(currentNode->getToken().isSingleQuote()){
                        //increment the programming counter.
                        callStack.back()++;

                        //move to next left sibling
                        currentNode = cst->getNodeAtAddress(callStack.back());
                        currentNode->getToken().setChar();
                        //if not a function call push token to vector.
                        postFix.push_back(currentNode->getToken());
                        //increment the programming counter.
                        callStack.back()++;

                        //move to next left sibling
                        currentNode = cst->getNodeAtAddress(callStack.back());


                    }else{
                        //if not a function call push token to vector.
                        postFix.push_back(currentNode->getToken());
                    }

                    //increment the programming counter.
                    callStack.back()++;

                    //move to next left sibling
                    currentNode = cst->getNodeAtAddress(callStack.back());

                }
                //once we find the node with a null right sibling, we also want to ad that as well to our if parts
                statement = currentNode->getToken().getTokenString();
                //std::cout << "IF PART ADDRESS: " << callStack.back() <<" statement: "<<statement<<std::endl;

                //if not a function call push token to vector.
                postFix.push_back(currentNode->getToken());

                //UNFINISHED HERE!
                //similiar to the opperation for assignment but for boolean opperation. need to evaluate the bool expression then set
                //the value if true or false to the variable below. that way we can either skip the if statements scope, or
                //know to evaluate the ontents of that if expression.

                //operate on the stack to get a result. VALUE IS set to basic value for testing.
                bool boolExpression = postFixEvalBool(postFix,callStartAddress);

                // now that we have all part of the boolean expression shift forward to the parenthesis
                callStack.back()++;
                currentNode = cst->getNodeAtAddress(callStack.back());
                statement = currentNode->getToken().getTokenString();

                //if result is true then  operate on this blocking scope like normal
                if(boolExpression == true){

                    //std::cout << "bool expression true"<< std::endl;
                    //std::cout << "processing If statement..." << std::endl;

                    //else if its false, then skip that block, using the block scoping to skip it
                }else{

                    //std::cout << "bool expression false"<< std::endl;

                    //important! the token we would be on if we didnt increment by one is a begin block
                    //this is bad because it messes up the skip function if bool is false.
                    //before we loop increment by one to simulate eating the begin block
                    callStack.back()++;

                    //set the temp scope variable to tell when we should stop ignoring code in the scope of the if statement.
                    int targetScope = blockScope;

                    //increase the scope by 1 to resemble eating the open paren.
                    blockScope++;

                    ////std::cout << "blockScope: "<<blockScope<<" targetScope: "<<targetScope<< std::endl;

                    //while the blockScope is greater than the targetScope
                    while(blockScope > targetScope){

                        // upodate node contents
                        currentNode = cst->getNodeAtAddress(callStack.back());
                        statement = currentNode->getToken().getTokenString();

                        //mock begin case. functions like the begin block case but specially tailored for skipping things in this scope
                        if (statement == "BEGIN BLOCK") {

                            blockScope++;
                            //std::cout << "BEGIN BLOCK in if skip ADDRESS: " << callStack.back() <<"blockScope: " << blockScope << std::endl;

                            //mock end case. functions like the begin block case but specially tailored for skipping things in this scope
                        }else if (statement == "END BLOCK") {

                            blockScope--;
                            //std::cout << "END BLOCK in if skip ADDRESS: " << callStack.back() <<" blockScope: " << blockScope << std::endl;
                        }

                        //move past the end block
                        callStack.back()++;
                    }
                }
                //move the token forward.
                currentNode = cst->getNodeAtAddress(callStack.back());
                statement = currentNode->getToken().getTokenString();
                //set variable to true only if next statement is an else statement
                if (statement == "ELSE" && !boolExpression) {
                    _else = true;
                }
                //std::cout << "statement: " << statement << std::endl;
                //std::cout << "current token: " << currentNode->getToken().getTokenString() << " blockScope: "
                         // << blockScope << std::endl;



                //Preform only if the Statement is else and if the boolexpression prior was false
            } else if (statement == "ELSE" && _else) {
                //std::cout << "processing else statement..." << std::endl;
                _else = false;      // to avoid future ambiguity set this false
                callStack.back()++;

                //SInce the boolexpression was true we skip anything inside the else statement
            } else if (statement == "ELSE" && !_else) {
                callStack.back()++;
                //std::cout << "Skipping else statement..." << std::endl;
                callStack.back()++;
                int targetScope = blockScope;
                blockScope++;
                //while the blockScope is greater than the targetScope
                while (blockScope > targetScope) {

                    // upodate node contents
                    currentNode = cst->getNodeAtAddress(callStack.back());
                    statement = currentNode->getToken().getTokenString();

                    //mock begin case. functions like the begin block case but specially tailored for skipping things in this scope
                    if (statement == "BEGIN BLOCK") {

                        blockScope++;
                        //std::cout << "BEGIN BLOCK in if skip ADDRESS: " << callStack.back() << "blockScope: "
                                  //<< blockScope << std::endl;

                        //mock end case. functions like the begin block case but specially tailored for skipping things in this scope
                    } else if (statement == "END BLOCK") {

                        blockScope--;
                        //std::cout << "END BLOCK in if skip ADDRESS: " << callStack.back() << " blockScope: "
                                  //<< blockScope << std::endl;
                    }

                    //move past the end block
                    callStack.back()++;
                }

            }else if (statement == "RETURN") {
                //get the next token which should ve the symbol or value we are returning
                callStack.back()++;
                currentNode = cst->getNodeAtAddress(callStack.back());
                statement = currentNode->getToken().getTokenString();

                //if its a variable then return that value
                if(currentNode->getToken().isIdentifier() == true){

                    //std::cout<<"found identifier to return."<<std::endl;

                    //search the symbol table for our return value
                    SymbolNode* symbolToReturn = symbol_table_list.lookupSymbolAtLocation(currentNode->getToken().getTokenString(),lookUpFunctionFromAddress(callStartAddress));
                    //set the return value in our parser to the symbols value we found.
                    returnValue = symbolToReturn->variableVal;
                    if(inAssignment > 0) {
                        tempToken = Token(std::to_string(returnValue));
                    }
                    //std::cout<<"returnValue: "<<returnValue<<std::endl;

                }

                //then pop the current mem of the call stack stopping the loop of this function since we are finished in this call
                callStack.pop_back();
                //std::cout<<"REACHED RETURN CALL."<<std::endl;

            //handles printing of output once we are finished.
            }else if (statement == "PRINTF") {
                callStack.back()++;
                currentNode = cst->getNodeAtAddress(callStack.back());
                statement = currentNode->getToken().getTokenString();
                std::vector<Token> variables;
                //std::cout << "adding tokens to variable vector" << std::endl;

                while(currentNode->getRight() != nullptr){
                    callStack.back()++;
                    currentNode = cst->getNodeAtAddress(callStack.back());
                    //std::cout << "added " << currentNode->getToken().getTokenString()<< " to vector" << std::endl;
                    variables.push_back(currentNode->getToken());
                }
                std::string test = statement;
                //need to determine if we are printing a string by its self or variables?
                //may need to parse through token string to put variable in the correct places.
                //attempting to replace %d and %s with tokens
                //std::cout<<"attempting print " <<std::endl;
                //std::cout << statement << std::endl;
                int vecInt = 0;
                size_t index = 0;
                //replace %s in printf
                while(true){
                    std::string replaceStr = "";
                    // Locate the substring to replace.
                    index = test.find("%s", index);
                    if (index == std::string::npos){
                        break;
                    }
                    if(arrayMapChar.find(variables[vecInt].getTokenString()) != arrayMapChar.end()){
                        for(int i = 0; i < arrayMapChar[variables[vecInt].getTokenString()].size() - 1; i++){
                            replaceStr += (char)arrayMapChar[variables[vecInt].getTokenString()][i];
                        }
                    }else{
                        SymbolNode* variableToDigit = symbol_table_list.lookupSymbolParam(variables[vecInt].getTokenString());
                        int varToInt = variableToDigit->variableVal;
                        replaceStr = std::to_string(varToInt);
                    }


                    // Make the replacement.
                    test.replace(index, 2, replaceStr);

                    // Advance index forward so the next iteration doesn't pick it up as well.
                    index++;
                    vecInt++;
                }
                index = 0;
                //replace %d in printf
                while(true){
                    // Locate the substring to replace.
                    index = test.find("%d", index);
                    if (index == std::string::npos){
                        break;
                    }

                    SymbolNode* variableToDigit = symbol_table_list.lookupSymbolParam(variables[vecInt].getTokenString());
                    int varToInt = variableToDigit->variableVal;
                    // Make the replacement.
                    test.replace(index, 2, std::to_string(varToInt));

                    // Advance index forward so the next iteration doesn't pick it up as well.
                    index++;
                    vecInt++;
                }
                //replace \n in printf statement
                index = 0;
                while(true){
                    // Locate the substring to replace.
                    index = test.find('\\', index);
                    if (index == std::string::npos){
                        break;
                    }

                    //Make the replacement.
                    if(test.find('n', index + 1)){
                        test.replace(index, 2, "\n");
                    }

                    //Advance index forward so the next iteration doesn't pick it up as well.
                    index += 1;
                }
                std::cout << test; //<< std::endl;
                callStack.back()++;

            // for the for loop we need to do the same logic as our assign case for parts 1 and 3.
            }else if (statement == "FOR EXPRESSION 1") {

                inAssignment++;
                //std::cout << statement << " ADDRESS: " << callStack.back() << std::endl;
                callStack.back()++;
                currentNode = cst->getNodeAtAddress(callStack.back());
                std::vector<Token> postFix;

                while(currentNode->getRight() != nullptr){
                    statement = currentNode->getToken().getTokenString();
                    //std::cout << "ASSIGNMENT PART ADDRESS: " << callStack.back() <<" statement: "<<statement<<std::endl;
                    int foundFunction = lookUpFunction(currentNode->getToken().getTokenString());

                    if(foundFunction != -1){
                        callStack.back()++;
                        callStack.back()++;
                        currentNode = cst->getNodeAtAddress(callStack.back());

                        while(currentNode->getToken().getTokenString() != ")"){
                            //std::cout<<"found param for function call: "<<currentNode->getToken().getTokenString()<<std::endl;
                            SymbolNode* symbolParam = symbol_table_list.lookupSymbolParam(lookUpFunctionFromAddress(foundFunction));
                            SymbolNode* equalToSymbol = symbol_table_list.lookupSymbolAtLocation(currentNode->getToken().getTokenString(),lookUpFunctionFromAddress(callStartAddress));
                            symbol_table_list.setVarVal(symbolParam,equalToSymbol->variableVal);
                            //std::cout<<"param value set to:"<<symbol_table_list.lookupSymbolParam(lookUpFunctionFromAddress(foundFunction))->variableVal<<"VALUE HERE VALUE HERE VALUE HERE"<<std::endl;
                            callStack.back()++;
                            currentNode = cst->getNodeAtAddress(callStack.back());

                        }
                        callStack.push_back(foundFunction);
                        interpret();
                        postFix.push_back(tempToken);

                    }else{
                        postFix.push_back(currentNode->getToken());
                    }
                    callStack.back()++;
                    currentNode = cst->getNodeAtAddress(callStack.back());
                }
                statement = currentNode->getToken().getTokenString();
                //std::cout << "ASSIGNMENT PART ADDRESS: " << callStack.back() <<" statement: "<<statement<<std::endl;

                postFix.push_back(currentNode->getToken());
                postFixEval(postFix,callStartAddress);
                callStack.back()++;
                inAssignment--;

                //currentNode = cst->getNodeAtAddress(callStack.back());
            // bool expression with some extra steps.
            }else if (statement == "FOR EXPRESSION 2" || statement == "WHILE") {

                //std::cout << "IN FOR 2" << std::endl;

                // Save the starting point of the FOR loop
                forLoopExpress2Start = callStack.back(); 
                //std::cout << "FOR LOOP ADDRESS: " << callStack.back() << std::endl;

                //important variable to tell where the for loop was called in memory
                int forLoopOriginalAdress = callStack.back();

                callStack.back()++;
                //std::cout << "FOR LOOP ADDRESS: " << callStack.back() << std::endl;
                currentNode = cst->getNodeAtAddress(callStack.back());
                
                //make vector to fill our tokens for our boolean operation
                std::vector<Token> postFix;

                while (currentNode->getRight() != nullptr) {
                    postFix.push_back(currentNode->getToken());
                    callStack.back()++;
                    currentNode = cst->getNodeAtAddress(callStack.back());
                }

                postFix.push_back(currentNode->getToken());
                callStack.back()++;
                currentNode = cst->getNodeAtAddress(callStack.back());

                //if we find the expression to be true, then we want to loop
                if(postFixEvalBool(postFix, callStartAddress)){

                    //std::cout << "for expression true"<< std::endl;
                    
                    //set the forloopstart, to the address right on the FOR EXPRESSION 2 node
                    forLoopExpress2Start = forLoopOriginalAdress;

                    //set the loops scope similiar to how we did with the if statement
                    forLoopScope = blockScope;

                    //set the forloop call loc to the starting address of the function that called it.
                    //that way the loop doesnt die if it calls a function.
                    forLoopCallLoc  = callStartAddress;

                    //now skip the for expression 3 since we already evaluated it.
                    while(statement != "BEGIN BLOCK" && forLoopExpress3Start != -1){
                        callStack.back()++;
                        currentNode = cst->getNodeAtAddress(callStack.back());
                        statement = currentNode->getToken().getTokenString();
                    }

                //otherwise we want to skip the for loops scope.
                }else{
                    // reset the loop variable since we are now out of the loop
                    forLoopExpress2Start = -1;
                    forLoopExpress3Start = -1;
                    forLoopScope = -1;
                    forLoopCallLoc  = -1;
                    callStack.back() = forLoopEnd;
                    forLoopEnd = -1;

                    //then same logic as our if statement when skipping the block
                    //std::cout << "bool expression in loop  false"<< std::endl;                 
                }

                //move the token forward.
                currentNode = cst->getNodeAtAddress(callStack.back());
                statement = currentNode->getToken().getTokenString();
                //std::cout <<"statement: "<<statement<<std::endl;
                //std::cout<<"current token: "<<currentNode->getToken().getTokenString()<<" blockScope: "<<blockScope<<std::endl;

            //christian made a good point, we need to skil this statement on the first activation of the loop. this isnt very difficult as we have some value already to tell if it is the first run.
            }else if (statement == "FOR EXPRESSION 3") {
                //if the forloop hasn't defined its end yet, the we are on the first run of the loop

                 //std::cout << "IN FOR 3" << std::endl;
                 //set the start location of our for expression 3
                 forLoopExpress3Start = callStack.back(); 


                if(forLoopEnd == -1){
                    //so skip the statement until we reach a open block

                    //std::cout << "SKIPPING THE FIRST ITTERATION OF THE FORLOOP" << std::endl;

                    while(statement != "BEGIN BLOCK"){
                        //grab nodes, until we reach a begin block
                        callStack.back()++;
                        currentNode = cst->getNodeAtAddress(callStack.back());
                        statement = currentNode->getToken().getTokenString();
                    }

                //otherwise function as for expression 3
                }else{
                    
                    //std::cout << "EVALUATING THE FOR 3 ASSIGNMENT" << std::endl;

                    inAssignment++;
                //std::cout << statement << " ADDRESS: " << callStack.back() << std::endl;
                callStack.back()++;
                currentNode = cst->getNodeAtAddress(callStack.back());
                std::vector<Token> postFix;

                while(currentNode->getRight() != nullptr){
                    statement = currentNode->getToken().getTokenString();
                    //std::cout << "ASSIGNMENT PART ADDRESS: " << callStack.back() <<" statement: "<<statement<<std::endl;
                    int foundFunction = lookUpFunction(currentNode->getToken().getTokenString());

                    if(foundFunction != -1){
                        callStack.back()++;
                        callStack.back()++;
                        currentNode = cst->getNodeAtAddress(callStack.back());

                        while(currentNode->getToken().getTokenString() != ")"){
                            //std::cout<<"found param for function call: "<<currentNode->getToken().getTokenString()<<std::endl;
                            SymbolNode* symbolParam = symbol_table_list.lookupSymbolParam(lookUpFunctionFromAddress(foundFunction));
                            SymbolNode* equalToSymbol = symbol_table_list.lookupSymbolAtLocation(currentNode->getToken().getTokenString(),lookUpFunctionFromAddress(callStartAddress));
                            symbol_table_list.setVarVal(symbolParam,equalToSymbol->variableVal);
                            //std::cout<<"param value set to:"<<symbol_table_list.lookupSymbolParam(lookUpFunctionFromAddress(foundFunction))->variableVal<<"VALUE HERE VALUE HERE VALUE HERE"<<std::endl;
                            callStack.back()++;
                            currentNode = cst->getNodeAtAddress(callStack.back());

                        }
                        callStack.push_back(foundFunction);
                        interpret();
                        postFix.push_back(tempToken);

                    }else{
                        postFix.push_back(currentNode->getToken());
                    }
                    callStack.back()++;
                    currentNode = cst->getNodeAtAddress(callStack.back());
                }
                statement = currentNode->getToken().getTokenString();
                //std::cout << "ASSIGNMENT PART ADDRESS: " << callStack.back() <<" statement: "<<statement<<std::endl;

                postFix.push_back(currentNode->getToken());
                postFixEval(postFix,callStartAddress);
                callStack.back()++;
                inAssignment--;

                    //do something special, jump to the memory address of the for expression 2 to evaluate the new increment.
                    callStack.back() = forLoopExpress2Start;
                }
                

                //currentNode = cst->getNodeAtAddress(callStack.back());
            // bool expression with some extra steps.
            }else if(statement == "CALL"){
            //if we find a call move one forward
            callStack.back()++;
            currentNode = cst->getNodeAtAddress(callStack.back());

            // same logic as when we called our function in assignment. 
            //if we find a function call in a assignment, then we need to perform some recursion
            //call our look up function to find out if the given token is a function in our function list
            int foundFunction = lookUpFunction(currentNode->getToken().getTokenString());

            //if the value does not return -1 then we have found a function call.
            if(foundFunction != -1){

                //move past the token that resembles the function call
                callStack.back()++;
                //then move past the token that resembles the open paren
                callStack.back()++;

                //grab next node that is the first function call parameter.
                currentNode = cst->getNodeAtAddress(callStack.back());

                //then loop through the params, so long as we dont get a close paren
                while(currentNode->getToken().getTokenString() != ")"){

                    //print out the function call parameters
                    //std::cout<<"found param for function call: "<<currentNode->getToken().getTokenString()<<std::endl;

                            
                    SymbolNode* symbolParam = symbol_table_list.lookupSymbolParam(lookUpFunctionFromAddress(foundFunction));
                    SymbolNode* equalToSymbol = symbol_table_list.lookupSymbolAtLocation(currentNode->getToken().getTokenString(),lookUpFunctionFromAddress(callStartAddress));
                    symbol_table_list.setVarVal(symbolParam,equalToSymbol->variableVal);

                    //std::cout<<"param value set to:"<<symbol_table_list.lookupSymbolParam(lookUpFunctionFromAddress(foundFunction))->variableVal<<"VALUE HERE VALUE HERE VALUE HERE"<<std::endl;

                    //move to the next element in memory
                    callStack.back()++;
                    currentNode = cst->getNodeAtAddress(callStack.back());

                    //set the paramlist to the variable. if a variable is a param then lookup that variable
                    //and set it to the param at this position.
                }

                //so we want to push the function memory address to the top of the callstack.
                callStack.push_back(foundFunction);

                //then we can recursively call interpret to interpret the function call.
                interpret();
                //after the function call we are able to resume at the same place in memory regardless of where the funtion is called.
                //in a assignment.

                //store the result of the function call in a token (done in RETURN)
                //add that token to the vector
            }
            callStack.back()++;
    }
        

        }
    }
}



