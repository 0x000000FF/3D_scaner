#include <iostream>
#include <gl\glut.h>
#include <vector>
#include "file.h"
#include "scan.h"

using namespace std;

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat zRot = 0.0f;
static int mx;
static int my;
static int mod = 1;
static int d = 500;
static int k = 1;
static bool startroll = false;
static bool translated = false;
static vector<vector<double>> GL_map;
static vector<vector<double>> GL_map_r;

GLfloat mat_ambient[]={0.8,0.8,0.8,0.8};
GLfloat mat_diffuse[]={0.8,0.8,0.8,0.8};
GLfloat light_position[]={1.5,1.0,-2.5,0.0};
startScan scan;
void Initinal()
{
	glClearColor(0,0,0,1);
	glColor3f(0,0,0);
}
void ChangeSize(int w,int h)
{
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-w/h,w/h,-1,1);
}
void myinit()
{
		glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
		glMaterialf(GL_FRONT,GL_SHININESS,30.0);
		glLightModeli(GL_LIGHTING,GL_TRUE );
	
		glLightfv(GL_LIGHT0,GL_POSITION,light_position);
		
		glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_AUTO_NORMAL);
		glEnable(GL_NORMALIZE);
		glFrontFace(GL_CW);
}//设置光照参数
void SpecialKeys(int key,int x,int y)
{
	if(key == GLUT_KEY_UP && d>0)
		d=d-25;
	if(key == GLUT_KEY_DOWN)
		d=d+25;
	if(key == GLUT_KEY_LEFT && k>1)
		k--;
//		zRot+=5;
	if(key == GLUT_KEY_RIGHT && k<5)
		k++;
//		zRot-=5;
	/*if(xRot>356)
		xRot=0;
	if(xRot<-1)
		xRot=355;
	if(yRot>356)
		yRot=0;
	if(yRot<-1)
		yRot=355;
	if(zRot>356)
		zRot=0;
	if(zRot<-1)
		zRot=355;*/
	glutPostRedisplay();
}
void Rollstart(int button,int state,int x,int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		startroll = true;
		mx=x;
		my=y;
	}
	else if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		startroll = false;
	}
}
void Roll(int x,int y)
{
	if(startroll && (x!=mx || y!=my))
	{
		yRot=yRot-(x-mx)*1.0/5;
		xRot=xRot-(y-my)*1.0/5;
		mx=x;
		my=y;
		if(xRot>359.8)
			xRot=0;
		if(xRot<0)
			xRot=359.8;
		if(yRot>359.8)
			yRot=0;
		if(yRot<0)
			yRot=359.8;
	}
	glutPostRedisplay();
}
void ProcessMenu1(int value)
{
	mod = value;
	glutPostRedisplay();
}
void M_Inter(vector<vector<float>> cloud)
{
	GL_map_r.resize(2*cloud.size());
	for (int i = 0;i < GL_map_r.size();i++)
	{
		GL_map_r[i].resize(cloud[0].size());
	}
	for (int i = 0;i < cloud.size();i++)
	{
		for(int j = 0;j < cloud[i].size();j++)
		{
			GL_map_r[2*i][j] = cloud[i][j];
		}
		
	}
	for (int i = 1;i < GL_map_r.size();i = i + 2)
	{
		for (int j = 0;j < GL_map_r[i].size();j++)
		{
			GL_map_r[i][j] = (GL_map_r[i-1][j] + GL_map_r[(i+1 == GL_map_r.size()) ? 0 : (i+1)][j])/2;
		}
	}
}
void R_To_X(vector<vector<double>> cloud)
{
	cout<<"坐标转换中…\n";
	int h,w;
	w = cloud.size();
	h = cloud[0].size();

	GL_map.resize(w);
	for(int i = 0;i < w;i++)
	{
		GL_map[i].resize(h * 3);
	}

	float x,y,z;
	for (int i = 0;i < w;i++)
	{

		for (int j = 0;j < h;j++)
		{
			x = (cos(float(2*i)*3.14159265358979/180))*cloud[i][j];
			y = -j;
			z = (sin(float(2*i)*3.14159265358979/180))*cloud[i][j];
			GL_map[i][3*j] = x;
			GL_map[i][3*j+1] = y;
			GL_map[i][3*j+2] = z;
		}
	}
	translated = true;
	cout<<"坐标转换完毕！\n";
}
void buildPointCloud()
{
	glDisable(GL_LIGHTING);
	glColor3f(0,1,0);
	float x,y,z;

	for (int i = 0;i < GL_map.size();i++)
	{
		glBegin(GL_POINTS);
		for (int j = 0;j < GL_map[i].size();j = j+3)
		{
			x = GL_map[i][j];
			y = GL_map[i][j + 1];
			z = GL_map[i][j + 2];
			if(x == 0 && z== 0)
				continue;
			glVertex3f(x/d,(1.6 * y + 425)/d,z/d);
		}
		glEnd();
		
	}
	glFlush();
}
void buildLines(int model)
{
	glDisable(GL_LIGHTING);
	glColor3f(0,1,0);
	float x,y,z;

	for (int i = 0;i < GL_map.size();i += model)
	{
		glBegin(GL_LINE_STRIP);
		for (int j = 0;j < GL_map[i].size();j = j+18)
		{
			x = GL_map[i][j];
			y = GL_map[i][j + 1];
			z = GL_map[i][j + 2];
			if(x == 0 && z== 0)
				continue;
			glVertex3f(x/d,(1.6 * y + 425)/d,z/d);
		}
		glEnd();

	}
//	glColor3f(1,0,0);

	for (int i = 0;i < GL_map[0].size();i = i + model*18)
	{
		glBegin(GL_LINE_LOOP);
		for (int j = 0;j < GL_map.size();j = j+1)
		{
			x = GL_map[j][i];
			y = GL_map[j][i + 1];
			z = GL_map[j][i + 2];
			if(x == 0 && z== 0)
				continue;
			glVertex3f(x/d,(1.6 * y + 425)/d,z/d);
		}
		glEnd();

	}
	glFlush();
}
void normal(float o[3],float a[3],float b[3],float out[3])
{
	out[0]=((a[1]-o[1])*(b[2]-o[2]))-((a[2]-o[2])*(b[1]-o[1]));
	out[1]=((a[2]-o[2])*(b[0]-o[0]))-((a[0]-o[0])*(b[2]-o[2]));
	out[2]=((a[0]-o[0])*(b[1]-o[1]))-((a[1]-o[1])*(b[0]-o[0]));

	float d;
	d = sqrt(out[0]*out[0]+out[1]*out[1]+out[2]*out[2]);

	if(d == 0)
	{
		return;
	}
	out[0]=out[0]/d;
	out[1]=out[1]/d;
	out[2]=out[2]/d;
}
void buildObj()
{
	glEnable(GL_LIGHTING);
	glColor3f(1,1,1);
	float x,y,z;

	float nor[3] = {0,0,0};
	float o[3] = {0,0,0};
	float a[3] = {0,0,0};
	float b[3] = {0,0,0};

	for (int i = 0;i < GL_map.size();i++)
	{
		for(int j=0;j < GL_map[i].size()-16;j += 15)
		{
			glBegin(GL_POLYGON);

			o[0] = GL_map[i][j];
			o[1] = GL_map[i][j+1];
			o[2] = GL_map[i][j+2];
			
			a[0] = GL_map[i][j+15];
			a[1] = GL_map[i][j+16];
			a[2] = GL_map[i][j+17];

			b[0] = GL_map[(i == GL_map.size()-1) ? 0 : (i+1)][j+9];
			b[1] = GL_map[(i == GL_map.size()-1) ? 0 : (i+1)][j+10];
			b[2] = GL_map[(i == GL_map.size()-1) ? 0 : (i+1)][j+11];

			normal(o,a,b,nor); 
			glNormal3d(nor[0],nor[1],nor[2]);

			x = GL_map[i][j];
			y = GL_map[i][j+1];
			z = GL_map[i][j+2];
		
			glVertex3f(x/d,(1.6 * y + 425)/d,z/d);
			
			x = GL_map[i][j+15];
			y = GL_map[i][j+16];
			z = GL_map[i][j+17];
	
			glVertex3f(x/d,(1.6 * y + 425)/d,z/d);

			x = GL_map[(i == GL_map.size()-1) ? 0 : (i+1)][j+15];
			y = GL_map[(i == GL_map.size()-1) ? 0 : (i+1)][j+16];
			z = GL_map[(i == GL_map.size()-1) ? 0 : (i+1)][j+17];
		
			glVertex3f(x/d,(1.6 * y + 425)/d,z/d);

			x = GL_map[(i == GL_map.size()-1) ? 0 : (i+1)][j];
			y = GL_map[(i == GL_map.size()-1) ? 0 : (i+1)][j+1];
			z = GL_map[(i == GL_map.size()-1) ? 0 : (i+1)][j+2];

			glVertex3f(x/d,(1.6 * y + 425)/d,z/d);

			glEnd();
		}	
	}
	glFlush();
}
void buildCross()
{
	glDisable(GL_LIGHTING);
	glColor3f(1,1,0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(-10,0,0);
	glVertex3f(10,0,0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0,-10,0);
	glVertex3f(0,10,0);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0,0,-10);
	glVertex3f(0,0,10);
	glEnd();
	glFlush();
}
void buildCube(float size)
{
	glColor3f(1,0,0);
	glutWireCube(size);
	glFlush();
}
void Display()
{
	if(scan.finished == false)
	{
		scan.openCamera();
		scan.getCap();
	}

	if(translated == false)
	{
		//scan.processing();
		R_To_X(scan.map);
		writeFile_R(scan.map);
		writeFile_X(GL_map);
		cout<<"building...\n";
		scan.~startScan();
	}
	/*if (scan.finished == false)
	{
		GL_map_r.resize(180);
		for (int i = 0;i < GL_map_r.size();i++)
		{
			GL_map_r[i].resize(480);
		}
		loadfile_r("r.txt",GL_map_r);
	}
	if(translated == false)
	{
		R_To_X(scan.map);
	}*/

	//scan.openCamera();
	//scan.cameratest();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(-xRot,1,0,0);
	glRotatef(-yRot,0,1,0);
	glRotatef(zRot,0,0,1);

	if (mod == 1)
	{
		buildPointCloud();
		buildCross();
	}
	else if (mod == 2)
	{
		buildLines(k);
		buildCross();
	}
	else if (mod == 3)
	{
		buildObj();
		buildCross();
	}
}
void startOpen_GL(int argc,char*argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(300,0);
	glutCreateWindow("3D_Scaner Open_GL Builder");

	int nMainMenu=glutCreateMenu(ProcessMenu1);
	glutAddMenuEntry("点云",1);
	glutAddMenuEntry("线框",2);
	glutAddMenuEntry("实体",3);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutMouseFunc(Rollstart);
	glutMotionFunc(Roll);
//	glutTimerFunc(33,timerProc,1);
	myinit();
	Initinal();
	glutDisplayFunc(Display);

	glutMainLoop();
}