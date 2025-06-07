#include "test.hpp"
#include <iostream>

TEST::TEST( std::string a) : _a(a) 
{

}

void TEST::Print(){
    std::cout << "value: " << _a << std::endl;
}    