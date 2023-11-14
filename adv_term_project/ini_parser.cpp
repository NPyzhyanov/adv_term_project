#include <string>
#include <fstream>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <sstream>

#include "ini_parser.h"
#include "display_message.h"

ini_parser::ini_parser(std::string filename)
{
    p_file = std::unique_ptr<std::ifstream>(new std::ifstream("../adv_term_project/" + filename + ".ini"));
    if (!p_file->is_open())
    {
        throw std::runtime_error("File \"" + filename + ".ini\" not found");
    }
    
    bool file_is_correct = check_file();
    if (!file_is_correct)
    {
        throw std::invalid_argument("Input file is not correct. Parser was not created.");
    }
    
    p_file->clear();
}

ini_parser::~ini_parser()
{
    p_file->close();
}

bool ini_parser::check_file()
{
    p_file->seekg(3);
    
    int line_counter = 0;
    bool file_is_correct = true;
    
    while(!p_file->eof())
    {
        line_counter++;
        std::string current_line;
        std::getline(*p_file, current_line, '\n');
        
        auto it = current_line.cbegin();
        bool comment_sign_found = false;
        bool square_bracket_opened = false;
        bool end_of_line_expected = false;
        bool assignment_operator_found = false;
        bool variable_assignment_type = true;
        bool non_space_symbol_found = false;
        std::string warning_message = "";
        while (it != current_line.cend() && !comment_sign_found)
        {
            if (( *it == ';') && !comment_sign_found)
            {
                comment_sign_found = true;
            }
            if (comment_sign_found && !non_space_symbol_found && !assignment_operator_found) // Строка содержит только комменатрий
            {
                variable_assignment_type = false;
            }
            
            if (end_of_line_expected && !comment_sign_found && (*it != ' '))
            {
                warning_message = "Excess data after \"]\" symbol!"; // Ошибка: в строке после "]" лишние данные
                break;
            }
            
            if (*it == '[')
            {
                if (square_bracket_opened)
                {
                    warning_message = "Square bracket can not be opened twice!"; // Ошибка: дважды открылась скобка
                    break;
                }
                else
                {
                    if (non_space_symbol_found)
                    {
                        warning_message = "Excess data before \"[\" symbol!"; // Ошибка: в строке перед "[" лишние данные
                        break;
                    }
                    else
                    {
                        square_bracket_opened = true;
                        variable_assignment_type = false;
                    }
                }
            }
            if (*it == ']')
            {
                if (square_bracket_opened)
                {
                    square_bracket_opened = false;
                    end_of_line_expected = true;
                }
                else
                {
                    warning_message = "Excess \"]\" symbol!"; // Ошибка: скобка закрылась не открывшись
                    break;
                }
            }
            if (variable_assignment_type && !non_space_symbol_found && (*it != ' '))
            {
                non_space_symbol_found = true;
            }
            if (*it == '=')
            {
                if (assignment_operator_found)
                {
                    warning_message = "Assignment operator found more than once!"; // Ошибка: оператор присвоения встретился дважды
                    break;
                }
                else if (square_bracket_opened)
                {
                    warning_message = "Section's name can not contain symbol \"=\""; // Ошибка: название секции не может содержать оператор присвоения
                    break;
                }
                else if (!non_space_symbol_found)
                {
                    warning_message = "Variable has an empty name!"; // Ошибка: имя переменной не может быть пустым
                    break;
                }
                else
                {
                    assignment_operator_found = true;
                    non_space_symbol_found = false;
                }
            }
            it++;
        }
        
        if (warning_message != "")
        {
            display_message("Line " + std::to_string(line_counter) + ". " + warning_message);
            file_is_correct = false;
            warning_message = "";
        }
        
        if (square_bracket_opened)
        {
            warning_message = "Square bracket is not closed!"; // Ошибка: скобка не закрылась
        }
        if (non_space_symbol_found && variable_assignment_type && !assignment_operator_found)
        {
             warning_message = "Operator \"=\" not found!"; // Ошибка: в строке с объявлением переменной нет знака "="
        }
        if (assignment_operator_found && !non_space_symbol_found)
        {
            display_message("Line " + std::to_string(line_counter) + ". Variable not assigned."); // Предупреждение: переменная не инициализирована
        }
        
        if (warning_message != "")
        {
            display_message("Line " + std::to_string(line_counter) + ". " + warning_message);
            file_is_correct = false;
        }
    }
    
    return file_is_correct;
}
