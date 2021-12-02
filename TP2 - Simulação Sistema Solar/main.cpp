// Inclusão das bibliotecas necessárias para o Sistema Solar
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#include "imageloader.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "glew32.lib")

using namespace std;

// Classe planeta
class Planeta {

public:
	float raio, distancia, orbita, velocidadeOrbita, inclinaEixo, animaEixo;
	// Criação do construtor usando raio, distancia, velocidade e inclinação do eixo
	Planeta(float _raio, float _distancia, float _orbita, float _velocidadeOrbita, float _inclinaEixo, float _animaEixo) {
		raio = _raio;
		distancia = _distancia;
		orbita = _orbita;
		velocidadeOrbita = _velocidadeOrbita;
		inclinaEixo = _inclinaEixo;
		animaEixo = _animaEixo;
	}

	// Função para desenhar as órbitas pequenas. Os traços das órbitas são brancos
	// possuem rotação e são desenhados com um raio interno e um raio externo baseado
	// na distância entre o satélite (lua) e o seu planeta
	void desenhaOrbitaPequena(void) {
		glPushMatrix();
		glColor3ub(255, 255, 255);
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glutWireTorus(0.001, distancia, 100.0, 100.0);
		glPopMatrix();
	}

	// As luas recebem uma função quádrica para o gluSphere. Todas as luas recebem
	// coloração branca, uma animação de órbita e a distância até o seu planeta
	// para a animação
	void desenhaLua(void) {
		GLUquadricObj *quadric;
		quadric = gluNewQuadric();
		glPushMatrix();
		glColor3ub(255, 255, 255);
		glRotatef(orbita, 0.0, 1.0, 0.0);
		glTranslatef(distancia, 0.0, 0.0);
		gluSphere(quadric, raio, 20.0, 20.0);
		glPopMatrix();
	}

};

// Criação do Sol, Planeta e Estrelas
Planeta sol(5.0, 0, 0, 0, 0, 0);				//	Sol
Planeta mer(1.0, 7, 0, 4.74, 02.11, 0);			//	Mercúrio
Planeta ven(1.5, 11, 0, 3.50, 177.0, 0);		//	Venus
Planeta terra(2.0, 16, 0, 2.98, 23.44, 0);		//	Terra
Planeta marte(1.2, 21, 0, 2.41, 25.00, 0);		//	Marte
Planeta jup(3.5, 28, 0, 1.31, 03.13, 0);		//	Jupiter
Planeta sat(3.0, 37, 0, 0.97, 26.70, 0);		//	Saturno
Planeta ura(2.5, 45.5, 0, 0.68, 97.77, 0);		//	Urano
Planeta net(2.3, 53.6, 0, 0.54, 28.32, 0);		//	Netuno
Planeta plu(0.3, 59, 0, 0.47, 119.6, 0);		//	Plutão

// Criação dos satélites
Planeta lua(.40, 3, 0, 5.40, 0, 0);			//	Lua			(Terra)
Planeta fob(.20, 1.8, 0, 2.30, 0, 0);		//	Fobos		(Marte)
Planeta dei(.24, 2.4, 0, 3.60, 0, 0);		//	Deimos		(Marte)
Planeta eur(.24, 4, 0, 4.40, 0, 0);			//	Europa		(Júpiter)
Planeta gan(.24, 4.7, 0, 5.00, 0, 0);		//	Ganímedes	(Júpiter)
Planeta cal(.24, 5.3, 0, 2.30, 0, 0);		//	Calisto		(Júpiter)
Planeta tit(.75, 3.7, 0, 2.40, 0, 0);		//	Titã		(Saturnp)
Planeta nix(.10, 1.5, 0, 5.00, 0, 0);		//	Nix			(Plutão)
Planeta puc(.26, 2.9, 0, 7.00, 0, 0);		//	Puck		(Urano)
Planeta tri(.36, 3.2, 0, 3.40, 0, 0);		//	Tritão		(Netuno)

