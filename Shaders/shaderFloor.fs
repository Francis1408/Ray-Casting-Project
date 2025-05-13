#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;

// Parameters to map the floor row
uniform vec2 floor;
uniform vec2 floorStep;
uniform int screenWidth;

uniform vec3 spriteColor;
uniform float offsetY;
uniform float offsetX;



void main()
{    
    // Assume TexCoords.x goes from 0.0 to 1.0 over the screen width
    // float pixelIndex = TexCoords.x * screenWidth;

    

    // float worldX = floor.x + pixelIndex * floorStep.x;
    // float worldY = floor.y + pixelIndex * floorStep.y;
    

    // // Adjusted texture lookup: only sample a vertical slice
    // vec2 newTexCoords = vec2(worldX, worldY);

    // GL_REPEAT willt tile the texture, so dont need to fract()
    color = vec4(spriteColor, 1.0) * texture(image, TexCoords);
}  