uniform sampler2D earthClouds;
uniform sampler2D skyColour;

uniform float sunHeightRel; //Scaled to [0,1]

varying vec3 sunPos;

varying vec3 sunDirection;
varying vec3 vertDirection;

#define PI 3.14159265


void main (void)
{
	vec2 skyTexCoords;

	vec3 normSunDirection = normalize(sunDirection);
	
	// Calculating what time of day it is (height of sun relative to its spinning axis)
	skyTexCoords.s = clamp(sunHeightRel, 0.0, 1.0);
	
	// Calculating angle between sun and fragment (with respect to viewer)
	skyTexCoords.t = acos(dot(normalize(vertDirection), normalize(sunDirection)))/PI;

    vec4 colour = texture2D(skyColour, skyTexCoords.st);
/*
	colour = colour + texture2D(starMap, gl_TexCoord[0].st) * (1.0 - min(colour.g + colour.r, 1.0)) *(0.7 - min(3.0*sunHeightRel, 0.7));
*/
    gl_FragColor = colour;
}
