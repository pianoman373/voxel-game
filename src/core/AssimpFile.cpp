#include "AssimpFile.hpp"
#include "Path.hpp"
#include "Resources.hpp"


void AssimpFile::processNode(Bone &b, aiNode *node) {
    // for (unsigned int i = 0; i < node->mNumChildren; i++) {
    //     aiNode *child = node->mChildren[i];

    //     aiVector3D position;
    //     aiQuaternion rotation;

    //     child->mTransformation.DecomposeNoScaling(rotation, position);

    //     Bone &childBone = b.addChild(Bone(child->mName.C_Str(), vec3(position.x, position.y, position.z), quaternion(rotation.w, rotation.x, rotation.y, rotation.z)));

    //     processNode(childBone, child);
    // }
}

AssimpFile::AssimpFile() {

}

AssimpFile::~AssimpFile() {

}

void AssimpFile::load(const aiScene *scene, Path workingDirectory) {
    // this->scene = scene;

    // for (unsigned int index = 0; index < scene->mNumMaterials; index++) {
    //     aiMaterial *aMaterial = scene->mMaterials[index];

    //     Material material;
    //     material.setPBRUniforms(vec3(0.5f), 0.5f, 0.0f);

    //     material.setDefaultPBRUniforms();

    //     if (aMaterial->GetTextureCount(aiTextureType_DIFFUSE)) {
    //         aiString albedoFile;
    //         aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &albedoFile);
    //         std::string albedoPath = workingDirectory.appendPath(albedoFile.C_Str());
    //         Texture albedo = Resources::getTexture(albedoPath);

    //         material.setUniformBool("albedoTextured", true);
    //         material.setUniformTexture("albedoTex", albedo, 0);
    //     }

    //     if (aMaterial->GetTextureCount(aiTextureType_HEIGHT)) {
    //         aiString normalFile;
    //         aMaterial->GetTexture(aiTextureType_HEIGHT, 0, &normalFile);
    //         std::string normalPath = workingDirectory.appendPath(normalFile.C_Str());
    //         Texture normal = Resources::getTexture(normalPath);

    //         material.setUniformBool("normalTextured", true);
    //         material.setUniformTexture("normalTex", normal, 1);
    //     }

    //     if (aMaterial->GetTextureCount(aiTextureType_SPECULAR)) {
    //         aiString metallicFile;
    //         aMaterial->GetTexture(aiTextureType_SPECULAR, 0, &metallicFile);
    //         std::string metallicPath = workingDirectory.appendPath(metallicFile.C_Str());
    //         Texture metallic = Resources::getTexture(metallicPath);

    //         material.setUniformBool("metallicTextured", true);
    //         material.setUniformTexture("metallicTex", metallic, 2);
    //     }

    //     if (aMaterial->GetTextureCount(aiTextureType_SHININESS)) {
    //         aiString roughnessFile;
    //         aMaterial->GetTexture(aiTextureType_SHININESS, 0, &roughnessFile);
    //         std::string roughnessPath = workingDirectory.appendPath(roughnessFile.C_Str());
    //         Texture roughness = Resources::getTexture(roughnessPath);

    //         material.setUniformBool("roughnessTextured", true);
    //         material.setUniformTexture("roughnessTex", roughness, 3);
    //     }

    //     materials.push_back(material);
    // }

    // for (unsigned int index = 0; index < scene->mNumMeshes; index++) {
    //     aiMesh *aMesh = scene->mMeshes[index];

    //     meshMaterialIndices.push_back(aMesh->mMaterialIndex);
    //     Mesh mesh;
    //    // Mesh &mesh = meshes[meshes.size()];

    //     mesh.positions.resize(aMesh->mNumVertices);
    //     mesh.normals.resize(aMesh->mNumVertices);
    //     mesh.indices.resize(aMesh->mNumFaces * 3);

    //     if (aMesh->mTangents) {
    //         mesh.tangents.resize(aMesh->mNumVertices);
    //     }

    //     if (aMesh->mNumUVComponents[0] > 0) {
    //         mesh.uvs.resize(aMesh->mNumVertices);
    //     }

    //     for (unsigned int i = 0; i < aMesh->mNumVertices; ++i) {
    //         mesh.positions[i] = vec3(aMesh->mVertices[i].x, aMesh->mVertices[i].y, aMesh->mVertices[i].z);
    //         mesh.normals[i] = vec3(aMesh->mNormals[i].x, aMesh->mNormals[i].y, aMesh->mNormals[i].z);

    //         if (aMesh->mTangents) {
    //             mesh.tangents[i] = vec3(aMesh->mTangents[i].x, aMesh->mTangents[i].y, aMesh->mTangents[i].z);

    //             if (isnan(mesh.tangents[i].x) || isnan(mesh.tangents[i].y) ||  isnan(mesh.tangents[i].z)) {
    //                 mesh.tangents[i] = {0.0f, 0.0f, 0.0f};
    //             }
    //         }
            

    //         if (aMesh->mTextureCoords[0]) {
    //             mesh.uvs[i] = vec2(aMesh->mTextureCoords[0][i].x, aMesh->mTextureCoords[0][i].y);
    //         }
    //     }

    //     if (aMesh->mNumBones > 0) {
    //         mesh.boneIDs.resize(aMesh->mNumVertices);
    //         mesh.boneWeights.resize(aMesh->mNumVertices);

    //         std::vector<std::vector<int>> tempBoneIDs;
    //         tempBoneIDs.resize(aMesh->mNumVertices);

    //         std::vector<std::vector<float>> tempBoneWeights;
    //         tempBoneWeights.resize(aMesh->mNumVertices);



    //         for (unsigned int i = 0; i < aMesh->mNumBones; i++) {
    //             aiBone *bone = aMesh->mBones[i];

    //             std::cout << bone->mName.C_Str() << std::endl;

    //             for (unsigned int j = 0; j < bone->mNumWeights; j++) {
    //                 aiVertexWeight weight = bone->mWeights[j];

    //                 tempBoneIDs[weight.mVertexId].push_back(i);
    //                 tempBoneWeights[weight.mVertexId].push_back(weight.mWeight);
    //             }
    //         }

    //         for (size_t i = 0; i < tempBoneIDs.size(); i++) {
    //             std::vector<int> idsAtVertex = tempBoneIDs[i];
    //             std::vector<float> weightsAtVertex = tempBoneWeights[i];

    //             size_t size = idsAtVertex.size();

    //             vec4i boneID;
    //             vec4 boneWeight;

    //             if (size > 0) {
    //                 boneID.x = idsAtVertex[0];
    //                 boneWeight.x = weightsAtVertex[0];
    //             }
    //             if (size > 1) {
    //                 boneID.y = idsAtVertex[1];
    //                 boneWeight.y = weightsAtVertex[1];
    //             }
    //             if (size > 2) {
    //                 boneID.z = idsAtVertex[2];
    //                 boneWeight.z = weightsAtVertex[2];
    //             }
    //             if (size > 3) {
    //                 boneID.w = idsAtVertex[3];
    //                 boneWeight.w = weightsAtVertex[3];
    //             }

    //             mesh.boneIDs[i] = boneID;
    //             mesh.boneWeights[i] = boneWeight;
    //         }
    //     }

    //     for (unsigned int f = 0; f < aMesh->mNumFaces; ++f) {
    //         for (unsigned int i = 0; i < 3; ++i) {
    //             mesh.indices[f * 3 + i] = aMesh->mFaces[f].mIndices[i];
    //         }
    //     }

    //     mesh.generate();

    //     meshes.push_back(mesh);
    // }
}

