#version 330 core
in vec2 texCoords;
uniform sampler2D tex;
uniform sampler2DMS bg;
out vec4 fragColor;
uniform bool ms;
uniform int aaSamples;

vec4 texMultisample(sampler2DMS t, vec2 coord)
{
	ivec2 texSize = textureSize(t);
	ivec2 texCoord = ivec2(coord * texSize);
	vec4 color = vec4(0);
	for(int i = 0; i < aaSamples; ++i){
		color += texelFetch(t, texCoord, i);
	}
	color /= float(aaSamples);
	return color;
}

void main()
{
	if(ms) fragColor = texMultisample(bg, texCoords);
	else fragColor = texture(tex, texCoords);
	if(fragColor.a < 0.2) discard;
}