// Variáveis globais de controle para animação, ativação das órbitas,
// mudança de ângulo de câmera, contagem de frames, ativação
// de nomes, zoom e cena de logotipo.
int animado = 0;
int orbitaGrandeAtivo = 1;
int orbitaPequenaAtivo = 1;
int luasAtivo = 1;
int mudaCamera = 0;
int contagemFrame = 0;
int nomesAtivo = 0;
int zoom = 50;
int cenaLogo = 1;

// Configuração da iluminação.
float posLuz[] = { 0.0, 0.0, -50.0, 1.0 };	// Posição da luz.
static float anguloLuz = 180;				// Ângulo da luz.
float direcaoLuz[] = { 1.0, 0.0, 0.0 };		// Direção da luz.
static float expoenteLuz = 5.0;				// Fator que influenciará na atenuação da luz.

// Função para o carregamento de texturas dos planetas, sol e satélites.
GLuint carregaTextura(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	return textureId;
}

// Declaração dos nomes das texturas.
GLuint solTextura, merTextura, venTextura, terraTextura, marteTextura, jupTextura, satTextura, uraTextura, netTextura, pluTextura, estrelaTextura, logoTextura, luaTextura;

// Função que permite o texto aparecer na cena. Usado para o nome dos planetas.
void escreveNome(void *font, char *string)
{
	char *c;
	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Função de configuração do Sistema Solar
void configura(void) {

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	// Configurando texturas dos objetos.
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	// Carregamento das texturas presentes no diretório.
	Image* estrela = loadBMP("estrelas.bmp");		estrelaTextura = carregaTextura(estrela);				delete estrela;
	Image* sol = loadBMP("sol.bmp");				solTextura = carregaTextura(sol);						delete sol;
	Image* mer = loadBMP("mercurio.bmp");			merTextura = carregaTextura(mer);						delete mer;
	Image* ven = loadBMP("venus.bmp");				venTextura = carregaTextura(ven);						delete ven;
	Image* terra = loadBMP("terra.bmp");			terraTextura = carregaTextura(terra);					delete terra;
	Image* marte = loadBMP("marte.bmp");			marteTextura = carregaTextura(marte);					delete marte;
	Image* jup = loadBMP("jupiter.bmp");			jupTextura = carregaTextura(jup);						delete jup;
	Image* sat = loadBMP("saturno.bmp");			satTextura = carregaTextura(sat);						delete sat;
	Image* ura = loadBMP("urano.bmp");				uraTextura = carregaTextura(ura);						delete ura;
	Image* net = loadBMP("netuno.bmp");				netTextura = carregaTextura(net);						delete net;
	Image* plu = loadBMP("plutao.bmp");				pluTextura = carregaTextura(plu);						delete plu;
	Image* logo = loadBMP("logo.bmp");				logoTextura = carregaTextura(logo);						delete logo;
	

	// Configuração da iluminação
	glEnable(GL_LIGHTING);

	float luzAmb[] = { 0.0, 0.0, 0.0, 0.5 };
	float luzDifESpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float globAmb[] = { 0.3, 0.3, 0.3, 0.3 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifESpec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzDifESpec);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_POSITION, posLuz);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, anguloLuz);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direcaoLuz);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, expoenteLuz);
}

// Configuração da iluminação
void ativaLuz() {
	glEnable(GL_LIGHTING);

	float luzAmb[] = { 0.0, 0.0, 0.0, 0.5 };
	float luzDifESpec[] = { 1.0, 1.0, 1.0, 1.0 };
	float globAmb[] = { 0.3, 0.3, 0.3, 0.3 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifESpec);
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzDifESpec);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_POSITION, posLuz);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, anguloLuz);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direcaoLuz);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, expoenteLuz);
}

