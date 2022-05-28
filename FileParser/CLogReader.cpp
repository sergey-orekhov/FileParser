//
//  CLogReader.cpp
//  FileParser
//
//  Created by Sergey Orekhov on 25.05.2022.
//
#include <cstdio>
#include <cstring>
#include "CLogReader.hpp"

CLogReader::CLogReader()
: buffer(nullptr)
, bufferCapacity(0)
, bufferSize(0)
, inProgress(false)
, listener(nullptr) {
    
}

CLogReader::~CLogReader() {
    if (buffer) {
        delete [] buffer;
        buffer = nullptr;
        bufferCapacity = 0;
        bufferSize = 0;
    }
    
    if (this->listener) {
        delete this->listener;
    }
}

bool CLogReader::SetFilter(const char *filter) {
    if (inProgress) {
        return false;
    }
    
    return matcher.SetFilter(filter);
}

bool CLogReader::AddSourceBlock(const char *block, const size_t block_size) {
    inProgress = !!block;
    
    const char *source = block ? block : "\n";
    size_t size = block ? block_size : 1;
    
    size_t freeSpace = bufferCapacity - bufferSize;
    if (freeSpace < size) {
        size_t newSize = bufferCapacity + size * 1.5;
        char *newBuffer = new char [newSize];
        
        if (!newBuffer) {
            // failed to allocate buffer
            return false;
        }
        
        if (buffer) {
            memmove(newBuffer, buffer, bufferSize);
        }
        
        buffer = newBuffer;
        bufferCapacity = newSize;
    }
        
    memcpy(buffer + bufferSize, source, size);
    bufferSize += size;
    
    char *it = buffer;
    char *begin = nullptr;
    char *remove = nullptr;
    while (it < buffer + bufferSize) {
        if (*it == '\n' || *it == '\r') {
            if (begin) {
                // end of the line
                *it = 0;
                if (matcher.Match(begin)) {
                    if (listener) {
                        listener->NewLineFound(begin);
                    }
                }
                begin = nullptr;
            }
            
            // remove this part of the line later
            remove = it;
        }
        else {
            if (!begin) {
                // begin of the line
                begin = it;
            }
        }
        
        it++;
    }
    
    if (remove) {
        // 'remove' is the iterator to the last control symbol, we should remove it also, so move to the next one.
        remove++;
        size_t removeLength = remove - buffer;
        memmove(buffer, remove, bufferSize - removeLength);
        bufferSize -= removeLength;
    }
    
    return true;
}

void CLogReader::SetListener(CLogReaderListener *listener) {
    if (this->listener) {
        delete this->listener;
    }
    
    this->listener = listener;
}
