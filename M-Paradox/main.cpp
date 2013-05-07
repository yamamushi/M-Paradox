//
//  main.cpp
//  M-Paradox
//
//  Created by Jonathan Rumion on 5/6/13.
//  Copyright (c) 2013 TAP. All rights reserved.
//

#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

int main()
{
    using namespace boost::lambda;
    typedef std::istream_iterator<int> in;
    
    std::for_each(
                  in(std::cin), in(), std::cout << (_1 * 3) << " " );
}

