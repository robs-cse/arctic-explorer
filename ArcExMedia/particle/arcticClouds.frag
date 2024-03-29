uniform sampler2D earthClouds;

uniform float sunHeightRelative; //Scaled to [0,1]

void main (void)
{
    vec4 colour = texture2D(earthClouds, gl_TexCoord[0].st);

    colour.a = smoothstep(0.15, 1.0, (colour.r + colour.g + colour.b)/3.0);

	colour.rgb = colour.rgb * (smoothstep(0.0, 1.0, sunHeightRelative));

    gl_FragColor = colour;
}
