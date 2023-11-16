#version 330 core

out vec4 fragColor;
  
in vec2 fragCoord; 

uniform float iTime;
uniform vec2 iResolution;
uniform vec2 iMouse;

void main()
{
    vec2 mouse = (2.*iMouse.xy-iResolution.xy)/iResolution.y;
    vec2 p = fragCoord;
    
    mouse = vec2(cos(iTime*.15-1.4), sin(iTime*.15-1.4))*.98;
    
    vec3 colj;
    vec2 zj = p;
    vec2 cj = mouse - vec2(.55, .0);
    
    for (float iter = 0.; iter < 200.; iter++) 
    {
        zj = vec2(zj.x*zj.x - zj.y*zj.y, 2.*zj.x*zj.y) + cj;
        
        if (dot(zj,zj) > 40.) 
        {
            float smooth_iter = iter + 2. - log(log(dot(zj,zj)))/log(2.);
            colj = clamp(cos(vec3(1.1,1.2,1.3) * pow(smooth_iter*2., .5)), 0., 1.);
            zj = vec2(0.); cj = vec2(0.);
        }
        
        if(dot(cj, cj) == 0.) 
        {
            break;
        }

        mouse = vec2(cos(iTime*.15-1.4), sin(iTime*.15-1.4))*.98;
    }
        
    fragColor = vec4(colj, 1.0);
    vec2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
}
