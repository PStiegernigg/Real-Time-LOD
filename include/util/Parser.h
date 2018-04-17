/**
* A text parser (used as base class for e.g. OBJ and MTL parsers).
*
* Originally created by Basil Fierz (loader.h).
* Modified by Philip Salzmann.
*/

#pragma once

#include <fstream>
#include <string>

struct ILoader;

class Parser {
public:
    Parser();
    ~Parser();

    virtual void parse(ILoader* loader, const std::string& path) = 0;
    bool eof() { return mEof; }

protected:
    virtual int convertTokenToInteger(const std::string& token) const;
    virtual double convertTokenToFloat(const std::string& token) const;

    virtual void setInputStream(std::ifstream* stream);
    virtual void readLine();
    virtual void readLine(std::string* out_string_ptr);
    virtual void skipWhiteSpace();
    virtual void skipLine();
    virtual void readString(std::string* out_string_ptr);
    virtual bool readFloat(float* f_ptr);
    virtual bool readInt(int* i_ptr);

private:
    std::ifstream* mStream;
    const unsigned int mBufferSize;
    // buffer of size mBufferSize
    char* mStreamBuffer;
    // alias for mStreamBuffer
    char* mCurrentBuffer;

    unsigned int mCurrentSizeAvailable;
    // reading position within mCurrentBuffer
    char* mBufferReadPtr;
    bool mEof;
};
