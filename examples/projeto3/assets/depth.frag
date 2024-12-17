#version 330 core

in vec3 fragColor; // Cor recebida do vertex shader
out vec4 outColor;

void main() {
  outColor = vec4(fragColor, 1.0); // Define a cor final
}