#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

#include "rendering/Shader.h"

Shader::Shader(const std::string& filename)
    : mShaderFilename(filename)
    , mProgramID{0}
    , mRecompileScheduled{false}
{
 
}

Shader::~Shader() {
    glDeleteProgram(mProgramID);
}

void Shader::useProgram() {
    assert(mProgramID != 0 && "Trying to use uncompiled shader program");

    if (mRecompileScheduled) {
        if (tryLoadShaderProgram()) {
            mUniformLocations.clear();
            mBadUniforms.clear();
        }

        mRecompileScheduled = false;
    }

    glUseProgram(mProgramID);
}

void Shader::setUniform(const std::string& name, float value) {
    if (!storeUniformLocation(name)) return;
    glProgramUniform1f(mProgramID, mUniformLocations[name], value);
}

void Shader::setUniform(const std::string& name, const glm::mat4& value) {
    if (!storeUniformLocation(name)) return;
    glProgramUniformMatrix4fv(mProgramID, mUniformLocations[name], 1, GL_FALSE, &value[0][0]);
}

void Shader::setUniformTexture(const std::string& name, GLenum value) {
    if (!storeUniformLocation(name)) return;
    glProgramUniform1i(mProgramID, mUniformLocations[name], value - GL_TEXTURE0);
}

void Shader::setUniform(const std::string& name, const glm::vec3& value) {
    if (!storeUniformLocation(name)) return;
    glProgramUniform3f(mProgramID, mUniformLocations[name], value.x, value.y, value.z);
}

void Shader::setUniform(const std::string& name, const glm::ivec3& value) {
    if (!storeUniformLocation(name)) return;
    glProgramUniform3i(mProgramID, mUniformLocations[name], value.x, value.y, value.z);
}

void Shader::setUniform(const std::string& name, const glm::vec2& value) {
    if (!storeUniformLocation(name)) return;
    glProgramUniform2f(mProgramID, mUniformLocations[name], value.x, value.y);
}

void Shader::setUniform(const std::string& name, const glm::ivec2& value) {
    if (!storeUniformLocation(name)) return;
    glProgramUniform2i(mProgramID, mUniformLocations[name], value.x, value.y);
}

void Shader::setUniform(const std::string& name, int value) {
    if (!storeUniformLocation(name)) return;
    glProgramUniform1i(mProgramID, mUniformLocations[name], value);
}

void Shader::setUniform(const std::string& name, uint value) {
    if (!storeUniformLocation(name)) return;
    glProgramUniform1ui(mProgramID, mUniformLocations[name], value);
}

bool Shader::storeUniformLocation(const std::string& name) {
    if (mUniformLocations.find(name) != mUniformLocations.end()) return true;

    GLint loc = glGetUniformLocation(mProgramID, name.c_str());
    if (loc == -1 && mBadUniforms.find(name) == mBadUniforms.end()) {
#if _DEBUG
        std::cerr << "WARNING: Uniform '" << name << "' does not exist or has illegal name." << std::endl;
#endif
        mBadUniforms[name] = true;
    }

    mUniformLocations[name] = loc;
    return true;
}


/**
 * @brief Tries to load, compile and link the shader file. 
 *        Starts watching for file changes and reloads automatically.
 *
 * @return Returns true if the initial loading of the shader program succeeeded. Note that file watching can still fail.
 */
bool Shader::init() {
    // Stop here if initial load doesn't work
    if (!tryLoadShaderProgram()) {
        return false;
    }
  
    return true;
}

/**
 * Sets the recompilation flag on the shader.
 * This is usually done from within a worker thread.
 */
void Shader::scheduleRecompile() {
    mRecompileScheduled = true;
}

std::string getParentDirectory(const std::string path) {
    auto pos = path.find_last_of("\\/");
    return (std::string::npos == pos) ? "" : path.substr(0, pos+1);
}

