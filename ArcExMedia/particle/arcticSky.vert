
uniform vec3 sunPosition;
uniform vec3 camPosition;

varying vec3 sunDirection;
varying vec3 vertDirection;

void main()
{
	sunDirection = sunPosition - camPosition;
	vertDirection = gl_Vertex.xyz - camPosition;
	

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
