#ifdef WIN32
#include <windows.h>
#endif

/*
  LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
  void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
  void DisableOpenGL(HWND, HDC, HGLRC);*/

/*
 *
 * Demonstrates how to load and display an Wavefront OBJ file.
 * Using triangles and normals as static object. No texture mapping.
 *
 * OBJ files must be triangulated!!!
 * Non triangulated objects wont work!
 * You can use Blender to triangulate
 *
 */

#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>


#include <math.h>
#include <time.h>

#define KEY_ESCAPE 27

//mode
#define BMP 1
#define COLOR_DEPTH 2
#define FPGA 3
#define SHADER 4
#define ROT 5
#define TRANS 6

float angle = 0;
float anglekey = 0;
float deltaangle = 1;
float rotationangle = 1;
float deltarayon = 1;
float rayon = 4;
float rayonkey = 4;
float eyeX = 4;
float eyeY = 0;
float eyeZ = 0;
float centerX = 0;
float centerY = 0;
float centerZ = 0;
char* objname = NULL;
float translationCamVector[3];
int scanMode = ROT;


/************************************************************************
  Window
************************************************************************/

//GlutWindow Parameters
int width=500;
int height=500;
char* title;
int frame=0, currenttime, timebase=0;
float fps = 0.0;

float field_of_view_angle;
float z_near;
float z_far;
float back_size=0;
bool process_enable = true;
bool rotate_on = false ;

// Chargement bmp
int mode=0;
int objtrue=0;
unsigned char* image;
unsigned char* depth;
unsigned char* color_depth;
int i;
int j;
int currentview;
float* profondeur;
int nbviews=1;
char filenameColor[50];
char filenameDepth[50];
char directoryName[50];
int cam=0;
int force_redraw=0;
int debug_cam=0;
GLuint texture_fond=0;
GLuint textureFPGA=0;

int go_fullscreen=0;
int background_2d=1;
bool bounce_on = false ;
bool forward = false ;
int bounced = 0 ;
unsigned int fpga_scale = 16;
unsigned int fpga_offset = 128;

//definition pour FPGA
int PAF3dOpen();
void PAF3dClose();
// Note input buffer size  and output buffer size should be rounded up to a multiple of page size
// Input buffer :  round_up<page_size>(width_image*height_image *4);
// Output buffer :  round_up<page_size>(4*width_image*height_image *4);
// /*******  allocation mémoire *********/
//    static uint32_t *mem;
//    static uint32_t *mem_out;
//    size_t nb_bytes = round_up<page_size>(width_image*height_image *4);
//    posix_memalign((void **)(&mem),page_size,nb_bytes);
//    posix_memalign((void **)(&mem_out),page_size,i4*nb_bytes);
void PAF3dGenerate(unsigned int width_image,unsigned int height_image, const char *inputBuffer, char *outputBuffer, unsigned int scale, unsigned int offset);	

char *fpga_in = NULL;
char *fpga_out = NULL;

/***************************************************************************
  Creation du fichier Bitmap
***************************************************************************/
void WriteHeader(FILE *bmpfile, unsigned int width , unsigned int height)
{
  int Type = 19778;                       //... 2 bytes
  int fSize = (width*height*1)+40+14;  //... 4 bytes
  int Reserved1 = 0;                      //... 2 bytes
  int Reserved2 = 0;                      //... 2 bytes
  int Offset = 40+14;                //... 4 bytes

  int hSize = 40;                         //... 4 bytes
  int nbPlane = 1;                        //... 2 bytes
  int nBPP = 24;                           //... 2 bytes
  int Compression = 0;                    //... 4 bytes
  int iSize = width*height*1;             //... 4 bytes
  int ResoX = 0;                          //... 4 bytes
  int ResoY = 0;                          //... 4 bytes
  int clrUsd = 256;                       //... 4 bytes
  int clrImp = 0;                         //... 4 bytes

  fwrite((char*)&Type, 2, 1, bmpfile);
  fwrite((char*)&fSize, 4, 1, bmpfile);
  fwrite((char*)&Reserved1, 2, 1, bmpfile);
  fwrite((char*)&Reserved2, 2, 1, bmpfile);
  fwrite((char*)&Offset, 4, 1, bmpfile);
  fwrite((char*)&hSize, 4, 1, bmpfile);
  fwrite((char*)&width, 4, 1, bmpfile);
  fwrite((char*)&height, 4, 1, bmpfile);
  fwrite((char*)&nbPlane, 2, 1, bmpfile);
  fwrite((char*)&nBPP, 2, 1, bmpfile);
  fwrite((char*)&Compression, 4, 1, bmpfile);
  fwrite((char*)&iSize, 4, 1, bmpfile);
  fwrite((char*)&ResoX, 4, 1, bmpfile);
  fwrite((char*)&ResoY, 4, 1, bmpfile);
  fwrite((char*)&clrUsd, 4, 1, bmpfile);
  fwrite((char*)&clrImp, 4, 1, bmpfile);
}


