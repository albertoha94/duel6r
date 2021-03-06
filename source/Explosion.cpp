/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Util.h"
#include "Explosion.h"

namespace Duel6
{
	ExplosionList::ExplosionList(const GameResources& resources, Float32 speed)
		: textures(resources.getExplosionTextures()), speed(speed)
	{
	}

	void ExplosionList::update(Float32 elapsedTime)
	{
		auto explIter = explosions.begin();
		while (explIter != explosions.end())
		{
			explIter->now += speed * elapsedTime;
			if (explIter->now > explIter->max)
			{
				explIter = explosions.erase(explIter);
			}
			else
			{
				++explIter;
			}
		}
	}

	void ExplosionList::render() const
	{
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GEQUAL, 1);

		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, textures.at(0).getId());
		glBegin(GL_QUADS);

		for (const Explosion& explosion : explosions)
		{
			glColor3ub(explosion.color.getRed(), explosion.color.getGreen(), explosion.color.getBlue());
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(explosion.centre.x - explosion.now, explosion.centre.y + explosion.now, 0.6f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(explosion.centre.x + explosion.now, explosion.centre.y + explosion.now, 0.6f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(explosion.centre.x + explosion.now, explosion.centre.y - explosion.now, 0.6f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(explosion.centre.x - explosion.now, explosion.centre.y - explosion.now, 0.6f);
		}

		glEnd();
		glColor3f(1, 1, 1);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);
	}

	void ExplosionList::add(const Vector& centre, Float32 startSize, Float32 maxSize, const Color& color)
	{
		Explosion explosion;
		explosion.centre = centre;
		explosion.now = startSize;
		explosion.max = maxSize;
		explosion.color = color;
		explosions.push_back(explosion);
	}
}