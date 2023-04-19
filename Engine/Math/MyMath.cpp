#include "MyMath.h"
#include <cmath>


Matrix4 MyMath::Initialize() {
	Matrix4 matInitialize {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return matInitialize;
}

Matrix4 MyMath::Scale(const Vector3& scale) {

	//�X�P�[�����O�s���錾
	Matrix4 matScale = {
		scale.x, 0.0f, 0.0f,   0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f,    0.0f, scale.z, 0.0f,
		0.0f, 0.0f,    0.0f, 1.0f
	};

	Matrix4 scaling = Initialize();
	scaling *= matScale;

	return scaling;
}

Matrix4 MyMath::Rotation(const Vector3& rotation, int X_1_Y_2_Z_3_XYZ_6) {
	int rotationX = 1;
	int rotationY = 2;
	int rotationZ = 3;
	int rotationXYZ = 6;

	if (X_1_Y_2_Z_3_XYZ_6 == rotationX) {
		Matrix4 matRotX = {
		  1.0f,0.0f,0.0f,0.0f,
		  0.0f,cos(rotation.x),sin(rotation.x),0.0f,
		  0.0f,-sin(rotation.x),cos(rotation.x),0.0f,
		  0.0f,0.0f,0.0f,1.0f
		};
		Matrix4 rotationX = Initialize();
		rotationX *= matRotX;

		return rotationX;
	}
	else if (X_1_Y_2_Z_3_XYZ_6 == rotationY) {
		Matrix4 matRotY = {
			cos(rotation.y), 0.0f, -sin(rotation.y), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			sin(rotation.y), 0.0f, cos(rotation.y),  0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		Matrix4 rotationY = Initialize();
		rotationY *= matRotY;

		return rotationY;
	}
	else if (X_1_Y_2_Z_3_XYZ_6 == rotationZ) {
		Matrix4 matRotZ = {
		  cos(rotation.z),sin(rotation.z),0.0f,0.0f,
		  -sin(rotation.z),cos(rotation.z),0.0f,0.0f,
		  0.0f,0.0f,1.0f,0.0f,
		  0.0f,0.0f,0.0f,1.0f
		};
		Matrix4 rotationZ = Initialize();
		rotationZ *= matRotZ;

		return rotationZ;
	}
	else {
		Matrix4 matRotXYZ;
		Matrix4 matRot_X, matRot_Y, matRot_Z;
		matRot_X = {
		  1.0f,0.0f,0.0f,0.0f,
		  0.0f,cos(rotation.x),sin(rotation.x),0.0f,
		  0.0f,-sin(rotation.x),cos(rotation.x),0.0f,
		  0.0f,0.0f,0.0f,1.0f
		};
		matRot_Y = {
			cos(rotation.y), 0.0f, -sin(rotation.y),
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			sin(rotation.y), 0.0f, cos(rotation.y),
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};
		matRot_Z = {
		  cos(rotation.z),sin(rotation.z),0.0f,0.0f,
		  -sin(rotation.z),cos(rotation.z),0.0f,0.0f,
		  0.0f,0.0f,1.0f,0.0f,
		  0.0f,0.0f,0.0f,1.0f
		};
		//�e���̉�]�s�������
		matRotXYZ = Initialize();

		matRotXYZ *= matRot_X;
		matRotXYZ *= matRot_Y;
		matRotXYZ *= matRot_Z;

		return matRotXYZ;
	}
}

//Vector3 Vector3::vector3Normalize(Vector3& v) {
//	float x, y, z, w;
//
//	w = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
//	x = v.x / w;
//	y = v.y / w;
//	z = v.z / w;
//
//	return Vector3(x, y, z);
//}

Matrix4 MyMath::Translation(const Vector3& Move) {
	Matrix4 matMove = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f,   1.0f,   0.0f,   0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		Move.x, Move.y, Move.z, 1.0f
	};
	Matrix4 move = Initialize();
	move *= matMove;

	return move;
}

Vector3 MyMath::vector3Normalize(const Vector3& v)
{
	float x, y, z, w;

	w = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	x = v.x / w;
	y = v.y / w;
	z = v.z / w;

	return Vector3(x, y, z);
}

//�x�N�g���ƍs��̊|���Z(�o��Vector3)
Vector3 MyMath::MatVector(Matrix4 matrix4, Vector3 vector3) {
	Vector3 matVector = { 0,0,0 };

	matVector.x = vector3.x * matrix4.m[0][0] + vector3.y * matrix4.m[1][0] + vector3.z * matrix4.m[2][0];
	matVector.y = vector3.x * matrix4.m[0][1] + vector3.y * matrix4.m[1][1] + vector3.z * matrix4.m[2][1];
	matVector.z = vector3.x * matrix4.m[0][2] + vector3.y * matrix4.m[1][2] + vector3.z * matrix4.m[2][2];

	return matVector;
}

Vector3 MyMath::MulVector3(Vector3 vector3, Vector3 s) {
	Vector3 M = vector3;

	M.x *= s.x;
	M.y *= s.y;
	M.z *= s.z;

	return M;
}

const Vector3 MyMath::SubVec(Vector3 v, Vector3 v2) {
	Vector3 V3 = v;

	V3.x -= v2.x;
	V3.y -= v2.y;
	V3.z -= v2.z;

	return V3;
}

Vector3 MyMath::GetWorldTransform(Matrix4 matrix4) {
	Vector3 worldVector = { 0,0,0 };

	worldVector.x = matrix4.m[3][0];
	worldVector.y = matrix4.m[3][1];
	worldVector.z = matrix4.m[3][2];

	return worldVector;
}

const Vector3 MyMath::AddVector3(const Vector3 v1, const Vector3 v2) {
	Vector3 V1(v1);

	V1.x += v2.x;
	V1.y += v2.y;
	V1.z += v2.z;

	return V1;
}

Vector3 MyMath::DivVecMat(const Vector3& vector3, const Matrix4& matrix4)
{
	Vector3 V3 = { 0,0,0 };
	float v1 = 0.0f;

	V3.x = vector3.x * matrix4.m[0][0] + vector3.y * matrix4.m[1][0] + vector3.z * matrix4.m[2][0] + 1 * matrix4.m[3][0];
	V3.y = vector3.x * matrix4.m[0][1] + vector3.y * matrix4.m[1][1] + vector3.z * matrix4.m[2][1] + 1 * matrix4.m[3][1];
	V3.z = vector3.x * matrix4.m[0][2] + vector3.y * matrix4.m[1][2] + vector3.z * matrix4.m[2][2] + 1 * matrix4.m[3][2];
	v1 = vector3.x * matrix4.m[0][3] + vector3.y * matrix4.m[1][3] + vector3.z * matrix4.m[2][3] + 1 * matrix4.m[3][3];

	V3.x /= v1;
	V3.y /= v1;
	V3.z /= v1;


	return { V3.x, V3.y, V3.z };
}

Vector4 MyMath::Vec4Mat4Mul(const Vector4& vec, const Matrix4& mat)
{
	Vector4 retVec = {};

	retVec.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + vec.w * mat.m[3][0];

	retVec.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + vec.w * mat.m[3][1];

	retVec.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + vec.w * mat.m[3][2];

	return retVec;
}

Matrix4 MyMath::setViewportMat(WinApp* window, const Vector3& v) {
	//�P�ʍs��̐ݒ�
	Matrix4 matViewport = Initialize();
	matViewport.m[0][0] = window->window_width / 2;
	matViewport.m[1][1] = -window->window_height / 2;
	matViewport.m[3][0] = (window->window_width / 2) + v.x;
	matViewport.m[3][1] = (window->window_height / 2) + v.y;
	return matViewport;
}

Matrix4 MyMath::MatrixInverse(Matrix4 pOut)
{
	Matrix4 mat;
	int i, j, loop;
	double fDat, fDat2;
	double mat_8x4[4][8];
	int flag;
	float* pF;
	double* pD;

	//8 x 4�s��ɒl������
	for (i = 0; i < 4; i++) {
		pF = pOut.m[i];
		for (j = 0; j < 4; j++, pF++) mat_8x4[i][j] = (double)(*pF);
		pD = mat_8x4[i] + 4;
		for (j = 0; j < 4; j++) {
			if (i == j)   *pD = 1.0;
			else         *pD = 0.0;
			pD++;
		}
	}

	flag = 1;
	for (loop = 0; loop < 4; loop++) {
		fDat = mat_8x4[loop][loop];
		if (fDat != 1.0) {
			if (fDat == 0.0) {
				for (i = loop + 1; i < 4; i++) {
					fDat = mat_8x4[i][loop];
					if (fDat != 0.0) break;
				}
				if (i >= 4) {
					flag = 0;
					break;
				}
				//�s�����ւ���
				for (j = 0; j < 8; j++) {
					fDat = mat_8x4[i][j];
					mat_8x4[i][j] = mat_8x4[loop][j];
					mat_8x4[loop][j] = fDat;
				}
				fDat = mat_8x4[loop][loop];
			}

			for (i = 0; i < 8; i++) mat_8x4[loop][i] /= fDat;
		}
		for (i = 0; i < 4; i++) {
			if (i != loop) {
				fDat = mat_8x4[i][loop];
				if (fDat != 0.0f) {
					//mat[i][loop]��mat[loop]�̍s�ɂ�����
					//(mat[j] - mat[loop] * fDat)���v�Z
					for (j = 0; j < 8; j++) {
						fDat2 = mat_8x4[loop][j] * fDat;
						mat_8x4[i][j] -= fDat2;
					}
				}
			}
		}
	}

	if (flag) {
		for (i = 0; i < 4; i++) {
			pF = mat.m[i];
			pD = mat_8x4[i] + 4;
			for (j = 0; j < 4; j++) {
				*pF = (float)(*pD);
				pF++;
				pD++;
			}
		}
	}
	else {
		//�P�ʍs������߂�
		mat = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
		};
	}

	pOut = mat;

	if (flag) return pOut;
	return pOut;
}