void CreateAndSaveBMP(const char *fileName, unsigned int width, unsigned int height, unsigned int nb_components, unsigned char *pixels)
{
  FILE *bmpfile;
  bmpfile = fopen(fileName, "wb");
  if(!bmpfile)
    {
      fprintf(stderr, "ERROR: FILE COULD NOT BE OPENED\n" );
      return ;
    }
  WriteHeader(bmpfile, width, height);

  if (nb_components==3) {
    unsigned int i, j;
    for(i=0; i<height; i++)
      {
	unsigned char *line = pixels + 3 * width * i;
	for(j=0; j<width; j++)
	  {
	    fputc(line[3*j + 2], bmpfile);
	    fputc(line[3*j + 1], bmpfile);
	    fputc(line[3*j], bmpfile);
	  }
      }
  } else {
    unsigned int i, j;
    for(i=0; i<height; i++)
      {
	unsigned char *line = pixels + width * i;
	for(j=0; j<width; j++)
	  {
	    fputc(line[j], bmpfile);
	    fputc(line[j], bmpfile);
	    fputc(line[j], bmpfile);
	  }
      }
  }

  fclose(bmpfile);
}

GLuint loadBmp(char *img_file)
{
    GLuint texture_id;
    unsigned int width, height;
  FILE *img = fopen(img_file, "r");
  if (img == NULL) return 0;

    int read ;
    fseek(img, 18, SEEK_SET);

    fread(&read, 4, 1, img);
    width = read ;

    fread(&read, 4, 1, img);
    height = read ;

    fseek(img, 54, SEEK_SET); // skip the BMP header

   char *data = (char *) malloc(sizeof(char) * 3 * width * height);
   fread(data, 1, 3 * width * height, img);
   fclose(img);


  glGenTextures(1, (GLuint *) &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

   free(data);

   return texture_id;
}


/***************************************************************************
  OBJ Loading
***************************************************************************/

typedef struct
{
public:

  float* normals;							// Stores the normals
  float* Faces_Triangles;					// Stores the triangles
  float* vertexBuffer;					// Stores the points which make the object
  long TotalConnectedPoints;				// Stores the total number of connected verteces
  long TotalConnectedTriangles;			// Stores the total number of connected triangles

  float centerObjX;
  float centerObjY;
  float centerObjZ;


} Model_OBJ;

void calculateNormal(float* coord1,float* coord2,float* coord3, float *nx, float *ny, float *nz );
int Model_OBJ_Load(Model_OBJ *obj, char *filename);	// Loads the model
void Model_OBJ_Draw(Model_OBJ *obj);					// Draws the model on the screen
void Model_OBJ_Release(Model_OBJ *obj);				// Release the model



#define POINTS_PER_VERTEX 3
#define TOTAL_FLOATS_IN_TRIANGLE 9

void calculateNormal(float *coord1, float *coord2, float *coord3, float *nx,  float *ny,float *nz  )
{
  /* calculate Vector1 and Vector2 */
  float va[3], vb[3], vr[3], val;
  va[0] = coord1[0] - coord2[0];
  va[1] = coord1[1] - coord2[1];
  va[2] = coord1[2] - coord2[2];

  vb[0] = coord1[0] - coord3[0];
  vb[1] = coord1[1] - coord3[1];
  vb[2] = coord1[2] - coord3[2];

  /* cross product */
  vr[0] = va[1] * vb[2] - vb[1] * va[2];
  vr[1] = vb[0] * va[2] - va[0] * vb[2];
  vr[2] = va[0] * vb[1] - vb[0] * va[1];

  /* normalization factor */
  val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );

  *nx = vr[0]/val;
  *ny = vr[1]/val;
  *nz = vr[2]/val;

}


