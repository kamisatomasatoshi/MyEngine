#pragma once

#include"BaseCollider.h"
#include"CollisionPrimitive.h"

#include"Model.h"


/// <summary>
/// ���b�V���Փ˔���I�u�W�F�N�g
/// </summary>
class MeshCollider :
	public BaseCollider
{
public:
	MeshCollider()
	{
		// ���b�V���`����Z�b�g
		shapeType = COLLISIONSHAPE_MESH;
	}

	/// <summary>
	/// �O�p�`�̔z����\�z����
	/// </summary>
	void ConstructTriangles(std::unique_ptr<Model>& model, Matrix4 matWorld);
	//void ConstructTriangles(fbxModel* model);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update(const Matrix4& worldPos) override;

	/// <summary>
	/// ���Ƃ̓����蔻��
	/// </summary>
	/// <param name="sphere">��</param>
	/// <param name="inter">��_�i�o�͗p�j</param>
	/// <returns>�������Ă��邩�ۂ�</returns>
	bool CheckCollisionSphere(const Sphere& sphere, Vector4* inter = nullptr, Vector4* reject = nullptr);

	/// <summary>
	/// ���C�Ƃ̓����蔻��
	/// </summary>
	/// <param name="sphere">���C</param>
	/// <param name="distance">�����i�o�͗p�j</param>
	/// <param name="inter">��_�i�o�͗p�j</param>
	/// <returns>�������Ă��邩�ۂ�</returns>
	bool CheckCollisionRay(const Ray& ray, float* distance = nullptr, Vector4* inter = nullptr);

private:
	std::vector<Triangle> triangles;
	// ���[���h�s��̋t�s��
 Matrix4 invMatWorld;
};