//
//  CLogReader.hpp
//  FileParser
//
//  Created by Sergey Orekhov on 25.05.2022.
//

#ifndef CLogReader_hpp
#define CLogReader_hpp

#include <stddef.h>
#include "Matcher.hpp"
#include "CLogReaderListener.hpp"

/*
 * This class receives the text file chunks
 * and provides the strings which match the filter.
 *
 * I would implement asynchonius processing here and wouldn't block AddSourceBlock,
 * but I'm not allowed to use any libraries, pure C++ only.
 */
class CLogReader final {
public:
    CLogReader();
    ~CLogReader();
    
    /*
     * Requirement: set string filter, false - error.
     * filter is a string including * and/or ?, where
     * * - is a substring of any symbols with any length.
     * ? - is one any symbol.
     *
     * It is unclear if this can be called after AddSourceBlock.
     *
     * Let me add some limitations:
     * - only chars from range [' '...'~'] are allowed, otherwise it returns error and has no effect.
     * - this function returns error if you call during the data processing.
     * - nullptr is allowed and means 'no filter'.
     */
    bool SetFilter(const char *filter);
    
    /*
     * Requirement: add next text chunk
     *
     * It is not clear how to recognize the last chunk.
     *
     * Let me add the following rule:
     * - if block is nullptr this means no more chunks expected, reset the state and become ready for next text processing.
     */
    bool AddSourceBlock(const char *block, const size_t block_size);
    
    void SetListener(CLogReaderListener *listener);
    
private:
    CLogReader(const CLogReader &) = delete;
    CLogReader(CLogReader &&) = delete;
    CLogReader &operator=(const CLogReader &) = delete;
    CLogReader &operator=(CLogReader &&) = delete;
    
    char *buffer;
    size_t bufferCapacity;
    size_t bufferSize;
    bool inProgress;
    Matcher matcher;
    CLogReaderListener *listener;
};

#endif /* CLogReader_hpp */