int Model_OBJ_Load(Model_OBJ *obj, char* filename)
{
  char line[4096];
  FILE *objFile = fopen(filename, "rt");
  if (objFile)													// If obj file is open, continue
    {
      fseek(objFile, 0, SEEK_END);
      long fileSize = ftell(objFile);									// get file size
      fseek(objFile, 0, SEEK_SET);										// we'll use this to register memory for our 3d model

      obj->vertexBuffer = (float*) malloc (fileSize*sizeof(float));			// Allocate memory for the verteces
      obj->Faces_Triangles = (float*) malloc(fileSize*sizeof(float));			// Allocate memory for the triangles
      obj->normals  = (float*) malloc(fileSize*sizeof(float));					// Allocate memory for the normals

      int triangle_index = 0;												// Set triangle index to zero
      int normal_index = 0;												// Set normal index to zero

      while (! feof(objFile) )											// Start reading file data
	{
	  fgets(line, 4096, objFile);											// Get line from file

	  if (line[0] == 'v')										// The first character is a v: on this line is a vertex stored.
	    {
	      line[0] = ' ';												// Set first character to 0. This will allow us to use sscanf

	      sscanf(line,"%f %f %f ",							// Read floats from the line: v X Y Z
		     &obj->vertexBuffer[obj->TotalConnectedPoints],
		     &obj->vertexBuffer[obj->TotalConnectedPoints+1],
		     &obj->vertexBuffer[obj->TotalConnectedPoints+2]);

	      obj->TotalConnectedPoints += POINTS_PER_VERTEX;					// Add 3 to the total connected points
	    }
	  if (line[0] == 'f')										// The first character is an 'f': on this line is a point stored
	    {
	      line[0] = ' ';												// Set first character to 0. This will allow us to use sscanf

	      int vertexNumber[4] = { 0, 0, 0 };
	      int texNumber[4] = { 0, 0, 0 };

	      if(  sscanf(line,"%i %i %i",		//if the structure of the obj is like 1243 3245 3456
		     &vertexNumber[0],
		     &vertexNumber[1],
			  &vertexNumber[2] )==3)
              {}
	      else if(sscanf(line,"%i/%i %i/%i %i/%i",   //if the structure is vtx/tx
                             &vertexNumber[2],
                             &texNumber[0],
                             &vertexNumber[1],
                             &texNumber[1],
                             &vertexNumber[0],
		      	     &texNumber[2])==6)
             {}; 										// each point represents an X,Y,Z.

	      vertexNumber[0] -= 1;										// OBJ file starts counting from 1
	      vertexNumber[1] -= 1;										// OBJ file starts counting from 1
	      vertexNumber[2] -= 1;										// OBJ file starts counting from 1


	      /********************************************************************
	       * Create triangles (f 1 2 3) from points: (v X Y Z) (v X Y Z) (v X Y Z).
	       * The vertexBuffer contains all verteces
	       * The triangles will be created using the verteces we read previously
	       */

	      int tCounter = 0;
	      for (int i = 0; i < POINTS_PER_VERTEX; i++)
		{
		  obj->Faces_Triangles[triangle_index + tCounter   ] = obj->vertexBuffer[3*vertexNumber[i] ];
		  obj->Faces_Triangles[triangle_index + tCounter +1 ] = obj->vertexBuffer[3*vertexNumber[i]+1 ];
		  obj->Faces_Triangles[triangle_index + tCounter +2 ] = obj->vertexBuffer[3*vertexNumber[i]+2 ];
		  tCounter += POINTS_PER_VERTEX;
		}

	      /*********************************************************************
	       * Calculate all normals, used for lighting
	       */
	      float coord1[3] = { obj->Faces_Triangles[triangle_index], obj->Faces_Triangles[triangle_index+1],obj->Faces_Triangles[triangle_index+2]};
	      float coord2[3] = {obj->Faces_Triangles[triangle_index+3],obj->Faces_Triangles[triangle_index+4],obj->Faces_Triangles[triangle_index+5]};
	      float coord3[3] = {obj->Faces_Triangles[triangle_index+6],obj->Faces_Triangles[triangle_index+7],obj->Faces_Triangles[triangle_index+8]};
	      float nx, ny, nz;
	      calculateNormal(coord1, coord2, coord3, &nx, &ny, &nz);

	      tCounter = 0;
	      for (int i = 0; i < POINTS_PER_VERTEX; i++)
		{
		  obj->normals[normal_index + tCounter ] = nx;
		  obj->normals[normal_index + tCounter +1] = ny;
		  obj->normals[normal_index + tCounter +2] = nz;
		  tCounter += POINTS_PER_VERTEX;
		}

	      triangle_index += TOTAL_FLOATS_IN_TRIANGLE;
	      normal_index += TOTAL_FLOATS_IN_TRIANGLE;
	      obj->TotalConnectedTriangles += TOTAL_FLOATS_IN_TRIANGLE;
	    }
	}
      fclose(objFile);														// Close OBJ file
    }
  else
    {
      fprintf(stderr, "Unable to open file");
    }
  return 0;
}












void Model_OBJ_Release(Model_OBJ *obj)
{
  free(obj->Faces_Triangles);
  free(obj->normals);
  free(obj->vertexBuffer);
}

void Model_OBJ_Draw(Model_OBJ *obj)
{
  glEnableClientState(GL_VERTEX_ARRAY);						// Enable vertex arrays
  glEnableClientState(GL_NORMAL_ARRAY);						// Enable normal arrays
  glVertexPointer(3,GL_FLOAT,	0,obj->Faces_Triangles);				// Vertex Pointer to triangle array
  glNormalPointer(GL_FLOAT, 0, obj->normals);						// Normal pointer to normal array
  glDrawArrays(GL_TRIANGLES, 0, obj->TotalConnectedTriangles);		// Draw the triangles
  glDisableClientState(GL_VERTEX_ARRAY);						// Disable vertex arrays
  glDisableClientState(GL_NORMAL_ARRAY);						// Disable normal arrays
}












/***************************************************************************
 * Program code
 ***************************************************************************/

Model_OBJ obj;
float g_rotation;

void calcObjCenter(){

  int trios  = obj.TotalConnectedPoints/POINTS_PER_VERTEX;
  for(int c = 1; c<trios;c++){
    obj.centerObjX += obj.vertexBuffer[3*c];
    obj.centerObjY += obj.vertexBuffer[3*c+1];
    obj.centerObjZ += obj.vertexBuffer[3*c+2];
  }

  obj.centerObjX = obj.centerObjX/(float)trios;
  obj.centerObjY = obj.centerObjY/(float)trios;
  obj.centerObjZ = obj.centerObjZ/(float)trios;

  printf("Barycentre de l'objet = (%f,%f,%f)\n",obj.centerObjX,obj.centerObjY,obj.centerObjZ);

}




















GLuint vertexShader=0;
GLuint fragmentShader=0;
GLuint programID=0;
GLuint textureIDs[8];

