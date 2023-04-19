#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <cassert>
#include "Input.h"
#include "ViewProjection.h"
#include <memory>
#include <list>

class BoxCollision {
public:
    //�v���C���[�ƃG�l�~�[�̍��W�����
    //�v���C���[�̔��a��x,y,z�̏��ɓ����
    //�G�l�~�[�̔��a��x,y,z�̏��ɓ����
    bool BoxCollision_1(Vector3 player, Vector3 enemy, Vector3 playerWidth,Vector3 enemyWidth);
    bool BoxCollision_(Vector3 player, Vector3 playerWidth, Vector2 enemyPosX, Vector2 enemyPosY, Vector2 enemyPosZ);
    bool BoxCollision_2(Vector3 player, Vector3 enemy, Vector3 playerWidth, Vector3 enemyWidth,bool flag);

    void Contact(int playerMove ,Vector3 player,WorldTransform enemy,Vector3 enemyRotationMove, Vector3 enemyMove);
private:
    //���[���h�ϊ��f�[�^
    WorldTransform worldTransform;

    ////���f��
    //Model* model = nullptr;
    ////���f��
    //Model* enemyBulletModel = nullptr;

    
};