Matrix4 MyMath::MakeInverse(const Matrix4* mat)
{

	//�|���o���@���s���s��
	float sweep[4][8]{};
	//�萔�{�p
	float constTimes = 0.0f;
	//���e����덷
	float MAX_ERR = 1e-10f;
	//�߂�l�p
	Matrix4 retMat;

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			//weep�̍����ɋt�s������߂�s����Z�b�g
			sweep[i][j] = mat->m[i][j];

			//sweep�̉E���ɒP�ʍs����Z�b�g
			sweep[i][4 + j] = MyMath::MakeIdentity().m[i][j];
		}
	}

	//�S�Ă̗�̑Ίp�����ɑ΂���J��Ԃ�
	for (size_t i = 0; i < 4; i++)
	{
		//�ő�̐�Βl�𒍖ڑΊp�����̐�Βl�Ɖ���
		float max = std::fabs(sweep[i][i]);
		size_t maxIndex = i;

		//i��ڂ��ő�̐�Βl�ƂȂ�s��T��
		for (size_t j = i + 1; j < 4; j++)
		{
			if (std::fabs(sweep[j][i]) > max)
			{
				max = std::fabs(sweep[j][i]);
				maxIndex = j;
			}
		}

		if (fabs(sweep[maxIndex][i]) <= MAX_ERR)
		{
			//�t�s��͋��߂��Ȃ�
			return MyMath::MakeIdentity();
		}

		//����(1):i�s�ڂ�maxIndex�s�ڂ����ւ���
		if (i != maxIndex)
		{
			for (size_t j = 0; j < 8; j++)
			{
				float tmp = sweep[maxIndex][j];
				sweep[maxIndex][j] = sweep[i][j];
				sweep[i][j] = tmp;
			}
		}

		//sweep[i][i]�Ɋ|�����1�ɂȂ�l�����߂�
		constTimes = 1 / sweep[i][i];

		//����(2):p�s�ڂ�a�{����
		for (size_t j = 0; j < 8; j++)
		{
			//����ɂ��sweep[i][i]��1�ɂȂ�
			sweep[i][j] *= constTimes;
		}

		//����(3)�ɂ��i�s�ڈȊO�̍s��i��ڂ�0�ɂ���
		for (size_t j = 0; j < 4; j++)
		{
			if (j == i)
			{
				//i�s�ڂ͂��̂܂�
				continue;
			}

			//i�s�ڂɊ|����l�����߂�
			constTimes = -sweep[j][i];

			for (size_t k = 0; k < 8; k++)
			{
				//j�s�ڂ�i�s�ڂ�a�{�����s�𑫂�
				//����ɂ��sweep[j][i]��0�ɂȂ�
				sweep[j][k] += sweep[i][k] * constTimes;
			}
		}
	}

	//sweep�̉E������mat�̋t�s��
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			retMat.m[i][j] = sweep[i][4 + j];
		}
	}

	return retMat;
}