void displayQuad(int is_fpga)
{
  float hw = 1;
  float hh = 1;

	glEnable(GL_TEXTURE_2D);
	
	if (!is_fpga) {		
		//we use the program shader we compiled
		glUseProgram(programID);
		
		//we activate the 2D textures
		GLint nbviewsloc = glGetUniformLocation(programID, "floatnbviews");
		glUniform1fARB(nbviewsloc, (float) nbviews);
  int i ;
  char texture[10] ;
  for(i = 0 ; i<nbviews ; i++) {
    sprintf(texture, "tex%d", i) ;
    GLint baseImageLoc = glGetUniformLocation(programID, texture);
    if (baseImageLoc>=0) {
      //fprintf(stderr, "Tx id %s: %d - location %d\n", texture, textureIDs[i], baseImageLoc);
      glActiveTexture(GL_TEXTURE0 + i);
      glUniform1i(baseImageLoc, i);
      glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
    }
  }

  //after the loop activating the textures, reactivate texture 0
  glActiveTexture(GL_TEXTURE0);

	
	}
	
	

  //  Clear screen and Z-buffer
  //  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  // if needed, move to ortho projection
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-hw, hw, -hh, hh, -20, 20);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //draw a fullscreen quad

  glBegin(GL_TRIANGLES);
  glTexCoord2f(0, 0);
  glVertex3f(-hw,-hh, 0 );
  glTexCoord2f(0, 1);
  glVertex3f(-hw, hh, 0 );
  glTexCoord2f(1, 1);
  glVertex3f( hw, hh, 0 );

  glTexCoord2f(1, 1);
  glVertex3f( hw, hh, 0 );
  glTexCoord2f(1, 0);
  glVertex3f( hw,-hh, 0 );
  glTexCoord2f(0, 0);
  glVertex3f(-hw,-hh, 0 );

  glEnd();


  // reload original matrix
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glUseProgram(0);
}




GLuint createShader(GLenum shaderType, char *ShaderTextFile)
{
  GLuint myShader;
  int size;
  char *shaderData;
  FILE *shaderFile = fopen(ShaderTextFile, "rt");
  if (!shaderFile) {
    fprintf(stderr, "Cannot open shader file %s\n", ShaderTextFile);
    return 0;
  }
  fseek(shaderFile, 0, SEEK_END);
  size = ftell(shaderFile);
  fseek(shaderFile, 0, SEEK_SET);

  shaderData = (char *)malloc(sizeof(char)*(size+1));
  fread(shaderData, 1, size, shaderFile);
  fclose(shaderFile);
  shaderData[size] = 0;
  myShader = glCreateShader(shaderType);

  glShaderSource(myShader, 1, (const GLchar**)	&shaderData, &size);

  glCompileShader(myShader);

  glGetShaderiv(myShader, GL_INFO_LOG_LENGTH, &size);

  if (size > 0) {
    int charsWritten  = 0;
    char *infoLog = (char *)malloc(size);
    glGetShaderInfoLog(myShader, size, &charsWritten, infoLog);
    if (charsWritten) {
        fprintf(stderr, "Error compiling shader %s: %s\n", ShaderTextFile, infoLog);
        fprintf(stderr, "Shader code is %s\n", shaderData);
    } else {
        fprintf(stderr, "Shader %s compiled OK\n", ShaderTextFile);
    }
    free(infoLog);
  } else {
      fprintf(stderr, "Shader %s compiled OK\n", ShaderTextFile);
  }

  free(shaderData);
  return myShader;
}



void createProgram()
{
  char *data;
  vertexShader = createShader(GL_VERTEX_SHADER, (char *)"vert.txt");
  fragmentShader = createShader(GL_FRAGMENT_SHADER, (char *)"frag.txt");

  programID = glCreateProgram();

  glAttachShader(programID, vertexShader);
  glAttachShader(programID, fragmentShader);

  glLinkProgram(programID);

  {
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(programID, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
      {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(programID, infologLength, &charsWritten, infoLog);
	if (charsWritten)
	  fprintf(stderr, "Error linking program: %s\n",infoLog);
        free(infoLog);
      }
  }

  glUseProgram(0);

  data = (char *)malloc(sizeof(char)*3*width*height);
  memset(data, 0xFF, sizeof(char)*3*width*height);

  glGenTextures(nbviews, textureIDs);

  int i ;
  for(i = 0 ; i<nbviews ; i++) {
    glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  }

  free(data);

}

void CalcTranslationCamVector(){

   float vectorCam[3];

   vectorCam[0]=eyeX-centerX;
   vectorCam[1]=eyeY-centerY;
   vectorCam[2]=eyeZ-centerZ;

   float vectorSize = sqrt(vectorCam[0]*vectorCam[0] + vectorCam[1]*vectorCam[1] + vectorCam[2]*vectorCam[2]);

   vectorCam[0]=vectorCam[0]/vectorSize;
   vectorCam[1]=vectorCam[1]/vectorSize;
   vectorCam[2]=vectorCam[2]/vectorSize;

   translationCamVector[0] = -vectorCam[2];
   translationCamVector[1] = 0.0;
   translationCamVector[2] = vectorCam[0];

     if (debug_cam)
        printf("vector cam rayon = %f\n",vectorSize);


}

void drawBackground3D()
{
    if (texture_fond==0) return;
    float s = 2*z_far/3;

  glDisable(GL_LIGHTING);
  //mise en place du fond
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_fond);
  glColor3f(1,1,1);

        glBegin(GL_QUADS);
	 //front face
         glNormal3f(0.0, 0.0, 1.0);
         glTexCoord2d(0, 0); glVertex3f(-s, -s, -s);
         glTexCoord2d(1, 0); glVertex3f(s, -s, -s);
         glTexCoord2d(1, 1); glVertex3f(s, s, -s);
         glTexCoord2d(0, 1); glVertex3f(-s, s, -s);

	 //right face
         glNormal3f(-1.0, 0.0, 0.0);
         glTexCoord2d(1, 0); glVertex3f(s, -s, -s);
         glTexCoord2d(1, 1); glVertex3f(s, s, -s);
         glTexCoord2d(0, 1); glVertex3f(s, s, s);
         glTexCoord2d(0, 0); glVertex3f(s, -s, s);

	 //back face
         glNormal3f(0.0, 0.0, -1.0);
         glTexCoord2d(1, 0); glVertex3f(-s, -s, s);
         glTexCoord2d(0, 0); glVertex3f(s, -s, s);
         glTexCoord2d(0, 1); glVertex3f(s, s, s);
         glTexCoord2d(1, 1); glVertex3f(-s, s, s);

	 //left face
         glNormal3f(1.0, 0.0, 0.0);
         glTexCoord2d(0, 0); glVertex3f(-s, -s, -s);
         glTexCoord2d(0, 1); glVertex3f(-s, s, -s);
         glTexCoord2d(1, 1); glVertex3f(-s, s, s);
         glTexCoord2d(1, 0); glVertex3f(-s, -s, s);

        glEnd();

   glDisable(GL_TEXTURE_2D);
}

