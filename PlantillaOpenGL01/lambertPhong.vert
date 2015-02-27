varying vec4 cLightDiff, cLightAmbt, cLightSpec;
varying vec4 cMatDiff, cMatAmbt, cMatSpec;
varying float cMatShin;
varying vec3 camDirection;
varying vec3 N;
varying vec4 L;

void main (void)
{
	vec4 V;

	V = gl_ModelViewMatrix * gl_Vertex; // Posicion de vector actual.
	N = normalize(gl_NormalMatrix * gl_Normal); // Direccion de la normal de vector actual. (Normal)
	L = normalize(gl_LightSource[0].position-V); // Direccion del vector de la luz al vector actual. (Incidencia)

	// Se consiguen los vectores del color de la luz.
	cLightAmbt = gl_LightSource[0].ambient;
	cLightDiff = gl_LightSource[0].diffuse;
	cLightSpec = gl_LightSource[0].specular;

	// Se consiguen los vectores del color del material.
	cMatAmbt = gl_FrontMaterial.ambient;
	cMatDiff = gl_FrontMaterial.diffuse;
	cMatSpec = gl_FrontMaterial.specular;
	cMatShin = gl_FrontMaterial.shininess;

	// La direccion de la camara es la opuesta a la del vector actual.
	camDirection = -V.xyz;

	// Es una variable de salida(out) que debe ser inicializada.
   	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}