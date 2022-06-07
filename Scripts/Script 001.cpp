//ALUMNOS
//Bendezu Vargas Pedro Jhonny
//Pastor Moreno Wiston Rikelme
//Tema: Practica Calificada 3

#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <iostream>
#include <time.h>
#include <locale.h>

#define TAM_VENTANA 500
#define ejeX 0
#define ejeY 1
#define PUNTOS_MAX 100
#define MAX_AMPLITUD 3.0
#define MIN_AMPLITUD 0.4

using namespace std;

//************* INICIO DECLARACION MODELOS DE FUNCIONES ************
void init(void);
void iniciarMenus();

void obtenerPuntoCatmullRom(double,int);

int generarRandom(int);
float generarRandomNormalizado();

void menu_HabilitarRegistrarPuntos(int);
void menu_GraficarPuntos(int);
void menu_GraficarCurva(int);
void menu_CambiarPosicion(int);
void menu_color(int);
void menu_grosor(int);
void menu_TipoBusqueda(int);
void menu_principal(int);

void logicaPrograma();
void graficarAreaBusqueda();
void calcularPosRelativa();
void calcularPosRelativaBusqueda();

void graficarPuntosRegistrados();
void graficarCurvaCatmullRom();
void graficarPuntoBusqueda(double,double);
void graficarCirculo(double,double);
void graficarCuadrado(double,double);

bool buscarPorVerificacionNormal(int,int);
bool buscarPorVerificacionCuadrado(double,double);
bool buscarPorVerificacionCirculo(float,float);
void generarPosCuadradoDeVerificacion(double,double);
bool verificarDentroLado(float,float,float,float,float,float,bool);
bool estaElPuntoDentroDelCuadrado(float,float);
bool estaElPuntoDentroDelCirculo(float,float,float,float);

void controlTecladoFlechas(int,int,int);
void controlarAreaBusqueda(int);

void clic(int,int,int,int);
void drag(int,int);
void mouseReposo(int,int);

void display(void);
void reshape(int,int);
void mensajeFuncionamiento();
//************ FIN DECLARACION MODELOS FUNCIONES

//************* INICIO DECLARACION DE VARIABLES *************
double tempX=0,tempY=0;
double tempXBusqueda = 0, tempYBusqueda = 0;
double puntoTempX,puntoTempY;
double t,t2,t3;
double incrementoT = 0.01;
double grosorLinea = 3;
double grosorPunto = 6;
double amplitudBusqueda = 0.4;
double pi = 3.1415;
double nroP = 30.0;//Numero de Particiones

bool estadoInicial = true;
bool graficarPuntos = estadoInicial, graficarCurva = estadoInicial;
bool registrarPuntos = estadoInicial, cambiarPosicion = !estadoInicial;
bool seEncontroPunto = false;
bool estaPresionado;

static GLfloat puntos[PUNTOS_MAX][2];
static GLfloat rgb[] = {1.0,0.0,0.0};
static GLfloat puntosCuadrado[4][2];

int posRGB = 0;

int nroPuntosRegistrados = 0;
int posEnArregloPuntoPorMover = 0;
int tipoVerificacion = 1; //1:Normal   2:Cuadrado   3:Circulo
int i;
int MITAD_VENTANA = TAM_VENTANA/2;;
//************* FIN DECLARACION DE VARIABLES ***************

int main(int argc, char** argv)
{
glutInit(&argc, argv);
glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
glutInitWindowSize(TAM_VENTANA,TAM_VENTANA);
glutInitWindowPosition(100,100);
glutCreateWindow("Spline Catmull-Rom");
init();
glutDisplayFunc(display);
glutReshapeFunc(reshape);
glutMotionFunc(drag);
glutIdleFunc(display);
glutMouseFunc(clic);
glutPassiveMotionFunc(mouseReposo);
glutSpecialFunc(controlTecladoFlechas);
glutMainLoop();
return 0;
}

//************* INICIO INICIALIZACION ***********
void init(void)
{
setlocale(LC_ALL, "");
glClearColor(1.0,1.0,1.0,0.0); //Parametros: rojo, amarillo, azul, el cuarto es el parametro alpha
glShadeModel(GL_FLAT);
glColor3f(1.0,0.0,0.0);
glLineWidth(grosorLinea);
glPointSize(grosorPunto);
mensajeFuncionamiento();
iniciarMenus();
}