bool Shader::tryLoadShaderProgram() {
    GLuint programID = glCreateProgram();

    // clear and reinitialize line mapping
    mLineToFile.clear();
    mLineToFile[0] = mShaderFilename;
    
    std::string shaderCode;
    if (!fileToString(mShaderFilename, shaderCode)) {
        return false;
    }

    if (!expandIncludes(getParentDirectory(mShaderFilename), shaderCode)) {
        return false;
    }

    auto secondLine = shaderCode.find_first_of("\n");
    if (secondLine == std::string::npos) return false;
    secondLine++;

    std::vector<std::pair<std::string, GLenum>> stages = {
        std::make_pair("_vertex_", GL_VERTEX_SHADER),
        std::make_pair("_fragment_", GL_FRAGMENT_SHADER),
        std::make_pair("_geometry_", GL_GEOMETRY_SHADER),
        std::make_pair("_tessc_", GL_TESS_CONTROL_SHADER),
        std::make_pair("_tesse_", GL_TESS_EVALUATION_SHADER),
        std::make_pair("_compute_", GL_COMPUTE_SHADER)
    };

    bool noErrors = true;
    for (auto stage : stages) {
        if (shaderCode.find(std::get<0>(stage)) != std::string::npos) {
            GLuint shader = glCreateShader(std::get<1>(stage));
            auto src = shaderCode;
            src.insert(secondLine, "#define " + std::get<0>(stage) + "\n");
            compileShader(shader, src);
            noErrors = noErrors && checkShader(shader);
            glAttachShader(programID, shader);
            glDeleteShader(shader);
        }
    }

    glLinkProgram(programID);

    if (!noErrors || !checkProgram(programID)) {
        glDeleteProgram(programID);
        return false;
    }

    glDeleteProgram(mProgramID); // remove current program
    mProgramID = programID;
    return true;
}

unsigned countLines(const std::string& str) {
    return std::count(str.begin(), str.end(), '\n') + 1;
}

bool Shader::expandIncludes(const std::string& workingDirectory, std::string& code) {
    std::regex r("#include \"(.*?)\"");
    std::smatch match;
    std::vector<std::string> includedFiles;
    while (true) {
        auto pos = code.find("#include");
        if (pos == std::string::npos) return true;

        auto includeLine = countLines(code.substr(0, pos)) - 1;
        auto codeLines = countLines(code);
        auto includingFile = getFileFromLine(includeLine);

        // get whole line
        auto endPos = code.find_first_of("\n", pos);
        auto line = code.substr(pos, endPos - pos);
        if (std::regex_search(line, match, r)) {
            auto fullPath = workingDirectory;
            fullPath.append(*(match.begin() + 1));
            if (std::find(includedFiles.begin(), includedFiles.end(), fullPath) != includedFiles.end()) {
                std::cout << "Skipping already included file in " << fullPath << std::endl;
                code.erase(pos, endPos - pos);
                continue;
            }
            includedFiles.push_back(fullPath);

            std::string src;
            if (!fileToString(fullPath, src)) return false;

            code.erase(pos, endPos - pos);
            code.insert(pos, src);

            // update line map
            auto c = countLines(src);
            std::map<unsigned, std::string> lineToFile; // create new map to spare us some hassle
            bool offset = includeLine == 0; // if include is on first line in file, offset all others right away
            for (auto it = mLineToFile.begin(); it != mLineToFile.end(); ++it) {
                if (!offset) {
                    if (it->first < includeLine) {
                        lineToFile[it->first] = it->second;
                    }
                    else {
                        lineToFile[it->first + c] = it->second;
                    }

                    auto nit = std::next(it);
                    if (nit == mLineToFile.end() || nit->first > includeLine) {
                        offset = true;
                        lineToFile[includeLine] = fullPath;

                        // if the current element is the last one of the original list and we inserted the new code in between the old one,
                        // add the including file a second time, after the included one.
                        if ((nit == mLineToFile.end() && includeLine < codeLines && it->first < includeLine)
                        // in this case the next file starts at least one line after the point of inclusion, which means that
                        // the inclusion isn't happening at the end of a file.
                        || (nit != mLineToFile.end() && nit->first > includeLine + 1 && it->second.compare(nit->second) != 0)) {
                            // the including file wraps around the included file
                            lineToFile[it->first + c + includeLine] = includingFile;
                        }

                        if (nit == mLineToFile.end()) break;
                        lineToFile[nit->first + c] = nit->second;
                    }
                }
                else {
                    lineToFile[it->first + c] = it->second;
                }
            }

            if (includeLine == 0) {
                // prepend it now so that we don't offset it as well
                mLineToFile[includeLine] = fullPath;
            }

            mLineToFile = lineToFile;

        }
    }
}

std::string Shader::getFileFromLine(unsigned line) const {
    std::string file = mShaderFilename;
    for (auto ltf : mLineToFile) {
        if (ltf.first <= line) file = ltf.second;
        else return file;
    }
    return file;
}

