#include "util/Parser.h"

#include <iostream>
#include <limits>
#include <cstring>
#include <assert.h>

Parser::Parser()
    : mStream(nullptr)
    , mBufferSize(512 * 1024)
    , mStreamBuffer(nullptr)
    , mCurrentBuffer(nullptr)
    , mCurrentSizeAvailable(0)
    , mBufferReadPtr(nullptr)
     ,mEof(false)
{
    mStreamBuffer = new char[mBufferSize];
    mCurrentBuffer = mStreamBuffer;
    mBufferReadPtr = mCurrentBuffer;
}

Parser::~Parser() {
    delete[] mStreamBuffer;
}

void Parser::setInputStream(std::ifstream* stream) {
    mStream = stream;
}

void Parser::readLine() {
    // check if a full line is still available
    char* current_pointer = mBufferReadPtr;
    while (current_pointer < mCurrentBuffer + mCurrentSizeAvailable)
    {
        if ((*current_pointer) == '\n')
        {
            return;
        }
        ++current_pointer;
    }

    // else: there is no line available... copy data to the beginning if there are any and read as much as
    // possible into the buffer
    bool full_line_available = false;
    while (!full_line_available)
    {
        // FIXME: This was originally a unsigned int that caused a underflow in some cases - investigate!
        long long int copy_amount = (mCurrentBuffer + mCurrentSizeAvailable) - mBufferReadPtr;

        if (copy_amount == 0 && mStream->eof()) {
            copy_amount = mCurrentBuffer - mBufferReadPtr;
        }

        if (copy_amount > 0 && mBufferReadPtr != mCurrentBuffer) {
            memmove(mCurrentBuffer, mBufferReadPtr, copy_amount);
        }
        else if (copy_amount < 0 && mStream->eof()) {
            mEof = true;
            return;
        }

        mBufferReadPtr = mCurrentBuffer;
        // we can safely typecast here as copy_amount cannot exceed mBufferSize
        mCurrentSizeAvailable = (unsigned int)copy_amount;

        if (mStream->eof()) {
            full_line_available = true;
            continue;
        }

        mStream->read(mCurrentBuffer + mCurrentSizeAvailable, mBufferSize - mCurrentSizeAvailable);
        std::streamsize amount_read = mStream->gcount();
        if (amount_read == 0) {
            std::cerr << "Stream error: No bytes read!" << std::endl;
        }
        assert(amount_read < std::numeric_limits<unsigned int>::max());
        mCurrentSizeAvailable += (unsigned int) amount_read;

        current_pointer = mBufferReadPtr;
        while (current_pointer < mCurrentBuffer + mCurrentSizeAvailable)
        {
            if ((*current_pointer) == '\n')
            {
                full_line_available = true;
                break;
            }
            ++current_pointer;
        }
    }
}

void Parser::skipWhiteSpace() {
    while (
        mBufferReadPtr < mCurrentBuffer + mCurrentSizeAvailable &&
        (
        (*mBufferReadPtr) == ' ' ||
        (*mBufferReadPtr) == '\t' ||
        (*mBufferReadPtr) == '\r'
        ))
    {
        ++mBufferReadPtr;
    }
}

void Parser::skipLine() {
    while (mBufferReadPtr < mCurrentBuffer + mCurrentSizeAvailable && (*mBufferReadPtr) != '\n')
    {
        ++mBufferReadPtr;
    }
    if (mBufferReadPtr < mCurrentBuffer + mCurrentSizeAvailable)
    {
        ++mBufferReadPtr;
    }
}

void Parser::readLine(std::string* out_string_ptr) {
    char* begin_ptr = mBufferReadPtr;
    skipLine();

    char c = *mBufferReadPtr;
    *mBufferReadPtr = '\0';

    *out_string_ptr = begin_ptr;

    *mBufferReadPtr = c;
}

void Parser::readString(std::string* out_string_ptr) {
    skipWhiteSpace();

    char* begin_ptr = mBufferReadPtr;
    while (
        (*mBufferReadPtr) != ' ' &&
        (*mBufferReadPtr) != '\t' &&
        (*mBufferReadPtr) != '\r' &&
        (*mBufferReadPtr) != '\n'
        )
    {
        ++mBufferReadPtr;
    }

    char c = *mBufferReadPtr;
    *mBufferReadPtr = '\0';

    *out_string_ptr = begin_ptr;

    *mBufferReadPtr = c;
}

bool Parser::readFloat(float* f_ptr) {
    skipWhiteSpace();

    char* begin_ptr = mBufferReadPtr;
    while (
        ((*mBufferReadPtr) >= '0' && (*mBufferReadPtr) <= '9') ||
        (*mBufferReadPtr) == '.' ||
        (*mBufferReadPtr) == '-' ||
        (*mBufferReadPtr) == 'E' ||
        (*mBufferReadPtr) == 'e'
        )
    {
        ++mBufferReadPtr;
    }

    //kaos_assert(CharTraits<char>::is_whitespace(*buffer_read_ptr_), "");

    if (mBufferReadPtr == begin_ptr)
    {
        return false;
    }

    char c = *mBufferReadPtr;
    *mBufferReadPtr = '\0';

    *f_ptr = (float) atof(begin_ptr);

    *mBufferReadPtr = c;

    return true;
}

bool Parser::readInt(int* i_ptr) {
    skipWhiteSpace();

    char* begin_ptr = mBufferReadPtr;
    while (
        ((*mBufferReadPtr) >= '0' && (*mBufferReadPtr) <= '9') ||
        (*mBufferReadPtr) == '-'
        )
    {
        ++mBufferReadPtr;
    }

    if (mBufferReadPtr == begin_ptr)
    {
        return false;
    }

    //kaos_assert(CharTraits<char>::is_whitespace(*buffer_read_ptr_) || (*buffer_read_ptr_) == '/', "");

    char c = *mBufferReadPtr;
    *mBufferReadPtr = '\0';

    *i_ptr = atoi(begin_ptr);

    *mBufferReadPtr = c;

    return true;
}

int Parser::convertTokenToInteger(const std::string& token) const {
    if (token.length() == 0)
        return 0;
    else
        return atoi(token.c_str());
}

double Parser::convertTokenToFloat(const std::string& token) const {
    if (token.length() == 0)
        return 0;
    else
        return atof(token.c_str());
}