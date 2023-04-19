#include "Vector2.h"
#include<cmath>  //sprt

Vector2::Vector2() :x(0), y(0)
{
}

Vector2::Vector2(float x, float y){
	this->x = x;
	this->y = y;
}

float Vector2::length()const {
	return std::sqrt(x * x + y * y);
}

Vector2& Vector2::normalize() {
	float len = length();
	if (len != 0) {
		return *this /= len;
	}
	return *this;
}

float Vector2::dot(const Vector2& v) const
{
	return x * v.x + y * v.y;
}

float Vector2::cross(const Vector2& v) const
{
	return x * v.y - y * v.x;
}

Vector2& Vector2::operator+=(const Vector2& v)
{
	this->x += v.x;
	this->y += v.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
	this->x -= v.x;
	this->y -= v.y;
	return *this;
}

Vector2& Vector2::operator*=(float s)
{
	this->x *= s;
	this->y *= s;
	return *this;
}

Vector2& Vector2::operator/=(float s)
{
	x /= s;
	y /= s;
	return *this;
}

//Vector2 �N���X�ɑ����Ȃ��֐��Q
const Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp = v1;
	Vector2 temp2 = v2;
	temp.x = temp.x + temp2.x;
	temp.y = temp.y + temp2.y;

	return temp;
}

const Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp = v1;
	Vector2 temp2 = v2;
	temp.x = temp.x - temp2.x;
	temp.y = temp.y - temp2.y;

	return temp;
}

const Vector2 operator*(const Vector2& v, float s)
{
	Vector2 temp;
	temp.x = v.x * s;
	temp.y = v.y * s;

	return temp;
}

const Vector2 operator*(float s, const Vector2& v)
{
	Vector2 temp;
	temp.x = v.x * s;
	temp.y = v.y * s;

	return temp;
}

const Vector2 operator/(const Vector2& v, float s)
{
	Vector2 temp;
	temp.x = v.x / s;
	temp.y = v.y / s;

	return temp;
}