// Função que cria as rotas orbitais.
void rotaOrbita(void) {

	glPushMatrix();
	glColor3ub(255, 255, 255);
	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	glutWireTorus(0.001, mer.distancia, 100.0, 100.0);
	glutWireTorus(0.001, ven.distancia, 100.0, 100.0);
	glutWireTorus(0.001, terra.distancia, 100.0, 100.0);
	glutWireTorus(0.001, marte.distancia, 100.0, 100.0);
	glutWireTorus(0.001, jup.distancia, 100.0, 100.0);
	glutWireTorus(0.001, sat.distancia, 100.0, 100.0);
	glutWireTorus(0.001, ura.distancia, 100.0, 100.0);
	glutWireTorus(0.001, net.distancia, 100.0, 100.0);
	glutWireTorus(0.001, plu.distancia, 100.0, 100.0);

	glPopMatrix();
}

// Função para desenhar a tela de Logo
void desenhaLogo(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, logoTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); 
	glVertex3f(-100, -100, -100);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(100, -100, -100);

	glTexCoord2f(1.0, 1.0); 
	glVertex3f(100, 100, -100);

	glTexCoord2f(0.0, 1.0); 
	glVertex3f(-100, 100, -100);
	glEnd();

	glutSwapBuffers();
}

// Função que desenha a cena do Sistema Solar
void desenhaCena(void) {

	contagemFrame++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Controle de zoom da câmera com Scroll do mouse, para aproximar e afastar
	if (mudaCamera == 0) {
		gluLookAt(0.0, zoom, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}
	if (mudaCamera == 1) {
		gluLookAt(0.0, 0.0, zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}
	if (mudaCamera == 2) {
		gluLookAt(0.0, zoom, 0.00001, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}

	// Estrutura 'if' para ativar e desativar o desenho das órbitas planetárias
	if (orbitaGrandeAtivo == 1) {
		rotaOrbita();
	}

	GLUquadric *quadric;
	quadric = gluNewQuadric();

	// Criação do Sol
	glPushMatrix();
	glRotatef(sol.orbita, 0.0, 1.0, 0.0);
	glTranslatef(sol.distancia, 0.0, 0.0);

	// Estrutura 'if' para ativar a exibição dos nomes dos planetas
	if (nomesAtivo == 1) {
		glRasterPos3f(-1.2, 7.0, 0.0);
		glColor3ub(255, 255, 255);
		escreveNome(GLUT_BITMAP_HELVETICA_12, "Sol");
	}

	// Posicionamento do Sol, centralizando ele na cena
	glPushMatrix();
	glRotatef(sol.inclinaEixo, 1.0, 0.0, 0.0);
	glRotatef(sol.animaEixo, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	// Acionamento de texturas
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, solTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);

	// Ativação da esfera com 100 fatias, para transmitir maior realismo
	gluSphere(quadric, sol.raio, 100.0, 100.0);

	// Desativação da textura
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	glPopMatrix();

	// Todos os outros planetas seguirão a mesma lógica implementada
	// no código referente ao sol, porém com as devidas mudanças
	// referenciais aplicadas.

	// Mercúrio
	glPushMatrix();
	glRotatef(mer.orbita, 0.0, 1.0, 0.0);
	glTranslatef(mer.distancia, 0.0, 0.0);

	if (nomesAtivo == 1) {
		glRasterPos3f(0.0, 3, 0.0);
		glColor3ub(255, 255, 255);
		escreveNome(GLUT_BITMAP_HELVETICA_12, "Mercúrio");
	}

	glPushMatrix();
	glRotatef(mer.inclinaEixo, 1.0, 0.0, 0.0);
	glRotatef(mer.animaEixo, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, merTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);

	gluSphere(quadric, mer.raio, 100.0, 100.0);

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	glPopMatrix();

	// Vênus
	glPushMatrix();
	glRotatef(ven.orbita, 0.0, 1.0, 0.0);
	glTranslatef(ven.distancia, 0.0, 0.0);

	if (nomesAtivo == 1) {
		glRasterPos3f(0.0, 3, 0.0);
		glColor3ub(255, 255, 255);
		escreveNome(GLUT_BITMAP_HELVETICA_12, "Venus");
	}

	glPushMatrix();
	glRotatef(ven.inclinaEixo, 1.0, 0.0, 0.0);
	glRotatef(ven.animaEixo, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, venTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);

	gluSphere(quadric, ven.raio, 100.0, 100.0);

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	glPopMatrix();

	// Código para planetas que possuem satélites:

		// Como já existem funções de classes que dizem ao OpenGL como
		// desenhar os rastros das órbitas e os satélites, elas serão
		// utilizadas dentro de estrutras 'if' para que seja possível
		// mantê-las habilitadas.

	// Criação da Terra, órbitas e Lua
	glPushMatrix();
	glRotatef(terra.orbita, 0.0, 1.0, 0.0);
	glTranslatef(terra.distancia, 0.0, 0.0);

	if (nomesAtivo == 1) {
		glRasterPos3f(0.0, 3, 0.0);
		glColor3ub(255, 255, 255);
		escreveNome(GLUT_BITMAP_HELVETICA_12, "Terra");
	}

	glPushMatrix();
	glRotatef(terra.inclinaEixo, 1.0, 0.0, 0.0);
	glRotatef(terra.animaEixo, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, terraTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);

	gluSphere(quadric, terra.raio, 100.0, 100.0);

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	// Estrutura 'if' para habilitar o desenho das órbitas lunares
	if (orbitaPequenaAtivo == 1) {
		lua.desenhaOrbitaPequena();
	}
	// Estrutura 'if' para habilitar o desenho dos satélites
	if (luasAtivo == 1) {
		lua.desenhaLua();
	}
	glPopMatrix();

	// Criação de Marte, suas órbitas e satélites
	glPushMatrix();
	glRotatef(marte.orbita, 0.0, 1.0, 0.0);
	glTranslatef(marte.distancia, 0.0, 0.0);

	if (nomesAtivo == 1) {
		glRasterPos3f(0.0, 3, 0.0);
		glColor3ub(255, 255, 255);
		escreveNome(GLUT_BITMAP_HELVETICA_12, "Marte");
	}

	glPushMatrix();
	glRotatef(marte.inclinaEixo, 1.0, 0.0, 0.0);
	glRotatef(marte.animaEixo, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, marteTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);

	gluSphere(quadric, marte.raio, 100.0, 100.0);

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	if (orbitaPequenaAtivo == 1) {
		fob.desenhaOrbitaPequena();
		dei.desenhaOrbitaPequena();
	}
	if (luasAtivo == 1) {
		fob.desenhaLua();
		dei.desenhaLua();
	}
	glPopMatrix();

	// Criação de Júpiter, suas órbitas e satélites
	glPushMatrix();
	glRotatef(jup.orbita, 0.0, 1.0, 0.0);
	glTranslatef(jup.distancia, 0.0, 0.0);

	if (nomesAtivo == 1) {
		glRasterPos3f(0.0, 4.4, 0.0);
		glColor3ub(255, 255, 255);
		escreveNome(GLUT_BITMAP_HELVETICA_12, "Jupiter");
	}

	glPushMatrix();
	glRotatef(jup.inclinaEixo, 1.0, 0.0, 0.0);
	glRotatef(jup.animaEixo, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, jupTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);

	gluSphere(quadric, jup.raio, 100.0, 100.0);

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	if (orbitaPequenaAtivo == 1) {
		eur.desenhaOrbitaPequena();
		gan.desenhaOrbitaPequena();
		cal.desenhaOrbitaPequena();
	}
	if (luasAtivo == 1) {
		eur.desenhaLua();
		gan.desenhaLua();
		cal.desenhaLua();
	}
	glPopMatrix();

	// Criação de Saturno, suas órbitas e luas
	glPushMatrix();
	glRotatef(sat.orbita, 0.0, 1.0, 0.0);
	glTranslatef(sat.distancia, 0.0, 0.0);

	if (nomesAtivo == 1) {
		glRasterPos3f(0.0, 4.4, 0.0);
		glColor3ub(255, 255, 255);
		escreveNome(GLUT_BITMAP_HELVETICA_12, "Saturno");
	}

	glPushMatrix();
	glRotatef(sat.inclinaEixo, 1.0, 0.0, 0.0);
	glRotatef(sat.animaEixo, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, satTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);

	gluSphere(quadric, sat.raio, 100.0, 100.0);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	// Criação do anel de Saturno
	glPushMatrix();
	glColor3ub(255, 218, 185);  //255, 222, 173
	glRotatef(-63.0, 1.0, 0.0, 0.0);
	glutWireTorus(0.2, 6.0, 30.0, 30.0);
	glutWireTorus(0.4, 5.0, 30.0, 30.0);

	glPopMatrix();

	if (orbitaPequenaAtivo == 1) {
		tit.desenhaOrbitaPequena();
	}
	if (luasAtivo == 1) {
		tit.desenhaLua();
	}
	glPopMatrix();


	// Correção de sombreamento
	glColor3ub(255, 255, 255);

	// Criação de Urano, suas órbitas e satélites
	glPushMatrix();
	glRotatef(ura.orbita, 0.0, 1.0, 0.0);
	glTranslatef(ura.distancia, 0.0, 0.0);

	if (nomesAtivo == 1) {
		glRasterPos3f(0.0, 4.4, 0.0);
		glColor3ub(255, 255, 255);
		escreveNome(GLUT_BITMAP_HELVETICA_12, "Urano");
	}

	glPushMatrix();
	glRotatef(ura.inclinaEixo, 1.0, 0.0, 0.0);
	glRotatef(ura.animaEixo, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, uraTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);

	gluSphere(quadric, ura.raio, 100.0, 100.0);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	//Criação do anel de Urano
	glPushMatrix();
	glColor3ub(220, 220, 220);
	glRotatef(-20.0, 0.0, 0.0, 0.0);
	glutWireTorus(0.3, 3.9, 60.0, 60.0);

	glPopMatrix();

	if (orbitaPequenaAtivo == 1) {
		puc.desenhaOrbitaPequena();
	}
	if (luasAtivo == 1) {
		puc.desenhaLua();
	}
	glPopMatrix();

	// Criação de Netuno, suas órbitas e luas
	glPushMatrix();
	glRotatef(net.orbita, 0.0, 1.0, 0.0);
	glTranslatef(net.distancia, 0.0, 0.0);

	if (nomesAtivo == 1) {
		glRasterPos3f(0.0, 4.4, 0.0);
		glColor3ub(255, 255, 255);
		escreveNome(GLUT_BITMAP_HELVETICA_12, "Netuno");
	}

	glPushMatrix();
	glRotatef(net.inclinaEixo, 1.0, 0.0, 0.0);
	glRotatef(net.animaEixo, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, netTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);

	gluSphere(quadric, net.raio, 100.0, 100.0);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	if (orbitaPequenaAtivo == 1) {
		tri.desenhaOrbitaPequena();
	}
	if (luasAtivo == 1) {
		tri.desenhaLua();
	}
	glPopMatrix();

	// Criação de Plutão, suas órbitas e luas
	glPushMatrix();
	glRotatef(plu.orbita, 0.0, 1.0, 0.0);
	glTranslatef(plu.distancia, 0.0, 0.0);

	if (nomesAtivo == 1) {
		glRasterPos3f(0.0, 3.0, 0.0);
		glColor3ub(255, 255, 255);
		escreveNome(GLUT_BITMAP_HELVETICA_12, "Plutao");
	}

	glPushMatrix();
	glRotatef(plu.inclinaEixo, 1.0, 0.0, 0.0);
	glRotatef(plu.animaEixo, 0.0, 1.0, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, pluTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	gluQuadricTexture(quadric, 1);

	gluSphere(quadric, plu.raio, 100.0, 100.0);

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	if (orbitaPequenaAtivo == 1) {
		nix.desenhaOrbitaPequena();
	}
	if (luasAtivo == 1) {
		nix.desenhaLua();
	}
	glPopMatrix();

	// Criação das estrelas
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, estrelaTextura);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBegin(GL_POLYGON);
	glTexCoord2f(-1.0, 0.0); glVertex3f(-200, -200, -100);
	glTexCoord2f(2.0, 0.0); glVertex3f(200, -200, -100);
	glTexCoord2f(2.0, 2.0); glVertex3f(200, 200, -100);
	glTexCoord2f(-1.0, 2.0); glVertex3f(-200, 200, -100);
	glEnd();

	// Texturas das estrelas
	glBindTexture(GL_TEXTURE_2D, estrelaTextura);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-200, -83, 200);
	glTexCoord2f(8.0, 0.0); glVertex3f(200, -83, 200);
	glTexCoord2f(8.0, 8.0); glVertex3f(200, -83, -200);
	glTexCoord2f(0.0, 8.0); glVertex3f(-200, -83, -200);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	glutSwapBuffers();
}

// Função que o glutDisplayFunc utilizará para determinar
// qual cena deve ser desenhada. Uma estrutura 'if' será
// utilizada para que a simulação inicie após o usuário
// clicar com o mouse.
void desenhaCenaEmOrdem(void) {
	if (cenaLogo == 1) {
		desenhaLogo();
	}
	else {
		desenhaCena();
	}
}

// Função para redimensionar a cena, utilizaremos o gluPerspective
// para que tudo que esteja na cena seja visível e não seja distorcido.
void redimensiona(int l, int a) {
	glViewport(0, 0, l, a);

	float aspect = (float)l / (float)a;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0, aspect, 0.1, 500);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity;
}

// Função que fará a animação dos planetas rotacioanndo ao redor do Sol.
void animar(int n) {

	if (animado) {

		mer.orbita += mer.velocidadeOrbita;
		ven.orbita += ven.velocidadeOrbita;
		terra.orbita += terra.velocidadeOrbita;
		marte.orbita += marte.velocidadeOrbita;
		jup.orbita += jup.velocidadeOrbita;
		sat.orbita += sat.velocidadeOrbita;
		ura.orbita += ura.velocidadeOrbita;
		net.orbita += net.velocidadeOrbita;
		plu.orbita += plu.velocidadeOrbita;
		lua.orbita += lua.velocidadeOrbita;
		fob.orbita += fob.velocidadeOrbita;
		dei.orbita += dei.velocidadeOrbita;
		eur.orbita += eur.velocidadeOrbita;
		gan.orbita += gan.velocidadeOrbita;
		cal.orbita += cal.velocidadeOrbita;
		tit.orbita += tit.velocidadeOrbita;
		nix.orbita += nix.velocidadeOrbita;
		puc.orbita += puc.velocidadeOrbita;
		tri.orbita += tri.velocidadeOrbita;

		// Estrutura 'if' que verificará se o objeto já realizou uma órbita de 360 graus,
		// ou seja, deu uma volta completa em torno do Sol, e habilita que a
		// animação continue de maneira suave.
		if (mer, ven, terra, marte, jup, sat, ura, net, plu, lua, fob, dei, eur, gan, cal, tit, nix, puc, tri.orbita > 360.0) {
			mer, ven, terra, marte, jup, sat, ura, net, plu, lua, fob, dei, eur, gan, cal, tit, nix, puc, tri.orbita -= 360.0;
		}

		mer.animaEixo += 10.0;
		ven.animaEixo += 10.0;
		terra.animaEixo += 10.0;
		marte.animaEixo += 10.0;
		jup.animaEixo += 10.0;
		sat.animaEixo += 10.0;
		ura.animaEixo += 10.0;
		net.animaEixo += 10.0;
		plu.animaEixo += 10.0;

		if (mer, ven, terra, marte, jup, sat, ura, net, plu.animaEixo > 360.0) {
			mer, ven, terra, marte, jup, sat, ura, net, plu.animaEixo -= 360.0;
		}

		glutPostRedisplay();

		// A cena irá atualizar a cada 24 milissegundos para deixar a animação mais suave.
		glutTimerFunc(24, animar, 1);
	}
}

// Função que contará e exibirá os frames na tela,
// caso queira ativá-la, basta remover os comentários presentes dentro das
// funções 'main' e 'controles'.
// Foi desativada com o intuito de não 'poluir' o console.
void contarFrame(int valor) {
	if (valor != 0) {
		cout << "FPS = " << contagemFrame << endl;
	}
	contagemFrame = 0;
	glutTimerFunc(1000, contarFrame, 1);
}

// Função para controle do mouse e exibição da logo
void controleMouse(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		if (cenaLogo) cenaLogo = 0;

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) exit(0);
	glutPostRedisplay();
}

