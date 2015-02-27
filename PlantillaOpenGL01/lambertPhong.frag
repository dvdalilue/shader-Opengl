varying vec4 cLightDiff, cLightAmbt, cLightSpec;
varying vec4 cMatDiff, cMatAmbt, cMatSpec;
varying float cMatShin;
varying vec3 camDirection;
varying vec3 N;
varying vec4 L; // vector de incidencia

void main (void)  
{     
   
   vec4 cFinal = vec4(0.0,0.0,0.0,1.0);
   float iDiff, iSpec;
   vec3 vRef;

   //Componente Specular Phong
   vRef = -normalize(reflect(L.xyz,N));
   iSpec = pow(max(dot(vRef, normalize(camDirection)), 0.0),10.0);

   // Ln   = normalize(L.xyz);
   // V    = -normalize(camDirection);

   // float ndotl = max(0, dot(Ln, N));

   // t = dot(N, Ln); t = t/(dot(N, Ln) + dot(N, V));

   // iSpec = pow(ndotl, 0.5); // 1.0 - k); 

   //Componente difuso.
   //iDiff = max(dot(normalize(N),normalize(L.xyz)), 0.0) ;
   iDiff = max(t, 0.0);

   //Componente difuso.
   iDiff = max(dot(normalize(N),normalize(L.xyz)), 0.0) ;

   cFinal = vec4(10.0,0.0,0.0,1.0)*cLightAmb*cMatAmb + iDiff*(cLightDiff*cMatDiff) + iSpec*(cLightSpec*cMatSpec);
   
   cFinal.w = 1.0;
   gl_FragColor = cFinal;

}    