void drawBackground2D()
{
    if (texture_fond==0) return;
    float hw = 1;
    float hh = 1;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-hw, hw, -hh, hh, -20, 20);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

     //mise en place du fond
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_fond);
  glDisable(GL_LIGHTING);
  glColor3f(1,1,1);

    glBegin(GL_TRIANGLES);
    glTexCoord2f(0, 0);
    glVertex3f(-hw,-hh, 0 );
    glTexCoord2f(0, 1);
    glVertex3f(-hw, hh, 0 );
    glTexCoord2f(1, 1);
    glVertex3f( hw, hh, 0 );
    glTexCoord2f(1, 1);
    glVertex3f( hw, hh, 0 );
    glTexCoord2f(1, 0);
    glVertex3f( hw,-hh, 0 );
    glTexCoord2f(0, 0);
    glVertex3f(-hw,-hh, 0 );
    glEnd();

   glDisable(GL_TEXTURE_2D);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glClear(GL_DEPTH_BUFFER_BIT);
}

void displayModel()
{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(eyeX,eyeY,eyeZ, centerX,centerY,centerZ, 0,1,0);

  if (background_2d) drawBackground2D();
  else drawBackground3D();

  glEnable(GL_LIGHTING);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  GLint LightPos[4] = {0,0,10,0};
  if (objtrue == 1)  LightPos[2] = -10;
  glLightiv(GL_LIGHT0,GL_POSITION,LightPos);
  glPopMatrix();

  glEnable( GL_COLOR_MATERIAL );

  if(objtrue==2){
    glPushMatrix();
    //glRotatef(g_rotation,0,1,0);
    //glRotatef(90,0,1,0);
    //g_rotation++;
    glTranslatef(-obj.centerObjX, -obj.centerObjY, -obj.centerObjZ);

    Model_OBJ_Draw(&obj);
    glPopMatrix();
  } else if (objtrue == 1){
    int MatSpec [4] = {1,1,1,1};
    glMaterialiv(GL_FRONT_AND_BACK,GL_SPECULAR,MatSpec);
    glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,100);

    glutSolidTeapot(1) ;

  } else {


    //Multi-colored side - FRONT
    glBegin(GL_POLYGON);

    glColor3f( 1.0, 0.0, 0.0 );     glVertex3f(  0.5, -0.5, -0.5 );      // P1 is red
    glColor3f( 0.0, 1.0, 0.0 );     glVertex3f(  0.5,  0.5, -0.5 );      // P2 is green
    glColor3f( 0.0, 0.0, 1.0 );     glVertex3f( -0.5,  0.5, -0.5 );      // P3 is blue
    glColor3f( 1.0, 0.0, 1.0 );     glVertex3f( -0.5, -0.5, -0.5 );      // P4 is purple

    glEnd();

    // White side - BACK
    glBegin(GL_POLYGON);
    glColor3f(   1.0,  1.0, 1.0 );
    glVertex3f(  0.5, -0.5, 0.5 );
    glVertex3f(  0.5,  0.5, 0.5 );
    glVertex3f( -0.5,  0.5, 0.5 );
    glVertex3f( -0.5, -0.5, 0.5 );
    glEnd();

    // Purple side - RIGHT
    glBegin(GL_POLYGON);
    glColor3f(  1.0,  0.0,  1.0 );
    glVertex3f( 0.5, -0.5, -0.5 );
    glVertex3f( 0.5,  0.5, -0.5 );
    glVertex3f( 0.5,  0.5,  0.5 );
    glVertex3f( 0.5, -0.5,  0.5 );
    glEnd();
    // Green side - LEFT
    glBegin(GL_POLYGON);
    glColor3f(   0.0,  1.0,  0.0 );
    glVertex3f( -0.5, -0.5,  0.5 );
    glVertex3f( -0.5,  0.5,  0.5 );
    glVertex3f( -0.5,  0.5, -0.5 );
    glVertex3f( -0.5, -0.5, -0.5 );
    glEnd();
    // Blue side - TOP
    glBegin(GL_POLYGON);
    glColor3f(   0.0,  0.0,  1.0 );
    glVertex3f(  0.5,  0.5,  0.5 );
    glVertex3f(  0.5,  0.5, -0.5 );
    glVertex3f( -0.5,  0.5, -0.5 );
    glVertex3f( -0.5,  0.5,  0.5 );
    glEnd();

    // Red side - BOTTOM
    glBegin(GL_POLYGON);
    glColor3f(   1.0,  0.0,  0.0 );
    glVertex3f(  0.5, -0.5, -0.5 );
    glVertex3f(  0.5, -0.5,  0.5 );
    glVertex3f( -0.5, -0.5,  0.5 );
    glVertex3f( -0.5, -0.5, -0.5 );
    glEnd();


  }


  glFlush();

}

