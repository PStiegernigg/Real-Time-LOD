/**
 * The Shader class is a generic wrapper around OpenGL shader programs.
 *
 * A custom file type ("*.shdr") is used for specifying shaders.
 * This format allows for inclusion (#include "filename.shdr") as well
 * as defining multiple shading stages within the same file. For the latter,
 * each stage should be wrapped within a #ifdef block, checking for one
 * of the following defines:
 *
 *  _vertex_
 *  _fragment_
 *  _geometry_
 *  _tessc_
 *  _tesse_
 * _compute_
 *
 * @author Philip Salzmann
 */

#pragma once 

#include <GL/glew.h>
#include <atomic>
#include <map>
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader(const std::string& filename);
    virtual ~Shader();

    bool init();

    // calls glUseProgram on the shaders program id
    void useProgram();

    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, const glm::mat4& value);
    // subtracts GL_TEXTURE0 before passing the value
    void setUniformTexture(const std::string& name, GLenum value);
    void setUniform(const std::string& name, const glm::vec3& value);
    void setUniform(const std::string& name, const glm::ivec3& value);
    void setUniform(const std::string& name, const glm::vec2& value);
    void setUniform(const std::string& name, const glm::ivec2& value);
    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, uint value);
private:
    bool tryLoadShaderProgram();
    bool expandIncludes(const std::string& workingDirectory, std::string& code);
    std::string getFileFromLine(unsigned line) const;
    bool fileToString(const std::string& filename, std::string& result) const;
    void compileShader(const GLuint& shaderID, const std::string& shaderCode) const;
    bool checkShader(const GLuint& shaderID) const;
    bool checkProgram(const GLuint& programID) const;
    // prints error with correct line numbers (after inclusions)
    void printError(const std::string& error) const;
    void scheduleRecompile();

    // tries to find the uniform location for the given name
    // and stores it in mUniformLocations.
    // returns true if the location contained in mUniformLocations
    bool storeUniformLocation(const std::string& name);

    std::string         mShaderFilename;
    GLuint              mProgramID;
    std::atomic<bool>   mRecompileScheduled; // set by worker thread

    std::map<std::string, GLint> mUniformLocations;
    // we have to store which locations we don't know
    // since usually a set uniform call comes in each frame,
    // and we can't print a warning every time
    //
    // (the bool doesn't have any meaning)
    std::map<std::string, bool> mBadUniforms;

    // maps line number ranges to their respective file (when expanding includes)
    std::map<unsigned, std::string> mLineToFile;
};
