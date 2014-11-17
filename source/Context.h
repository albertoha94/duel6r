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

#ifndef DUEL6_CONTEXT_H
#define DUEL6_CONTEXT_H

#include <stack>
#include <SDL2/SDL_keycode.h>
#include "Type.h"

namespace Duel6
{
	class Context
	{
	private:
		static std::stack<Context*> contextStack;

	public:
		virtual ~Context()
		{}

		static bool exists()
		{
			return !contextStack.empty();
		}

		static Context& getCurrent()
		{
			return *getTopContext();
		}

		virtual bool isCurrent() const final
		{
			return (this == getTopContext());
		}

		virtual bool is(const Context& context) const final
		{
			return (this == &context);
		}

		virtual void keyEvent(SDL_Keycode keyCode, Uint16 keyModifiers) = 0;
		virtual void textInputEvent(const char* text) = 0;
		virtual void update(Float32 elapsedTime) = 0;
		virtual void render() const = 0;

		static void push(Context& context)
		{
			Context* lastContext = getTopContext();
			if (lastContext != nullptr)
			{
				lastContext->beforeClose(&context);
			}

			contextStack.push(&context);
			context.beforeStart(lastContext);
		}

		static void pop()
		{			
			if (exists())
			{
				Context& currentContext = getCurrent();
				contextStack.pop();
				Context* prevContext = getTopContext();
				currentContext.beforeClose(prevContext);

				if (prevContext != nullptr)
				{
					prevContext->beforeStart(&currentContext);
				}
			}
		}

	protected:
		virtual void beforeStart(Context* prevContext) = 0;
		virtual void beforeClose(Context* nextContext) = 0;

		static Context* getTopContext()
		{
			if (contextStack.size() > 0)
			{
				return contextStack.top();
			}

			return nullptr;
		}
	};
}

#endif