
uniform vec3 sunPosition;
uniform vec3 camPosition;
uniform float starMoveTime; // scaled to [0,1]

varying vec3 sunDirection;
varying vec3 vertDirection;

void main()
{
	sunDirection = sunPosition - camPosition;
	vertDirection = gl_Vertex.xyz - camPosition;
	
	gl_TexCoord[0] = gl_MultiTexCoord0 - starMoveTime;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
