#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor; // Entrada de cor

out vec3 fragColor; // Cor enviada ao fragment shader

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
  fragColor = inColor; // Passa a cor para o fragment shader
  gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(inPosition, 1.0);
}
