struct Resultado{
    int idEq1;
    int idEq2;
    int codRes;
    char estadio[20];

}

struct Equipo{
    int idEq;
    nombre[20];
    int puntos;
}


struct partidosJugadosEnEstadio{
    char estadio[20];
    int empatados;
    int jugados;
}




int main(){

    Equipo equipos[] = cargarEquipos("EQUIPOS.dat");


FILE* file = fopen("RESULTADOS.dat","r+b");
Resultado r = read(file);


    while(EOF(file)){

        procesarPartido()

        r = read(file);
    }


    return 0;
}


Equipo cargarEquipos[](string archivo){
FILE* file = fopen(archivo,"r+b");
Equipo e = read(file);


while(){}


}


Equipo* mejorEquipo(Equipo equipos[], int size){
    Equipo* mejor = &equipo[0];
    while(int i =1 < size){
        if(equipos[i].puntos > mejor->puntos){
            mejor = &equipo[i];
        }
    }
    return mejor;
}