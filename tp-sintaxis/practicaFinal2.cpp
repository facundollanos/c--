#include <iostream>
#include <cstring>
#include <stdio.h>

template <typename T>
Nodo<T>* unionOrdenada(Nodo<T>* l, Pila<T>* p) {
    Nodo<T>* resultado = nullptr;  // lista vacía

    // Recorro lista enlazada
    while (l != nullptr) {
        if (!buscar(resultado, l->valor, ordenamientoAscendente)) {
            insertarOrdenado(resultado, l->valor, ordenamientoAscendente);
        }
        l = l->sig;
    }

    // Recorro pila (desapilando)
    while (!esVacia(p)) {
        T valor = desapilar(p);
        if (!buscar(resultado, valor, ordenamientoAscendente)) {
            insertarOrdenado(resultado, valor, ordenamientoAscendente);
        }
    }

    return resultado;
}


template <typename T>
Nodo<T>* interseccion(Nodo<T>* l, Nodo<T>* l2) {
    Nodo<T>* resultado = nullptr;

    for (Nodo<T>* aux1 = l; aux1 != nullptr; aux1 = aux1->sig) {
        for (Nodo<T>* aux2 = l2; aux2 != nullptr; aux2 = aux2->sig) {
            if (aux1->valor == aux2->valor) {
                agregar(resultado, aux1->valor);
            }
        }
    }

    return resultado;
}

// Definición de la estructura
struct Bulto {
    int codigoDeProducto;
    int peso;
    char tipo;
    int cantidadDeBultos;
};

// Criterio de orden: por código de producto
int criterioCodigo(const Bulto& b1, const Bulto& b2) {
    return b1.codigoDeProducto - b2.codigoDeProducto;
}

// Agregar ordenado en la lista
void agregarOrdenado(Nodo<Bulto>*& l, const Bulto& b) {
    insertarOrdenado(l, b, criterioCodigo);
}


void mostrarSinStock(Nodo<Bulto>* l){
    for(Nodo<Bulto>* aux = l;aux != nullptr; aux = aux->sig){
        if(aux->cantidadDeBultos<0){

        }
    }
}


void mostrarPesados(Nodo<Bulto>* l){
    for(Nodo<Bulto>* aux = l;aux != nullptr; aux = aux->sig){
        if(aux->peso>100 && aux->tipo == 'B'){
            
        }
    }
}



int criterioAscendente(int a, int b){
    return a - b;
}


struct jugador{
    int id;
    char nombre[30];
    int goles;
    int minutosJugados;

}





void cargarMatriz(int& matriz[3][4]){
    for(int i=0;i<4;i++){
        for(int j=0;j<3;j++){
            if(hayEncuesta(i,j,l))
        }
    }
}

int main(){


while(LeerGol()){

}



    return 0;
}


// Como no se exactamente la cantidad de goles y porque el enunciado me lo pide usare Nodos\


int random100() {
    return rand() % 101;  // de 0 a 100
}


void cargarMatriz(int matriz[4][4]){
    for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){

if(i==j){
    matriz[i][j]=0;
}else{
    matriz[i][j]=random100();
}

    }
}
}

int[] masAlejados(int matriz[4][4]) {

    int [2] resultado;
    int maximo = -1;
        for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
        if(matriz[i][j] > maximo){
            maximo = matriz[i][j];
            resultado[0] = i;
            resultado[1]=j;
        }
    }

    
}


int Diagonal(int m[N][N]) {
    int suma = 0;
    for (int i = 0; i < N; i++) {
        suma += m[i][i];
    }
    return suma;
}

int main(){

    int matriz[4][4];
    cargarMatriz(matriz);

    int [2] alejados = masAlejados(matriz);
    int diagonal = Diagonal(matriz);


    return 0;
}


if(f= fopen("parcial.data","rb+")){
    fread(&x,sizeof(x),10,f);
    
}




void insertarAlFinal(Node*& lista, const char* path)
{
    int x;
    FILE* f = fopen(path, "rb");

    if (!f) return;



    while (fread(&x, sizeof(int), 1, f) == 1)
    {
        if(condicion(x)){
        Node* nuevo = new Node();
        nuevo->valor = x;
        nuevo->ptr = NULL;

        if (lista == NULL)
        {
            lista = nuevo;   // primer nodo
        }
        else
        {
            Node* aux = lista;
            while (aux->ptr != NULL)
                aux = aux->ptr;

            aux->ptr = nuevo;  // insertar al final
        }
    }
}

    fclose(f);
}


bool condicion(int x){
    return x%2 !=0 && x>1000 && x<2000;
}


struct NodoTransaccion{
    int numeroCuenta;
    int monto;
    NodoTransaccion* sig;
}

struct Saldo{
    int numeroCuenta;
    int saldoActual;
    int transaccionesPorCuenta;
}


void actualizarSaldos(NodoTransaccion* transacciones, Saldo saldo[], int len){
    NodoTransaccion* aux = transacciones;
    while(aux!=nullptr){
        for(int i=0;i<len;i++){
            if(saldo[i]).numeroCuenta == aux->numeroCuenta){
                saldo[i].saldoActual += aux->monto;
                saldo[i].transaccionesPorCuenta++;
            }
        }
    }
}



Node* unirListas(Node* l1, Node* l2) {

    Node* resultado = nullptr;

    Node* aux = l1;
    while (aux != nullptr) {
        unirSinRepetir(resultado, aux->valor);
        aux = aux->ptr;
    }

    aux = l2;
    while (aux != nullptr) {
        unirSinRepetir(resultado, aux->valor);
        aux = aux->ptr;
    }

    return resultado;



}





