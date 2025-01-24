#include <iostream>
#include <string>

/**
 * @class Printer
 * @brief a class to initialize a static object with a constructor to print it without us
 * 
 * This class creates a static object, which are initialized at project startup
 * All logic that outputs text to the console is handled by the constructor of the class
 */
class Printer {
public:
    Printer(const std::string& message) {
        std::cout << message;
    }
};

// Static objects to print the desired output
static Printer hello("Hello, ");
static Printer world("World!");
static Printer newline("\n");

// the compiler requires a main function to compile, but this doesn't do anything.
// all printing to the console is handled by the static objects
int main(){
    return 0;
}
