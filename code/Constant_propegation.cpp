#pragma once
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
#include "precheck.h"

using namespace std;

/*namespace charinfo {

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

}*/

class PropagationAndFolding {

    std::string code = "";
    const char* BufferPtr;

    std::unordered_map<std::string, int> constant_table;
    std::unordered_map<std::string, bool> boolean_table;
public: PropagationAndFolding(const llvm::StringRef& Buffer) {    

		BufferPtr = Buffer.begin();
        const char* end = BufferPtr + 1;
        while (*end) {              
		    end++;
	    }
        llvm::StringRef Context(BufferPtr, end - BufferPtr);
        code = (std::string) Context;
	}

public:
    bool isOperator(char c)
    {
        
        return c == '+' || c == '-' || c == '*' || c == '/'
            || c == '^';
    }

    
    int precedence(char op)
    {
        
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

    bool evaluateBooleanExpression(const std::string& expression) {
        if(expression == "true") {
            return true;
        } else if(expression == "false") {
            return false;
        }
        if(expression == "1") {
            return true;
        } else if(expression == "0") {
            return false;
        }
        std::istringstream iss(expression);
        int leftOperand, rightOperand;
        std::string op;

        
        iss >> leftOperand >> op >> rightOperand;

        // Evaluate the logical expression
        if (op == "<") {
            return leftOperand < rightOperand;
        } else if (op == ">") {
            return leftOperand > rightOperand;
        } else if (op == "<=") {
            return leftOperand <= rightOperand;
        } else if (op == ">=") {
            return leftOperand >= rightOperand;
        } else if (op == "==") {
            return leftOperand == rightOperand;
        } else if (op == "!=") {
            return leftOperand != rightOperand;
        } else {
            std::cerr << "Invalid operator: " << op << std::endl;
            return false;
        }
    }

    
    int evaluateExpression(const string& expression)
    {
        stack<char> operators; 
        stack<int> operands; 

        stringstream ss(expression);
                                   

        string token;
        while (getline(
            ss, token,
            ' ')) { 
            if (token.empty())
                continue; 
            if (isdigit(token[0])) { 
                int num;
                stringstream(token)
                    >> num;
                operands.push(num); 
                                  
            }
            else if (isOperator(token[0])) { 
                                            
                char op = token[0];
                
                while (!operators.empty()
                    && precedence(operators.top())
                            >= precedence(op)) {
                   
                    int b = operands.top();
                    operands.pop();
                    int a = operands.top();
                    operands.pop();
                    char op = operators.top();
                    operators.pop();
                    
                    operands.push(applyOp(a, b, op));
                }
             
                operators.push(op);
            }
            else if (token[0] == '(') {
                                        
                
                operators.push('(');
            }
            else if (token[0] == ')') {
                                        
                while (!operators.empty()
                    && operators.top() != '(') {
                    
                    int b = operands.top();
                    operands.pop();
                    int a = operands.top();
                    operands.pop();
                    char op = operators.top();
                    operators.pop();
                    
                    operands.push(applyOp(a, b, op));
                }
               
                operators.pop();
            }
        }

        
        while (!operators.empty()) {
          
            int b = operands.top();
            operands.pop();
            int a = operands.top();
            operands.pop();
            char op = operators.top();
            operators.pop();
         
            operands.push(applyOp(a, b, op));
        }

        
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

        // for (const auto& line : lines) {
        //     for (const auto& l : line) {
        //         cout << l;
        //         cout << "\n";
        //     }
        //     cout << "\n";
        // }

        std::vector<char*> splitResults;

    // Iterate through each char* in the lines vector
       // for (char* line : lines[0]) {
            // Convert char* to std::string
            std::string strLine(lines[0][0]);
            //std::string strLine(line);
        
            // Split the string by ';'
            std::stringstream ss(strLine);
            std::string item;
            while (std::getline(ss, item, ';')) {
                char* cstr = strdup(item.c_str()); // Use strdup to allocate and copy
            
            // Allocate additional memory for the ';' character and null terminator
            char* cstrWithSemicolon = (char*)malloc(strlen(cstr) + 2); // +1 for ';' +1 for '\0'
            if (cstrWithSemicolon) {
                strcpy(cstrWithSemicolon, cstr); // Copy the original string
                cstrWithSemicolon[strlen(cstr)] = ';'; // Add the ';' character
                cstrWithSemicolon[strlen(cstr) + 1] = '\0'; // Null terminate the new string
                splitResults.push_back(cstrWithSemicolon); // Store the new string
            }
            free(cstr); // Free the original cs
            }
        //}

        // Output the results
        // for (const char* result : splitResults) {
        //     std::cout << result << std::endl;
        // }

        std::vector<char*> live_lines = splitResults;
        // cout << live_lines.size() << "\n";
        std::vector<std::string> new_lines;
        int o = 0;
        for (const auto& lin : live_lines/*size_t i=0; i<live_lines.size(); i++*/) {
            
           // cout << "319\n";
            const char* Buffer = lin;
           // cout << "320 \n";
            while (charinfo::isWhitespace(*Buffer)) ++Buffer;
           // cout << "322 \n";

            // checking left side of "=" and store it in Context1
            while (!charinfo::isLetter(*Buffer) && !charinfo::isSemiColon(*Buffer)) ++Buffer;

            const char* end = Buffer + 1;


            while ((charinfo::isLetter(*end) || charinfo::isDigit(*end)) )
                ++end;

            //cout << "333 \n";
            int e = 0 ;
            llvm::StringRef Context1(Buffer, end - Buffer);
            string con1 = (string)Context1;
            if(con1=="int") {
               e = 1;
               Buffer = end;
               while (charinfo::isWhitespace(*Buffer)) ++Buffer;
               end = Buffer + 1;


               while ((charinfo::isLetter(*end) || charinfo::isDigit(*end)) )
                ++end; 
                llvm::StringRef Context1(Buffer, end - Buffer);con1 = (string)Context1;
            } else if(con1=="bool") {
                e = 2;
               Buffer = end;
               while (charinfo::isWhitespace(*Buffer)) ++Buffer;
               end = Buffer + 1;


               while ((charinfo::isLetter(*end) || charinfo::isDigit(*end)) )
                ++end; 
                llvm::StringRef Context1(Buffer, end - Buffer);con1 = (string)Context1;
            }
           // cout << con1 << "\n";
            if(e==1) {
                boolean_table[con1] = false;
            } else if(e==2) {
                boolean_table[con1] = true;
            }
            Buffer = end;
                
            // checking right side of "=" and store them in s array
            while (charinfo::isWhitespace(*Buffer)) ++Buffer;
            if(charinfo::isSemiColon(*Buffer)) {
                if(boolean_table[con1]) {
                    constant_table[con1] = 0;
                    new_lines.push_back("");
                    new_lines[o].append("bool ").append(con1).append(" ;\n");
                } else {
                    constant_table[con1] = 0;
                    new_lines.push_back("");
                   new_lines[o].append("int ").append(con1).append(" ;\n");
                }
                o++;
                continue;
            } 
            
            if(charinfo::isEqual(*Buffer)){
               // cout << "343 \n";

                Buffer++;

                while (charinfo::isWhitespace(*Buffer)) ++Buffer;    


                string s;  // for variables or numbers which are right side of "="

                while (!charinfo::isSemiColon(*Buffer)){

                    while(charinfo::isWhitespace(*Buffer)) Buffer++;
                    if (charinfo::isSemiColon(*Buffer)) break;
                    
                    const char* end = Buffer + 1;
                    while(charinfo::isLetter(*end) || charinfo::isDigit(*end)) end++; 
                   // cout << "360 \n";

                    llvm::StringRef Context2(Buffer, end - Buffer);
                    string jvd = (std::string) Context2;
                    if(jvd[0]>= '0' && jvd[0] <= '9') {
                    s.append(jvd);
                    s.append(" ");

                    } else {
                       s.append(to_string(constant_table[jvd]));
                       s.append(" ");
                    
                    }
                    Buffer = end;
                    while(charinfo::isWhitespace(*Buffer)) Buffer++;
                    if(*Buffer == '+' || *Buffer =='-' || *Buffer =='/' || *Buffer =='*' || *Buffer =='>' || *Buffer =='<'){

                        s = s + *Buffer; 
                        s.append(" ");
                        Buffer++;

                    }

                    while(charinfo::isWhitespace(*Buffer)) Buffer++;

                }

               // cout << s << endl;
                bool first = false;
                bool boltype = false;
                int calculatedNum;
                if(boolean_table[con1]) {
                    boltype = evaluateBooleanExpression(s);
                } else {
                  calculatedNum = evaluateExpression(s);
                } 
                //cout << calculatedNum << "\n";
                if(constant_table.count(con1) == 0) {
                    first = true;
                }
                if(boolean_table[con1]) {
                    
                    if(boltype) {
                        calculatedNum=1;
                    } else {
                        calculatedNum = 0;
                    }
                    constant_table[con1] = calculatedNum;
                } else {
                  constant_table[con1] = calculatedNum;
                } 
                // cout << "this is test " << con1 << "\n";
                 new_lines.push_back("");
                 if(first && boolean_table[con1]) {
                    new_lines[o].append("bool ");
                 } else if(first && !boolean_table[con1]){
                   new_lines[o].append("int "); 
                 }
                new_lines[o].append(con1).append(" = ");
                if(boolean_table[con1]) {
                    if(calculatedNum == 1) {
                        new_lines[o].append("true;\n");
                    } else {
                        new_lines[o].append("false;\n");
                    }
                } else {
                   new_lines[o].append(to_string(calculatedNum)).append(";\n");
                }
                // printf("===\n");  
                //}

                
            }
            o++;
        }

        // for (auto line : new_lines ) {
        //     cout << line << "\n";
        // }
        // for (const auto& pair : constant_table) {
        //     std::cout << pair.first << ": " << pair.second << std::endl;
        // }

        //new_lines.pop_back();
        return new_lines;
    }

};
