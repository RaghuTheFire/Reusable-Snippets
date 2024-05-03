#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>

std::vector < std::string > parseXML(const std::string & xmlData) 
{
  std::vector < std::string > tokens;
  std::string token;
  std::stack < char > tagStack;
  bool isTag = false;
  bool isClosingTag = false;

  for (char c: xmlData) 
  {
    if (c == '<') 
    {
      if (!token.empty()) 
      {
        tokens.push_back(token);
        token.clear();
      }
      isTag = true;
      if (tagStack.empty() || tagStack.top() != '/') 
      {
        tagStack.push(c);
      } 
      else 
      {
        isClosingTag = true;
        tagStack.pop();
      }
    } 
    else
    if (c == '>') 
    {
      if (isTag) 
      {
        tokens.push_back(token);
        token.clear();
      }
      isTag = false;
      isClosingTag = false;
      if (!tagStack.empty()) 
      {
        tagStack.pop();
      }
    } 
    else
    if (isTag) 
    {
      token += c;
    } 
    else 
    {
      token += c;
    }
  }

  if (!token.empty()) 
  {
    tokens.push_back(token);
  }

  return tokens;
}

int main() 
{
  std::ifstream file("input.xml");
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string xmlData = buffer.str();

  std::vector < std::string > tokens = parseXML(xmlData);

  for (const std::string & token: tokens) 
  {
    std::cout << token << std::endl;
  }

  return 0;
}
