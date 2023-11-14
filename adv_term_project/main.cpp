#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <sstream>

#include "display_message.h"
#include "ini_parser.h"

int main()
{
    ini_parser* p_parser;
    try
    {
        p_parser = new ini_parser("in");
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        return 1;
    }
    
    try
    {
        auto value1 = p_parser->get_value<double>("Section1.var1");
        std::cout << "In [Section1]: var1 = " << value1 << ", var1 / 4 = " << value1 / 4 << std::endl;
        
        auto value2 = p_parser->get_value<std::string>("Section1.var3");
        std::cout << "In [Section1]: var3 = \"" << value2 << "\"" << std::endl;
        
        auto value3 = p_parser->get_value<int>("Section3.n");
        std::cout << "In [Section3]: n = " << value3 << ", n / 4 = " << value3 / 4 << std::endl;
    }
    catch(const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }
    
    delete p_parser;
    p_parser = nullptr;

    return 0;
}
