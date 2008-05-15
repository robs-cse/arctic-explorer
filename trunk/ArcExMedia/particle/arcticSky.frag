uniform sampler2D skyColour;
uniform sampler2D cloudsDay;
uniform sampler2D cloudsNight;

varying vec2 skyTexCoords;


void main (void)
{
/*
//   gl_FragColor = vec4(0.0,0.0,0.0,0.0);
   gl_FragColor = colour;
*/
/*

	
	vec2 skyST = skyTexCoords;

	if (skyST.t  <= 0.2)
	{
	skyST.t = 0.2; //yellow
	}
	else
	{
	skyST.t = 1.0; //blue
	}

//	skyST.t = 0.001; //blue

*/
/*
*/
	
//   vec4 colour = texture2D(skyColour, skyST);
//   vec4 colour = texture2D(skyColour, gl_TexCoord[0].st);
	
   vec4 colour = texture2D(skyColour, skyTexCoords.st);
   gl_FragColor = colour;
}
