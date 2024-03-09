#pragma once
#include <objloader.h>

class Camera
{
	Matrix3x3 Rx(const float angle) {
		return Matrix3x3(
			1, 0, 0,
			0, cosf(angle), -sin(angle),
			0, sin(angle), cosf(angle));
	}

	Matrix3x3 Rz(const float angle) {
		return Matrix3x3(
			cosf(angle), -sin(angle), 0,
			sin(angle), cosf(angle), 0,
			0, 0, 1);
	}

	float to_radians(float deg) {
		return deg * M_PI / 180;
	}


public:
	Vector3 eye = Vector3(17.0f, -29.0f, 8.0f);
	Vector3 up = Vector3(0.0f, 0.0f, 1.0f);
	Vector3 view_dir = Vector3(.0f, 1.0f, 0.0f);

	float yaw;
	float pitch;

	Camera() {
		view_dir.Normalize();
		up.Normalize();
		yaw = atan2f(view_dir.y, view_dir.x) - M_PI_2;
		pitch = acosf(-view_dir.z) - M_PI_2;
	}

	void update_viewdir(){
		Vector3 new_view_dir = Rz(yaw) * Rx(pitch) * Vector3(0, 1, 0);
		new_view_dir.Normalize();
		view_dir = new_view_dir;
	}

	void change_angles(float x_offset, float y_offset) {
		yaw += to_radians(-x_offset);
		pitch += to_radians(-y_offset);

		if (pitch > to_radians(89.0f))
			pitch = to_radians(89.0f);
		if (pitch < to_radians(-89.0f))
			pitch = to_radians(-89.0f);

		update_viewdir();
	}

	Matrix4x4 get_view_matrix() {
		Vector3 z = - view_dir;
		z.Normalize();
		Vector3 x = up.CrossProduct(z);
		x.Normalize();
		Vector3 y = z.CrossProduct(x);
		Matrix4x4 view_matrix = Matrix4x4(x, y, z, eye);
		view_matrix.EuclideanInverse();
		return view_matrix;
	}

	Matrix4x4 get_projection_matrix() {
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		float window_width = viewport[2];
		float window_height = viewport[3];
		
		float near_plane = 1;
		float far_plane = 100;
		float fov_x = 60 * M_PI / 180;
		float half_width_m = tan(fov_x / 2);
		float aspect = float(window_height) / window_width;
		float a = (near_plane + far_plane) / (near_plane - far_plane);
		float b = 2 * near_plane * far_plane / (near_plane - far_plane);
		
		auto P = Matrix4x4(
			1 / half_width_m, 0, 0, 0,
			0, -aspect / half_width_m, 0, 0,
			0, 0, a, b,
			0, 0, -1, 0
		);

		return P;
	}

	Matrix4x4 get_viewport_matrix() {
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		Matrix4x4 P = Matrix4x4();
		P.set(0, 0, 100 * 2.0f / viewport[2]);
		P.set(1, 1, 100 * 2.0f / viewport[3]);
		return P;
	}
};

