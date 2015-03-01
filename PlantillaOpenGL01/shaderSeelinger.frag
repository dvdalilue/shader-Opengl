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

float distro(vec3 n, vec3 h, float m)
{
    float ndoth,
          beta,
          tanbeta,
          tanbeta_over_m,
          D;

    ndoth = dot(n, h);
    beta = acos(ndoth);
    tanbeta = tan(beta);
    tanbeta_over_m = tanbeta/m;
    D = exp(-(tanbeta_over_m*tanbeta_over_m));

    D /= 4.0*m*m*pow(ndoth, 4.0);

    return D;
}

float geom(vec3 n, vec3 h, vec3 l, vec3 v)
{
    float ndoth,
          ndotv,
          ndotl,
          vdoth,
          masking,
          shadowing;

    ndoth = dot(n, h);
    ndotv = dot(n, v);
    ndotl = dot(n, l);
    vdoth = dot(v, h);

    masking   = (2.0*ndoth*ndotv)/vdoth;
    shadowing = (2.0*ndoth*ndotl)/vdoth;

    return min(1.0, min(masking, shadowing));
}

float fresnel(vec3 normal, vec3 light, float indexR)
{
   float R0 = pow((1.0 - indexR), 2.0);

   R0 /= pow((1.0 + indexR), 2.0);

   return R0 + (1.0 - R0)*pow(1.0 - dot(light, normal), 5.0);
}

float cookTorrance(float m)
{
    vec3 V, Nf, H, Ln;
    float cook, D, G, vdotn;

    Nf = normalize(N);
    Ln = normalize(L.xyz);
    V  = normalize(camDirection);

    if (dot(Nn, V) < 0.0) { Nf = -V; }

    H = normalize(Ln + V);
    D = distro(Nf, H, m);
    G = geom(Nf, H, Ln, V);

    cook  = D*G;
    vdotn = dot(V, Nf);

    cook /= vdotn;
    cook /= 3.1415;

    return cook;
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
