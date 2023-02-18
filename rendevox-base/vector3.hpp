#ifndef VECTOR3_HPP
#define VECTOR3_HPP

struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    Vector3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

#endif
