// FiltroGaussiano.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>

using namespace std;

void leerFichero(int& tamanyo_mascara, float& desviacion_tipica) {
    ifstream ficheroLec("BenchmarkConfig2.txt");
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

    __asm {

    incializar:
        mov ecx, 0;
        mov esi, 0;
        mov eax, 0;
        mov edx, 0;
        mov ebx, 0;

    buclei:
        cmp ecx, tamanyo_mascara;
        jae buclej;
        cmp esi, tamanyo_mascara;
        jae terminar;


        inc eax;

    compi:
        cmp ecx, 0;
        jbe compj;

    inci:

        //Falta la division y la llamada a la otra función

        mov ebx, [mascara];
        mov ebx, [ebx + esi * 4];
        mov[ebx + ecx * 4], eax;

        inc ecx;
        jmp buclei;

    compj:
        cmp esi, 0;
        jbe asignar;
        jmp inci;

    buclej:
        mov ecx, 0;
        inc esi;
        jmp buclei;

    asignar:
        mov edx, 10;
        jmp inci;

    terminar:


    }

}


float calcularExponente(float desviacion_tipica, int tamanyo_mascara, int i, int j) {
    int i_c, j_c;
    float mult = 2.0;
    int suma = 0;
    int sign = -1;
    float resultado;
    _asm {
        mov eax, [i]
        mov ebx, [i]
        mul ebx //eax = eax*ebx | eax = i^2
        mov[i_c], eax //i_c = i^2
        mov eax, [j]
        mov ebx, [j]
        mul ebx //eax = eax * ebx --> j^2
        mov[j_c], eax //j_c = j^2
        fld[desviacion_tipica] //st(0) = desviacion_tipica
        fmul st(0), st(0) //st(0) = desviacion_tipica^2
        fld[mult] //cargamos 2.0 en st(0) | st(1) = desviacion_tipica^2
        fmul st(0), st(1) //st(0) = 2*desviación_típica^2 | st(1) = desviacion_tipica^2
        //sumamos j_c + i_c
        mov eax,[i_c] //eax = i_c
        mov ebx,[j_c] //ebx = j_c
        add ebx //eax = i_c + j_c
        mov [suma], eax //suma = eax
        //metemos suma en la pila
        fild [suma] //st(0) = suma | st(1) = 2*desviacion_tipica^2 | st(2) = desviacion_tipica^2
        fdiv st(0),st(1) //st(0) = suma/2*desviacion_tipica^2 | st(1) = 2*desviacion_tipica^2 | st(2) = desviacion_tipica^2
        //cargamos -1 para cambiar el signo
        fild [sign] //st(0) = -1.0 | st(1) = suma/2*desviacion_tipica^2 | st(2) = 2*desviacion_tipica^2 | st(3) = desviacion_tipica^2
        fmul st(0),st(1) //st(0) = -1 * suma/2*desviacion_tipica^2 | st(1) = suma/2*desviacion_tipica^2 | st(2) = 2*desviacion_tipica^2 | st(3) = desviacion_tipica^2
        fst[resultado] //guardamos resultado
        fstp st(0) //st(0) = suma / 2 * desviacion_tipica ^ 2 | st(1) = 2 * desviacion_tipica ^ 2 | st(2) = desviacion_tipica ^ 2
        fstp st(1) //st(0) = 2 * desviacion_tipica ^ 2 | st(1) = desviacion_tipica ^ 2
        fstp st(2) //st(0) = desviacion_tipica ^ 2
        fstp st(3) //stack vacío
        
      

    }
  
    return resultado;
}




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
            mascara_filtro[i][j] = -1;

        }
    }

    for (int i = 0; i < tamanyo_mascara; i++) {
        for (int j = 0; j < tamanyo_mascara; j++) {
            cout << calcularExponente(desviacion_tipica, tamanyo_mascara, i, j) << ", ";

        }
        cout << endl;
    }

   

    generadorMascara(tamanyo_mascara, desviacion_tipica, mascara_filtro);
    c = 1 / calcularC(tamanyo_mascara, mascara_filtro);


    for (int i = 0; i < tamanyo_mascara; i++) {
        delete[] mascara_filtro[i];
    }

    delete[] mascara_filtro;
}