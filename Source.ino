/*
 Name:		Sketch2.ino
 Created:	6/14/2019 10:54:24 AM
 Author:	Doug
*/

// the setup function runs once when you press reset or power the board
#include <RGBmatrixPanel.h>
#include <ArduinoSTL.h>
#include <list>
#define CLK  8
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
using namespace std;

/* Secao de defines do jogo da cobrinha*/
#define DIREITA 1
#define ESQUERDA 2
#define BAIXO 3
#define CIMA 4
#define ALTURA 16
#define LARGURA 32
//-------------------------------
char buf;
bool comidapresente = false;
int movdirection;
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);
class PecaCobrinha {
	int x;
	int y;
public:
	const int getX() { return x; }
	const int getY() { return y; }
	void setX(const int x) { this->x = x; }
	void setY(const int y) { this->y = y; }
	 };

class Comida {
	int x;
	int y;
public:
	const int getX() { return x; }
	const int getY() { return y; }
	void setX(const int x) { this->x = x; }
	void setY(const int y) { this->y = y; }
};

list<PecaCobrinha> ListaCobrinha;
list<PecaCobrinha>::iterator it;
Comida comidinha;
bool alive = false;

void setJogo(list<PecaCobrinha>* ListaCobrinha, Comida* comida) {
	int x, y;
	PecaCobrinha aux;
	x = 3;
	y = 4;
	aux.setX(x);
	aux.setY(y);
	ListaCobrinha->push_back(aux);
	
	for (x = 0; x < 3; x++) {
		aux.setY(--y);
		ListaCobrinha->push_back(aux);
	}
	
	alive = true;
}
void setComida(list<PecaCobrinha>* listacobrinha, Comida* comidinha, bool* comida) {
	bool repete = false;
	int tempx, tempy;
	while (1){
	tempx = rand() % LARGURA - 1;
	tempy = rand() % ALTURA - 1;
	for (it = listacobrinha->begin(); it != listacobrinha->end(); it++) {
		if (it->getX() == tempx && it->getY() == tempy)
			repete = true;
	}
	if (!repete)
		break;
	}
	comidinha->setX(tempx);
	comidinha->setY(tempx);
	*comida = true;
}

void update(list<PecaCobrinha>* listacobrinha, Comida* comidinha, int direction, bool* comeu) {
	int tempx, tempy, removex, removey, segpecax, segpecay, resx, resy;
	it = listacobrinha->begin();
	tempx = it->getX();
	tempy = it->getY();
	it++;
	segpecax = it->getX();
	segpecay = it->getY();
	resx = tempx - segpecax;
	resy = tempy - segpecay;
	switch (direction) {
	case BAIXO:
		if (resx == 0 && resy == -1)
			direction = CIMA;
		break;
	case CIMA:
		if (resx == 0 && resy == 1)
			direction = BAIXO;
		break;
	case ESQUERDA:
		if (resx == 1 && resy == 0)
			direction = DIREITA;
		break;
	case DIREITA:
		if (resx == -1 && resy == 0)
			direction = ESQUERDA;
		break;
	}
	switch (direction) {
	case BAIXO:
		tempy++;
		break;
	case CIMA:
		tempy--;
		break;
	case DIREITA:
		tempx++;
		break;
	case ESQUERDA:
		tempx--;
		break;
	case 0:
		break;
	}
	if (tempx < 0 || tempx >= LARGURA || tempy < 0 || tempy >= ALTURA) {
		alive = false;
		return;
	}
	PecaCobrinha aux;
	aux.setX(tempx);
	aux.setY(tempy);
	it++;
	for (it = listacobrinha->begin(); it != listacobrinha->end(); it++) {
		if (aux.getX() == it->getX() && aux.getY() == it->getY()){
			alive = false;
			return;
		}
	}
	listacobrinha->push_front(aux);
	if (tempx == comidinha->getX() && tempy == comidinha->getY())
		* comeu = false;
	if (*comeu == false)
		return;
	listacobrinha->pop_back();
}

void draw(list<PecaCobrinha>* listacobrinha, Comida* comidinha,bool* comeu) {
	int exp = 1;
	//printf("-------------\n");
	for (it = listacobrinha->begin(); it != listacobrinha->end(); it++) {
		if (exp == 1)
			matrix.drawPixel(it->getX(), it->getY(), matrix.Color333(7, 0, 0));
		else
			matrix.drawPixel(it->getX(), it->getY(), matrix.Color333(0, 0, 7));
		printf("%d  %d  %d\n", it->getX(), it->getY(), exp);
		exp *= -1;
		}
	printf("-------------\n");
	if (*comeu == true)
		matrix.drawPixel(comidinha->getX(), comidinha->getY(), matrix.Color333(0, 7, 0));
	
}
void setup()
{
	pinMode(13, OUTPUT);
	Serial.begin(9600);
	matrix.begin();
}

void loop()
{
	setJogo(&ListaCobrinha, &comidinha);
	setComida(&ListaCobrinha, &comidinha, &comidapresente);
	while (alive) {
		if (Serial.available()) {
			buf = Serial.read();
			switch (buf) {
			case 'w':
				movdirection = CIMA;
				break;
			case 's':
				movdirection = BAIXO;
				break;
			case 'd':
				movdirection = DIREITA;
				break;
			case 'a':
				movdirection = ESQUERDA;
				break;
			default:
				printf("%c\n ----", buf);
				break;
			}
		}
		update(&ListaCobrinha, &comidinha, movdirection, &comidapresente);
		if (!comidapresente)
			setComida(&ListaCobrinha, &comidinha, &comidapresente);
		draw(&ListaCobrinha, &comidinha,&comidapresente);
		delay(200);
	}
}