void moveCam(){

  eyeX = centerX + rayon*cos(angle*6.28318/360.0);
  eyeZ = centerZ + rayon*sin(angle*6.28318/360.0);

  if (debug_cam)
    printf("center %f %f %f\neye %f %f %f\nRayon %f\n", centerX,centerY,centerZ,eyeX,eyeY,eyeZ, rayon);
}


void displayfps(){
	char* s;
	frame++;
	currenttime = glutGet(GLUT_ELAPSED_TIME);
	if ((currenttime - timebase) > 1000) {
		//sprintf(s,"FPS:%4.2f",
		//		frame*1000.0/(currenttime-timebase));
		fprintf(stderr,"FPS:%4.2f\r",
				frame*1000.0/(currenttime-timebase));
		timebase = currenttime;
		frame = 0;
	}
	
}

void display()
{
  if((width != glutGet(GLUT_WINDOW_WIDTH)) || (height != glutGet(GLUT_WINDOW_HEIGHT))){
  	  width = glutGet(GLUT_WINDOW_WIDTH);
  	  height = glutGet(GLUT_WINDOW_HEIGHT);
	  //framebufferloadtexture();
  }


  if(rotate_on) {
    anglekey = anglekey - rotationangle;
    glutPostRedisplay();
  }

  if(bounce_on){
    if(forward){
      rayonkey = rayonkey + deltarayon/2;
    } else {
      rayonkey = rayonkey - deltarayon/2;
    }
    glutPostRedisplay();
    if(bounced == 30) {
      forward = !forward ;
      bounced = 0 ;
    }
    bounced++ ;
  }


  if (go_fullscreen) {
   glutFullScreen();
   go_fullscreen=0;
  }

  angle = anglekey;
  rayon = rayonkey;

  if(scanMode == TRANS){
    CalcTranslationCamVector();
    centerX = centerX - (((float)nbviews-1)/2)*deltaangle*rayonkey/360*6.2830*translationCamVector[0];
    centerZ = centerZ - (((float)nbviews-1)/2)*deltaangle*rayonkey/360*6.2830*translationCamVector[2];
  }else if(scanMode == ROT){
    angle = angle -(((float)nbviews-1)/2)*deltaangle;
  }



    for (currentview=0; currentview<nbviews; currentview++) {

//      if(currentview == nbviews - 1) process_enable = false;

      //if (debug_cam) printf("Camera %d\n",currentview);



      moveCam();


      displayModel();

      if ((mode==FPGA) || (mode==BMP)) {
#ifdef USE_FPGA
		  if (mode==FPGA) {
		  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)fpga_in);
		  } else 
#endif
		  {
			  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)image);
		  }
		  glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT,  GL_FLOAT, (GLvoid*)profondeur);

		  if (mode==BMP) {

#if 1
		for(i=0; i<width*height; i++)
	  {
	    depth[i] = (unsigned char ) (0xFF * (1-profondeur[i]) );
	    }
#else
	float med = 0.0 ;
	for(i=0; i<width*height; i++)
        {
           depth[i] = (unsigned char ) (0xFF * profondeur[i]);
           med = med + depth[i];
        }
        med = med/(width*height);
        for(i=0; i<width*height; i++)
        {
           if(depth[i]>med) depth[i] = 0;
           else depth[i] = depth[i]/med;
        }

#endif
		  }
		  
		  
#ifdef USE_FPGA
	if(mode == FPGA){

	  for(int l = 0;l<width;l++){
	    for(int m = 0;m<height;m++){
	      /*
		   fpga_in[4*width*m+4*l] = image[3*width*m +3*l];
	      fpga_in[4*width*m+4*l + 1] = image[3*width*m +3*l +1];
	      fpga_in[4*width*m+4*l + 2] = image[3*width*m +3*l + 2];
		  */
			
			fpga_in[4*width*m+4*l + 3] =  (unsigned char ) (0xFF * (1-profondeur[i]) );
	    }
	  }

		
	PAF3dGenerate(width, height, fpga_in, fpga_out, fpga_scale, fpga_offset);	
	
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,textureFPGA);		
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 2*width, 2*height, 0, GL_RGB, GL_UNSIGNED_BYTE, fpga_out);
		glDisable(GL_TEXTURE_2D);
		
	}
#endif
		  
		  
	if(mode == BMP){
	  sprintf(filenameColor,"images/cam_%d_frame_%04d.bmp",currentview,cam);
	  sprintf(filenameDepth,"images/camdepth_%d_frame_%04d.bmp",currentview,cam);

	  CreateAndSaveBMP(filenameColor, width, height, 3, image);
	  CreateAndSaveBMP(filenameDepth, width, height, 1, depth);
	}
      }

      if (mode==SHADER) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureIDs[currentview]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
	glDisable(GL_TEXTURE_2D) ;

      }


    if(scanMode == TRANS){
      CalcTranslationCamVector();
      centerX = centerX + deltaangle*rayon/360*6.2830*translationCamVector[0];
      centerZ = centerZ + deltaangle*rayon/360*6.2830*translationCamVector[2];
    }else if(scanMode == ROT){
      angle = angle + deltaangle;
    }


      //if (debug_cam) printf("angle = %f\n\n",angle);
      if(mode!=SHADER)
	      break;
    }


