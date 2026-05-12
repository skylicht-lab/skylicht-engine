/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "Graphics2D/CCanvas.h"
#include "ParticleSystem/CParticleComponent.h"

namespace Skylicht
{
	namespace Particle
	{
		/**
		 * @class CGUIParticle
		 * @ingroup ParticleSystem
		 * @brief 2D GUI element that displays a 3D particle system.
		 * @details Allows particle effects to be used within the UI canvas.
		 * 
		 * ### Example
		 * @code
		 * // You need to register the plugin first (e.g., in CApplication::initPlugin)
		 * // CGUIParticle::registerPlugin();
		 * 
		 * // Then create a GUI particle element via factory
		 * CGUIParticle *guiParticle = (CGUIParticle*)CGUIFactory::getInstance()->createGUI("CGUIParticle", parent);
		 * guiParticle->setParticle("Particles/Magic.particle", true);
		 * guiParticle->playParticle();
		 * @endcode
		 */
		class CGUIParticle : public CGUIElement
		{
		protected:
			/**
			 * @brief The source path to the .particle file.
			 */
			std::string m_source;

			/**
			 * @brief Whether the particle system plays automatically upon loading.
			 */
			bool m_autoPlay;

			/**
			 * @brief Scaling factor for the 3D particle system in 2D UI space.
			 */
			float m_particleScale;

			/**
			 * @brief Depth value (Z) for rendering the particle system relative to the UI.
			 */
			float m_depthZ;

			/**
			 * @brief The underlying particle component instance.
			 */
			Particle::CParticleComponent* m_ps;

			/**
			 * @brief Cached up vector for billboarding.
			 */
			core::vector3df m_billboardUp;

			/**
			 * @brief Cached look vector for billboarding.
			 */
			core::vector3df m_billboardLook;

		public:
			/**
			 * @brief Registers the CGUIParticle class to the CGUIFactory.
			 */
			static void registerPlugin();

			/**
			 * @brief Constructor.
			 * @param canvas The parent canvas.
			 * @param parent The parent GUI element.
			 */
			CGUIParticle(CCanvas* canvas, CGUIElement* parent);

			/**
			 * @brief Constructor with a predefined rectangle.
			 * @param canvas The parent canvas.
			 * @param parent The parent GUI element.
			 * @param rect The element's rectangle.
			 */
			CGUIParticle(CCanvas* canvas, CGUIElement* parent, const core::rectf& rect);

			virtual ~CGUIParticle();

			/**
			 * @brief Updates the particle system animations.
			 * @param camera The current rendering camera.
			 */
			virtual void update(CCamera* camera);

			/**
			 * @brief Renders the particle system into the 2D canvas.
			 * @param camera The current rendering camera.
			 */
			virtual void render(CCamera* camera);

			/**
			 * @brief Creates a serializable object for property editing.
			 * @return A new CObjectSerializable instance.
			 */
			virtual CObjectSerializable* createSerializable();

			/**
			 * @brief Loads properties from a serializable object.
			 * @param object The serializable data.
			 */
			virtual void loadSerializable(CObjectSerializable* object);

			/**
			 * @brief Sets the particle system source file.
			 * @param source Path to the .particle file.
			 * @param autoPlay If true, starts playing immediately.
			 */
			void setParticle(const char* source, bool autoPlay);

			/**
			 * @brief Starts playback of the particle system.
			 */
			void playParticle();

			/**
			 * @brief Stops playback and clears existing particles.
			 */
			void stopParticle();

			/**
			 * @brief Reloads the particle system from the current source path.
			 */
			void reload();

			/**
			 * @brief Returns the underlying particle component.
			 * @return Pointer to CParticleComponent.
			 */
			inline Particle::CParticleComponent* getParticle()
			{
				return m_ps;
			}

			DECLARE_GETTYPENAME(CGUIParticle)

		protected:

			/**
			 * @brief Renders a specific particle group.
			 * @param g The particle group to render.
			 */
			void renderParticleBuffer(Particle::CGroup* g);

		};
	}
}