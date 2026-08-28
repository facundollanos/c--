
struct Node{
    Empleado data;
    Node* next;
} typedef Node;


struct Nodo{
    int data;
    Node* next;
} typedef Nodo;


struct Empleado{
    char nombre[40];
    int edad;
}typedef Empleado;

void anadirNodo(Node*& head, Node* tail, Empleado value){

Node* nuevo = new Node();
nuevo->data = value;
nuevo->next = nullptr;

if(head == nullptr){
    head = nuevo;}
    else{
        Node* aux = head;
        while(aux->next != nullptr){
            aux = aux->next;
        }
        aux->next = nuevo;
    }


}



int sumar(stream& archivo){
int suma = 0;
int numero;

while(fread(&numero,sizeof(int),1,archivo)){
    suma += numero;
}

return suma
}



int sumar(int * arreglo[], int size){

int suma = 0;

for(int o=0; o<size; o++){
    suma += arreglo[o];


}
}

int sumar(Nodo* nodo){
    int suma = 0;

    while(nodo!=nullptr){
        suma += nodo->data;
        nodo = nodo->next;
    }

    return suma;
}





void cargarEmpelados(Node*& head, Node* tail,char path[]){

FILE* archivo = fopen(path,"r");
Empleado emp = fread(path,"r");


while(!feof(archivo)){
    anadirNodo(head,tail,emp);
    emp = fread(path,"r");
}



}


int main(){







    return 0;
}