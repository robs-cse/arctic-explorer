uniform sampler2D starMap;

uniform float sunHeightRelative; //Scaled to [0,1]

void main (void)
{
    vec4 colour = texture2D(starMap, gl_TexCoord[0].st);

    colour.a = smoothstep (0.1, 0.9, (colour.r + colour.g + colour.b)/2.0);
	colour = colour * (1.0 - smoothstep(0.0, 0.5, sunHeightRelative));


    gl_FragColor = colour;
}
