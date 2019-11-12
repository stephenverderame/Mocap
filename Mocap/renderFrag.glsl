#version 330 core
out vec4 fragColor;
in vec3 fragPos;
in vec3 normal;
const vec3 lightPos = vec3(0, 3, -4);
const vec3 viewPos = vec3(0, 1, -3);
const vec3 lightColor = vec3(0.9);
void main(){
	vec3 color = vec3(0.5, 0.5, 0.5);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
	vec3 specular = spec * lightColor;

	vec3 lighting = (color * 0.2) + diffuse + specular;
	fragColor = vec4(lighting, 1.0);
}