Bone AssimpFile::getSkeleton() {
    Bone bone;

    // if (scene) {
    //     aiNode *rootNode = scene->mRootNode->FindNode("root");

    //     aiVector3D position;
    //     aiQuaternion rotation;

    //     //std::cout << rootNode->mName.C_Str() << std::endl;

    //     rootNode->mTransformation.DecomposeNoScaling(rotation, position);
        
    //     bone.name = rootNode->mName.C_Str();
    //     bone.position = vec3(position.x, position.y, position.z);
    //     bone.startingPosition = bone.position;
    //     bone.rotation = quaternion(rotation.w, rotation.x, rotation.y, rotation.z);
    //     bone.startingRotation = bone.rotation;

    //     processNode(bone, rootNode);
    // }

    return bone;
}

Mesh &AssimpFile::getMesh(unsigned int index) {
    return meshes[index];
}

unsigned int AssimpFile::numMeshes() {
    return 0;
}


Animation AssimpFile::getAnimation() {
     Animation ret;

//     if (scene) {
//         aiAnimation* anim = scene->mAnimations[1];




//         float ticks = (float)anim->mTicksPerSecond;
//         ret.length = (float)anim->mDuration / ticks;



//         for (unsigned int i = 1; i < anim->mNumChannels; i++) {
//             aiNodeAnim* nodeAnim = anim->mChannels[i];



//             auto &keyframes = ret.keyframes[nodeAnim->mNodeName.C_Str()];

// //        std::cout << anim->mChannels[i]->mNodeName.C_Str() << std::endl;
// //
// //        std::cout << nodeAnim->mNumPositionKeys << std::endl;
// //        std::cout << nodeAnim->mNumRotationKeys << std::endl;
// //        std::cout << nodeAnim->mNumScalingKeys << std::endl;

//             for (unsigned int j = 0; j < nodeAnim->mNumPositionKeys; j++) {
//                 Transform t;

//                 aiVectorKey posKey = nodeAnim->mPositionKeys[j];
//                 aiQuatKey rotKey = nodeAnim->mRotationKeys[j];
//                 aiVectorKey scaleKey = nodeAnim->mScalingKeys[j];

//                 t.position = vec3(posKey.mValue.x, posKey.mValue.y, posKey.mValue.z);
//                 t.rotation = quaternion(rotKey.mValue.w, rotKey.mValue.x, rotKey.mValue.y, rotKey.mValue.z);
//                 t.scale = vec3(scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z);

//                 keyframes.push_back({(float)posKey.mTime / ticks, t});

//             }
//         }
//     }

    return ret;
}

void AssimpFile::addToScene(Scene &scene) {
    for (int i = 0; i < meshes.size(); i++) {
        scene.createMeshObject(meshes[i], materials[meshMaterialIndices[i]], Transform(), "");
    }
}