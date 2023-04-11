#pragma once

#include "glm/glm.hpp"

class Camera
{
public:

	enum class Mode {Orbit, Freefly};
	
	Camera();
	~Camera();
	
	// Copying camera can be used for example for debug views and such
	Camera(const Camera& rhs) = default;
	Camera& operator=(const Camera& rhs) = default;

	void SetPolarFloat(glm::vec3 target, float radius, float heading, float pitch);

	void Update(float dt);
		
	// Camera mode adjustments
	void SetOrbitTarget(glm::vec3 target, float startDistance);
	void SetFollowTarget(glm::vec3 target, float startDistance);

	// Camera Controls
	void AdjustZoom(float zoomAdjust);
	
	void Pan(float x, float y);
	
	void PanLeft(float panValue);
	void PanRight(float panValue);


	 glm::mat4 GetViewMatrix() const;

private:

	enum class MovementMode { PolarFloat, Orbit, Freefly, Transition };
	
	// The coordinate system terminology is taken from the 3D Math Primer Book by 
	// Dunn and Barberry. We'll make 0 in heading to be "forward" along the +z axis
	// Pitch is positive "downwards" to maintain the left handed rule 
	struct PolarCoodinates
	{
		PolarCoodinates() = default;
		PolarCoodinates(glm::vec3 pole_, float r, float heading_, float pitch_):
			pole(pole_), radius(r), heading(heading_), pitch(pitch_)
		{}
		
		glm::vec3 pole;
		
		// r
		float radius;
		
		// Theta
		float heading;
		
		// Phi
		float pitch;
	};


	void RecalculatePolarPosition();

private:

	glm::vec3 m_Position;

	PolarCoodinates m_PolarCoordinates;

	MovementMode m_MovementMode;

	float m_RotationSpeedRad;
	float m_ForwardSpeed;
	
	glm::vec3 m_TargetPoint;





};