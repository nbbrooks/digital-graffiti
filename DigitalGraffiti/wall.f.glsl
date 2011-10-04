#version 110

uniform float fade_factor;
//uniform sampler2D textures[2];
uniform sampler2D textures[1];

varying vec2 texcoord;

void main()
{
    //gl_FragColor = mix(
    //    texture2D(textures[0], texcoord),
    //    texture2D(textures[1], texcoord),
    //    fade_factor
    //);
    gl_FragColor = texture2D(textures[0], texcoord);
}
