
/*** @file main.cpp
 * @brief Sample application to demonstrate Doxygen documentation
 * @author Your Name
 * @version 1.0
 * @date 2023-05-25
 */

#include <iostream>
#include <vector>
#include <string>

/**
 * @brief A simple class to represent a person
 */
class Person 
{
  public:
      /**
       * @brief Constructor for Person class
       * @param name The name of the person
       * @param age The age of the person
       */
      Person(const std::string& name, int age) : m_name(name), m_age(age) {}
  
      /**
       * @brief Get the name of the person
       * @return The name of the person
       */
      std::string getName() const { return m_name; }
  
      /**
       * @brief Get the age of the person
       * @return The age of the person
       */
      int getAge() const { return m_age; }
  
  private:
      std::string m_name; ///< The name of the person
      int m_age;          ///< The age of the person
};

/**
 * @brief A function to print information about a person
 * @param person The person object
 */
void printPersonInfo(const Person& person) 
{
    std::cout << "Name: " << person.getName() << ", Age: " << person.getAge() << std::endl;
}

/**
 * @brief The main function of the program
 * @return 0 on successful execution
 */
int main() 
{
    // Create a vector of Person objects
    std::vector<Person> people = 
    {
        Person("Alice", 25),
        Person("Bob", 30),
        Person("Charlie", 35)
    };

    // Print information about each person
    for (const auto& person : people) 
    {
        printPersonInfo(person);
    }

    return 0;
}
