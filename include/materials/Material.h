/**
 * @author Philip Salzmann
 */

#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "Texture.h"
#include "rendering/Shader.h"

class Material {
public:
    Material();
    virtual ~Material();

    void bindMaterial(float dummy, std::shared_ptr<Shader> shader);

    void setPhongExponent(float exp);

    void setDiffuseColor(glm::vec3 color);
    void setDiffuseMap(std::shared_ptr<Texture> diffuse);
    void setSpecularColor(glm::vec3 color);
    void setSpecularMap(std::shared_ptr<Texture> specular);
    void setNormalMap(std::shared_ptr<Texture> normal);
private:
    // empty texture is bound when no map is set (for diffuse, specular...)
    static GLuint sEmptyTexture;

    glm::vec3 mDiffuseColor{ 1.f, 1.f, 1.f };
    glm::vec3 mSpecularColor{ 1.f, 1.f, 1.f };
    float mPhongExponent = 1.f;
    std::shared_ptr<Texture> mDiffuseMap;
    std::shared_ptr<Texture> mSpecularMap;
    std::shared_ptr<Texture> mNormalMap;

    /**
     * Binds the given texture to a uniform location.
     * If the texture does not exist, an empty texture is bound instead.
     */
    void bindTexture(const std::shared_ptr<Texture> texture, const GLenum unit, const std::string& uniform, std::shared_ptr<Shader> shader) const;
};

