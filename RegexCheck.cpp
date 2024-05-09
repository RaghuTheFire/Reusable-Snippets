
#include <iostream>
#include <regex>
#include <string>

bool check_regex(const std::string& pattern) 
{
    try 
    {
        std::regex regex(pattern);
        return true;
    } 
    catch (const std::regex_error& e) 
    {
        return false;
    }
}

int main() 
{
    std::string regex_pattern = "^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+$";
    bool is_valid = check_regex(regex_pattern);
    std::cout << "The regex pattern '" << regex_pattern << "' is valid: " << std::boolalpha << is_valid << std::endl;
    return 0;
}

