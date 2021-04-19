// FiltroGaussianoSSE.cpp : Este archivo contiene la función "main".La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>
#include <time.h>


using namespace std;

const float e = 2.71828182;
const int tamanyo_imagen = 100;

void leerFichero(int& tamanyo_mascara, float& desviacion_tipica) {
    ifstream ficheroLec("BenchmarkConfig3.txt");
    string s;
    int contador = 1;
    if (ficheroLec.is_open()) {
        while (ficheroLec >> s) {
            ficheroLec >> s;
            if (contador == 1) {
                ficheroLec >> tamanyo_mascara;
            }
            else {
                ficheroLec >> desviacion_tipica;
            }
            contador++;
        }
    }
    else {
        cout << "Error al abrir el fichero" << endl;
    }
}



float calcularExponente(int tamanyo_mascara, float desviacion_tipica, int i, int j) {
    float exponente = -(pow(i - ((tamanyo_mascara - 1) / 2), 2) + pow(j - ((tamanyo_mascara - 1) / 2), 2)) / (2 * pow(desviacion_tipica, 2));
    return exponente;

}

void generadorMascara(int tamanyo_mascara, float desviacion_tipica, int** mascara) {
    float smallestCoef = 0;
    float coef = 0;
    for (int i = 0; i < tamanyo_mascara; i++) {
        for (int j = 0; j < tamanyo_mascara; j++) {
            coef = pow(e, calcularExponente(tamanyo_mascara, desviacion_tipica, i, j));
            if (i == 0 && j == 0) {
                smallestCoef = coef;
            }
        }
    }
}

float calcularC(int tamanyo_mascara, int** mascara) {
    int suma = 0;
    int maxCoger = 4;

    if (tamanyo_mascara < 4) maxCoger = tamanyo_mascara;
    unsigned j = 0;
    if (maxCoger < 4) {
        for (int i = 0; i < maxCoger; i++) {
            _asm {
                mov eax, mascara
                mov ebx, [i]
                imul ebx, 4
                add eax, ebx // eax = mascara + i * 4
                mov eax, [eax]
                mov ebx, [j]
                mov eax, [eax + ebx * 4] // posición actual en eax
                add[suma], eax //sumo el valor de la posición actual 
                inc[j]
            }
            
        }
    }
    else {
        _asm
        {
            //cargamos los 4 primeros
            mov eax, mascara
            mov eax, [eax]
            movups xmm1, dword ptr[eax]
        }
    }
    
    j += maxCoger;
    for (unsigned i = 0; i < tamanyo_mascara; i++) {
        for (; j < tamanyo_mascara; j++) {
            if (tamanyo_mascara - j >= maxCoger) {
                if (maxCoger >= 4) {
                    _asm {
                        mov eax, mascara
                        mov ebx, [i]
                        imul ebx, 4
                        add eax, ebx // eax = mascara + i * 4
                        mov ebx, [j]
                        mov eax, [eax + ebx * 4] // posición actual en eax
                        movups xmm0, dword ptr[eax] //cargar 4 ints en mmx0
                        addps xmm1, xmm0

                        //j += maxCoger-1
                        mov ebx, [maxCoger]
                        add ebx, [j]
                        dec ebx
                        mov [j], ebx
                        
                    }
                }
                
            }
            else {
                _asm {
                    mov eax, mascara
                    mov ebx, [i]
                    imul ebx, 4
                    add eax, ebx // eax = mascara + i * 4
                    mov eax, [eax]
                    mov ebx, [j]
                    mov eax, [eax + ebx * 4] // posición actual en eax
                    add [suma], eax //sumo el valor de la posición actual 
                    inc [j]
                }
                
            }
        }
        j = 0;
    }
    int aux = 0;
    
    _asm {
        movshdup    xmm0, xmm1
        addps       xmm1, xmm0
        movhlps     xmm0, xmm1
        addss       xmm1, xmm0
        movss word ptr[aux], xmm1
    }
    suma = suma + aux;
    float result = suma;
    return result;
}

void rellenarMatriz(int** matriz, int tamanyo_mascara) {
    //rellenamos la matriz con valores para probar
    int contador = 0;
    for (int i = 0; i < tamanyo_mascara; i++) {
        matriz[i] = new int[tamanyo_mascara];
    }

    for (int i = 0; i < tamanyo_mascara; i++) {
        for (int j = 0; j < tamanyo_mascara; j++) {
            
            matriz[i][j] = contador;
            contador++;
        }
    }
}

