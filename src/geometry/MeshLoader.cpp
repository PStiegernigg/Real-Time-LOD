#include "geometry/MeshLoader.h"

#include <assert.h>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

MeshLoader::MeshLoader()
: mInstances() {

}

MeshLoader::~MeshLoader() {

}

bool MeshLoader::loadFromFile(const std::string& fileName) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    if (!scene) {
        std::cerr << importer.GetErrorString();
        return false;
    }

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLuint> indices;

    for (uint i = 0; i < scene->mNumMeshes; i++) {
        const aiMesh* mesh = scene->mMeshes[i];
        std::shared_ptr<Mesh> groupMesh = std::make_shared<Mesh>();

        for (uint j = 0; j < mesh->mNumVertices; j++) {
            vertices.push_back(mesh->mVertices[j].x);
            vertices.push_back(mesh->mVertices[j].y);
            vertices.push_back(mesh->mVertices[j].z);
        }

        if (mesh->HasNormals()) {
            for (uint j = 0; j < mesh->mNumVertices; j++) {
                normals.push_back(mesh->mNormals[j].x);
                normals.push_back(mesh->mNormals[j].y);
                normals.push_back(mesh->mNormals[j].z);
            }
            
            groupMesh->setNormals(std::move(normals));
        }
        else {
            std::cout << "Warning: Mesh does not provide vertex normals." << std::endl;
        }

        for (uint j = 0; j < mesh->mNumFaces; j++) {
            indices.push_back(mesh->mFaces[j].mIndices[0]);
            indices.push_back(mesh->mFaces[j].mIndices[1]);
            indices.push_back(mesh->mFaces[j].mIndices[2]);
        }

        groupMesh->setVertices(std::move(vertices));
        groupMesh->setIndices(std::move(indices));
        groupMesh->addLevelError(0.0f);
        groupMesh->initBuffers(0);
        groupMesh->computeBoundingVolumes();
        std::unique_ptr<Instance> instance(new Instance(groupMesh));
        mInstances.push_back(std::move(instance));
    }

    return true;
}

std::unique_ptr<Instance> MeshLoader::getNextInstance() {
    if (mInstances.empty()) return nullptr;

    std::unique_ptr<Instance> instance = std::move(mInstances.back());
    mInstances.pop_back();
    return instance;
}