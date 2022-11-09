#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include "GL\freeglut.h"

using namespace std;

clock_t time1 = clock();
clock_t time2 = clock();
double dt;

void display();
void clamp(double& val, double min, double max);

double deltataime()
{
	time1 = time2;
	time2 = clock();
	return (double)((double)(time2 - time1) / (double)CLOCKS_PER_SEC);
}

void refreshviewport(double ps, double prp)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	double ws = ps * 3;
	double hs = ps * 3 * prp;
	
	gluOrtho2D(-ps * 1.5* prp, ps*1.5* prp, ps * 1.5 , -ps * 1.5 );
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

}

//писалка данных
class recorder
{
private:
	vector <double>dataset = {};
public:
	void writedata(double x)
	{
		dataset.push_back(x);
	};
	void reset()
	{
		dataset.clear();
	};
	void give_vector(vector<double>& v)
	{
		v.clear();
		int vs = 5000;
		int ip = vs - dataset.size();
		if (ip > 0)
		{
			for (int i = 0; i < ip; i++)
				v.push_back(0);
			for (int i = 0; i < dataset.size(); i++)
				v.push_back(dataset[i]);
		}
		else
		{
			for (int i = dataset.size() - vs; i < dataset.size(); i++)
				v.push_back(dataset[i]);
		}
	}

	void drawchart(double len)
	{
		double w = 1.5 * 2 * len;
		double h = 1.5 * 2 * len;
		double chx = - len;
		vector<double> dv = {};
		give_vector(dv);
		double stp = w / dv.size();
		glBegin(GL_LINES);
		  glColor3f(0.5f, 0.5f, 1);
		  glVertex2d(chx, -w/2);
		  glVertex2d(chx, w/2);
		glEnd();
		glBegin(GL_LINE_STRIP);
			glColor3f(0, 1, 1);
			for (int i = 0; i < dv.size(); i++)
			{
				glVertex2d(chx+dv[i]/5, (i*stp) - (h / 2));
			}
		glEnd();
	}

} cord;



//определяем класс маятника и спавним глобальный объект
class pendulum
{
public:
	pendulum() { 
	cang = 270; ares = 0; cspd = 0; 
	radius = 10; px = 0; py = 0; strlen = 0;
	acc = 0; f_m = 1;
	};
	void movependulum();
	void setang(double ang) { cang = ang; };
	void setres(double res) { ares = res; };
	void setspd(double spd) { cspd = spd; };
	void setrad(double rad) { radius = rad; };
	void setlen(double len) { strlen = len; };
	double getx() { return px; };
	double gety() { return py; };
	double getr() { return radius; };
	double getlen() { return strlen; };
private:
	double cang;
	double ares;
	double cspd;
	double radius;
	double px; double py;
	double ppx; double ppy;
	double f_m; double f_res;
	double pspd;
	double acc;
	double strlen;
	double calcairres();
	int spdv;
} pnd;

//функция отвечает за перерасчет положения маятника
void pendulum::movependulum()
{
	
	pspd = cspd;

	ppx = px; ppy = py;
	
	double dt = deltataime();
	clamp(dt, 0.0001, 1);

	//double cspdy = radius * 7 * 10 * dt;

	cang += cspd * dt;

	if (cang > 360)
		while (cang > 360) cang -= 360;
	if (cang < 0)
		while (cang < 0) cang += 360;

	px = strlen * cos(cang * 3.14 / 180);
	py = strlen * sin(cang * 3.14 / 180);

	double dpx = px - ppx;
	double dpy = py - ppy;

	// F_ar = dp / dt  =>  dp = F_ar * dt
	double p_ar = calcairres();
	double p_move = pow(radius,2) * 3.14 * 7 * cspd * sin(cang*3.14 / 180);
	double pc = abs(p_ar / (p_ar+f_m));
	clamp(pc, 0, 1);

	double cspd_y = cspd / sin(cang*3.14/180);

	cspd_y -= 10 * radius * 7 * dt;

	cspd = cspd_y * sin(cang*3.14 / 180);

	cspd += (-1) * (cspd * pc) * dt;

	acc =  (cspd - pspd)/dt;

	f_m = pow(radius, 2) * 3.14 * 7 * acc;
	

	printf("cang=%f at dt=%f; cspd = %f; pc = %f; p_ar=%f; py=%f\n", cang, dt, cspd, pc,p_ar,py);
}

double pendulum::calcairres()
{
	//Pv = c_x·S·v^2·ρ/2
	//where c_x = res; v = spd; ro = 1.225; S = 2*3.14*pr;
	return ares * cspd * cspd * 3.14 * radius * 1.225;
}

//функция ограничивает значение числа
void clamp(double& val, double min, double max)
{
	double tv = val;
	if (tv < min) tv = min;
	if (tv > max) tv = max;
	val = tv;
}

//функция отвечает за период бездействия
void idle()
{
	pnd.movependulum();
	display();
}

//изменение размера окна
void resize(int w, int h)
{
	double prop = (double)w / (double)h;
	printf("RESHAPED AT %f\n\n\n\n\n\n\n",prop);
	refreshviewport(pnd.getlen(),prop);
}


//функция отвечает за рендеринг
void display() {  
	
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(90, 0, 0, 1);

	double as = 20;
	
	glBegin(GL_LINES);
		glColor3f(0, 0, 1);
		glVertex2d(0, 0);
		glVertex2d(pnd.getlen(), 0);
	glEnd();

	glBegin(GL_LINE_STRIP);
	  glColor3f(1, 0, 0); 
	  for (int a=0; a<=360; a+=as)
	    glVertex2d(pnd.getx()+pnd.getr()*cos(a* 3.14 / 180), pnd.gety() + pnd.getr()*sin(a* 3.14 / 180));
	glEnd();

	glBegin(GL_LINES);
		glColor3f(1, 1, 1);
		glVertex2d(0, 0);
		glVertex2d(pnd.getx(), pnd.gety());		
	glEnd();

	cord.writedata(pnd.gety());
	cord.drawchart(pnd.getlen());

	glutSwapBuffers();
	

}


int main(int argc, char** argv) {  

	//исходные данные
	printf("Input starting pendulum speed\n");
	double spd = 0;
	cin >> spd;
	clamp(spd, -100, 100);
	printf("Input pendulum air resistance\n");
	double airres = 0;
	cin >> airres;
	clamp(airres, 0, 10);
	printf("Input starting pendulum string length\n");
	double ps = 0;
	cin >> ps;
	clamp(ps, -100, 100);

	pnd.setspd(spd);
	pnd.setres(airres);
	pnd.setlen(ps);

	//запуск glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);    
	glutInitWindowSize(800, 600);         
	glutInitWindowPosition(100, 100);    
	glutCreateWindow("Pendulum Simulation");
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
	glutDisplayFunc(display);           

	refreshviewport(ps,1.33);

	glutMainLoop();
					
	return 0;

}

