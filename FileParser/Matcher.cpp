//
//  Matcher.cpp
//  FileParser
//
//  Created by Sergey Orekhov on 25.05.2022.
//

#include "Matcher.hpp"
#include <cstring>

Matcher::Matcher() {
    filterExpression = nullptr;
}

Matcher::~Matcher() {
    if (filterExpression) {
        delete [] filterExpression;
        filterExpression = nullptr;
    }
}

bool Matcher::SetFilter(const char *filter) {
    if (filter) {
        const char *it = filter;
        while (*it != 0) {
            if ((*it < 32) || (*it > 126)) {
                // invalid character
                return false;
            }
            
            it++;
        }
    }
    
    if (filterExpression) {
        delete [] filterExpression;
        filterExpression = nullptr;
    }
    
    if (filter) {
        size_t len = strlen(filter);
        filterExpression = new char[len + 1];
        memcpy(filterExpression, filter, len);
        filterExpression[len] = 0;
    }
    else {
        filterExpression = nullptr;
    }
    
    return true;
}

bool Matcher::Match(const char *string) {
    if (filterExpression == nullptr) {
        return true;
    }
    
    return InternalMatch(string, filterExpression);
}

bool Matcher::InternalMatch(const char *string, const char *expression) {
    if (*expression == 0) {
        return *string == 0;
    }
    else if (*expression == '*') {
        const char *it = string;
        // try variants
        while(true) {
            if (InternalMatch(it, expression+1)) {
                return true;
            }
            
            if (*it == 0) {
                break;
            }
            
            it++;
        }
        return false;
    }
    else if (*string == 0) {
        // expression is not empty
        // but string is empty
        // failure
        return false;
    }
    else if (*expression == '?') {
        // just go to next symbol
        return InternalMatch(string+1, expression+1);
    }
    else {
        // same char
        if (*expression == *string) {
            return InternalMatch(string+1, expression+1);
        }
        
        return false;
    }
}
