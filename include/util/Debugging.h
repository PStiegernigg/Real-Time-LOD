/**
 * This file defines some helper macros for debugging the application.
 *
 * @author Philip Salzmann
 */

#pragma once
#include <iostream>
#include <string>

// Utility macros for the NVIDIA Tools Extension Library
// (for use with Nsight)
#if _DEBUG
#include "nvToolsExt.h"
#define NVTX_PUSH_RANGE(name) nvtxRangePushA(name);
#define NVTX_POP_RANGE() nvtxRangePop();
#else
#define NVTX_PUSH_RANGE(name)
#define NVTX_POP_RANGE()
#endif

#if _DEBUG
#define GET_FILENAME(path) ((std::string::npos == std::string(path).find_last_of("\\/")) ? path : std::string(path).substr(std::string(path).find_last_of("\\/")+1))

#define PRINT_ERROR(error) std::cerr << GET_FILENAME(__FILE__) << "(" << __LINE__ << ") : " << error << std::endl;

#define CLEAR_GL_ERRORS() while (glGetError() != GL_NO_ERROR) {};

#define PRINT_GL_ERROR() {\
    GLenum error = glGetError();\
    switch (error) {\
    case GL_NO_ERROR: break;\
    case GL_INVALID_ENUM:                   PRINT_ERROR("GL_INVALID_ENUM") break;\
    case GL_INVALID_VALUE:                  PRINT_ERROR("GL_INVALID_VALUE") break;\
    case GL_INVALID_OPERATION:              PRINT_ERROR("GL_INVALID_OPERATION") break;\
    case GL_INVALID_FRAMEBUFFER_OPERATION:  PRINT_ERROR("GL_INVALID_FRAMEBUFFER_OPERATION") break;\
    case GL_OUT_OF_MEMORY:                  PRINT_ERROR("GL_OUT_OF_MEMORY") break;\
    case GL_STACK_UNDERFLOW:                PRINT_ERROR("GL_STACK_UNDERFLOW") break;\
    case GL_STACK_OVERFLOW:                 PRINT_ERROR("GL_STACK_OVERFLOW") break;\
    };\
}
#else
#define CLEAR_GL_ERRORS()
#define PRINT_GL_ERROR()
#endif
