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

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

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
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
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
			c.scale = c.scale*1.5;
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

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
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

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
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

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
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



void SearchingNormal(double A[], double B[], double C[], double Normal[]) {
	double AB[] = { B[0] - A[0],B[1] - A[1],B[2] - A[2] };
	double AC[] = { C[0] - A[0],C[1] - A[1],C[2] - A[2] };
	double ab[] = {
					AB[1] * AC[2] - AB[2] * AC[1],
					AB[2] * AC[0] - AB[0] * AC[2],
					AB[0] * AC[1] - AB[1] * AC[0]
	};
	Normal[0] = ab[0];
	Normal[1] = ab[1];
	Normal[2] = ab[2];


}

void Render(OpenGL *ogl)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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
	double A[] = { 0,0,0 };
	double B[] = { 4,-5,0 };
	double C[] = { 8,-2,0 };
	double D[] = { 3,2,0 };
	double E[] = { 4,8,0 };
	double F[] = { 0,11,0 };
	double G[] = { -9,5,0 };
	double H[] = { -4,-4,0 };

	double A1[] = { 0,0,1 };
	double B1[] = { 4,-5,1 };
	double C1[] = { 8,-2,1 };
	double D1[] = { 3,2,1 };
	double E1[] = { 4,8,1 };
	double F1[] = { 0,11,1 };
	double G1[] = { -9,5,1 };
	double H1[] = { -4,-4,1 };

	double normal[] = { 0,0,0 };

	glBegin(GL_QUAD_STRIP);
	glColor3d(1, 0, 0);
	SearchingNormal(B, B1, A1, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(A);
	glVertex3dv(A1);
	glVertex3dv(B);
	glVertex3dv(B1);


	glColor3d(0, 1, 0);
	SearchingNormal(C, C1, B1, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(B);
	glVertex3dv(B1);
	glVertex3dv(C);
	glVertex3dv(C1);

	glColor3d(0, 0, 1);
	SearchingNormal(D, D1, C1, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(C);
	glVertex3dv(C1);
	glVertex3dv(D);
	glVertex3dv(D1);

	glColor3d(1, 0, 1);
	SearchingNormal(E, E1, D1, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(D);
	glVertex3dv(D1);
	glVertex3dv(E);
	glVertex3dv(E1);

	glColor3d(1, 0, 0);
	SearchingNormal(F, F1, E1, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(E);
	glVertex3dv(E1);
	glVertex3dv(F);
	glVertex3dv(F1);

	glColor3d(1, 1, 0);
	SearchingNormal(G, G1, F1, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(F);
	glVertex3dv(F1);
	glVertex3dv(G);
	glVertex3dv(G1);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
	glColor3d(1, 1, 0);
	SearchingNormal(A, A1, H1, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(H);
	glVertex3dv(H1);
	glVertex3dv(A);
	glVertex3dv(A1);
	glEnd();


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glBegin(GL_QUADS);  //������� ������

	glColor4d(0, 0, 1, 0.75);
	SearchingNormal(A1, B1, C1, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0, 1);
	SearchingNormal(A1, D1, E1, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(A1);
	glVertex3dv(D1);
	glVertex3dv(E1);
	glVertex3dv(F1);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0, 1);
	SearchingNormal(H1, A1, F1, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(A1);
	glVertex3dv(H1);
	glVertex3dv(G1);
	glVertex3dv(F1);
	glEnd();

	glBegin(GL_QUADS);  //������ ������
	glColor3d(0, 1, 1);
	glNormal3d(0, 0, -1);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 1, 1);
	glVertex3dv(A);
	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(F);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 1, 1);
	glVertex3dv(A);
	glVertex3dv(H);
	glVertex3dv(G);
	glVertex3dv(F);
	glEnd();




	double qwer1[] = { -11.65,0.4,0 };  //�������� ������ ���������� � ������
	double qwer2[] = { -11.65,0.4,1 };
	double qwer3[] = { -11.65,0.6,0 };
	double qwer4[] = { -11.65,0.6,1 };
	double qwercen[] = { -6.5, 0.5, 0 };
	double qwercen1[] = { -6.5, 0.5, 1 };
	glBegin(GL_TRIANGLE_STRIP);
	glColor3d(0, 0, 0);
	SearchingNormal(qwer4, qwer3, qwer2, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(qwer1);
	glVertex3dv(qwer2);
	glVertex3dv(qwer3);
	glVertex3dv(qwer4);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3d(0, 0, 0);
	SearchingNormal( qwer2, qwercen1, qwer4, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(qwer2);
	glVertex3dv(qwercen1);
	glVertex3dv(qwer4);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
	glColor3d(0, 0, 0);
	SearchingNormal(qwer3, qwercen, qwer1, normal);
	glNormal3d(normal[0], normal[1], normal[2]);
	glVertex3dv(qwer1);
	glVertex3dv(qwercen);
	glVertex3dv(qwer3);
	glEnd();

	double x = -20;
	glBegin(GL_TRIANGLE_STRIP);     //������� ������ ����� ����������
	glColor3d(0, 0, 0);
	double tukx = x;
	double tuky = -sqrt(26.5 - pow((x + 6.5), 2)) + 0.5;
	while (x <= 5)
	{

		double y = -sqrt(26.5 - pow((x + 6.5), 2)) + 0.5;
		if ((x <= -4) && (y <= 5))
		{
			double VUP1[] = { tukx, tuky,0 };
			double VUP2[] = { tukx,tuky,1 };
			double VUP3[] = { x,y,1 };
			SearchingNormal(VUP3, VUP2, VUP1, normal);
			glNormal3d(normal[0], normal[1], normal[2]);
			glVertex3d(x, y, 0);
			glVertex3d(x, y, 1);
			tuky = y;
			tukx = x;
		}
		x = x + 0.001;
	}
	glEnd();

	double u = -20;
	double tuku = u;
	tuky = sqrt(26.5 - pow((u + 6.5), 2)) + 0.5;
	glBegin(GL_TRIANGLE_STRIP);		//������� ������ ����� ����������
	glColor3d(0, 0, 0);
	while (u <= 5)
	{
		double y = sqrt(26.5 - pow((u + 6.5), 2)) + 0.5;
		if ((u <= -9) && (y <= 5))
		{
			double VUP1[] = { tuku, tuky,0 };
			double VUP2[] = { tuku,tuky,1 };
			double VUP3[] = { u,y,1 };
			SearchingNormal(VUP1, VUP2, VUP3, normal);
			glNormal3d(normal[0], normal[1], normal[2]);
			glVertex3d(u, y, 0);
			glVertex3d(u, y, 1);
			tuku = u;
			tuky = y;
		}
		u = u + 0.001;
	}
	glEnd();
	u = -20;
	glBegin(GL_TRIANGLE_STRIP);			//������� ������
	glColor3d(0, 0, 0);
	while (u <= 5)
	{

		double y = sqrt(26.5 - pow((u + 6.5), 2)) + 0.5;
		if ((u <= -9) && (y <= 5))
		{
			glVertex3d(u, y, 0);
			glVertex3d(-6.5, 0.5, 0);
		}
		u = u + 0.001;
	}
	glEnd();
	x = -20;
	glBegin(GL_TRIANGLE_STRIP);		//������� ������
	glColor3d(0, 0, 0);
	while (x <= 5)
	{

		double y = -sqrt(26.5 - pow((x + 6.5), 2)) + 0.5;
		if ((x <= -4) && (y <= 5))
		{
			glVertex3d(x, y, 0);
			glVertex3d(-6.5, 0.5, 0);
		}

		x = x + 0.001;
	}
	glEnd();

	u = -20;
	glBegin(GL_TRIANGLE_STRIP);		//������� ������
	glColor3d(0, 0, 0);
	while (u <= 5)
	{

		double y = sqrt(26.5 - pow((u + 6.5), 2)) + 0.5;
		if ((u <= -9) && (y <= 5))
		{
			glVertex3d(u, y, 1);
			glVertex3d(-6.5, 0.5, 1);
		}
		u = u + 0.001;
	}
	glEnd();
	x = -20;
	glBegin(GL_TRIANGLE_STRIP);		//������� ������
	glColor3d(0, 0, 0);
	while (x <= 5)
	{

		double y = -sqrt(26.5 - pow((x + 6.5), 2)) + 0.5;
		if ((x <= -4) && (y <= 5))
		{
			glVertex3d(x, y, 1);
			glVertex3d(-6.5, 0.5, 1);
		}
		x = x + 0.001;
	}
	glEnd();


	

	//������ ��������� ���������� ��������
	double A5[2] = { -4, -4 };
	double B5[2] = { 4, -4 };
	double C5[2] = { 4, 4 };
	double D5[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A5);
	glTexCoord2d(1, 0);
	glVertex2dv(B5);
	glTexCoord2d(1, 1);
	glVertex2dv(C5);
	glTexCoord2d(0, 1);
	glVertex2dv(D5);

	glEnd();
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
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

