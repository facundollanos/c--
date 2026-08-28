
struct empleado{
    char nombre[30];
    int edad;
    float salario;
};

empleado buscarEmpleado(empleado arr[],empleado empl, int len, int (*criterio)(empleado, empleado)){
    


    int encontrado = buscar(arr, empl, len, criterio);

    return arr[encontrado];

}





int main(){
    return 0;
}