void iniciarMenus(){
int submenu_registrarPuntos = glutCreateMenu(menu_HabilitarRegistrarPuntos);
 glutAddMenuEntry("Activado",1);
 glutAddMenuEntry("Desactivado",0);

  int submenu_graficarPuntos = glutCreateMenu(menu_GraficarPuntos);
 glutAddMenuEntry("Activado",1);
 glutAddMenuEntry("Desactivado",0);

  int submenu_graficarCurva = glutCreateMenu(menu_GraficarCurva);
 glutAddMenuEntry("Activado",1);
 glutAddMenuEntry("Desactivado",0);

 int submenu_color=glutCreateMenu(menu_color);
 glutAddMenuEntry("Rojo",1);
 glutAddMenuEntry("Verde",2);
 glutAddMenuEntry("Azul",3);
 glutAddMenuEntry("Aleatorio",4);

 int submenu_cambiarPosicion = glutCreateMenu(menu_CambiarPosicion);
 glutAddMenuEntry("Activado",1);
 glutAddMenuEntry("Desactivado",0);

 int submenu_grosor = glutCreateMenu(menu_grosor);
 glutAddMenuEntry("3",3);
 glutAddMenuEntry("6",6);

 int submenu_tipoBusqueda = glutCreateMenu(menu_TipoBusqueda);
 glutAddMenuEntry("Busqueda Normal con el puntero",1);
 glutAddMenuEntry("Busqueda considerando un cuadrado",2);
 glutAddMenuEntry("Busqueda considerando un circulo",3);

 glutCreateMenu(menu_principal);
 glutAddSubMenu("Insertar los puntos de control",submenu_registrarPuntos);
 glutAddSubMenu("Graficar puntos de control",submenu_graficarPuntos);
 glutAddSubMenu("Graficar curva",submenu_graficarCurva);
 glutAddSubMenu("Colores",submenu_color);
 glutAddSubMenu("Cambiar posición de los puntos de control",submenu_cambiarPosicion);
 glutAddSubMenu("Grosor",submenu_grosor);
 glutAddSubMenu("Tipo Búsqueda",submenu_tipoBusqueda);

 glutAddMenuEntry("Esc",4);

 // usaremos el boton derecho del mouse
 glutAttachMenu(GLUT_RIGHT_BUTTON);
 }
//*************** FIN INICIALIZACION ****************

//*************** INICIO LOGICA CURVA CATMULL-ROM ********

void obtenerPuntoCatmullRom(double t,int pos){
    t2 = pow(t,2);//t al cuadrado
    t3 = pow(t,3);//t al cubo
    //puntos[punto i][eje y]
    puntoTempX = 0.5*(puntos[pos-1][ejeX]*(2*t2-t3-t)+
        puntos[pos][ejeX]*(2-5*t2+3*t3)+
        puntos[pos+1][ejeX]*(t+4*t2-3*t3)+
        puntos[pos+2][ejeX]*(-t2+t3));
    puntoTempY = 0.5*(puntos[pos-1][ejeY]*(2*t2-t3-t)+
        puntos[pos][ejeY]*(2-5*t2+3*t3)+
        puntos[pos+1][ejeY]*(t+4*t2-3*t3)+
        puntos[pos+2][ejeY]*(-t2+t3));
}

//*************** FIN LOGICA CURVA CATMULL-ROM ***********

//************* INICIO GENERAR RANDOMS **************
int generarRandom(int maxValue){
    srand(time(NULL));
    int temp = (rand()*1357)%maxValue;
    return temp;
}

float generarRandomNormalizado(){
    int myRand = generarRandom(255);
    return myRand/255.0;
}
//************* FIN GENERAR RANDOMS

//************* INICIO MENUS *****************
void menu_HabilitarRegistrarPuntos(int opcion)
{
    registrarPuntos = (opcion == 1);
}

void menu_GraficarPuntos(int opcion)
{
    graficarPuntos = (opcion == 1);
}

void menu_GraficarCurva(int opcion)
{
    graficarCurva = (opcion == 1);
}

void menu_CambiarPosicion(int opcion)
{
    cambiarPosicion = (opcion == 1);
}

void menu_color(int opcion)
{
    switch(opcion){
        case 1:
            rgb[0] = 1.0; rgb[1] = 0.0 ; rgb[2] = 0.0; break;
        case 2:
            rgb[0] = 0.0; rgb[1] = 1.0 ; rgb[2] = 0.0; break;
        case 3:
            rgb[0] = 0.0; rgb[1] = 0.0 ; rgb[2] = 1.0; break;
        case 4:
            posRGB = (posRGB+1)%3;
            rgb[posRGB] = generarRandomNormalizado();
            break;
    }
}

void menu_grosor(int opcion){
    grosorLinea = opcion * 1.0;
    grosorPunto = opcion * 2.0;
}

void menu_TipoBusqueda(int opcion){
    tipoVerificacion = opcion;
}

