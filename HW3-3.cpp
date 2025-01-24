#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cmath>

/**
 * @class Nth_Power
 * @brief a functor class to compute the nth power of a given integer.
 * 
 * The class is initialized with an integer n, specifying the power to which numbers will be raised.
 * The functor overloads the operator() method to compute and return the nth power of the input integer.
 */
class Nth_Power {
    int n;
public:
    /**
     * @brief Constructs the Nth_power functor.
     * @param power The int n specifying the power to raise numbers to.
     */
    Nth_Power(int power) : n(power) {}

    /**
     * @brief Computes the nth power of the input int.
     * @param x The integer to be raised to the nth power.
     * @return int x^n
     */
    int operator()(int x) const {
        return static_cast<int>(std::pow(x, n));
    }
};

//this function is copied directly from the homework slide, with the namespaces specified as necessary
int main()
{ std::vector<int> v = { 1, 2, 3, 4, 5 };
Nth_Power cube{3};
std::cout << cube(7) << std::endl; // prints 343
// print first five cubes
transform(v.begin(), v.end(),
std::ostream_iterator<int>(std::cout, ", "), cube);
}

