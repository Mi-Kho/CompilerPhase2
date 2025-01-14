
#include "Constant_propegation.cpp"

class CFoi{
    PropagationAndFolding& pro;
    std::vector<std::string> variables;
    std::vector<char*> variable_end_ptr;
    std::vector<std::vector<char*>> line_start;
    std::vector<std::string> live_variables;
    std::vector<char*> live_lines;
    char* last_result_line;
    std::vector<char*> final_program;


    void find_variables(){

        pro.find_vars(variables, variable_end_ptr);
        pro.find_lines(variables, line_start);
    }

public:
    std::string pointer_to_string(){

        std::string code = "";
        std::vector<std::string> lines = pro.cons_fold(variables,line_start);
        for (std::string line : lines){
            code += line;
        }
        return code;
    }

public:
	CFoi(PropagationAndFolding& pro) : pro(pro)
	{
		find_variables();
	}
};