void menu_principal(int opcion)
{
    if (opcion == 4)
        exit(-1);
}
//************* FIN MENUS ********************

//************ INICIO LOGICA GENERAL ***************
void logicaPrograma(){
    if(graficarCurva){
        graficarCurvaCatmullRom();
    }
    if(graficarPuntos){
        graficarPuntosRegistrados();
    }
    if(cambiarPosicion && !(seEncontroPunto && estaPresionado)){
        graficarAreaBusqueda();
    }

}

void graficarAreaBusqueda(){
    switch(tipoVerificacion){
        case 1:
            graficarPuntoBusqueda(tempXBusqueda,tempYBusqueda);
            break;
        case 2:
            graficarCuadrado(tempXBusqueda,tempYBusqueda);
            break;
        case 3:
            graficarCirculo(tempXBusqueda,tempYBusqueda);
            break;
    }
}

void calcularPosRelativa(int x,int y){
    //Evaluamos con el operador ternario para hacerlo en una sola linea
    //USO : valor = condicion? valor_caso_verdadero: valor_caso_falso
    x = x<MITAD_VENTANA?(MITAD_VENTANA-x)*-1:x-MITAD_VENTANA;
    y = y<MITAD_VENTANA?(MITAD_VENTANA-y):(y-MITAD_VENTANA)*-1;
    tempX = x /(MITAD_VENTANA/nroP);
    tempY = y /(MITAD_VENTANA/nroP);
}

void calcularPosRelativaBusqueda(int x,int y){
    //Evaluamos con el operador ternario para hacerlo en una sola linea
    //USO : valor = condicion? valor_caso_verdadero: valor_caso_falso
    x = x<MITAD_VENTANA?(MITAD_VENTANA-x)*-1:x-MITAD_VENTANA;
    y = y<MITAD_VENTANA?(MITAD_VENTANA-y):(y-MITAD_VENTANA)*-1;
    tempXBusqueda = x /(MITAD_VENTANA/nroP);
    tempYBusqueda = y /(MITAD_VENTANA/nroP);
}

//************** FIN LOGICA GENERAL  ***************

//************ INICIO LOGICA PARA GRAFICAR ***************
void graficarPuntosRegistrados(){
    glBegin(GL_POINTS);
    for(i=0;i<nroPuntosRegistrados;i++){
        glColor3f(1.0-rgb[0],rgb[1],rgb[2]);
        if(i == posEnArregloPuntoPorMover && cambiarPosicion && seEncontroPunto && estaPresionado){
            glColor3f(1.0-rgb[0],1.0-rgb[1],1.0-rgb[2]);
        }
        glVertex2f(puntos[i][ejeX],puntos[i][ejeY]);
    }
    glEnd();
}

void graficarCurvaCatmullRom(){
    if(nroPuntosRegistrados>=4){
        glColor3f(rgb[0],rgb[1],rgb[2]);
        for(i=1;i<nroPuntosRegistrados-2;i++){
            glBegin(GL_LINE_STRIP);
            for(t=0.0;t<=1;t+=incrementoT){
                obtenerPuntoCatmullRom(t,i);
                glVertex2f(puntoTempX,puntoTempY);
            }
            glEnd();
        }
    }
}

void graficarPuntoBusqueda(double x,double y){
    glBegin(GL_POINTS);
    glVertex2f(x,y);
    glEnd();
}

void graficarCirculo(double x,double y){
    glBegin(GL_LINE_LOOP);
    for(double ang=0.0;ang<=2*pi;ang+=2*pi/10){
        glVertex2f(x+amplitudBusqueda*sin(ang),y+amplitudBusqueda*cos(ang));
    }
    glEnd();
}

void graficarCuadrado(double x,double y){
    //generarPosCuadradoDeVerificacion(x,y);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x-amplitudBusqueda,y+amplitudBusqueda);
    glVertex2f(x+amplitudBusqueda,y+amplitudBusqueda);
    glVertex2f(x+amplitudBusqueda,y-amplitudBusqueda);
    glVertex2f(x-amplitudBusqueda,y-amplitudBusqueda);
    glEnd();
}

//************ FIN LOGICA PARA GRAFICAR ***************

//*********** INICIO LOGICA DE BUSQUEDA ***************
bool buscarPorVerificacionNormal(int x,int y){
    for(int k=0;k<nroPuntosRegistrados;k++){
        //cout<<puntos[k][ejeX]<<","<<puntos[k][ejeY]<<"   |  "<<x<<","<<y<<endl;
        if((int)puntos[k][ejeX]==x && (int)puntos[k][ejeY]==y){
            posEnArregloPuntoPorMover = k;
            return true;
        }
    }
    return false;
}

