#pragma once

namespace Verlet
{
	class CParticle
	{
	public:
		core::vector3df Position;
		core::vector3df OldPosition;
		core::vector3df Velocity;
		core::vector3df AffectVelocity;

		bool IsConstraint;
		float Mass;
		float Friction;

	public:
		CParticle();

		virtual ~CParticle();

		inline void setPosition(const core::vector3df& pos)
		{
			Position = pos;
			OldPosition = pos;
		}
	};
}