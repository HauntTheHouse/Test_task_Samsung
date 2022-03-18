#include "MotionState.h"

MotionState::MotionState(glm::vec3& aPosition, float& aRotateAngle, glm::vec3& aRotatePosition)

    : mPosition(aPosition)
    , mRotateAngle(aRotateAngle)
    , mRotatePosition(aRotatePosition)
{
}

void MotionState::getWorldTransform(btTransform& worldTrans) const
{
    worldTrans.setOrigin(btVector3(mPosition.x, mPosition.y, mPosition.z));
    worldTrans.setRotation(btQuaternion(btVector3(mRotatePosition.x, mRotatePosition.y, mRotatePosition.z), mRotateAngle));
}

void MotionState::setWorldTransform(const btTransform& worldTrans)
{
    mPosition.x = worldTrans.getOrigin().x();
    mPosition.y = worldTrans.getOrigin().y();
    mPosition.z = worldTrans.getOrigin().z();

    mRotateAngle = worldTrans.getRotation().getAngle();
    mRotatePosition.x = worldTrans.getRotation().getAxis().x();
    mRotatePosition.y = worldTrans.getRotation().getAxis().y();
    mRotatePosition.z = worldTrans.getRotation().getAxis().z();
}
