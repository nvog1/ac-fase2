// FiltroGaussiano.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>

using namespace std;

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

void generadorMascara(int tamanyo_mascara, float desviacion_tipica, int** mascara) {

}

/*
float calcularExponente(float desviacion_tipica, int tamanyo_mascara, int i, int j) {

    _asm {
        mov eax, [i]
        mov ebx, [i]
        mul ebx// eax = eax*ebx
        mov[i_c], eax
        mov eax, [j]
        mov ebx, [j]
        mul ebx//eax = eax * ebx --> j^2
        mov[j_c], eax
        fld[desviacion_tipica]
        fmul st(0), st(0)//st(0)--> desviacion
        fld[mult]//cargamos 2.0
        fmul st(0), st(1)//2*desviación típica^2
        flid[j_c]// como convertir a flotante
    }
}*/




int calcularC(int tam_mascara, int** mascara_filtro) {
    int variable = 0;

    __asm {
    inicializar:
        mov edx, tam_mascara;
        mov ecx, 0;
        mov esi, 0;
        mov ebx, 0;
        mov eax, 0;

    bucle_i:
        cmp ecx, edx;
        jae bucle_j;

        cmp esi, edx;
        jae terminar;



        mov ebx, [mascara_filtro];
        mov ebx, [ebx + esi * 4];
        mov ebx, [ebx + ecx * 4];

        add eax, ebx;
        inc ecx;


        jmp bucle_i;


    bucle_j:
        inc esi;
        mov ecx, 0;

        jmp bucle_i;

    terminar:
        mov variable, eax;
    }
    return variable;
}

int main()
{
    int tamanyo_mascara;
    float desviacion_tipica;
    float c;

    leerFichero(tamanyo_mascara, desviacion_tipica);
    cout << tamanyo_mascara << " " << desviacion_tipica << endl;

    int** mascara_filtro = new int* [tamanyo_mascara];
    for (int i = 0; i < tamanyo_mascara; i++) {
        mascara_filtro[i] = new int[tamanyo_mascara];
    }

    //rellenamos la matriz de su puta madre con valores para probar
    for (int i = 0; i < tamanyo_mascara; i++) {
        for (int j = 0; j < tamanyo_mascara; j++) {
            mascara_filtro[i][j] = i + j;

        }
    }
/*
    for (int i = 0; i < tamanyo_mascara; i++) {
        for (int j = 0; j < tamanyo_mascara; j++) {
            cout << calcularExponente(desviacion_tipica, tamanyo_mascara, i, j) << ", ";

        }
        cout << endl;
    }*/

   

    generadorMascara(tamanyo_mascara, desviacion_tipica, mascara_filtro);
    c = 1 / calcularC(tamanyo_mascara, mascara_filtro);


    for (int i = 0; i < tamanyo_mascara; i++) {
        delete[] mascara_filtro[i];
    }

    delete[] mascara_filtro;
}