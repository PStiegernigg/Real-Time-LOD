#include "materials/Material.h"

GLuint Material::sEmptyTexture = 0;

Material::Material() {
    if (sEmptyTexture == 0) {
        glGenTextures(1, &sEmptyTexture);
        glBindTexture(GL_TEXTURE_2D, sEmptyTexture);
        unsigned char sEmptyTextureData[3] = { 255, 255, 255 };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &sEmptyTextureData);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
}

Material::~Material() {
}

void Material::bindMaterial(float dummy, std::shared_ptr<Shader> shader) {
    shader->setUniform("material.diffuseColor", mDiffuseColor);
    shader->setUniform("material.specularColor", mSpecularColor);
    shader->setUniform("material.phongExponent", dummy * mPhongExponent);

    bindTexture(mDiffuseMap, GL_TEXTURE0, "material.diffuseMap", shader);
    bindTexture(mSpecularMap, GL_TEXTURE0 + 1, "material.specularMap", shader);
    bindTexture(mNormalMap, GL_TEXTURE0 + 2, "material.normalMap", shader);

    shader->setUniform("material.useDiffuseMap", mDiffuseMap != nullptr);
    shader->setUniform("material.useSpecularMap", mSpecularMap != nullptr);
    shader->setUniform("material.useNormalMap", mNormalMap != nullptr);
}

void Material::bindTexture(const std::shared_ptr<Texture> texture, const GLenum unit, const std::string& uniform, std::shared_ptr<Shader> shader) const {
    if (texture != nullptr) {
        texture->bindTexture(unit);
    }
    else {
        glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_2D, sEmptyTexture);
    }

    shader->setUniformTexture(uniform, unit);
}

void Material::setPhongExponent(float exp) {
    mPhongExponent = exp;
}

void Material::setDiffuseColor(glm::vec3 color) {
    mDiffuseColor = color;
}

void Material::setDiffuseMap(std::shared_ptr<Texture> diffuse) {
    mDiffuseMap = diffuse;
}

void Material::setSpecularColor(glm::vec3 color) {
    mSpecularColor = color;
}

void Material::setSpecularMap(std::shared_ptr<Texture> specular) {
    mSpecularMap = specular;
}

void Material::setNormalMap(std::shared_ptr<Texture> normal) {
    mNormalMap = normal;
}