Matrix4 MyMath::ConvertXMMATtoMat4(DirectX::XMMATRIX XMMatrix) {
	Matrix4 result;
	for (int i = 0; i < 4; i++) {

		result.m[i][0] = DirectX::XMVectorGetX(XMMatrix.r[i]);
		result.m[i][1] = DirectX::XMVectorGetY(XMMatrix.r[i]);
		result.m[i][2] = DirectX::XMVectorGetZ(XMMatrix.r[i]);
		result.m[i][3] = DirectX::XMVectorGetW(XMMatrix.r[i]);
	}


	return result;
}


Matrix4 MyMath::LookAtLH(Vector3 eye, Vector3 target, Vector3 up) {
	Vector3 zaxis = target - eye;
	zaxis.normalize();
	Vector3 xaxis = up.cross(zaxis);
	xaxis.normalize();
	Vector3 yaxis = zaxis.cross(xaxis);
	yaxis.normalize();

	Matrix4 LookAt = {
		xaxis.x,	xaxis.y,	xaxis.z,	0,
		yaxis.x,	yaxis.y,	yaxis.z,	0,
		zaxis.x,	zaxis.y,	zaxis.z,	0,
		eye.x,		eye.y,		eye.z,		1
	};

	return LookAt;
}

Matrix4 MyMath::PerspectiveFovLH(float fovAngleY, float aspectRatio, float nearZ, float farZ) {

	float h = 1 / tan(fovAngleY * 0.5);
	float w = h / aspectRatio;
	float a = farZ / (farZ - nearZ);
	float b = (-nearZ * farZ) / (farZ - nearZ);

	Matrix4 perspectiveFovLH = {
		w,		 0,		 0,		 0,
		0,		 h,		 0,		 0,
		0,		 0,		 a,		 1,
		0,		 0,		 b,		 0
	};


	return perspectiveFovLH;
}

