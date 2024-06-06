#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"
#include <math.h>
#include <Windows.h>
#define PI 3.14159265
bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 12);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  





	double A[] = { 0, 0, 0 };
	double B[] = { 4, 0, 0 };
	double C[] = { 5, -6, 0 };
	double D[] = { -2, -10, 0 };
	double E[] = { -7, -7, 0 };
	double F[] = { -3, -1, 0 };
	double G[] = { -7, 4, 0 };
	double H[] = { -2, 8, 0 };

	double a[] = { 0, 0, 10 };
	double b[] = { 4, 0, 10 };
	double c[] = { 5, -6, 10 };
	double d[] = { -2, -10, 10 };
	double e[] = { -7, -7, 10 };
	double f[] = { -3, -1, 10 };
	double g[] = { -7, 4, 10 };
	double h[] = { -2, 8, 10 };

	glBegin(GL_POLYGON);   //������1
	glNormal3d(0, 0, -1);
	glColor3d(0, 0, 1);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(F);
	glEnd();
	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glColor3d(0.3, 0.8, 0.1);
	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(F);
	glEnd();
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, -1);
	glColor3d(0.3, 0.8, 0.1);
	glVertex3dv(F);
	glVertex3dv(G);
	glVertex3dv(H);
	glVertex3dv(A);
	glEnd();

	
	

	glBegin(GL_POLYGON);   //�����
	glNormal3d(0, 1, 0);
	glVertex3dv(A);
	glVertex3dv(a);
	glVertex3dv(b);
	glVertex3dv(B);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3d(60 / (10 * sqrt(37)), 10 / (10 * sqrt(37)), 0);
	glVertex3dv(B);
	glVertex3dv(b);
	glVertex3dv(c);
	glVertex3dv(C);
	glEnd();

	/*glBegin(GL_POLYGON);
	glColor3d(0.2, 0.7, 0.7);
	glVertex3dv(C);
	glVertex3dv(c);
	glVertex3dv(d);
	glVertex3dv(D);
	glEnd();*/

	glBegin(GL_POLYGON);
	glNormal3d(-30 / (10 * sqrt(34)), -50 / (10 * sqrt(34)), 0);
	glVertex3dv(D);
	glVertex3dv(d);
	glVertex3dv(e);
	glVertex3dv(E);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3d(-60 / (20 * sqrt(13)), 40 / (20 * sqrt(13)), 0);
	glColor3d(0.7, 0.7, 0.7);
	glVertex3dv(E);
	glVertex3dv(e);
	glVertex3dv(f);
	glVertex3dv(F);
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3d(-50 / (10 * sqrt(41)), -40 / (10 * sqrt(41)), 0);
	glVertex3dv(F);
	glVertex3dv(f);
	glVertex3dv(g);
	glVertex3dv(G);
	glEnd();

	/*glBegin(GL_POLYGON);       //�����, � ������� ���� ������� ����������
	glVertex3dv(G);
	glVertex3dv(g);
	glVertex3dv(h);
	glVertex3dv(H);
	glEnd();*/

	glBegin(GL_POLYGON);
	glNormal3d(80 / (20 * sqrt(17)), 20 / (20 * sqrt(17)), 0);
	glVertex3dv(H);
	glVertex3dv(h);
	glVertex3dv(a);
	glVertex3dv(A);
	glEnd();

	//���������� ���
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, -1);
	glVertex3dv(A);
	glVertex3dv(G);
	for (float i = 40; i < 250; i++) {
		double K[] = { -4.5 + sqrt(10.25) * cos((PI * i) / 180), 6 + sqrt(10.25) * sin((PI * i) / 180), 0 };
		if (((-4.5 + sqrt(10.25) * cos((PI * i) / 180)) <= -2 && (6 + sqrt(10.25) * sin((PI * i) / 180)) <= 8) || ((-4.5 + sqrt(10.25) * cos((PI * i) / 180)) >= -7 && (6 + sqrt(10.25) * sin((PI * i) / 180)) >= 4))
			glVertex3dv(K);
	}
	glVertex3dv(H);
	glEnd();
	
	//���������� �����
	glBegin(GL_TRIANGLE_STRIP);
	glColor3d(0, 0.7, 0);
	for (float i = 38.7; i < 220.7; i++) {
		double K[] = { -4.5 + sqrt(10.25) * cos((PI * i) / 180), 6 + sqrt(10.25) * sin((PI * i) / 180), 0 };
		double L[] = { -4.5 + sqrt(10.25) * cos((PI * i) / 180), 6 + sqrt(10.25) * sin((PI * i) / 180), 10 };
		if (((-4.5 + sqrt(10.25) * cos((PI * i) / 180)) <= -2 && (6 + sqrt(10.25) * sin((PI * i) / 180)) <= 8) || ((-4.5 + sqrt(10.25) * cos((PI * i) / 180)) >= -7 && (6 + sqrt(10.25) * sin((PI * i) / 180)) >= 4)) {
			double	ax = (-4.5 + sqrt(10.25) * cos((PI * (i+1)) / 180) -(-4.5 + sqrt(10.25) * cos((PI * i) / 180)));
			double ay = (6 + sqrt(10.25) * sin((PI * (i+1)) / 180) - (6 + sqrt(10.25) * sin((PI * i) / 180)));
			double nx = ay * 10;
			double ny = -ax * 10;
			glNormal3d(nx / sqrt(nx * nx + ny * ny), ny / sqrt(nx * nx + ny * ny), 0);
			glVertex3dv(K);
			glVertex3dv(L);
		}

	}
	glVertex3dv(G);
	glVertex3dv(g);
	glEnd();

	//����������
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, -1);
	glColor3d(0, 1, 0);
	glVertex3dv(F);
	glVertex3dv(C);
	for (float i = 60; i < 180; i++) {
		double K[] = { 2.65 + 4.63923 * cos((PI * i) / 180), -10 + 4.63923 * sin((PI * i) / 180), 0 };
		if (((2.65 + 4.63923 * cos((PI * i) / 180)) < 5 && (-10 + 4.63923 * sin((PI * i) / 180)) > -10) || ((2.65 + 4.63923 * cos((PI * i) / 180)) > -2 && (-10 + 4.63923 * sin((PI * i) / 180)) < -6))
			glVertex3dv(K);
	}
	glVertex3dv(D);
	glEnd();
	//���������� �����
	glBegin(GL_TRIANGLE_STRIP);
	glColor3d(0, 0.7, 0);
	for (float i = 59; i < 180; i++) {
		double K[] = { 2.65 + 4.63923 * cos((PI * i) / 180), -10 + 4.63923 * sin((PI * i) / 180), 0 };
		double L[] = { 2.65 + 4.63923 * cos((PI * i) / 180), -10 + 4.63923 * sin((PI * i) / 180), 10 };
		if (((2.65 + 4.63923 * cos((PI * i) / 180)) < 5 && (-10 + 4.63923 * sin((PI * i) / 180)) > -10) || ((2.65 + 4.63923 * cos((PI * i) / 180)) > -2 && (-10 + 4.63923 * sin((PI * i) / 180)) < -6)) {
			double	ax = (2.65 + 4.63923 * cos((PI * (i + 1)) / 180) - (2.65 + 4.63923 * cos((PI * i) / 180)));
			double ay = (-10 + 4.63923 * sin((PI * (i + 1)) / 180)-( - 10 + 4.63923 * sin((PI * i) / 180)));
			double nx = ay * 10;
			double ny = -ax * 10;
			glNormal3d(-nx/sqrt(nx*nx+ny*ny),-ny/sqrt(nx * nx + ny * ny), 0);
			glVertex3dv(K);
			glVertex3dv(L);
		}

	}
	glVertex3dv(D);
	glVertex3dv(d);
	glEnd();

	//������ ��������� ���������� ��������
	double A1[3] = { -2.05,-10.05,-0.05 };
	double B1[3] = { -2.05,-10.05,10.05 };
	double C1[3] = { -7.05,-7.05,10.05 };
	double D1[3] = { -7.05,-7.05,-0.05 };
	double A2[3] = { 0, 0.05, 0 };
	double B2[3] = { 0, 0.05, 10 };
	double C2[3] = { 4, 0.05, 10 };
	double D2[3] = { 4, 0.05, 0 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_POLYGON);
	glNormal3d(-30 / (10 * sqrt(34)), -50 / (10 * sqrt(34)), 0);
	glTexCoord3d(0, 0, 0);
	glVertex3dv(A1);
	glTexCoord3d(1, 0, 0);
	glVertex3dv(B1);
	glTexCoord3d(1, 1, 0);
	glVertex3dv(C1);
	glTexCoord3d(0, 1, 0);
	glVertex3dv(D1);
	glEnd();
	glBegin(GL_POLYGON);
	glNormal3d(0, 1, 0);
	glTexCoord3d(0, 0, 0);
	glVertex3dv(A2);
	glTexCoord3d(1, 0, 0);
	glVertex3dv(B2);
	glTexCoord3d(1, 1, 0);
	glVertex3dv(C2);
	glTexCoord3d(0, 1, 0);
	glVertex3dv(D2);

	glEnd();

	//����
	float xcos, ysin, tx, ty, angle, radian, X, Y, R = 2;
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_POLYGON);
	glColor4f(0.7, 0.9, 0.7, 1.0);
	glNormal3d(0, 0, 29);
	for (angle = 0.0; angle < 360.0; angle += 2.0) {
		radian = angle * (PI / 180.0f);
		xcos = (float)cos(radian);
		ysin = (float)sin(radian);
		X = xcos * R + 11;
		Y = ysin * R + 11;
		tx = xcos * 0.5 + 0.5;
		ty = ysin * 0.5 + 0.5;
		glTexCoord2f(tx, ty);
		glVertex3f(X, Y, 2);
	}
	glEnd();
	//��������������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_POLYGON); //������ 2
	glNormal3d(0, 0, 1);
	glColor4d(0.5, 0.4, 0.9, 0.9);
	glVertex3dv(a);
	glVertex3dv(b);
	glVertex3dv(c);
	glVertex3dv(f);
	glEnd();
	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, 1);
	glVertex3dv(d);
	glVertex3dv(e);
	glVertex3dv(f);
	glEnd();
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glVertex3dv(f);
	glVertex3dv(g);
	glVertex3dv(h);
	glVertex3dv(a);
	glEnd();
	//���������� ������
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, 1);
	//glColor3d(0, 1, 0);
	glVertex3dv(f);
	glVertex3dv(c);
	for (float i = 60; i < 180; i++) {
		double K[] = { 2.65 + 4.63923 * cos((PI * i) / 180), -10 + 4.63923 * sin((PI * i) / 180), 10 };
		if (((2.65 + 4.63923 * cos((PI * i) / 180)) < 5 && (-10 + 4.63923 * sin((PI * i) / 180)) > -10) || ((2.65 + 4.63923 * cos((PI * i) / 180)) > -2 && (-10 + 4.63923 * sin((PI * i) / 180)) < -6))
			glVertex3dv(K);
	}
	glVertex3dv(d);
	glEnd();
	//���������� ������
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0, 0, 1);
	glVertex3dv(a);
	glVertex3dv(h);
	for (float i = 40; i < 350; i++) {
		double K[] = { -4.5 + sqrt(10.25) * cos((PI * i) / 180), 6 + sqrt(10.25) * sin((PI * i) / 180), 10 };
		if (((-4.5 + sqrt(10.25) * cos((PI * i) / 180)) <= -2 && (6 + sqrt(10.25) * sin((PI * i) / 180)) <= 8) || ((-4.5 + sqrt(10.25) * cos((PI * i) / 180)) >= -7 && (6 + sqrt(10.25) * sin((PI * i) / 180)) >= 4))
			glVertex3dv(K);
	}
	glVertex3dv(g);
	glEnd();
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//����� ��������� ���������� ��������


   //��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}