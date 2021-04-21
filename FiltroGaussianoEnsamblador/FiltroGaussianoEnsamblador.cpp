// FiltroGaussiano.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <fstream>
#include <vector>

using namespace std;
const float e = 2.71828182;
const int tamanyo_imagen = 5000;
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





float calcularExponente(float desviacion_tipica, int tamanyo_mascara, int i, int j) {
    int i_c, j_c;
    float mult = 2.0;
    int suma = 0;
    int divisor = 2;
    
    int sign = -1;
    float resultado;
    _asm {
        //i = i - (tamanyo_mascara-1)/2, j = j - (tamanyo_mascara-1)/2
        mov eax, [tamanyo_mascara]
        sub eax, 1 // eax = tamanyo_mascara - 1
        mov ebx, [divisor]
        xor edx,edx
        div ebx // eax = (tamanyo_mascara - 1) / 2 (división entera), edx = resto de la división
        //add eax, edx //eax = (tamanyo_mascara-1)/2 en float(creo)
        mov ebx, eax //ebx = (tamanyo_mascara-1)/2 en float(creo)
        
        mov eax, [i] //eax = i
        sub eax, ebx //eax = i - (tamanyo_mascara-1)/2
        mov [i], eax //i = i - (tamanyo_mascara-1)/2

        mov eax, [j] //eax = j
        sub eax, ebx //eax = j - (tamanyo_mascara-1)/2
        mov [j], eax //j = j - (tamanyo_mascara-1)/2


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
        add eax, ebx //eax = i_c + j_c
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


void generadorMascara(int tamanyo_mascara, float desviacion_tipica, int** mascara) {


    float coef_maspequenyo;
    int coeficiente;
    int i = 0;
    int j = 0;
    float expon;

    __asm {

    incializar:
        mov ecx, 0;
        mov esi, 0;
        mov eax, 0;
        mov edx, 0;
        mov ebx, 0;

    buclei:
        mov esi, j;
        mov ecx, i;
        cmp ecx, tamanyo_mascara;
        jae buclej;
        cmp esi, tamanyo_mascara;
        jae terminar;

    }
    expon = pow(e, calcularExponente(desviacion_tipica, tamanyo_mascara, i, j));
    __asm {
    compi:
        mov ecx, i;
        cmp ecx, 0;
        jbe compj;
    inci:

        mov ebx, 0;
        fld[coef_maspequenyo];
        fld[expon];
        fdiv st(0), st(1);
        fisttp coeficiente;
        FFREE st(1);
        FFREE st(2);
        FFREE st(3);
        FFREE st(4);
        FFREE st(5);
        mov eax, coeficiente;
        mov esi, j;
        mov ecx, i;
        mov ebx, [mascara];
        mov ebx, [ebx + esi * 4];
        mov[ebx + ecx * 4], eax;

        inc i;

        jmp buclei;

    compj:
        mov esi, j
            cmp esi, 0;
        jbe asignar;
        jmp inci;

    buclej:
        mov i, 0;
        inc j;
        jmp buclei;

    asignar:
        mov edx, expon;
        mov coef_maspequenyo, edx;

        jmp inci;

    terminar:


    }

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

void aplicarFiltro(int** imagen , int tamanyo_mascara, int desviacion_tipica, int** mascara_filtro) {
    int suma = 0;
    int producto_izq;
    int producto_dch;
    int i;
    int j;
    int c;
    int k;
    int lim; //limite del bucle i y j
    int lim1;//limite bucle de c y k
    int ini,ini1;//inicializador del bucle
    int filafiltro = 0;
    int columnafiltro = 0;
    _asm
    {
        mov ecx, [tamanyo_mascara]//movemos el tamaño de la mascará
        dec ecx//ecx = tamanyo_mascara-1
        mov [lim1], ecx//lim1 = tamayo_mascara-1
        mov eax, ecx //eax = tamanyo_mascara-1
        xor edx, edx
        mov ebx, 2//movemos el divisor
        div ebx//dividimoos eax = eax/2
        mov ecx, eax
        mov eax, [tamanyo_imagen]//cargamos en ecx tamaño imagen
        dec eax //tamanyo_imagen - 1
        sub eax,ecx //eax = (tamanyo_imagen-1 - tamanyo_mascara -1) / 2
        mov [lim], eax //movemos lim = (tamanyo_imagen - 1) - (tamanyo_mascara - 1) / 2

        mov [ini],ecx //ini= tamanyo_mascara-1 / 2

        mov [i],ecx //inicializamos el iterador i

 bucle1:      
        mov ebx, [i]
        mov ecx, [lim]//límite del bulce

        cmp ecx,ebx //comprobamos si se llega al final del bulce
        je fin
        mov eax,[ini]//
        mov [j], eax // j = (tamanyo_mascara-1) / 2
        jmp bucle2 // si no hemos llegado al final del bucle 



 bucle2:
        mov ecx, [lim]
        cmp [j],ecx//comparamos si j < limite
        je finbucle2 // si son iguales saltamos al bucle1
        mov esi,[i]
        mov edi,[ini]
        sub esi,edi
        mov [c], esi//inicializamos c = i -(tamanyo_mascara-1)/2
        mov [ini1],esi 
        add edi,[i]
        mov edx,edi//el limite de los dos bucles internos
        jmp bucle3 //empezamos el bucle 3
 bucle3:
        cmp [c],edx //miramos si hemos llegado al final del bucle
        je finbucle3
        mov esi,[ini1]
        mov [k],esi
        jmp bucle4

 bucle4:
        cmp[k], edx
        je finbucle4
        mov ecx, [c]//cargamos c
        mov ebx, [k]//cargamos k
        //mov eax,[ini]//cargamos (tamanyo-1)/2


        //mov ebx, 4
        mov edi, 0
        add edi, ecx
        add edi, ebx//sumamos fila y columna
        //imul ecx,ebx//multiplicamos por 4*c
        mov eax, [imagen]//sacamos producto_izquierdo
        mov eax, [eax]
        mov eax,[eax + 4 * edi]
       //mov ebx,[mascara_filtro+ecx]//sacamos producto_derecho
        mov edi,[filafiltro]
        add edi,[columnafiltro]
        //imul esi,ebx//multiplicamos por 4
        mov ebx, 0
        mov ebx,[mascara_filtro]//sacamos el elemento
        mov ebx,[ebx]
        mov ebx, [ebx + 4 * edi]
        imul eax,ebx//multiplicamos prod_izq*prod_derecho
        mov ecx,[suma]//sacamos el contendio de suma
        //imul eax,ebx//multiplicamos producto izquierdo y producto derecho
        add ecx,eax //sumamoos suma + producto_izq * producto_dch
        mov [suma],ecx
        
        inc [k]//incrementamos el índice de k++
        inc[columnafiltro]
        jmp bucle4//volvemos a iterar

 finbucle4:
        inc [c]
        inc [filafiltro]
        mov [columnafiltro], 0
        jmp bucle3
 finbucle3:
        mov [filafiltro], 0 
        mov [columnafiltro], 0//dejamos a cero filaFiltro y columna filtro
        mov [suma], 0//ponemos a 0 la suma
        inc [j]//incrementaamos el iterador
        jmp bucle2
 finbucle2:
        inc[i]//incrementamos el índice i
        jmp bucle1 //volvemos a iterar
         
  fin:
       

    }
}
void generarImagenAleatoria(int** imagen) {
    int num;
    for (int i = 0; i < tamanyo_imagen; i++) {
        for (int j = 0; j < tamanyo_imagen; j++) {
            num = rand() % 256;
            imagen[i][j] = num;
        }
    }
}
void rellenarMatriz(int** matriz, int tamanyo_mascara) {
    //rellenamos la matriz con valores para probar

    for (int i = 0; i < tamanyo_mascara; i++) {
        matriz[i] = new int[tamanyo_mascara];
    }

    for (int i = 0; i < tamanyo_mascara; i++) {
        for (int j = 0; j < tamanyo_mascara; j++) {
            matriz[i][j] = -1;
        }
    }
}
int main()
{
    int tamanyo_mascara;
    float desviacion_tipica;
    float c;
    
    int** imagen = new int* [tamanyo_imagen];
    
    srand(time(NULL));

    leerFichero(tamanyo_mascara, desviacion_tipica);

    int** mascara_filtro = new int* [tamanyo_mascara];

    rellenarMatriz(mascara_filtro, tamanyo_mascara);
    rellenarMatriz(imagen, tamanyo_imagen);
    generarImagenAleatoria(imagen);

    clock_t inicio = clock();
    for (int i = 0; i < 10; i++) {
        generadorMascara(tamanyo_mascara, desviacion_tipica, mascara_filtro);
        c = 1 / calcularC(tamanyo_mascara, mascara_filtro);
        aplicarFiltro(imagen, tamanyo_mascara, desviacion_tipica, mascara_filtro);
    }
    clock_t fin = clock();
    float media = (double(fin - inicio) / ((clock_t)1000));
    media /= 10;
    cout << media << endl;

    for (int i = 0; i < tamanyo_mascara; i++) {
        delete[] mascara_filtro[i];
    }

    delete[] mascara_filtro;
    for (int i = 0; i < tamanyo_imagen; i++) {
        delete[] imagen[i];
    }

    delete[] imagen;

    
}