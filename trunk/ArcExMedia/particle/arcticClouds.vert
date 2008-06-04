//cloudsMoveTime

uniform float cloudsMoveTime; // scaled to [0,1]

void main()
{
	gl_TexCoord[0].s = gl_MultiTexCoord0.s + cloudsMoveTime;
	gl_TexCoord[0].t = gl_MultiTexCoord0.t;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
