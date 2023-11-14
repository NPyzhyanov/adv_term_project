#ifndef INI_PARSER_H
#define INI_PARSER_H

class ini_parser
{
public:
    ini_parser(std::string filename);
    ~ini_parser();
    
private:
    std::unique_ptr<std::ifstream> p_file;
    bool check_file();
    
public:
    template <class T> T get_value(std::string var_id)
    {
        auto it_point = std::find(var_id.begin(), var_id.end(), '.');
        std::string section = "";
        section.assign(var_id.begin(), it_point);
        std::string variable = "";
        variable.assign(it_point + 1, var_id.end());
        std::string value = "";
        
        p_file->clear();
        p_file->seekg(3);
        
        bool variable_found = false;
        bool section_found = false;
        bool check_1_more_time = false;
        while(!variable_found && !p_file->eof())
        {
            std::string current_line;
            
            while(!section_found && !p_file->eof())
            {
                if (check_1_more_time)
                {
                    check_1_more_time = false;
                }
                else
                {
                    std::getline(*p_file, current_line, '\n');
                }
                
                auto it_comment_symbol = std::find(current_line.begin(), current_line.end(), ';');
                while (it_comment_symbol > current_line.begin() && *(it_comment_symbol - 1) == ' ')
                {
                    it_comment_symbol--;
                }
                current_line.erase(it_comment_symbol, current_line.end());
                
                if (current_line == ("[" + section + "]"))
                {
                    section_found = true;
                }
            }
            
            std::string var_name;
            while (!variable_found && section_found && !p_file->eof())
            {
                std::getline(*p_file, current_line, '\n');
                
                auto it_begin = current_line.begin();
                if (it_begin < current_line.end())
                {
                    while (it_begin < current_line.end() && *(it_begin) == ' ')
                    {
                        it_begin++;
                    }
                }
                current_line.erase(current_line.begin(), it_begin);
                
                if (current_line.begin() != current_line.end())
                {
                    if (*(current_line.begin()) == '[')
                    {
                        check_1_more_time = true;
                        section_found = false;
                        break;
                    }
                }
                
                auto it_comment_symbol = std::find(current_line.begin(), current_line.end(), ';');

                if (it_comment_symbol > current_line.begin())
                {
                    while (*(it_comment_symbol - 1) == ' ')
                    {
                        it_comment_symbol--;
                        if (it_comment_symbol == current_line.begin())
                        {
                            break;
                        }
                    }
                }
                current_line.erase(it_comment_symbol, current_line.end());
                
                auto it_assignment = std::find(current_line.begin(), current_line.end(), '=');
                
                auto it_meaningful_name_end = it_assignment;
                while (it_meaningful_name_end > current_line.begin() && *(it_meaningful_name_end - 1) == ' ')
                {
                    it_meaningful_name_end--;
                }
                
                var_name.assign(current_line.begin(), it_meaningful_name_end);
                
                if (var_name == variable)
                {
                    variable_found = true;
                }
                else
                {
                    continue;
                }
                
                auto it_meaningful_value_begin = it_assignment + 1;
                while (it_meaningful_value_begin < current_line.end() && *(it_meaningful_value_begin) == ' ')
                {
                    it_meaningful_value_begin++;
                }
                
                value.assign(it_meaningful_value_begin, current_line.end());
            }
            
            if (check_1_more_time)
            {
                continue;
            }
            
            if (!section_found)
            {
                throw std::runtime_error("Section \"" + section + "\" not found");
            }
            if (!variable_found)
            {
                throw std::runtime_error("Variable \"" + variable + "\" in the section \"" + section + "\" not found");
            }
        }
        
        std::stringstream transitional_type;
        transitional_type << value;
        
        T desired_type;
        transitional_type >> desired_type;
        
        return desired_type;
    }
};

#endif // INI_PARSER_H
