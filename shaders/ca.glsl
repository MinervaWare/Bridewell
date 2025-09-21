/* 
 * Copyright (c) 2025 MinervaWare LLC
 * Licensed under the Apache License, Version 2.0. See LICENSE.
 * Chromatic Aberration Shader
 */


@ctype mat4 mat4x4
@ctype vec3 vec3
@ctype vec4 vec4
@ctype vec2 vec2

@vs vs_ca

in vec2 ca_position;
in vec2 ca_texcoord;
out vec2 uv;

void main() {
	uv = ca_texcoord;
	gl_Position = vec4(ca_position,0.0,1.0);
}

@end

@fs fs_ca

layout(binding=0) uniform texture2D ca_texture;
layout(binding=0) uniform sampler ca_smp;

layout(binding=1) uniform ca_params {
	float startDist;
	float endDist;
	float strength;
};

in vec2 uv;
out vec4 frag_color;

void main() {
	float z = texture(sampler2D(ca_texture, ca_smp), uv).a;
	float f = clamp((z-startDist)/max(endDist-startDist,1e-6),0.0,1.0);

	vec3 col = texture(sampler2D(ca_texture, ca_smp), uv).rgb;

	vec2 offsetDir = uv-vec2(0.5);
	float radius = length(offsetDir);

	vec2 dir = (radius>=0) ? (offsetDir/radius) : vec2(0.0);
	float mag = strength * f * radius;

	vec2 offs = dir*mag;
	float r = texture(sampler2D(ca_texture, ca_smp),uv+offs).r;
	float g = col.g;
	float b = texture(sampler2D(ca_texture, ca_smp),uv-offs).b;

	frag_color = vec4(r,g,b,1.0);
}

@end

@program ca vs_ca fs_ca