#if 0
    angle = angle -((float)nbviews+1)/2*deltaangle;
    printf("Caract\E9ristiques finales : \n");
    moveCam();
    printf("angle = %f\n\n",angle);
#endif

    //anglekey = anglekey + rotationangle;
    cam++;
    //currentview = 0;

    if ((mode==SHADER) || (mode==FPGA)) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      /* long int startc, stopc ;
	 startc = clock();*/

		displayQuad((mode==FPGA) ? 1 : 0);
      /*  glFlush();

      stopc = clock();
      fprintf(stderr, "Le calcul a pris %f secondes.\n", (float)(stopc - startc) / CLOCKS_PER_SEC);*/

     if(scanMode == TRANS){
         CalcTranslationCamVector();
         centerX = centerX -(((float)nbviews+1)/2)*deltaangle*rayonkey/360*6.2830*translationCamVector[0];
         centerZ = centerZ - (((float)nbviews+1)/2)*deltaangle*rayonkey/360*6.2830*translationCamVector[2];
      }

      glDisable(GL_TEXTURE_2D);
      glUseProgram(0);


    }

  //finally flush the last frame drawn
  glutSwapBuffers();
  displayfps();
}



void initialize ()
{
  glMatrixMode(GL_PROJECTION);
  glViewport(0, 0, width, height);
  GLfloat aspect = (GLfloat) width / height;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(field_of_view_angle, aspect, z_near, z_far);
  glMatrixMode(GL_MODELVIEW);
  //  glShadeModel( GL_SMOOTH );
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
  glClearDepth( 1.0f );
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
  glDisable(GL_LIGHTING);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat amb_light[] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat diffuse[] = { 0.6, 0.6, 0.6, 1 };
  GLfloat specular[] = { 0.7, 0.7, 0.3, 1 };
  GLint LightPos[4] = {0,0,3,1};
  glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
  glLightiv(GL_LIGHT0,GL_POSITION,LightPos);
  glEnable( GL_COLOR_MATERIAL );
  glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

}



void keyboard ( unsigned char key, int x, int y )
{
  switch ( key ) {
  case KEY_ESCAPE:
    exit(0);
    break;
  case 'd':{ //Tourner la cam\E9ra vers la gauche
    anglekey = anglekey + rotationangle;
    glutPostRedisplay();
  }
    break;
  case 'q':{ //Tourner la camera vers la droite
    anglekey = anglekey - rotationangle;
    glutPostRedisplay();
  }
    break;
  case 's':{ //reculer
    rayonkey = rayonkey + deltarayon;
    glutPostRedisplay();
  }
    break;
  case 'z':{ //avancer
    rayonkey = rayonkey - deltarayon;
    glutPostRedisplay();
  }
    break;
  case 'a':{ //entrelacer--
    deltaangle = deltaangle - 0.1;
    glutPostRedisplay();
    printf("Nouvelle variation d'angle = %f\n\n",deltaangle);
  }
    break;
  case 'e':{ //entrelacer++
    deltaangle = deltaangle + 0.1;
    glutPostRedisplay();
    printf("Nouvelle variation d'angle = %f\n\n",deltaangle);
  }
    break;
  case 'r':{ //tourner en continu
    rotate_on = !rotate_on ;
    glutPostRedisplay();
  }

    break;
  case 'u':{ //Monter
    centerY = centerY - 1;
    glutPostRedisplay();
  }
    break;
  case 'j':{ //Descendre
    centerY = centerY + 1;
    glutPostRedisplay();
  }
 break;
  case 'v':{ //Cube
    objtrue = 0 ;
    glutPostRedisplay();
  }
 break;
  case 'b':{ //Teapot
    objtrue = 1 ;
    glutPostRedisplay();
  }
 break;
  case 'n':{ //Objname
    objtrue = 2 ;
    glutPostRedisplay();
  }
    break;
  case 'f':
    go_fullscreen = 1;
    break;
  case 'h':{ //gauche

    CalcTranslationCamVector();
    centerX = centerX + deltarayon*translationCamVector[0];
    centerZ = centerZ + deltarayon*translationCamVector[2];
    moveCam();
    glutPostRedisplay();
  }
    break;
  case 'k':{ //droite
    CalcTranslationCamVector();
    centerX = centerX - deltarayon*translationCamVector[0];
    centerZ = centerZ - deltarayon*translationCamVector[2];
    moveCam();
    glutPostRedisplay();
  }
    break;
  case 'c':{ //droite
    centerX = obj.centerObjX;
    centerY = obj.centerObjY;
    centerZ = obj.centerObjZ;
    glutPostRedisplay();
  }
    break;
  case 'w':{ //droite
    scanMode = ROT;
    glutPostRedisplay();

  }
    break;
  case 'x':{ //droite
    scanMode = TRANS;
    glutPostRedisplay();

  }
  case 'm':
    background_2d = !background_2d;
    glutPostRedisplay();

    break;
  case 'l':
    bounce_on = !bounce_on ;
    glutPostRedisplay();
    break;
  case 'p':
    mode = (mode==SHADER) ? 0 : SHADER;
    glutPostRedisplay();
  default:
    break;
  }

  // if(mode == SHADER){
    //process_enable = true;
    // }


}

