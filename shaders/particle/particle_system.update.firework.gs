#version 330

layout(points) in;
layout(points) out;
layout(max_vertices = 50) out;

in vec3 Position0[];
in vec3 Velocity0[];
in int Age0[];
in int Type0[];

out vec3 Position1;
out vec3 Velocity1;
out int Age1;
out int Type1;

uniform float gDeltaTime;
uniform int gTime;
uniform int gLauncherLifetime;
uniform int gShellLifetime;
uniform int gSecondaryShellLifetime;
uniform vec3 gPositionOriginal;
uniform vec3 gVelocityOriginal;
uniform vec3 gGrativityAccel;
uniform sampler1D gRandomTexture;

const int NumNoiseSample = 137;

#define PARTICLE_TYPE_LAUNCHER 0
#define PARTICLE_TYPE_SHELL 1
#define PARTICLE_TYPE_SECONDARY_SHELL 2

vec3 GetRandomDir(float TexCoord)
{
    vec3 Dir = texture(gRandomTexture, TexCoord).xyz;
    Dir -= vec3(0.5, 0.5, 0.5);
    return Dir;
}

void main()
{
    int Age = Age0[0] + 1;

    if (Type0[0] == PARTICLE_TYPE_LAUNCHER)
    {
        if (Age >= gLauncherLifetime)
        {
            float coord = (gTime % NumNoiseSample) * 0.001;
            vec3 Dir = GetRandomDir(coord);
            Dir.y = max(Dir.y, 0.5);

            Position1 = gPositionOriginal;
            Velocity1 = gVelocityOriginal + normalize(Dir);
            Age1 = 0;
            Type1 = PARTICLE_TYPE_SHELL;

            EmitVertex();
            EndPrimitive();

            Age = 0;
        }

        Position1 = gPositionOriginal;
        Velocity1 = gVelocityOriginal;
        Age1 = Age;
        Type1 = PARTICLE_TYPE_LAUNCHER;

        EmitVertex();
        EndPrimitive();
    }
    else
    {
        float DeltaTimeSecs = gDeltaTime;
        vec3 DeltaP = DeltaTimeSecs * Velocity0[0];
        vec3 DeltaV = gGrativityAccel * DeltaTimeSecs;

        if (Type0[0] == PARTICLE_TYPE_SHELL)
        {
            if (Age < gShellLifetime)
            {
                Position1 = Position0[0] + DeltaP;
                Velocity1 = Velocity0[0] + DeltaV;
                Age1 = Age;
                Type1 = PARTICLE_TYPE_SHELL;

                EmitVertex();
                EndPrimitive();
            }
            else
            {
                for (int i = 0 ; i < 10 ; i++)
                {
                    float coord = ((gTime + i * i) % NumNoiseSample) * 0.001;
                    vec3 Dir = GetRandomDir(coord);

                    Position1 = Position0[0];
                    Velocity1 = normalize(Dir);
                    Age1 = 0;
                    Type1 = PARTICLE_TYPE_SECONDARY_SHELL;

                    EmitVertex();
                    EndPrimitive();
                }
            }
        }
        else
        {
            if (Age < gSecondaryShellLifetime)
            {
                Position1 = Position0[0] + DeltaP;
                Velocity1 = Velocity0[0] + DeltaV;
                Age1 = Age;
                Type1 = PARTICLE_TYPE_SECONDARY_SHELL;

                EmitVertex();
                EndPrimitive();
            }
        }
    }
}