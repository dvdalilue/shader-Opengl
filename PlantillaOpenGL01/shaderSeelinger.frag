varying vec4 cLightDiff, cLightAmbt, cLightSpec;
varying vec4 cMatDiff, cMatAmbt, cMatSpec;
varying float cMatShin;
varying vec3 camDirection;
varying vec3 N, Nn;
varying vec4 L; // vector de incidencia

uniform float k;
uniform float specular;

//especular
float seelinger() 
{
   float tmp, rV;
   vec3 Ln, V;
   
   Ln = normalize(L.xyz);
   V  = normalize(camDirection);

   tmp = max(0.0, dot(Ln, Nn));
   rV = max(0.0, dot(V , Nn));

   tmp = tmp/(tmp + rV);

   return tmp;
}

float minnaert(float l)
{
    float ndotl, ndotv,
          tmp;
    vec3 Ln, V;

    Ln = normalize(L.xyz);
    V  = normalize(camDirection);

    ndotl = max(0.0, dot(Ln, Nn));
    ndotv = abs(dot(V , Nn));

    tmp = pow(ndotl*ndotv, 1.0 - l);

    return tmp;
}

float fresnel(vec3 normal, vec3 light, float indexR)
{
    return 0.0;
}

float cookTorrance()
{
    return 0.0;
}

void main (void)  
{
    vec4 cFinal;
    float iDiff, iSpec;

    iSpec = minnaert(k);
 
    iDiff = seelinger();

    cFinal = (cLightAmbt*cMatAmbt) + iDiff*(cLightDiff*cMatDiff) + specular*iSpec*(cLightSpec*cMatSpec);
    cFinal.w = 1.0;

    gl_FragColor = cFinal;
}