void aplicarFiltro(int imagen[tamanyo_imagen][tamanyo_imagen], int tamanyo_mascara, int desviacion_tipica, int** mascara_filtro) {
    int suma = 0;
    int sumaTotal = 0;
    int filaFiltro = 0;
    int columnaFiltro = 0;
    int columnaImagen;
    int columna;
    int numeroEnteros = pow(tamanyo_mascara, 2);

    for (int i = (tamanyo_mascara - 1) / 2; i < (tamanyo_imagen - 1) - (tamanyo_mascara - 1) / 2; i++) {
        for (int j = (tamanyo_mascara - 1) / 2; j < (tamanyo_imagen - 1) - (tamanyo_mascara - 1) / 2; j++) {
            for (int c = i - ((tamanyo_mascara - 1) / 2); c < (i + ((tamanyo_mascara - 1) / 2)); c++) {
                for (int k = i - ((tamanyo_mascara - 1) / 2); k < (i + ((tamanyo_mascara - 1) / 2)) - 1; k++) {
                    columnaImagen = k + c * tamanyo_mascara - c;
                    columna = columnaFiltro + filaFiltro * tamanyo_mascara - filaFiltro;
                    if(numeroEnteros >= 4){
                        _asm {
                            mov esi, c; //guardo las filas de la imagen
                            mov edi, columnaImagen; //guardo la columna de la imagen
                            mov ebx, imagen; //Guardo la direccion de memoria de la imagen
                            imul esi, 4; //Guardo la cantidad de bytes que hay hasta la fila
                            imul edi, 4; //Guardo la cantidad de bytes que hay hasta la columna
                            add ebx, esi; //Añado la cantidad de bytes a la direccion de imagen para la fila
                            add ebx, edi; //Añado la cantidad de bytes a la direccion de imagen para la columna
                            //mov ebx, [ebx + edi * 4];
                            movups xmm0, dword ptr[ebx]; //Guardo 4 enteros de la imagen

                            mov esi, filaFiltro; //guardo las filas del filtro
                            mov edi, columna; //guardo la columna del filtro
                            mov ebx, mascara_filtro; //Guardo la direccion de memoria del filtro
                            imul esi, 4; //Guardo la cantidad de bytes que hay hasta la fila
                            add ebx, esi; //Añado la cantidad de bytes a la direccion del filtro para la fila
                            mov ebx, [ebx + edi * 4]; //Añado la cantidad de bytes a la direccion del filtro para la columna
                            movups xmm1, dword ptr[ebx]; //Guardo 4 enteros del filtro

                            pmulld xmm1, xmm0; //multiplicamos los 4 números de la imagen y los 4 números del filtro 

                            movshdup    xmm0, xmm1;
                            addps       xmm1, xmm0;
                            movhlps     xmm0, xmm1;
                            addss       xmm1, xmm0;
                            movss word ptr[suma], xmm1;
                            mov ecx, sumaTotal;
                            add ecx, suma;
                            mov[sumaTotal], ecx;
                            mov ecx, tamanyo_mascara;
                            add[k], ecx;
                        }
                        if (k >= tamanyo_mascara) {
                            k -= tamanyo_mascara;
                            c++;
                        }

                        numeroEnteros -= 4;
                    }
                    else {
                        _asm {
                            mov esi, c; //guardo las filas de la imagen
                            mov edi, columnaImagen; //guardo la columna de la imagen
                            mov ebx, imagen; //Guardo la direccion de memoria de la imagen
                            imul esi, 4; //Guardo la cantidad de bytes que hay hasta la fila
                            imul edi, 4; //Guardo la cantidad de bytes que hay hasta la columna
                            add ebx, esi; //Añado la cantidad de bytes a la direccion de imagen para la fila
                            add ebx, edi; //Añado la cantidad de bytes a la direccion de imagen para la columna
                            mov eax, [ebx];
                            

                            mov esi, filaFiltro; //guardo las filas del filtro
                            mov edi, columna; //guardo la columna del filtro
                            mov ebx, mascara_filtro; //Guardo la direccion de memoria del filtro
                            imul esi, 4; //Guardo la cantidad de bytes que hay hasta la fila
                            add ebx, esi; //Añado la cantidad de bytes a la direccion del filtro para la fila
                            mov ebx, [ebx + edi * 4]; //Añado la cantidad de bytes a la direccion del filtro para la columna
                            mul ebx;

                            add [sumaTotal], eax //sumo el valor de la posición actual 
                        }
                        numeroEnteros--;
                    }
                    //suma += imagen[c][k] * mascara_filtro[filaFiltro][columnaFiltro];
                    columnaFiltro++;
                }
                columnaFiltro = 0;
                filaFiltro++;
            }
            numeroEnteros = pow(tamanyo_mascara, 2);
            filaFiltro = 0;
            columnaFiltro = 0;
            suma = 0;
        }
    }
}

void generarImagenAleatoria(int imagen[tamanyo_imagen][tamanyo_imagen]) {
    int num;
    for (int i = 0; i < tamanyo_imagen; i++) {
        for (int j = 0; j < tamanyo_imagen; j++) {
            num = rand() % 256;
            imagen[i][j] = num;
        }
    }
}

int main()
{
    int tamanyo_mascara;
    float desviacion_tipica;
    float c;
    int imagen[tamanyo_imagen][tamanyo_imagen];

    srand(time(NULL));

    generarImagenAleatoria(imagen);

    leerFichero(tamanyo_mascara, desviacion_tipica);
    cout << tamanyo_mascara << " " << desviacion_tipica << endl;

    int** mascara_filtro = new int* [tamanyo_mascara];
    rellenarMatriz(mascara_filtro, tamanyo_mascara);

    generadorMascara(tamanyo_mascara, desviacion_tipica, mascara_filtro);
    c = 1 / calcularC(tamanyo_mascara, mascara_filtro);

    aplicarFiltro(imagen, tamanyo_mascara, desviacion_tipica, mascara_filtro);

    for (int i = 0; i < tamanyo_imagen; i++) {
        for (int j = 0; j < tamanyo_imagen; j++) {
            if (j != 0) {
                cout << " ";
            }
            cout << imagen[i][j];
        }
        cout << endl;
    }
    cout << endl;
    /*
    for (int i = 0; i < tamanyo_mascara; i++) {
        for (int j = 0; j < tamanyo_mascara; j++) {
            cout << calcularExponente(tamanyo_mascara, desviacion_tipica, i, j) << ", ";

        }
        cout << endl;
    }*/



    for (int i = 0; i < tamanyo_mascara; i++) {
        delete[] mascara_filtro[i];
    }

    delete[] mascara_filtro;
}