#version 330

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aVelocity;
layout (location = 2) in int aAge;
layout (location = 3) in int aType;

out vec3 Position0;
out vec3 Velocity0;
out int Age0;
out int Type0;

void main()
{
    Position0 = aPosition;
    Velocity0 = aVelocity;
    Age0 = aAge;
    Type0 = aType;
}