void on_exit(){
	fprintf(stdout,"\n");

#ifdef USE_FPGA
	if (mode==FPGA) { 
		PAF3dClose();
		free(fpga_in);
		free(fpga_out);
	}
#endif
	

  free(image);
  free(depth);
  free(color_depth);
  free(profondeur);
}











int main(int argc, char **argv)
{
    char *image_fond=NULL;
  // set window values
  for (int n = 1;n<argc;n++) {
    if (!strncmp(argv[n], "-nbviews=", 9))
      nbviews = atoi(argv[n]+9);
    else if (!strncmp(argv[n], "-angle=", 7))
      deltaangle= atof(argv[n]+7);
    else if (!strncmp(argv[n], "-width=", 7))
      width= atoi(argv[n]+7);
    else if (!strncmp(argv[n], "-height=", 8))
      height= atoi(argv[n]+8);
    else if (!strcmp(argv[n], "-bmp"))
      mode = BMP;
    else if (!strcmp(argv[n], "-fpga"))
      mode = FPGA;
    else if (!strcmp(argv[n], "-shader"))
      mode = SHADER;
    else if (!strcmp(argv[n], "-dbg"))
      debug_cam = 1;
    else if (!strncmp(argv[n], "-obj=", 5)){
      objtrue = 2 ;
      objname = argv[n]+5;
    } else if (!strncmp(argv[n], "-img=", 5)){
        image_fond = argv[n]+5;
    }else if (!strncmp(argv[n], "-rot", 5)){
        scanMode = ROT;
    }else if (!strncmp(argv[n], "-trans", 5)){
        scanMode = TRANS;
    }
    else if (!strncmp(argv[n], "-obj1", 5)){
      objtrue = 1 ;
    }
    else if (!strncmp(argv[n], "-bs=", 4)){
      back_size = atof(argv[n]+4) ;
    }


  }

  title = (char *) "OpenGL/GLUT OBJ Loader.";
  field_of_view_angle = 45;
  z_near = 1.0f;
  z_far = 500.0f;
  if (!back_size) back_size = z_far/2;

  //Initialisation des tableaux
  image = (unsigned char*) malloc(width*height*3*sizeof(unsigned char));
  depth = (unsigned char*) malloc(width*height*sizeof(unsigned char));
  profondeur = (float*) malloc(width*height*sizeof(float));


  //Initialisation des variables gloables
  angle = -(((float)nbviews-1)/2)*deltaangle;
  printf("angle initial = %f\n",angle);

  deltarayon = 0.5;
  eyeX = 0;
  eyeY = 1;
  eyeZ = 4;
  centerX = 0;
  centerY = 0;
  centerZ = 0;

  rayon = sqrt((eyeX-centerX)*(eyeX-centerX)+(eyeZ-centerZ)*(eyeZ-centerZ));

  eyeX = centerX + rayon*cos(angle*6.28318/360.0);
  eyeZ = centerZ + rayon*sin(angle*6.28318/360.0);



  // initialize and run program
  glutInit(&argc, argv);                                                // GLUT initialization
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );            // Display Mode
  glutInitWindowSize(width,height);					// set window size
  glutCreateWindow(title);			                	// create Window
  initialize();

  glewExperimental=1;
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
    printf("Ready for GLSL - GLEW version %s - GL version %s\n",  glewGetString(GLEW_VERSION), glGetString( GL_VERSION ) );
  else {
    printf("Not totally ready :(  \n");
    exit(1);
  }


  createProgram();

  if (image_fond) {
   texture_fond = loadBmp(image_fond);
  }

  if(objtrue){
    Model_OBJ_Load(&obj, objname);
    calcObjCenter();

/*
    centerX = obj.centerObjX;
    centerY = obj.centerObjY;
    centerZ = obj.centerObjZ;
*/

    rayon = sqrt((eyeX-centerX)*(eyeX-centerX)+(eyeZ-centerZ)*(eyeZ-centerZ));

    eyeX = centerX + rayon*cos(angle*6.28318/360.0);
    eyeZ = centerZ + rayon*sin(angle*6.28318/360.0);


  }

  //Informations consoles generales
  printf("Informations initialisation\n\n");
  printf("Nombre de vue = %d\n",nbviews);
  printf("Variation d'angle entre deux cam\E9ras = %f\n",deltaangle);
  printf("Nom du .obj  = %s\n\n",objname);

#ifdef USE_FPGA
	if (mode==FPGA) {
		PAF3dOpen();

		posix_memalign((void **)(&fpga_in),4096,width*height*4);
		posix_memalign((void **)(&fpga_out),4096,4*width*height*4);		

		glGenTextures(1, (GLuint *) &textureFPGA);
		glBindTexture(GL_TEXTURE_2D, textureFPGA);
		
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 2*width, 2*height, 0, GL_RGB, GL_UNSIGNED_BYTE, fpga_out);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		
	}
#endif


  glutDisplayFunc(display);			                	// register Display Function
//  glutIdleFunc( display );				        	// register Idle Function
  glutKeyboardFunc( keyboard );						// register Keyboard Handler
  atexit( on_exit );
  //glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);


  glutMainLoop();

  return 0;
}


