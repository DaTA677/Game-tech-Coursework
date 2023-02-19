/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "Matrix4.h"
#include "Vector3.h"

namespace NCL {
	using namespace NCL::Maths;
	enum class CameraType {
		Orthographic,
		Perspective
	};

	class Camera {
	public:
		Camera(void) {
			left	= 0;
			right	= 0;
			top		= 0;
			bottom	= 0;

			pitch		= 0.0f;
			yaw			= 0.0f;

			fov			= 45.0f;
			nearPlane	= 1.0f;
			farPlane	= 100.0f;

			camType		= CameraType::Perspective;
		};

		Camera(float pitch, float yaw, const Vector3& position) : Camera() {
			this->pitch		= pitch;
			this->yaw		= yaw;
			this->position	= position;

			this->fov		= 45.0f;
			this->nearPlane = 1.0f;
			this->farPlane	= 100.0f;

			this->camType	= CameraType::Perspective;
		}

		~Camera(void) = default;

		void UpdateCamera(float dt);

		float GetFieldOfVision() const {
			return fov;
		}

		float GetNearPlane() const {
			return nearPlane;
		}

		float GetFarPlane() const {
			return farPlane;
		}

		Camera& SetNearPlane(float val) {
			nearPlane = val;
			return *this;
		}
		
		Camera& SetFarPlane(float val) {
			farPlane = val;
			return *this;
		}

		//Builds a view matrix for the current camera variables, suitable for sending straight
		//to a vertex shader (i.e it's already an 'inverse camera matrix').
		Matrix4 BuildViewMatrix() const;

		Matrix4 BuildViewMatrix(Vector3 pos) const;

		Matrix4 BuildProjectionMatrix(float aspectRatio = 1.0f) const;

		//Gets position in world space
		Vector3 GetPosition() const { return position; }
		//Sets position in world space
		Camera&	SetPosition(const Vector3& val) { position = val;  return *this; }

		//Gets yaw, in degrees
		float	GetYaw()   const { return yaw; }
		//Sets yaw, in degrees
		Camera&	SetYaw(float y) { yaw = y;  return *this; }

		//Gets pitch, in degrees
		float	GetPitch() const { return pitch; }
		//Sets pitch, in degrees
		Camera& SetPitch(float p) { pitch = p; return *this; }

		static Camera BuildPerspectiveCamera(const Vector3& pos, float pitch, float yaw, float fov, float near, float far);
		static Camera BuildOrthoCamera(const Vector3& pos, float pitch, float yaw, float left, float right, float top, float bottom, float near, float far);

		Vector3 GetCameraForward() const { return Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Matrix4::Rotation(pitch, Vector3(1, 0, 0)) * Vector3(0, 0, -1); }
		Vector3 GetCameraBack() const { return Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Matrix4::Rotation(pitch, Vector3(1, 0, 0)) * Vector3(0, 0, 1); }
		Vector3 GetCameraRight() const { return Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Matrix4::Rotation(pitch, Vector3(1, 0, 0)) * Vector3(1, 0, 0); }
		Vector3 GetCameraLeft() const { return Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Matrix4::Rotation(pitch, Vector3(1, 0, 0)) * Vector3(-1, 0, 0); }
		Vector3 GetCameraUp() const { return Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Matrix4::Rotation(pitch, Vector3(1, 0, 0)) * Vector3(0,1,0); }
		Vector3 GetCameraDown() const { return Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Matrix4::Rotation(pitch, Vector3(1, 0, 0)) * Vector3(0,-1,0); }

		void MoveCamera() { moveCamera = !moveCamera; }

	protected:
		CameraType camType;

		float	nearPlane;
		float	farPlane;
		float	left;
		float	right;
		float	top;
		float	bottom;

		float	fov;
		float	yaw;
		float	pitch;
		Vector3 position;

		bool moveCamera = true;
	};
}