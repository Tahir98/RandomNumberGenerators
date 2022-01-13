#include "VertexBuffer.h"
#include "glad/glad.h"
#include "Log/Log.h"


namespace OpenGL {
	VertexBuffer::VertexBuffer(const float* vertices, const unsigned int size, const unsigned int usage) {
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

		count = size / 4;

		APP_LOG_INFO("Vertex buffer created id:{0}", id);
	}

	VertexBuffer::~VertexBuffer() {
		APP_LOG_INFO("Vertex buffer deleted id:{0}", id);
		glDeleteBuffers(1, &id);
	}

	void VertexBuffer::subData(const unsigned int index, const unsigned int size, const float* vertices) {
		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferSubData(GL_ARRAY_BUFFER, index, size, vertices);
	}

	void VertexBuffer::bind() {
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}

	void VertexBuffer::unbind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	unsigned int VertexBuffer::getCount() {
		return count;
	}

}
