
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <llvm/ADT/StringRef.h>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stack>

using namespace std;

namespace charinfo {

    LLVM_READNONE inline bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\f' ||
               c == '\v' || c == '\r' || c == '\n';
    }

    LLVM_READNONE inline bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    LLVM_READNONE inline bool isLetter(char c) {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z');
    }

    LLVM_READNONE inline bool isSemiColon(char c) {
        return c == ';';
    }

    LLVM_READNONE inline bool isEqual(char c) {
        return c == '=';
    }

}

class PropagationAndFolding {

    std::string code = "";
    const char* BufferPtr;

    std::unordered_map<std::string, int> constant_table;
public: PropagationAndFolding(const llvm::StringRef& Buffer) {    // constructor scans the whole context

		BufferPtr = Buffer.begin();
        const char* end = BufferPtr + 1;
        while (*end) {              // since end of context is 0 -> !0 = true -> end of context
		    end++;
	    }
        llvm::StringRef Context(BufferPtr, end - BufferPtr);
        code = (std::string) Context;
	}

public:
    bool isOperator(char c)
    {
        // Returns true if the character is an operator
        return c == '+' || c == '-' || c == '*' || c == '/'
            || c == '^';
    }

    // Function to get the precedence of an operator
    int precedence(char op)
    {
        // Returns the precedence of the operator
        if (op == '+' || op == '-')
            return 1;
        if (op == '*' || op == '/')
            return 2;
        if (op == '^')
            return 3;
        return 0;
    }

