// FiltroGaussiano.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>


using namespace std;

const float e = 2.71828182;
const int tamanyo_imagen = 100;

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

void aplicarFiltro(int** imagen, int tamanyo_mascara, int desviacion_tipica, int** mascara_filtro) {
    int suma = 0;
    int producto_izq;
    int producto_dch;
    for (int i = (tamanyo_mascara - 1) / 2; i < (tamanyo_imagen - 1) - (tamanyo_mascara - 1) / 2; i++) {
        for (int j = (tamanyo_mascara - 1) / 2; j < (tamanyo_imagen - 1) - (tamanyo_mascara - 1) / 2; j++) {
            for (int c = 0; c < tamanyo_mascara - 1; c++) {
                for (int k = 0; k < tamanyo_mascara - 1; k++) {

                    producto_izq = imagen[c - (tamanyo_mascara - 1) / 2][k - (tamanyo_mascara - 1) / 2];
                    producto_dch = mascara_filtro[c - (tamanyo_mascara - 1) / 2][k - (tamanyo_mascara - 1) / 2];

                    suma += producto_izq * producto_dch;
                }
            }
        }
    }
}


int main()
{
    int tamanyo_mascara;
    float desviacion_tipica;
    float c;
    int imagen = 0;

    leerFichero(tamanyo_mascara, desviacion_tipica);
    cout << tamanyo_mascara << " " << desviacion_tipica << endl;

    int** mascara_filtro = new int* [tamanyo_mascara];
    rellenarMatriz(mascara_filtro,tamanyo_mascara);

    for (int i = 0; i < tamanyo_mascara; i++) {
        for (int j = 0; j < tamanyo_mascara; j++) {
            cout << calcularExponente(tamanyo_mascara, desviacion_tipica, i, j) << ", ";

        }
        cout << endl;
    }

    generadorMascara(tamanyo_mascara, desviacion_tipica, mascara_filtro);
    c = 1 / calcularC(tamanyo_mascara, mascara_filtro);
    cout << c << endl;

    for (int i = 0; i < tamanyo_mascara; i++) {
        delete[] mascara_filtro[i];
    }

    delete[] mascara_filtro;
}