// Função para controlar o zoom com o scroll do mouse
void scrollMouse(int scroll, int direcao, int x, int y)
{
	if (direcao > 0 && zoom < 100) {
		zoom--;
	}
	if (direcao < 0 && zoom > -75) {
		zoom++;
	}
	glutPostRedisplay();
}

// Função para receber dados do teclado e interagir com a simulação
void entradaTecla(unsigned char tecla, int x, int y) {

	switch (tecla) {

	case 27: exit(0); break;
	case ' ':
		if (animado)
			animado = 0;
		else {
			animado = 1;
			animar(1);
		}
		break;

	case 'o':
		if (orbitaPequenaAtivo)
			orbitaPequenaAtivo = 0;
		else
			orbitaPequenaAtivo = 1;
		glutPostRedisplay();
		break;

	case 'O':
		if (orbitaGrandeAtivo)
			orbitaGrandeAtivo = 0;
		else
			orbitaGrandeAtivo = 1;
		glutPostRedisplay();
		break;

	case 'l':
		if (luasAtivo)
			luasAtivo = 0;
		else
			luasAtivo = 1;
		glutPostRedisplay();
		break;

	case 'L':
		if (luasAtivo)
			luasAtivo = 0;
		else
			luasAtivo = 1;
		glutPostRedisplay();
		break;

	case 'n':
		if (nomesAtivo)
			nomesAtivo = 0;
		else
			nomesAtivo = 1;
		glutPostRedisplay();
		break;

	case 'N':
		if (nomesAtivo)
			nomesAtivo = 0;
		else
			nomesAtivo = 1;
		glutPostRedisplay();
		break;

	case '1':
		mudaCamera = 0;
		glutPostRedisplay();
		break;

	case '2':
		mudaCamera = 1;
		glutPostRedisplay();
		break;

	case '3':
		mudaCamera = 2;
		glutPostRedisplay();
		break;

	}
}

