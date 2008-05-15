
uniform float timeOfDay;
uniform vec3 sunPosition;
uniform vec3 camPosition;

varying vec2 skyTexCoords;

#define PI 3.14159265

void main()
{

	skyTexCoords.s = timeOfDay;

	vec3 sunDirection = normalize(sunPosition - camPosition);
	vec3 vertDirection = normalize(gl_Vertex.xyz - camPosition);
	
	skyTexCoords.t = acos(dot(vertDirection, sunDirection))/PI;


	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