void unirSinRepetir(Node* lista, int elemento){
    
    Node* aux = lista;
    while(aux != nullptr){
        if(aux->valor == elemento){
            return;}
        aux = aux->ptr;

        }
        insertarOrdenado(lista,elemento);
    }
}

void insertarOrdenado(Node*& lista, int elemento){
    Node* aux = lista;
    Node* nuevo = new Node();
    nuevo->valor = elemento;
    nuevo->ptr = nullptr;


if(lista == nullptr){
    lista = nuevo;
}else if(aux->valor > elemento){
    nuevo->ptr = lista;
    lista = nuevo;
}
else{
    while(aux->ptr != nullptr && aux->ptr->valor < elemento){
        aux = aux->ptr;
    }
    nuevo->ptr = aux->ptr;
    aux->ptr = nuevo;


}


struct nota2parcial{
    int legajo;
    int nota;
}

struct listaNotas{
    nota2parcial nota;
    listaNotas* sig;
}


struct Notas{
    int legajo;
    int primerParcial;
    int segundoParcial;
}

Notas  actualizarNotas[](listaNotas* notasParcial2, Notas notasParciales[], int len){
    ListaNotas* aux = notasParcial2;


    Notas auxNotasParciales[len] = notasParciales;
    while(aux!=nullptr){
        for(int i=0;i<len;i++){
            if(aux->legajo == notasParciales[i].legajo){
                notasParciales[i].segundoParcial = aux->nota;
                
            }
        }
    }


        for(int i=0;i<len;i++){

if(notasParciales[i].segundoParcial >=6){
    return auxNotasParciales[];
}else{
    return notasParciales[];
}

        }

}



int main(){


    int op_code;
    cin >> op_code;
switch(op_code){

    case 1:{
        ingresar


        break;
    }
    case 2:{
        // Ejercicio 2
        break;
    }
    case 3:{
        // Ejercicio 3
        break;
    }
    case 4:{
        // Ejercicio 4
        break;
    } case 5:{
        // Ejercicio 5
        break;
    }default:{
        cout << "Codigo de operacion invalido." << endl;
        break;
    }
}


    return 0;
}




fread(&variable,size(tipoDato),1,nombreArchivo);
while(!feof(nombreArchivo)){
    //procesar variable



    
    fread(&variable,size(tipoDato),1,nombreArchivo);
}




void push(Pila*& pila, int empleado){
    Nodo* Pila aux = new Nodo();
    aux->empleado = empleado;
    aux->sig = pila;
    pila = aux;
}


int pop(Pila*& pila){
    int empleado = pila->empelado;
    Node* aux = pila;
    pila = pila->sig;
    delete aux;
    return empleado;
}


void mostrarEmpleados(Empleado empleados*,int len){


for(int i=0;i<len;i++){
    cout << empleados[i].nombre << " " << empleados[i].apellido << endl;
}

}


void queue(Cola*& cola, int cliente){
    Nodo* aux = new Nodo();
    aux->cliente = cliente;
    aux->sig = nullptr;

    if(cola==nullptr){
        cola = aux;
    }else{
        Nodo* temp = cola;
        while(temp->sig != nullptr){
            temp = temp->sig;
        }
        temp->sig = aux;
    }
    }
}



struct modelo{
    int color;
    int modelo;
    int cantidadVendida;
}


void cargarMatriz(int matriz[7][5], char* path){
FILE* f = fopen(path."rb");
modelo m;

while(fread(&m,sizeof(modelo),1,f)){

matriz[m.modelo][m.color]+= m.cantidadVendida;

}
}


void mostrarTotalxModel(int matriz[7][5]){
    
int totalModelo[7] = {0};

for(int i=0;i<7;i++){
    for(int j=0;j<5;j++){
        totalModelo[i] += matriz[i][j];
    }


}

void mostrarTotalxColor(int matriz[7][5]){


    int totalColor[5] = {0};

    for(int j=0;j<5;j++){
        for(int i=0;i<7;i++){
            totalColor[j] += matriz[i][j];
        }
    }

    void mostrarGeneral(int matriz[7][5]){
        
        int total =0;

        for(int i=0;i<7;i++){
            for(int j=0;j<5;j++){
                total += matriz[i][j];
            }
        }


        void mostrarTrianguloSuperior(int matriz[n][n]){
            for(int i=0;i<n;i++){
                for(int j=i;j<n;j++){

                    if(i<=j){
                        cout "estoy dentro de la diagonal superior" << endl;
                    }


                }
            }
        }







}


struct Nodo {
    string token;
    int repeticiones;
    Nodo* sig;
};


void mostrar(Node* registros[]){
    

for(int i=0;i<5;i++){
cout <<"id de categoria"<< i << endl;
cout << "categoria tipo token" << TipoToken(i) << endl;


int total =0;
Node* aux = registros[i];
while(aux != nullptr){
total += aux->repeticiones;
aux = aux->sig;
}

cout << "total de repeticiones: " << total << endl;
}


}






void cargarLexema(FILE* f, Nodo listas[]){

rewind(f);
Lexema lexema;

while(fread(&lexema,sizeof(Lexema),1,f)){

    insertarOrdenado(listas[r.id],lexema.);




}


int main(){

FILE* f = fopen("lexemas.data","rb");
Nodo* nodos[5] = {nullptr,nullptr,nullptr,nullptr,nullptr};

cargarLexema(f,nodos);
mostrar(nodos);


fclose(f);

    return 0;
}






int main(){




return 0;


}


int c = 10;
int* a = &c;
int* b = &c;