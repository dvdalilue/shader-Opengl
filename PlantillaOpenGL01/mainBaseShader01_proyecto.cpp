#include <conio.h>
#include <stdlib.h>
#include <iostream>
#include <GL\glew.h>
#include <GL\freeglut.h>
// assimp include files. These three are usually needed.
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include "glsl.h"

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

using namespace std;

// the global Assimp scene object
const aiScene* scene = NULL;
GLuint scene_list = 0;
aiVector3D scene_min,
		   scene_max,
		   scene_center;


cwc::glShaderManager SM;
cwc::glShader *shader;

GLfloat posLX;
GLfloat posLZ;

GLuint ProgramObject;
GLint locKMinnaert, locSpecular;
GLfloat valorSpecular = 0.0f;

void ejesCoordenada()
{
	
	glLineWidth(2.5);
	glBegin(GL_LINES);
		glColor3f(1.0,0.0,0.0);
		glVertex2f(0,10);
		glVertex2f(0,-10);
		glColor3f(0.0,0.0,1.0);
		glVertex2f(10,0);
		glVertex2f(-10,0);
	glEnd();

	glLineWidth(1.5);
	int i;
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINES);
		for(i = -10; i <=10; i++){
			if (i!=0) {		
				if ((i%2)==0){	
					glVertex2f(i,0.4);
					glVertex2f(i,-0.4);

					glVertex2f(0.4,i);
					glVertex2f(-0.4,i);
				}else{
					glVertex2f(i,0.2);
					glVertex2f(i,-0.2);

					glVertex2f(0.2,i);
					glVertex2f(-0.2,i);

				}
			}
		}
		
	glEnd();

	glLineWidth(1.0);
}

void changeViewport(int w, int h)
{
	
	float aspectratio;

	if (h==0)
		h=1;

	
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(30, (GLfloat) w/(GLfloat) h, 1.0, 200.0);
   glMatrixMode (GL_MODELVIEW);
}

void init()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
   
   	shader = SM.loadfromFile("shaderSeelinger.vert","shaderSeelinger.frag"); // load (and compile, link) from file
   	if (shader==0) 
		std::cout << "Error Loading, compiling or linking shader\n";
	else
      {
         ProgramObject = shader->GetProgramObject();
		 locKMinnaert = glGetUniformLocation(ProgramObject, "k");
		 locSpecular = glGetUniformLocation(ProgramObject, "specular");
         
		 if (locKMinnaert == -1){
			 std::cout << "Warning: can't find uniform variable k!\n";
		 }else{
			//glUniform1f(locSpecular, 0.0f);
		 };

         if (locSpecular == -1){
            std::cout << "Warning: can't find uniform variable specular!\n";
		 }else{
			//glUniform1f(locKMinnaert, 0.5f);
		 }
      }
	
	

	posLX = 10.0;
	posLZ = 10.0;
}

void get_bounding_box_for_node (const aiNode* nd, aiVector3D* min, aiVector3D* max, aiMatrix4x4* trafo)
{
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	prev = *trafo;
	aiMultiplyMatrix4(trafo,&nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp,trafo);

			min->x = aisgl_min(min->x,tmp.x);
			min->y = aisgl_min(min->y,tmp.y);
			min->z = aisgl_min(min->z,tmp.z);

			max->x = aisgl_max(max->x,tmp.x);
			max->y = aisgl_max(max->y,tmp.y);
			max->z = aisgl_max(max->z,tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
	}
	*trafo = prev;
}

void get_bounding_box (aiVector3D* min, aiVector3D* max)
{
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);
	
	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene->mRootNode,min,max,&trafo);
}

int loadasset (const char* path)
{
	// we are taking one of the postprocessing presets to avoid
	// spelling out 20+ single postprocessing flags here.
	scene = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene) {
		get_bounding_box(&scene_min,&scene_max);
		scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
		scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
		scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
		return 0;
	}
	return 1;
}

void recursive_render (const aiScene *sc, const aiNode* nd)
{
	unsigned int i;
	unsigned int n = 0, t;
	aiMatrix4x4 m = nd->mTransformation;

	// update transform
	aiTransposeMatrix4(&m);
	glPushMatrix();
	glMultMatrixf((float*)&m);

	// draw all meshes assigned to this node
	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

		//apply_material(sc->mMaterials[mesh->mMaterialIndex]);

		if(mesh->mNormals == NULL) {
			glDisable(GL_LIGHTING);
		} else {
	glEnable(GL_LIGHTING);
		}

		for (t = 0; t < mesh->mNumFaces; ++t) {
			const aiFace* face = &mesh->mFaces[t];
			GLenum face_mode;

			switch(face->mNumIndices) {
				case 1: face_mode = GL_POINTS; break;
				case 2: face_mode = GL_LINES; break;
				case 3: face_mode = GL_TRIANGLES; break;
				default: face_mode = GL_POLYGON; break;
			}

			glBegin(face_mode);

			for(i = 0; i < face->mNumIndices; i++) {
				int index = face->mIndices[i];
				if(mesh->mColors[0] != NULL)
					glColor4fv((GLfloat*)&mesh->mColors[0][index]);
				if(mesh->mNormals != NULL) 
					glNormal3fv(&mesh->mNormals[index].x);
				glVertex3fv(&mesh->mVertices[index].x);
			}

			glEnd();
		}

	}

	// draw all children
	for (n = 0; n < nd->mNumChildren; ++n) {
		recursive_render(sc, nd->mChildren[n]);
	}

	glPopMatrix();
}

