#include "./core/graphics/render_device.h"
#include "./core/graphics/shader.h"

// External
#include <GL/glew.h>

namespace CRT
{
	RenderDevice::RenderDevice(Window* _window)
	{
		glewInit();

		// Setup RenderTarget
		glGenTextures(1, &m_RenderTargetID);
		glBindTexture(GL_TEXTURE_2D, m_RenderTargetID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _window->GetWidth(), _window->GetHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Create shader
		m_Shader = new Shader("./assets/vertex_shader.glsl", "./assets/pixel_shader.glsl");
	}

	RenderDevice::~RenderDevice()
	{
		delete m_Shader;
		glDeleteTextures(1, &m_RenderTargetID);
	}

	void RenderDevice::CopyFrom(Surface* _surface)
	{
		glBindTexture(GL_TEXTURE_2D, m_RenderTargetID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _surface->GetWidth(), _surface->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _surface->GetBuffer());
	}

	void RenderDevice::Present()
	{
		m_Shader->Bind();
		m_Shader->SetInputTexture(0, m_RenderTargetID, "c");

		static GLuint vao = 0;
		if (!vao)
		{
			// generate buffers
			static const GLfloat verts[] = { -1, -1, 0, 1, -1, 0, -1, 1, 0, 1, -1, 0, -1, 1, 0, 1, 1, 0 };
			static const GLfloat uvdata[] = { 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0 };
			
			uint32_t vertexBuffer;
			uint32_t UVBuffer;
			{
				glGenBuffers(1, &vertexBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
			}
			{
				glGenBuffers(1, &UVBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
				glBufferData(GL_ARRAY_BUFFER, sizeof(uvdata), uvdata, GL_STATIC_DRAW);
			}
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindVertexArray(0);
		}
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}