Matrix4 MyMath::MakeIdentity()
{
	Matrix4 me;
	return me;
}

// �l��͈͓��ɔ[�߂�
float MyMath::Clamp(float Value, const float low, const float high)
{
	if (high < Value)
	{
		Value = high;
	}

	if (Value < low)
	{
		Value = low;
	}

	return Value;
}

Matrix4 MyMath::Matrix4Orthographic(
	float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ) {
	assert(fabsf(viewRight - viewLeft) > 0.00001f);
	assert(fabsf(viewTop - viewBottom) > 0.00001f);
	assert(fabsf(farZ - nearZ) > 0.00001f);

	float width = 1.0f / (viewRight - viewLeft);
	float height = 1.0f / (viewTop - viewBottom);
	float fRange = 1.0f / (farZ - nearZ);
	float sx = width * 2.0f;
	float sy = height * 2.0f;
	float sz = fRange;
	float tx = -(viewLeft + viewRight) * width;
	float ty = -(viewTop + viewBottom) * height;
	float tz = -fRange * nearZ;

	Matrix4 m{ sx, 0.0f, 0.0f, 0.0f, 0.0f, sy, 0.0f, 0.0f, 0.0f, 0.0f, sz, 0.0f, tx, ty, tz, 1.0f };

	return m;
}

float MyMath::GetAngle(float angle)
{
	return angle * (PI / 180);
}

