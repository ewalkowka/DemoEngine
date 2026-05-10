#version 460 core

in vec3 vColor;

out vec4 FragColor;

uniform float uTime;

void main()
{
    float pulse = 0.5 + 0.5 * sin(uTime);

    FragColor = vec4(vColor * pulse, 1.0);
}