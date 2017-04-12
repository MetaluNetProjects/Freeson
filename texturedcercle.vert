//
precision highp float;
uniform mat4 modelViewProjectionMatrix;
attribute vec2 texcoord;
attribute vec4 position;

varying vec2 coord;

void main()
{
    coord = texcoord;

    gl_Position = modelViewProjectionMatrix * position;
}
