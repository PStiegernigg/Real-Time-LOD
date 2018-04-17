cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME RTLOD)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(X11 REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(ImageMagick COMPONENTS Magick++)

include_directories(
    ./include
    ./vendor/glew-2.1.0/include
    ./vendor/glfw-3.2.1/include
    ./vendor/glm
    ./vendor/AntTweakBar/include
    ./vendor/assimp-3.3.1/include
    ${ImageMagick_INCLUDE_DIRS}
    
)

set(SOURCES
    ./src/main.cpp
    ./src/ui/GUI.cpp
    ./src/ui/GUIBar.cpp
    ./src/ui/InputHandler.cpp
    ./src/util/Timer.cpp    
    ./src/util/Screenshot.cpp
    ./src/util/Parser.cpp
    ./src/lights/DirectionalLight.cpp
    ./src/lights/Light.cpp
    ./src/lights/PointLight.cpp
    ./src/lights/SpotLight.cpp
    ./src/rendering/Shader.cpp
    ./src/rendering/RenderBuffer.cpp
    ./src/rendering/RenderContext.cpp
    ./src/rendering/DirectRenderTechnique.cpp
    ./src/cameras/Camera.cpp
    ./src/cameras/OrthoCamera.cpp
    ./src/cameras/PerspectiveCamera.cpp
    ./src/materials/Texture.cpp
    ./src/materials/Material.cpp
    ./src/geometry/Mesh.cpp
    ./src/geometry/Plane.cpp
    ./src/geometry/SceneLoader.cpp
    ./src/geometry/MeshLoader.cpp
    ./src/geometry/Instance.cpp
    ./src/geometry/Scene.cpp
    ./src/geometry/BoundingSphere.cpp
    ./src/geometry/BoundingBox.cpp
    ./src/geometry/LODGenerator.cpp
)

set(HEADERS
    ./include/config.h
    ./include/ui/GUI.h
    ./include/ui/GUIBar.h
    ./include/ui/InputHandler.h
    ./include/util/Timer.h
    ./include/util/Enumerators.h
    ./include/util/Screenshot.h
    ./include/util/Parser.h
    ./include/util/FloatToIntConversion.h
    ./include/util/Debugging.h
    ./include/util/Util.h
    ./include/lights/DirectionalLight.h
    ./include/lights/Light.h
    ./include/lights/PointLight.h
    ./include/lights/SpotLight.h
    ./include/rendering/Shader.h
    ./include/rendering/RenderBuffer.h
    ./include/rendering/RenderContext.h
    ./include/rendering/DirectRenderTechnique.h
    ./include/cameras/Camera.h
    ./include/cameras/PerspectiveCamera.h
    ./include/cameras/OrthoCamera.h
    ./include/materials/Texture.h
    ./include/materials/Material.h
    ./include/geometry/Mesh.h
    ./include/geometry/Plane.h
    ./include/geometry/SceneLoader.h
    ./include/geometry/MeshLoader.h
    ./include/geometry/Instance.h
    ./include/geometry/Scene.h
    ./include/geometry/BoundingSphere.h
    ./include/geometry/BoundingBox.h
    ./include/geometry/LODGenerator.h
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME}
    ${SOURCES} ${HEADERS}
)

ADD_DEFINITIONS (-DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

add_library(glew SHARED IMPORTED)
set_property(TARGET glew PROPERTY
             IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glew-2.1.0/lib/libGLEW.so)
 
add_library(AntTweakBar SHARED IMPORTED)
set_property(TARGET AntTweakBar PROPERTY
             IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/vendor/AntTweakBar/lib/libAntTweakBar.so)

add_library(glfw SHARED IMPORTED)
set_property(TARGET glfw PROPERTY
             IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw-3.2.1/src/libglfw.so)

add_library(assimp SHARED IMPORTED)
set_property(TARGET assimp PROPERTY
             IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp-3.3.1/lib/libassimp.so)


target_link_libraries(${PROJECT_NAME}
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    ${X11_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${ImageMagick_LIBRARIES}
    
    glew
    glfw
    AntTweakBar
    assimp
)
