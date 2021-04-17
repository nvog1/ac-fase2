// FiltroGaussiano.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>
#include <time.h>


using namespace std;

const float e = 2.71828182;
const int tamanyo_imagen = 20;

void leerFichero(int& tamanyo_mascara, float& desviacion_tipica) {
    ifstream ficheroLec("BenchmarkConfig.txt");
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
    float exponente = -(pow(i - ((tamanyo_mascara - 1) / 2),2)+pow(j - ((tamanyo_mascara - 1) / 2),2))/(2*pow(desviacion_tipica,2));
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
            else {
                //Nothing
            }
            mascara[i][j] = coef / smallestCoef;
        }
    }
}

float calcularC(int tamanyo_mascara, int** mascara) {
    float suma = 0;
    for (unsigned i = 0; i < tamanyo_mascara; i++) {
        for (unsigned j = 0; j < tamanyo_mascara; j++) {
            suma += mascara[i][j];
        }
    }
    return suma;
}

void rellenarMatriz(int** matriz,int tamanyo_mascara) {
    //rellenamos la matriz de su puta madre con valores para probar

    for (int i = 0; i < tamanyo_mascara; i++) {
        matriz[i] = new int[tamanyo_mascara];
    }

    for (int i = 0; i < tamanyo_mascara; i++) {
        for (int j = 0; j < tamanyo_mascara; j++) {
            matriz[i][j] = -1;
        }
    }
}

void aplicarFiltro(int imagen[tamanyo_imagen][tamanyo_imagen], int tamanyo_mascara, int desviacion_tipica, int** mascara_filtro) {
    int suma = 0;
    int filaFiltro = 0;
    int columnaFiltro = 0;
    for (int i = (tamanyo_mascara - 1) / 2; i < (tamanyo_imagen - 1) - (tamanyo_mascara - 1) / 2; i++) {
        for (int j = (tamanyo_mascara - 1) / 2; j < (tamanyo_imagen - 1) - (tamanyo_mascara - 1) / 2; j++) {
            for (int c = i - ((tamanyo_mascara - 1) / 2); c < (i + ((tamanyo_mascara - 1) / 2)); c++) {
                for (int k = i - ((tamanyo_mascara - 1) / 2); k < (i + ((tamanyo_mascara - 1) / 2)) - 1; k++) {

                    suma += imagen[c][k] * mascara_filtro[filaFiltro][columnaFiltro];
                    columnaFiltro++;
                }
                columnaFiltro = 0;
                filaFiltro++;
            }
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
    rellenarMatriz(mascara_filtro,tamanyo_mascara);

    generadorMascara(tamanyo_mascara, desviacion_tipica, mascara_filtro);
    c = 1 / calcularC(tamanyo_mascara, mascara_filtro);

    aplicarFiltro(imagen, tamanyo_mascara, desviacion_tipica, mascara_filtro);

    for (int i = 0; i < tamanyo_imagen; i++) {
        for (int j = 0; j < tamanyo_imagen; j++) {
            if (j != 0) {
                cout << " ";
            }
            cout << imagen[i][j] ;
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