bool buscarPorVerificacionCuadrado(double x,double y){
    generarPosCuadradoDeVerificacion(x,y);
    for(int k=0;k<nroPuntosRegistrados;k++){
        if(estaElPuntoDentroDelCuadrado(puntos[k][ejeX],puntos[k][ejeY])){
            posEnArregloPuntoPorMover = k;
            return true;
        }
    }
    return false;
}

bool buscarPorVerificacionCirculo(float x,float y){
    for(int k=0;k<nroPuntosRegistrados;k++){
        if(estaElPuntoDentroDelCirculo(puntos[k][ejeX],puntos[k][ejeY],x,y)){
            posEnArregloPuntoPorMover = k;
            return true;
        }
    }
    return false;
}

void generarPosCuadradoDeVerificacion(double x,double y){
    //cout<<"Posiciones central del cuadrado "<<x<<","<<y<<endl;
    puntosCuadrado[0][ejeX] = (x-amplitudBusqueda); puntosCuadrado[0][ejeY] = (y+amplitudBusqueda);
    puntosCuadrado[1][ejeX] = (x+amplitudBusqueda); puntosCuadrado[1][ejeY] = (y+amplitudBusqueda);
    puntosCuadrado[2][ejeX] = (x-amplitudBusqueda); puntosCuadrado[2][ejeY] = (y-amplitudBusqueda);
    puntosCuadrado[3][ejeX] = (x+amplitudBusqueda); puntosCuadrado[3][ejeY] = (y-amplitudBusqueda);
    /*cout<<"x:"<<x<<" y:"<<y<<" amplitud:"<<amplitudBusqueda<<endl;
    cout<<puntosCuadrado[0][0]<<" "<<puntosCuadrado[0][1]<<endl;
    cout<<puntosCuadrado[1][0]<<" "<<puntosCuadrado[1][1]<<endl;
    cout<<puntosCuadrado[2][0]<<" "<<puntosCuadrado[2][1]<<endl;
    cout<<puntosCuadrado[3][0]<<" "<<puntosCuadrado[3][1]<<endl;*/
}

bool verificarDentroLado(float x1,float y1, float x2, float y2, float xp, float yp,bool mayor_o_menor){
    //mayor_o_menor
    //para true verificar si es mayor o igual
    //para false verificar si es menor o igual
    int d = (x2-x1)*(yp-y1)-(xp-x1)*(y2-y1);
    //cout<<x1<<","<<y1<<"  "<<x2<<","<<y2<<" "<<xp<<","<<yp<<" --> "<<d<<endl;
    return mayor_o_menor?d>=0:d<=0;
}

bool estaElPuntoDentroDelCuadrado(float x, float y){
    bool resp = verificarDentroLado(puntosCuadrado[0][0],puntosCuadrado[0][1],puntosCuadrado[1][0],puntosCuadrado[1][1],x,y,false);
    resp = resp && verificarDentroLado(puntosCuadrado[0][0],puntosCuadrado[0][1],puntosCuadrado[2][0],puntosCuadrado[2][1],x,y,true);
    resp = resp && verificarDentroLado(puntosCuadrado[2][0],puntosCuadrado[2][1],puntosCuadrado[3][0],puntosCuadrado[3][1],x,y,true);
    resp = resp && verificarDentroLado(puntosCuadrado[3][0],puntosCuadrado[3][1],puntosCuadrado[1][0],puntosCuadrado[1][1],x,y,true);
    return resp;
}

bool estaElPuntoDentroDelCirculo(float posBuscadaX,float posBuscadaY, float centroX, float centroY){
    return sqrt(pow(centroX-posBuscadaX,2)+pow(centroY-posBuscadaY,2))<=amplitudBusqueda*1.0;
}

//*********** FIN LOGICA DE BUSQUEDA ******************

//*********** INICIO MANEJO EVENTO DEL TECLADO **************************
void controlTecladoFlechas(int key,int x,int y){
    controlarAreaBusqueda(key);
}

void controlarAreaBusqueda(int key){
    switch(key){
    case 101:
        if(amplitudBusqueda<MAX_AMPLITUD){
            amplitudBusqueda += 0.1;
            cout<<"Amplitud Actual: "<<amplitudBusqueda<<endl;
        }
        break;
    case 103:
        if(amplitudBusqueda> MIN_AMPLITUD){
            amplitudBusqueda -= 0.1;
            cout<<"Amplitud Actual: "<<amplitudBusqueda<<endl;
        }
        break;
    }

}
//*********** FIN MANEJO EVENTO DEL TECLADO *****************************

