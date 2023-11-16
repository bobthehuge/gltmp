#version 330 core

out vec4 fragColor;
  
in vec2 fragCoord; 

uniform float iTime;
uniform vec2 iResolution;
uniform vec2 iMouse;

/* vec3 palette( float t ) */ 
/* { */
/*     vec3 a = vec3(0.5, 0.5, 0.5); */
/*     vec3 b = vec3(0.5, 0.5, 0.5); */
/*     vec3 c = vec3(1.0, 1.0, 1.0); */
/*     vec3 d = vec3(0.263,0.416,0.557); */

/*     return a + b*cos( 6.28318*(c*t+d) ); */
/* } */

void main()
{
	/* vec2 uv = fragCoord; */
    /* vec2 uv0 = uv; */
    /* vec3 finalColor = vec3(0.0); */
    
    /* for (float i = 0.0; i < 4.0; i++) { */
        /* uv = fract(uv * 1.5) - 0.5; */

        /* float d = length(uv) * exp(-length(uv0)); */

        /* vec3 col = palette(length(uv0) + i*.4 + iTime*.4); */

        /* d = sin(d*8. + iTime)/8.; */
        /* d = abs(d); */

        /* d = pow(0.01 / d, 1.2); */

        /* finalColor += col * d; */
    /* } */

    vec2 mouse = (2.*iMouse.xy-iResolution.xy)/iResolution.y;
	vec2 p = fragCoord;
    
    mouse = vec2(cos(iTime*.15-1.4), sin(iTime*.15-1.4))*.98; // default mouse animation
    
    vec3 colj;
    vec2 zj = p;
    vec2 cj = mouse - vec2(.55, .0); // julia iteration point
    
    for (float iter = 0.; iter < 200.; iter++) 
	{
        zj = vec2(zj.x*zj.x - zj.y*zj.y, 2.*zj.x*zj.y) + cj; // julia iteration
        
        if (dot(zj,zj) > 40.) 
		{
            float smooth_iter = iter + 2. - log(log(dot(zj,zj)))/log(2.);
            colj = clamp(cos(vec3(1.1,1.2,1.3) * pow(smooth_iter*2., .5)), 0., 1.);
            zj = vec2(0.); cj = vec2(0.);
        }
        
        if(dot(cj, cj) == 0.) break; // break if both mandelbrot & julia stopped

		mouse = vec2(cos(iTime*.15-1.4), sin(iTime*.15-1.4))*.98; // default mouse animation
    }
        
    fragColor = vec4(colj, 1.0);        
}
