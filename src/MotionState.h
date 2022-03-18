#pragma once
#ifndef MOTION_STATE
#define MOTION_STATE

#include <LinearMath/btMotionState.h>
#include <glm/vec3.hpp>

class MotionState : public btMotionState
{
public:
    MotionState(glm::vec3& aPosition, float& aRotateAngle, glm::vec3& aRotatePosition);

    void getWorldTransform(btTransform& worldTrans) const override;

    void setWorldTransform(const btTransform& worldTrans) override;

private:
    glm::vec3& mPosition;
    float& mRotateAngle;
    glm::vec3& mRotatePosition;
};

#endif // MOTION_STATE
