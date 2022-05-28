//
//  Matcher.hpp
//  FileParser
//
//  Created by Sergey Orekhov on 25.05.2022.
//

#ifndef Matcher_hpp
#define Matcher_hpp

/*
 * This class is responsible to find a match of the filter in the given string.
 */
class Matcher {
public:
    Matcher();
    ~Matcher();
    
    bool SetFilter(const char *filter);
    
    bool Match(const char *string);
    
private:
    Matcher(const Matcher &) = delete;
    Matcher(Matcher &&) = delete;
    Matcher &operator=(const Matcher &) = delete;
    Matcher &operator=(Matcher &&) = delete;
    
    bool InternalMatch(const char *string, const char *expression);
    
    char *filterExpression;
};

#endif /* Matcher_hpp */