//*********** INICIO MANEJO DE EVENTOS DEL MOUSE ******************
void clic(int button, int state, int x, int y){
    //EVENTO PRESIONAR BOTON
    estaPresionado = state == GLUT_DOWN;
    calcularPosRelativaBusqueda(x,y);
    if(registrarPuntos && !cambiarPosicion){
        if(state == GLUT_UP){
            if(nroPuntosRegistrados < PUNTOS_MAX){
                if(button == GLUT_LEFT_BUTTON){
                    puntos[nroPuntosRegistrados][ejeX]= tempXBusqueda;
                    puntos[nroPuntosRegistrados][ejeY]= tempYBusqueda;
                    nroPuntosRegistrados++;
                }
            }
        }
    }
    if(cambiarPosicion){
        switch(tipoVerificacion){
            case 1:
                seEncontroPunto = buscarPorVerificacionNormal(tempXBusqueda,tempYBusqueda);
                break;
            case 2:
                seEncontroPunto = buscarPorVerificacionCuadrado(tempXBusqueda,tempYBusqueda);
                break;
            case 3:
                seEncontroPunto = buscarPorVerificacionCirculo(tempXBusqueda,tempYBusqueda);
                break;
        }
        //cout<<"Punto Encontrado "<<seEncontroPunto<<endl;
    }
}

void drag(int x,int y){
    //EVENTO ARRASTRAR
    calcularPosRelativaBusqueda(x,y);
    if(seEncontroPunto && estaPresionado){
        //cout<<x<<","<<y<<" TEMPS:"<<tempX<<","<<tempY<<endl;
        puntos[posEnArregloPuntoPorMover][ejeX]= tempXBusqueda;
        puntos[posEnArregloPuntoPorMover][ejeY]= tempYBusqueda;
    }
}

void mouseReposo(int x,int y){
    calcularPosRelativaBusqueda(x,y);
   // cout<<x<<" "<<y<<endl;
}

//*********** FIN MANEJO DE EVENTOS DEL MOUSE*********************

//*********** INICIO EVENTOS DE LA VENTANA ***********************
void display(void)
{
glClear(GL_COLOR_BUFFER_BIT);
glPushMatrix(); // salva el estado actual de la matriz
glLineWidth(grosorLinea);
glPointSize(grosorPunto);
glColor3f(rgb[0],rgb[1],rgb[2]);
logicaPrograma();
glPopMatrix(); // reecupera el estado del matriz
glFlush();
}

void reshape(int w, int h)
{
glViewport(0,0,(GLsizei)w, (GLsizei)h);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(-nroP,nroP,-nroP,nroP,-nroP,nroP);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
}

void mensajeFuncionamiento(){
    cout<<"FUNCIONAMIENTO PROGRAMA"<<endl<<endl;
    cout<<"Cosas adicionales a la tarea:"<<endl<<endl;;
    cout<<"************** Tipo Busqueda **************"<<endl;
    cout<<"El submenu tipo de busqueda te permite elegir en como se detactará el nuevo punto,"<<endl;
    cout<<"donde cada opción trabaja con una amplitud que podemos cambiar, y representa lo siguiente:"<<endl;
    cout<<" - NORMAL: Se intentará hacer que la posición del cursor coincida con uno de los puntos"<<endl;
    cout<<" - CUADRADO: Se generará un cuadrado alrededor del cursor y se buscará en esta área"<<endl;
    cout<<" - CIRCULO: Se generará un circulo alrededor del cursor y se buscará en esta área"<<endl;
    cout<<endl<<"************** Amplitud de busqueda *******"<<endl;
    cout<<"Indica el radio del circulo de busqueda o la mitad del cuadrado de busqueda"<<endl;
    cout<<" - Su rango es de "<<MIN_AMPLITUD<<" a "<<MAX_AMPLITUD<<endl;
    cout<<" - Su valor puede aumentarse o reducirse con las flechas arriba y abajo"<<endl;
    cout<<endl<<"Amplitud Actual: "<<amplitudBusqueda<<endl;
}

//**************FIN EVENTOS DE LA VENTANA *****************

//ALGUNAS PAGINAS VISTAS AL HACER EL PROGRAMA
/*
https://stackoverflow.com/questions/20288861/mouse-drag-object-in-opengl-glut
https://stackoverflow.com/questions/2571402/how-to-use-glortho-in-opengl
https://qastack.mx/programming/2752725/finding-whether-a-point-lies-inside-a-rectangle-or-not
https://www.i-ciencias.com/pregunta/31476/como-saber-si-un-punto-esta-dentro-de-una-circunferencia4
https://community.khronos.org/t/how-to-use-arrow-keys/43131/8
*/