/**
 *
 * -- Seccion Modiicable -- /
 *
 */

void render()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat zExtent, xExtent, xLocal, zLocal;
    int loopX, loopZ;

	glLoadIdentity ();                       
	gluLookAt(10.0, 3.0, 5.0, 0.0, 1.5, 0.0, 0.0, 1.0, 0.0);
	

	// Luz y material
	GLfloat mat_diffuse[] = { 1.0, 0.7, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 0.5, 0.5, 1.0 };
	GLfloat mat_ambient[] = { 0.1, 0.1 ,0.1, 1.0 };
	GLfloat mat_shininess[] = { 1.0 };
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	

    GLfloat light_ambient[]  = { 0.2, 0.2, 0.2, 1.0    };
	GLfloat light_diffuse[]  = { 0.8, 0.8, 0.8, 1.0    };
	GLfloat light_specular[] = { 0.2, 0.2, 0.2, 0.8    };
	GLfloat light_position[] = { 10.0, 10.0, 10.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);   


	//Suaviza las lineas
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_LINE_SMOOTH );	

	
	glPushMatrix();

	if (shader) {
		shader->begin();
	};


	// COdigo para el mesh
	glEnable(GL_NORMALIZE);
	glTranslatef(0.0, -2.0, 0.0);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glScalef(30.0, 30.0, 30.0);
	if(scene_list == 0) {
	    scene_list = glGenLists(1);
	    glNewList(scene_list, GL_COMPILE);
            // now begin at the root node of the imported data and traverse
            // the scenegraph by multiplying subsequent local transforms
            // together on GL's matrix stack.
	    recursive_render(scene, scene->mRootNode);
	    glEndList();
	}
	glCallList(scene_list);
	
	glPopMatrix();
	
	
	glPushMatrix();
	glTranslatef(5.2, 3.25, 0.0);
	glutSolidSphere(0.4f,30,30);
	glPopMatrix();
	 

	if (shader) shader->end();

	glDisable(GL_BLEND);
	glDisable(GL_LINE_SMOOTH);

	glutSwapBuffers();
}

void imprimirEstado() {
	cout << "\n\n\n\n\n\n\n\n";
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	cout << "Estado de las variables\n";
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	cout << "[x++/x--]\n\n";
	cout << " Specular [1]: " <<  valorSpecular << "\n";
	cout << "Color Verde luz [3]: " << "ALGO" << "\n";
	cout << "Color Azul luz [4]: " <<  "ALGO" << "\n";
//	cout << "Reflexion [c]: " << ("BOOL" ? "On" : "Off") <<"\n";
//	cout << "Iluminacion [v]: " << (iluminacion ? "On" : "Off") << "\n";
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
}


void Keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
	case 27:             
		exit (0);
		break;
	case '1':
		
         ProgramObject = shader->GetProgramObject();
		
		 locSpecular = glGetUniformLocation(ProgramObject, "specular");
         
		glGetUniformfv(ProgramObject, locSpecular, &valorSpecular);
		 cout << valorSpecular << "<----- VALOR";
		if (valorSpecular  == 1.0) {
			glUniform1f(locSpecular, 0.0f);
		}else{
		   glUniform1f(locSpecular, 1.0f);
		}

	break;
  }
  imprimirEstado();
  glutPostRedisplay();
}

int main (int argc, char** argv)
{

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(960,540);

	glutCreateWindow("Shader - Opengl");

	// Codigo para cargar la geometria usando ASSIMP
	aiLogStream stream;
	// get a handle to the predefined STDOUT log stream and attach
	// it to the logging system. It remains active for all further
	// calls to aiImportFile(Ex) and aiApplyPostProcessing.
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
	aiAttachLogStream(&stream);

	// ... same procedure, but this stream now writes the
	// log messages to assimp_log.txt
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
	aiAttachLogStream(&stream);

	// the model name can be specified on the command line. If none
	// is specified, we try to locate one of the more expressive test 
	// models from the repository (/models-nonbsd may be missing in 
	// some distributions so we need a fallback from /models!).
	if( 0 != loadasset( argc >= 2 ? argv[1] : "dragon_vrip_res2.ply")) {
		if( argc != 1 || (0 != loadasset( "dragon_vrip_res2.ply") && 0 != loadasset( "dragon_vrip_res2.ply"))) { 
			return -1;
		}
	}

	init ();
	glutReshapeFunc(changeViewport);

	glutDisplayFunc(render);
	glutKeyboardFunc(Keyboard);
	
	glutMainLoop();
	return 0;
}
