#include "Camera.h"

#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera() :
	m_MovementMode(MovementMode::Orbit),
	m_RotationSpeedRad(glm::half_pi<float>()),
	m_ForwardSpeed(0.0f),
	m_TargetPoint(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Camera::~Camera()
{
}

void Camera::SetPolarFloat(glm::vec3 target, float radius, float heading, float pitch)
{
	m_PolarCoordinates.pole = target;
	m_PolarCoordinates.radius = radius;
	m_PolarCoordinates.heading = heading;
	m_PolarCoordinates.pitch = pitch;

	RecalculatePolarPosition();

}

void Camera::Update(float dt)
{
	if(m_MovementMode == MovementMode::Orbit)
	{
		
	}
	
	else if(m_MovementMode == MovementMode::Freefly)
	{
	
	}

	else if(m_MovementMode == MovementMode::Transition)
	{
	
	}

}

void Camera::AdjustZoom(float zoomAdjust)
{
	if(m_MovementMode == MovementMode::Orbit || m_MovementMode == MovementMode::PolarFloat)
	{
		m_PolarCoordinates.radius += zoomAdjust;
	}
}

void Camera::Pan(float x, float y)
{
	if (m_MovementMode == MovementMode::Orbit || m_MovementMode == MovementMode::PolarFloat)
	{
		m_PolarCoordinates.heading += x;
		m_PolarCoordinates.pitch -= y;
	}

	m_PolarCoordinates.heading = glm::mod(m_PolarCoordinates.heading, glm::two_pi<float>());
	m_PolarCoordinates.pitch = glm::clamp(m_PolarCoordinates.pitch, -glm::half_pi<float>(), glm::half_pi<float>());
	
	RecalculatePolarPosition();
}

void Camera::PanLeft(float panValue)
{
	if(m_MovementMode == MovementMode::Orbit || m_MovementMode == MovementMode::PolarFloat)
	{
		m_PolarCoordinates.heading += panValue;
	}

	m_PolarCoordinates.heading = glm::mod(m_PolarCoordinates.heading, glm::two_pi<float>());

}

void Camera::PanRight(float panValue)
{
	if(m_MovementMode == MovementMode::Orbit || m_MovementMode == MovementMode::PolarFloat)
	{
		m_PolarCoordinates.heading -= panValue;
	}
	
	m_PolarCoordinates.heading = glm::mod(m_PolarCoordinates.heading, glm::two_pi<float>());
}

void Camera::SetOrbitTarget(glm::vec3 target, float startDistance)
{
	m_MovementMode = MovementMode::Orbit;
	
	m_PolarCoordinates.pole = target;
	m_PolarCoordinates.radius = startDistance;
}

void Camera::SetFollowTarget(glm::vec3 target, float startDistance)
{
	m_MovementMode = MovementMode::PolarFloat;
	
	m_PolarCoordinates.pole = target;
	m_PolarCoordinates.radius = startDistance;
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(m_Position, m_TargetPoint, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::RecalculatePolarPosition()
{
	float x, y, z;

	x = m_PolarCoordinates.radius
		* glm::cos(m_PolarCoordinates.pitch)
		* glm::sin(m_PolarCoordinates.heading);
	
	y = -m_PolarCoordinates.radius * glm::sin(m_PolarCoordinates.pitch);
	
	z = m_PolarCoordinates.radius
		* glm::cos(m_PolarCoordinates.pitch)
		* glm::cos(m_PolarCoordinates.heading);
	 
	m_Position = glm::vec3(x, y, z);
}