// Função que exibirá os controles para a interação com a simulação
void controles(void) {

	cout << ">>> CONTROLES DA SIMULACAO <<<\n" << endl;
	cout << "Pressione a tecla ESPACO para iniciar/pausar a simulacao.\n" << endl;
	cout << "Pressione a tecla ESC para fechar a simulacao.\n" << endl;
	cout << "Pressione a tecla 'o' para exibir/ocultar as orbitas dos satelites.\n" << endl;
	cout << "Pressione a tecla 'O' para exibir/ocultar as orbitas dos planetas.\n" << endl;
	cout << "Pressione a tecla 'l' ou 'L' para exibir/ocultar os satelites.\n" << endl;
	cout << "Pressione as teclas 1, 2 e 3 para mudar o angulo da camera.\n" << endl;
	cout << "Pressione a tecla 'n' ou 'N' para exibir/ocultar o nome dos\nplanetas, do Sol e dos satelites.\n" << endl;
	cout << "Role o scroll do mouse para aproximar ou afastar (zoom).\n" << endl;
	//cout << "Contagem de Frames:\n" << endl;
}

// Função 'main' que executará a simulação
int main(int argc, char **argv) {

	// Música de fundo
	PlaySound("background.wav", NULL, SND_FILENAME | SND_ASYNC);

	controles();
	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(600, 0);
	glutCreateWindow("Sistema Solar | por Heitor Macedo");

	glutDisplayFunc(desenhaCenaEmOrdem);
	glutReshapeFunc(redimensiona);
	glutMouseFunc(controleMouse);
	glutKeyboardFunc(entradaTecla);
	glutMouseWheelFunc(scrollMouse);

	glewExperimental = GL_TRUE;
	glewInit();

	//contarFrame(0);
	configura();
	glutMainLoop();
}