bool Shader::fileToString(const std::string& filename, std::string& result) const {
    std::ifstream fileHandle(filename, std::ios::in);
    if (!fileHandle.is_open()) {
        std::cerr << mShaderFilename << ": Unable to open file '" << filename << "'" << std::endl;
        return false;
    }
    
    std::string str((std::istreambuf_iterator<char>(fileHandle)), std::istreambuf_iterator<char>());
    result = str;

    fileHandle.close();
    return true;
}

void Shader::compileShader(const GLuint& shaderID, const std::string& shaderCode) const {
    const char* cStringShaderCode{shaderCode.c_str()};
    glShaderSource(shaderID, 1, &cStringShaderCode, NULL);
    glCompileShader(shaderID);

}

bool Shader::checkShader(const GLuint& shaderID) const {
    GLint status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);

    if (!status) {
        std::cerr << "\aError(s) during shader compilation: " << mShaderFilename << std::endl;
        GLint logLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> shaderErrorMsg(logLength);
        glGetShaderInfoLog(shaderID, logLength, NULL, &shaderErrorMsg[0]);
        printError(std::string(shaderErrorMsg.begin(), shaderErrorMsg.end()));
        return false;
    }

    return true;
}

bool Shader::checkProgram(const GLuint& programID) const {
    GLint linkStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus) {
        std::cerr << "Error(s) during program linking." << std::endl;
        GLint logLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> programErrorMsg(std::max(logLength, int(1)));
        glGetProgramInfoLog(programID, logLength, NULL, &programErrorMsg[0]);
        std::cerr << std::string(programErrorMsg.begin(), programErrorMsg.end()) << std::endl;
        return false;
    }

    return true;

    // TODO: we omit validation for now, as it should generally be done after
    //       binding uniforms.

    //GLint validateStatus;
    //glValidateProgram(programID);
    //glGetProgramiv(programID, GL_VALIDATE_STATUS, &validateStatus);
    //if (!validateStatus) {
    //    std::cerr << "Error(s) during program validation." << std::endl;
    //    GLint logLength;
    //    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
    //    std::vector<char> programErrorMsg(std::max(logLength, int(1)));
    //    glGetProgramInfoLog(programID, logLength, NULL, &programErrorMsg[0]);
    //    fprintf(stdout, "%s\n", &programErrorMsg[0]);
    //    return false;
    //}

    //return true;
}

void Shader::printError(const std::string& error) const {
    // error output is vendor specific unfortunately
    std::regex nvidia("^\\d+?\\((\\d+)\\) :");  // e.g. "0(9) : error C1008: undefined variable"
    std::regex amd("^ERROR: \\d+?:(\\d+): ");   // e.g. "ERROR: 1:6: error(#202) No matching overloaded function found"
    std::smatch match;

    std::stringstream ss(error);
    std::string line;
    std::string outputString;
    std::string prevFile = "";
    bool hasMatches = false;

    while (std::getline(ss, line, '\n')) {
        bool nvidiaMatch = std::regex_search(line, match, nvidia);
        bool amdMatch = false;
        if (!nvidiaMatch) {
            amdMatch = std::regex_search(line, match, amd);
        }

        if (nvidiaMatch || amdMatch) {
            hasMatches = true;
            // subtract one as we are starting from zero
            // additionally, we added one line at the very beginning of the file (the stage #define)!
            unsigned l = std::atol(match[1].str().c_str()) - 2;
            auto file = getFileFromLine(l); 

            // translate line number relative to file
            auto fl = l;
            for (auto it = std::next(mLineToFile.begin()); it != mLineToFile.end(); ++it) {
                auto prev = std::prev(it);
                if (prev->first < l && prev->second != file) {
                    fl -= it->first - prev->first - 1;
                }
            }

            if (file != mShaderFilename) --fl;

            if (prevFile != file) {
                prevFile = file;
                outputString += "\n" + file + ":\n";
            }

            std::ostringstream oss;
            oss << fl + 1 << ":";

            if (nvidiaMatch) {
                outputString += std::regex_replace(line, nvidia, oss.str()) + "\n";
            }
            else {
                outputString += std::regex_replace(line, amd, oss.str()) + "\n";
            }
        }
        else {
            outputString += line + "\n";
        }
    }

    if (!hasMatches && mLineToFile.size() > 1) {
        outputString += "WARNING: Failed to parse vendor specific error message. Line numbers may be wrong.\n";
    }

    std::cout << outputString << std::endl;
}
