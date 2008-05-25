
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
	
	
	/*
	
	gl_MultiTexCoord0.s
	gl_MultiTexCoord0.t
	
	t/s
	
	
	gl_TexCoord[0].s = gl_MultiTexCoord0.s;
	gl_TexCoord[0].t = gl_MultiTexCoord0.t;
	

	
	float s = gl_MultiTexCoord0.s;
	float t = gl_MultiTexCoord0.t;
	
	gl_TexCoord[0].s = gl_MultiTexCoord0.s * cos(angle)   -   gl_MultiTexCoord0.t * sin(angle);
	gl_TexCoord[0].t = gl_MultiTexCoord0.s * sin(angle)   +   gl_MultiTexCoord0.t * cos(angle);
	*/

	float angle = starMoveTime * PI * 2.0;

	
//	gl_TexCoord[0].s = gl_MultiTexCoord0.s * cos(angle);
	gl_TexCoord[0].s = gl_MultiTexCoord0.s + starMoveTime;
	gl_TexCoord[0].t = gl_MultiTexCoord0.t;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
