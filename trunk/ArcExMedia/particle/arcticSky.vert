
uniform vec3 sunPosition;
uniform vec3 camPosition;
uniform float starMoveTime; // scaled to [0,1]

varying vec3 sunDirection;
varying vec3 vertDirection;

#define PI 3.14159265

void main()
{
	sunDirection = sunPosition - camPosition;
	vertDirection = gl_Vertex.xyz - camPosition;
	
	float angle = starMoveTime * PI * 2.0;

	
//	gl_TexCoord[0].s = gl_MultiTexCoord0.s * cos(angle);
	gl_TexCoord[0].s = gl_MultiTexCoord0.s + starMoveTime;
	gl_TexCoord[0].t = gl_MultiTexCoord0.t;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
