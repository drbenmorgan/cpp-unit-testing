//! \file test_divide.cpp
#include <stdexcept>

/* Return result of dividing numerator by denominator */
double divide(double numerator, double denominator)
{
   if(denominator == 0) {
       throw std::invalid_argument("Division by zero");
   }

   return numerator / denominator;
}

int main()
{
   double a = divide(4, 2); // OK
}
