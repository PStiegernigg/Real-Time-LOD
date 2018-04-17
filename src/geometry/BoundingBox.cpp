#include "geometry/BoundingBox.h"

BoundingBox::BoundingBox() {}

BoundingBox::BoundingBox(const glm::vec3& minCorner, const glm::vec3& dimensions)
    : mMinCorner(minCorner)
    , mDimensions(dimensions)
{}

BoundingBox::BoundingBox(const BoundingBox& orig)
    : mMinCorner(orig.mMinCorner)
    , mDimensions(orig.mDimensions)
{}

BoundingBox::~BoundingBox() {}

void BoundingBox::include(const BoundingBox& bBox) {
    auto pd = mDimensions;
    auto pmc = mMinCorner;

    if (bBox.mMinCorner.x + bBox.mDimensions.x > mMinCorner.x + mDimensions.x) {
        mDimensions.x += (bBox.mMinCorner.x + bBox.mDimensions.x) - (mMinCorner.x + mDimensions.x);
    }

    if (bBox.mMinCorner.y + bBox.mDimensions.y > mMinCorner.y + mDimensions.y) {
        mDimensions.y += (bBox.mMinCorner.y + bBox.mDimensions.y) - (mMinCorner.y + mDimensions.y);
    }

    if (bBox.mMinCorner.z + bBox.mDimensions.z > mMinCorner.z + mDimensions.z) {
        mDimensions.z += (bBox.mMinCorner.z + bBox.mDimensions.z) - (mMinCorner.z + mDimensions.z);
    }

    if (bBox.mMinCorner.x < mMinCorner.x) {
        auto delta = mMinCorner.x - bBox.mMinCorner.x;
        mMinCorner.x = bBox.mMinCorner.x;
        mDimensions.x += delta;
    }

    if (bBox.mMinCorner.y < mMinCorner.y) {
        auto delta = mMinCorner.y - bBox.mMinCorner.y;
        mMinCorner.y = bBox.mMinCorner.y;
        mDimensions.y += delta;
    }

    if (bBox.mMinCorner.z < mMinCorner.z) {
        auto delta = mMinCorner.z - bBox.mMinCorner.z;
        mMinCorner.z = bBox.mMinCorner.z;
        mDimensions.z += delta;
    }

    assert(mDimensions.x >= pd.x && mDimensions.y >= pd.y && mDimensions.z >= pd.z);
    assert(mMinCorner.x <= pmc.x && mMinCorner.y <= pmc.y && mMinCorner.z <= pmc.z);
}

// FIXME: store transformation matrix like BoundingSphere
void BoundingBox::transform(const glm::mat4& matrix) {
    glm::vec3 v[8];
    v[0] = glm::vec3(mMinCorner.x, mMinCorner.y, mMinCorner.z);
    v[1] = glm::vec3(mMinCorner.x + mDimensions.x, mMinCorner.y, mMinCorner.z);
    v[2] = glm::vec3(mMinCorner.x, mMinCorner.y + mDimensions.y, mMinCorner.z);
    v[3] = glm::vec3(mMinCorner.x, mMinCorner.y, mMinCorner.z + mDimensions.z);

    v[4] = glm::vec3(mMinCorner.x + mDimensions.x, mMinCorner.y + mDimensions.y, mMinCorner.z); // XY
    v[5] = glm::vec3(mMinCorner.x + mDimensions.x, mMinCorner.y, mMinCorner.z + mDimensions.z); // XZ
    v[6] = glm::vec3(mMinCorner.x, mMinCorner.y + mDimensions.y, mMinCorner.z + mDimensions.z); // YZ
    v[7] = glm::vec3(mMinCorner.x + mDimensions.x, mMinCorner.y + mDimensions.y, mMinCorner.z + mDimensions.z); // XYZ

    float xMin = std::numeric_limits<float>::max();
    float xMax = std::numeric_limits<float>::min();
    float yMin = std::numeric_limits<float>::max();
    float yMax = std::numeric_limits<float>::min();
    float zMin = std::numeric_limits<float>::max();
    float zMax = std::numeric_limits<float>::min();

    for (int i = 0; i < 7; ++i) {
        auto t = matrix * glm::vec4(v[i], 1.f);

        if (t.x < xMin) xMin = t.x;
        if (t.x > xMax) xMax = t.x;
        if (t.y < yMin) yMin = t.y;
        if (t.y > yMax) yMax = t.y;
        if (t.z < zMin) zMin = t.z;
        if (t.z > zMax) zMax = t.z;
    }

    mMinCorner = glm::vec3(xMin, yMin, zMin);
    auto maxCorner = glm::vec3(xMax, yMax, zMax);
    mDimensions = maxCorner - mMinCorner;
}

glm::vec3 BoundingBox::getMinCorner() const {
    return mMinCorner;
}

glm::vec3 BoundingBox::getDimensions() const {
    return mDimensions;
}
