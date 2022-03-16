#vertex shader
#version 430 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

uniform mat4 view;
uniform mat4 projection;

out vec3 pos;
out vec3 normal;

void main() {
	gl_Position = vec4(vPos, 1) * view * projection;
	pos = vPos;
	normal = vNormal;
}



#fragment shader
#version 430 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float intensity;
};

uniform vec3 camPos;

uniform Material material;
uniform Light light;

in vec3 pos;
in vec3 normal;
out vec4 oColor;

void main() {
	//Ambient color
	vec3 ambient = material.ambient * light.ambient;

	//Diffuse color
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(-light.direction);

	float diff = max(dot(lightDir, norm), 0.0f);
	vec3 diffuse = diff * material.diffuse * light.diffuse;

	//Specular color
	vec3 viewDir = normalize(camPos - pos);
	vec3 reflectDir = normalize(reflect(-lightDir, norm));
	vec3 specular = material.specular * pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess) * light.specular;

	float att = light.intensity;

	float n = dot(norm, vec3(0, 1, 0));

	if (pos.y < 55.0f) {
		if (n >= 0.85f) {
			oColor = vec4(0, 1, 0, 1);
		}
		else if (n < 0.85f && n >= 0.75f) {
			oColor = vec4(0, 0, 0, 0);
			oColor += vec4(vec3(1, 1, 0) * (0.85f - n) * 10.0f, 0.5f);
			oColor += vec4(vec3(0, 1, 0) * (n - 0.75f) * 10.0f, 0.5f);
		}
		else {
			oColor = vec4(1, 1, 0, 1);
		}
		

		if (pos.y >= 35) {
			vec4 color;
			color = oColor * ((55.0f - pos.y) / 20.0f);
			color += vec4(1,1,1,1) * ((pos.y - 35.0f) / 20.0f);

			oColor = color;
		}

		//Output color
		oColor.w = 1.0f;
		oColor = vec4(ambient + vec3(diffuse + specular) * att, 1.0f) * oColor;
	}
	else {
		oColor = vec4(1, 1, 1, 1) * vec4(ambient + vec3(diffuse + specular) * att, 1.0f);
	}

	
	

}