#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
//uniform float ZBuffer[512]; // By defauly our screen size is 1024, so half of it

// Parameters to map the floor row
uniform int screenWidth;

uniform vec3 spriteColor;



void main()
{    

    //int x = int(gl_FragCoord.x)
    //float z = ZBuffer[x]

    // GL_REPEAT willt tile the texture, so dont need to fract()
    color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
} 