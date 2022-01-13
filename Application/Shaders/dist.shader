#vertex shader
#version 430 core

layout(location = 0) in vec3 vPos;

uniform int i;

void main() {
	if (i == 0) {
		gl_Position = vec4(vPos, 1);
	}
	else {
		gl_Position = vec4(vPos.x * 2.0f - 1.0f, vPos.y * 2.0f - 1.0f, vPos.z, 1);
	}
	
}



#fragment shader
#version 430 core

uniform vec3 color;
out vec4 o_color;

void main() {
	o_color = vec4(color, 1);
}