    // Function to apply an operator to two operands
    int applyOp(int a, int b, char op)
    {
        // Applies the operator to the operands and returns the
        // result
        switch (op) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
        case '^':
            return pow(a, b);
        default:
            return 0;
        }
    }

    // Function to parse and evaluate a mathematical expression
    int evaluateExpression(const string& expression)
    {
        stack<char> operators; // Stack to hold operators
        stack<int> operands; // Stack to hold operands

        stringstream ss(expression); // String stream to parse
                                    // the expression

        string token;
        while (getline(
            ss, token,
            ' ')) { // Parse the expression token by token
            if (token.empty())
                continue; // Skip empty tokens
            if (isdigit(token[0])) { // If the token is a number
                double num;
                stringstream(token)
                    >> num; // Convert the token to a number
                operands.push(num); // Push the number onto the
                                    // operand stack
            }
            else if (isOperator(token[0])) { // If the token is
                                            // an operator
                char op = token[0];
                // While the operator stack is not empty and the
                // top operator has equal or higher precedence
                while (!operators.empty()
                    && precedence(operators.top())
                            >= precedence(op)) {
                    // Pop two operands and one operator
                    int b = operands.top();
                    operands.pop();
                    int a = operands.top();
                    operands.pop();
                    char op = operators.top();
                    operators.pop();
                    // Apply the operator to the operands and
                    // push the result onto the operand stack
                    operands.push(applyOp(a, b, op));
                }
                // Push the current operator onto the operator
                // stack
                operators.push(op);
            }
            else if (token[0] == '(') { // If the token is an
                                        // opening parenthesis
                // Push it onto the operator stack
                operators.push('(');
            }
            else if (token[0] == ')') { // If the token is a
                                        // closing parenthesis
                // While the operator stack is not empty and the
                // top operator is not an opening parenthesis
                while (!operators.empty()
                    && operators.top() != '(') {
                    // Pop two operands and one operator
                    int b = operands.top();
                    operands.pop();
                    int a = operands.top();
                    operands.pop();
                    char op = operators.top();
                    operators.pop();
                    // Apply the operator to the operands and
                    // push the result onto the operand stack
                    operands.push(applyOp(a, b, op));
                }
                // Pop the opening parenthesis
                operators.pop();
            }
        }

        // While the operator stack is not empty
        while (!operators.empty()) {
            // Pop two operands and one operator
            int b = operands.top();
            operands.pop();
            int a = operands.top();
            operands.pop();
            char op = operators.top();
            operators.pop();
            // Apply the operator to the operands and push the
            // result onto the operand stack
            operands.push(applyOp(a, b, op));
        }

        // The result is at the top of the operand stack
        return operands.top();
    }


    void find_vars(std::vector<std::string>& variables, std::vector<char*>& variable_end_ptr){

    const char* pointer = BufferPtr;

    while (*pointer) {                      // since end of context is 0 -> !0 = true -> end of context
        
        while (*pointer && charinfo::isWhitespace(*pointer)) {      // Skips whitespace like " "
            ++pointer;
        }

        if (charinfo::isLetter(*pointer)) {   // looking for keywords or identifiers like "int", a123 , ...

            const char* end = pointer + 1;

            while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
                ++end;                          // until reaches the end of lexeme
            // example: ".int " -> "i.nt " -> "in.t " -> "int. "

            llvm::StringRef Context(pointer, end - pointer);  // start of lexeme, length of lexeme

            // dar zir mikhad "int resault" ra peida konad
            // baraye ma result is output ✅
            if (Context == "int") {}

            else if (Context == "output") {

                // here wants to check if its the first time mitting the var or not...
                bool duplicate = false;

                for (const auto& element1 : variables) {

                    if(element1 == Context){
                        duplicate = true;
                    }
                }
                
                // here saves last char addres of every variable.
                if(!duplicate){
                    variables.push_back("output");
                    variable_end_ptr.push_back((char*) end);
                }
                
            }
            else {
                bool duplicate = false;

                for (const auto& element1 : variables) {

                    if(element1 == Context){
                        duplicate = true;
                    }
                }
                if(!duplicate){
                    variables.push_back((std::string) Context);
                    variable_end_ptr.push_back((char*) end);
                }
            }

            // pointer comes to end to check other strings.
			pointer = end;

        }

        ++pointer;
    }

    return;

	};

    void find_lines(std::vector<std::string> variables, std::vector<std::vector<char*>>& lines){

        for (const auto& variable : variables) {

            const char* pointer = BufferPtr;
            const char* line_start = BufferPtr;
            std::vector<char*> single_lines;

            while (*pointer) {                      // since end of context is 0 -> !0 = true -> end of context
        
                while (*pointer && charinfo::isWhitespace(*pointer)) {      // Skips whitespace like " "
                    ++pointer;
                }

                if (charinfo::isLetter(*pointer)) {   // looking for keywords or identifiers like "int", a123 , ...

                    const char* end = pointer + 1;

                    while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
                        ++end;                          // until reaches the end of lexeme

                    llvm::StringRef Context(pointer, end - pointer);  // start of lexeme, length of lexeme

                    if ((std::string) Context == variable) {

                        bool duplicate = false;

                        for (const auto& element1 : single_lines) {

                            if(element1 == line_start){
                                duplicate = true;
                            }
                        }
                        if(!duplicate){
                            single_lines.push_back((char*) line_start);
                        }
                    }

                    pointer = end;

                }
                if(charinfo::isSemiColon(*pointer)){
                    line_start = pointer+1;
                }

                ++pointer;
            }
            // all the lines which have this variable will be added to lines... 
            lines.push_back(single_lines);
        }
    }

    std::vector<std::string> cons_fold(std::vector<std::string> variables, std::vector<std::vector<char*>> lines) {
        std::vector<char*> live_lines = lines[0];
        std::vector<std::string> new_lines;
        for (size_t i=0; i<live_lines.size(); i++) {

            const char* Buffer = live_lines[i];

            // checking left side of "=" and store it in Context1
            while (!charinfo::isLetter(*Buffer) && !charinfo::isSemiColon(*Buffer)) ++Buffer;

            const char* end = Buffer + 1;


            while ((charinfo::isLetter(*end) || charinfo::isDigit(*end)) )
                ++end;

            llvm::StringRef Context1(Buffer, end - Buffer);
            string con1 = (string)Context1;

            Buffer = end;
                
            // checking right side of "=" and store them in s array
            while (charinfo::isWhitespace(*Buffer)) ++Buffer;
                
            if(charinfo::isEqual(*Buffer)){
                Buffer++;

                while (charinfo::isWhitespace(*Buffer)) ++Buffer;    


                string s[10];  // for variables or numbers which are right side of "="
                int x = 0;

                while (!charinfo::isSemiColon(*Buffer)){

                    while(charinfo::isWhitespace(*Buffer)) Buffer++;
                    if (charinfo::isSemiColon(*Buffer)) break;
                    
                    const char* end = Buffer + 1;
                    while(charinfo::isLetter(*end) || charinfo::isDigit(*end)) end++; 

                    llvm::StringRef Context2(Buffer, end - Buffer);
                    s[x] = to_string(constant_table[(std::string) Context2]);
                    x++;

                    Buffer = end;
                    if(*Buffer == '+' || *Buffer =='-' || *Buffer =='/' || *Buffer =='*' ){

                        s[x] = *Buffer; 
                        x++;
                        Buffer++;

                    }

                    while(charinfo::isWhitespace(*Buffer)) Buffer++;

                }

                bool canPropagate = true;
                string m = "";
                for( int z = 0 ; z < x ; z++ ){ // check if there is any value in s array which is not a number => Age Hame adad bashan propagation okeye dige✅

                    if(s[z] == "+" || s[z] == "-" || s[z] =="/" || s[z] =="*" ) continue;

                    if(!(47 < s[z][0] && s[z][0] > 58)) { // if first letter is a number so it can not be a name for variables.
                        

                        canPropagate = false;
                        break;

                    }
                }

                for( int z = 0 ; z < x ; z++ ){ // check if there is any value in s array which is not a number => Age Hame adad bashan propagation okeye dige✅

                    m.append(s[z]);
                    m.append(" ");
                }


                if( canPropagate ) {

                    int calculatedNum = evaluateExpression(m); // calculatedNum is the final number which should be placed instead of the right side of variable. 
                    
                    // from here we make propagation 👇
                    constant_table[con1] = calculatedNum;
                    new_lines[i] = con1 + " = " + to_string(calculatedNum);  
                }

                ++Buffer;
            }
        }
        return new_lines;
    }

};
