uniform sampler2D skyColour;
uniform sampler2D atmosphereDensity;
uniform float sunHeightRel;

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
    gl_FragColor = colour;
}
