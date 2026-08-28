
typedef struct NodoCola{
    char dato[40];
    struct NodoCola *siguiente;
} NodoCola;


typedef struct tipoInfoPrioridades{
    NodoCola *frente;
    NodoCola *final;
} tipoInfoPrioridades;


typedef struct NodoPrioridades{
    tipoInfoPrioridades info;
    struct NodoPrioridades *siguiente;
} NodoPrioridades;


NodoPrioridades* crearColaPrioridades(){

NodoPrioridades* aux;

tipoInfoPrioridades* info;

    for(int i=0;i<5;i++){

info->frente = NULL;
info->final = NULL;
agregar(aux,info);

    }



return aux;


}

void agregar(NodoPrioridades*& lista, tipoInfoPrioridades* elem){

NodoPrioridades* nuevo = new Nodo();
nuevo->info = nullptr;
nuevo->siguiente = nullptr;

if(lista == nullptr){
    lista = nuevo;
}else{
    Nodo* aux = lista;
    while(aux->siguiente != nullptr){
        aux = aux->siguiente;
    }
    aux->siguiente = nuevo;
}


    
}

void insertarEnPrioridad(NodoPrioridades*& lista, int prioridad, char paciente[]){

NodoCola* nuevo = new NodoCola();
nuevo->dato = paciente;
nuevo->siguiente = nullptr;
NodoPrioridades* aux = lista;

for(int i=0;i<prioridad;i++){
    aux = aux->siguiente;}
    queue(aux->info.frente,aux->info.final,paciente);
    





}

NodoPrioridades* elementoMinimo(NodoPrioridades* lista){

}


char* quitarMinimo(NodoPrioridades*& lista){
      
}


int prioridadVacia(NodoPrioridades* lista){

}


int main(){


    return 0;
}