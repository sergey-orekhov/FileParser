//
//  CLogReaderListener.hpp
//  FileParser
//
//  Created by Sergey Orekhov on 25.05.2022.
//

#ifndef CLogReaderListener_h
#define CLogReaderListener_h

class CLogReaderListener {
public:
    
    virtual ~CLogReaderListener() = default;
    
    virtual void NewLineFound(const char *line) = 0;
};

#endif /* CLogReaderListener_h */
