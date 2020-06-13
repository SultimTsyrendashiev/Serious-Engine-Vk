#version 400

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 0) uniform sampler2D aTexture0;
layout (set = 1, binding = 0) uniform sampler2D aTexture1;

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec4 inTexCoord01;
layout (location = 2) in vec4 inTexCoord23;

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform ColorScale
{
   layout(offset = 64) float scale;
} colorScale;

void main()
{
   outColor = 
     texture(aTexture0, inTexCoord01.xy) * texture(aTexture1, inTexCoord01.zw) *
     inColor * colorScale.scale;
}