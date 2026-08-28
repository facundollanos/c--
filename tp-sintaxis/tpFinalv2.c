#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Compilador del Lenguaje Micro (Fischer) */

#define NUMESTADOS 20
#define NUMCOLS 15
#define TAMLEX (32+1)
#define TAMNOM (20+1)

FILE *in;

typedef enum {
    INICIO, FIN, LEER, ESCRIBIR, ID, CONSTANTE, PARENIZQUIERDO, PARENDERECHO,
    PUNTOYCOMA, COMA, ASIGNACION, SUMA, RESTA, FDT, ERRORLEXICO, CONSTANTE_ENTERO,
    CONSTANTE_REAL, CONSTANTE_CARACTER, MIENTRAS, SI, ENTONCES, FIN_SI, SINO,FIN_MIENTRAS, REPETIR,REPETIR_FIN, HASTA, HACER,MAYOR,MENOR, IGUAL
} TOKEN;



typedef enum {
    T_ENTERO,
    T_FLOAT,
    T_CHAR
} TipoDato;


typedef struct {
    char identifi[TAMLEX];
    TOKEN t;
    TipoDato tipo;
} RegTS;






RegTS TS[1000] = {
   {"inicio", INICIO, T_ENTERO},
   {"fin", FIN, T_ENTERO},
   {"leer", LEER, T_ENTERO},
   {"escribir", ESCRIBIR, T_ENTERO},
   {"si", SI, T_ENTERO},
   {"entonces", ENTONCES, T_ENTERO},
   {"sino", SINO, T_ENTERO},
   {"cortarCondicional", FIN_SI, T_ENTERO},
   {"mientras", MIENTRAS, T_ENTERO},
   {"hacer", HACER, T_ENTERO},
   {"cortarMientras", FIN_MIENTRAS, T_ENTERO},
   {"repetir", REPETIR, T_ENTERO},
   {"hasta", HASTA, T_ENTERO},
   {"cortarRepetir",REPETIR_FIN,T_ENTERO}
//    {"$", 99, T_ENTERO}   // SIEMPRE EL ÚLTIMO
};

int lengthTS = 12;




typedef struct {
    TOKEN clase;
    char nombre[TAMLEX];
    TipoDato tipo;   
float valor;
} REG_EXPRESION;


RegTS RegExAux;




char buffer[TAMLEX];
TOKEN tokenActual;
int flagToken = 0;

/* Prototipos */
int esEntero(const char *lexema);
int esFloat(const char *lexema);
int esChar(const char *lexema);
void AsignarConTipo(REG_EXPRESION izq, REG_EXPRESION der);
void actualizarID(const char *id, TipoDato tipo);
void Condicion(REG_EXPRESION *res);
void OperadorRelacional(char *op);
REG_EXPRESION GenCondicion(REG_EXPRESION izq, char *op, REG_EXPRESION der);


TOKEN scanner();
int columna(int c);
int estadoFinal(int e);
void Objetivo(void);
void Programa(void);
void ListaSentencias(void);
void Sentencia(void);
void ListaIdentificadores(void);
void Identificador(REG_EXPRESION *presul);
void ListaExpresiones(void);
void Expresion(REG_EXPRESION *presul);
void Primaria(REG_EXPRESION *presul);
void OperadorAditivo(char *presul);
REG_EXPRESION ProcesarCte(void);
REG_EXPRESION ProcesarId(void);
char *ProcesarOp(void);
void Leer(REG_EXPRESION in);
void Escribir(REG_EXPRESION out);
REG_EXPRESION GenInfijo(REG_EXPRESION e1, char *op, REG_EXPRESION e2);
void Match(TOKEN t);
TOKEN ProximoToken();
void ErrorLexico();
void ErrorSintactico();
void Generar(char *co, char *a, char *b, char *c);
char *Extraer(REG_EXPRESION *preg);
int Buscar(char *id, RegTS *TS, TOKEN *t);
void Colocar(const char *id, RegTS *TS);
void Chequear(char *s);
void Comenzar(void);
void Terminar(void);
void Asignar(REG_EXPRESION izq, REG_EXPRESION der);
TipoDato BuscarTipo(const char *id, TOKEN *tok);
/*************************** Programa Principal ************************/
int main(int argc, char *argv[]) {
    char nomArchi[TAMNOM];
    int l;

    if (argc == 1) {
        printf("Debe ingresar el nombre del archivo fuente (en lenguaje Micro) en la linea de comandos\n");
        return -1;
    }
    if (argc != 2) {
        printf("Numero incorrecto de argumentos\n");
        return -1;
    }
    strcpy(nomArchi, argv[1]);
    l = strlen(nomArchi);
    if (l > TAMNOM) {
        printf("Nombre incorrecto del Archivo Fuente\n");
        return -1;
    }
    if (nomArchi[l-1] != 'm' || nomArchi[l-2] != '.') {
        printf("Nombre incorrecto del Archivo Fuente\n");
        return -1;
    }
    if ((in = fopen(nomArchi, "r")) == NULL) {
        printf("No se pudo abrir archivo fuente\n");
        return -1;
    }

    Objetivo();
    fclose(in);
    return 0;
}

/* ------------------ Parser ------------------ */

void Objetivo(void) {
    Programa();
    Match(FDT);
    Terminar();
}

void Programa(void) {
    Comenzar();
     Match(INICIO);
    ListaSentencias();
    Match(FIN);
}

void ListaSentencias(void) {
    while (1) {
        switch (ProximoToken()) {
            case ID:
            case LEER:
            case ESCRIBIR:
            case SI:
            case MIENTRAS:
            case REPETIR:
                Sentencia();
                break;

            case FIN_SI:
            case FIN_MIENTRAS:
                return; 

            default:
                return;
        }
    }
}

void Sentencia(void) {
    TOKEN tok = ProximoToken();
    REG_EXPRESION izq, der;
    switch (tok) {
        case ID:
            Identificador(&izq);
            Match(ASIGNACION);
            Expresion(&der);
            // Asignar(izq, der);
            AsignarConTipo(izq,der);
            Match(PUNTOYCOMA);
            break;
        case LEER:
            Match(LEER);
            Match(PARENIZQUIERDO);
            ListaIdentificadores();
            Match(PARENDERECHO);
            Match(PUNTOYCOMA);
            break;
        case ESCRIBIR:
            Match(ESCRIBIR);
            Match(PARENIZQUIERDO);
            ListaExpresiones();
            
            Match(PARENDERECHO);
            Match(PUNTOYCOMA);
            break; 

case SI:
    Match(SI);
    Match(PARENIZQUIERDO);

    REG_EXPRESION cond_si;
    Condicion(&cond_si);      

    Match(PARENDERECHO);
    Match(ENTONCES);
    ListaSentencias();
    Match(FIN_SI);
    Match(PUNTOYCOMA);
    break;

case MIENTRAS:
    Match(MIENTRAS);
    Match(PARENIZQUIERDO);

    REG_EXPRESION cond_mientras;
    Condicion(&cond_mientras);

    Match(PARENDERECHO);
    Match(HACER);
    ListaSentencias();
    Match(FIN_MIENTRAS);
    Match(PUNTOYCOMA);
    break;

case REPETIR:
    Match(REPETIR);
    Match(PARENIZQUIERDO);

    REG_EXPRESION cond_repetir;
    Condicion(&cond_repetir);

    Match(PARENDERECHO);
    Match(HACER);
    ListaSentencias();
    Match(REPETIR_FIN);
    Match(PUNTOYCOMA);
    break;
    

        default: return;
    }
}

void ListaIdentificadores(void) {
    TOKEN t;
    REG_EXPRESION reg;
    Identificador(&reg);
    Leer(reg);
    for (t = ProximoToken(); t == COMA; t = ProximoToken()) {
        Match(COMA);
        Identificador(&reg);
        Leer(reg);
    }
}


void ListaExpresiones(void) {
    TOKEN t;
    REG_EXPRESION reg;
    Expresion(&reg);
    Escribir(reg);
    for (t = ProximoToken(); t == COMA; t = ProximoToken()) {
        Match(COMA);
        Expresion(&reg);
        Escribir(reg);
    }
}

void Expresion(REG_EXPRESION *presul) {
    REG_EXPRESION operandoIzq, operandoDer;
    char op[TAMLEX];
    TOKEN t;
    Primaria(&operandoIzq);
    for (t = ProximoToken(); t == SUMA || t == RESTA; t = ProximoToken()) {
        OperadorAditivo(op);
        Primaria(&operandoDer);
        operandoIzq = GenInfijo(operandoIzq, op, operandoDer);
    }
    *presul = operandoIzq;
}


void Primaria(REG_EXPRESION *presul) {
    TOKEN tok = ProximoToken();
    switch (tok) {
        case ID:
            Identificador(presul);
            break;

        case CONSTANTE_ENTERO:
        case CONSTANTE_REAL:
        case CONSTANTE_CARACTER:
            Match(tok);
            *presul = ProcesarCte();  
            break;

        case PARENIZQUIERDO:
            Match(PARENIZQUIERDO);
            Expresion(presul);
            Match(PARENDERECHO);
            break;

        default:
            ErrorSintactico();
            break;
    }
}


void OperadorAditivo(char *presul) {
    TOKEN t = ProximoToken();
    if (t == SUMA || t == RESTA) {
        Match(t);
        strcpy(presul, ProcesarOp());
    } else {
        ErrorSintactico();
    }
}

/* ------------------ Semánticas ------------------ */



void Identificador(REG_EXPRESION *presul) {
    Match(ID);
    *presul = ProcesarId();
}

REG_EXPRESION ProcesarId(void) {
    REG_EXPRESION reg;
    Chequear(buffer);

    reg.clase = ID;
    strcpy(reg.nombre, buffer);
    reg.tipo = RegExAux.tipo;
    return reg;
}




int Buscar(char *id, RegTS *TS, TOKEN *t)
{
    for(int i = 0; i < lengthTS; i++)
    {
        if(strcmp(id, TS[i].identifi) == 0)
        {
            *t = TS[i].t;      
            RegExAux = TS[i]; 
            return 1;
        }
    }
    return 0; 
}

void Colocar(const char *id, RegTS *TS){
    if(lengthTS >= 1000) return;

    strcpy(TS[lengthTS].identifi, id);
    TS[lengthTS].t = ID;
    TS[lengthTS].tipo = T_ENTERO;
    lengthTS++;
}


void Chequear(char *s) {
    TOKEN t;
    if (!Buscar(s, TS, &t)) {
        Colocar(s, TS);
        Generar("Declara", s, "-", "");
        // if (tipo == T_ENTERO)
        //     Generar("Declara", s, "Entera", "");
        // else if (tipo == T_FLOAT)
        //     Generar("Declara", s, "Real", "");
        // else if (tipo == T_CHAR)
        //     Generar("Declara", s, "Caracter", "");
    }
}


REG_EXPRESION ProcesarCte(void) {
    REG_EXPRESION reg;

    if (esChar(buffer)) {
        reg.clase = CONSTANTE_CARACTER;
        reg.tipo = T_CHAR;
        reg.valor = buffer[1]; 
    }
    else if (esFloat(buffer)) {
        reg.clase = CONSTANTE_REAL;
        reg.tipo = T_FLOAT;
        sscanf(buffer, "%f", &reg.valor);
    }
    else {
        reg.clase = CONSTANTE_ENTERO;
        reg.tipo = T_ENTERO;
        sscanf(buffer, "%f", &reg.valor);
    }

    strcpy(reg.nombre, buffer);
    return reg;
}


int esEntero(const char *lexema) {
    for (int i = 0; lexema[i]; i++) {
        if (!isdigit(lexema[i])) return 0;
    }
    return 1;
}

int esFloat(const char *lexema) {
    int punto = 0;
    int digitos = 0;
    for (int i = 0; lexema[i]; i++) {
        if (lexema[i] == '.') {
            if (punto) return 0; 
            punto = 1;
        } else if (isdigit(lexema[i])) {
            digitos = 1;
        } else {
            return 0;
        }
    }
    return (punto && digitos);
}

int esChar(const char *lexema) {
    return (lexema[0] == '\'' &&
            lexema[1] != '\0' &&
            lexema[2] == '\'' &&
            lexema[3] == '\0');
}



void Condicion(REG_EXPRESION *res) {
    REG_EXPRESION izq, der;
    char op[4];

    Expresion(&izq);
    if (ProximoToken() == MENOR || ProximoToken() == MAYOR || ProximoToken() == IGUAL) {
        OperadorRelacional(op);
        printf("Operador relacional: %s\n", op);
    } else {
        ErrorSintactico();
    }

    Expresion(&der);

    *res = GenCondicion(izq, op, der);
}

void OperadorRelacional(char *op) {
    TOKEN t = ProximoToken();

    switch (t) {
        case MENOR:
            Match(MENOR);
            strcpy(op, "<");
            break;

        case MAYOR:          
            Match(MAYOR);
            strcpy(op, ">");
            break;

        case IGUAL:          
            Match(IGUAL);
            strcpy(op, "=="); 
            break;


        default:
            ErrorSintactico();
            break;
    }
}


REG_EXPRESION GenCondicion(REG_EXPRESION izq, char *op, REG_EXPRESION der) {
    REG_EXPRESION res;
    static int numTemp = 1;
    char nombreTemp[20];

    sprintf(nombreTemp, "Bool&%d", numTemp++);
    strcpy(res.nombre, nombreTemp);
    res.clase = ID;

    if (strcmp(op, "<") == 0)
        Generar("CMP_LT", izq.nombre, der.nombre, nombreTemp);
    else if (strcmp(op, ">") == 0)
        Generar("CMP_GT", izq.nombre, der.nombre, nombreTemp);
    else if (strcmp(op, "==") == 0)
        Generar("CMP_EQ", izq.nombre, der.nombre, nombreTemp);

    return res;
}


char *ProcesarOp(void) { return buffer; }

void Escribir(REG_EXPRESION out) {
    char tipo[20];

    switch (RegExAux.tipo) {
        case T_ENTERO:      strcpy(tipo, "Entera"); break;
        case T_FLOAT:      strcpy(tipo, "Real"); break;
        case T_CHAR:      strcpy(tipo, "Caracter"); break;
        default:             strcpy(tipo, "Desconocido"); break;
    }

    Generar("Write", out.nombre, tipo, "");
}

void Leer(REG_EXPRESION in) {
    char tipo[16];

    switch (RegExAux.tipo) {
        case T_ENTERO:     strcpy(tipo, "Entera"); break;
        case T_FLOAT:      strcpy(tipo, "Real"); break;
        case T_CHAR:      strcpy(tipo, "Caracter"); break;
        default:             strcpy(tipo, "Desconocido"); break;
    }

    Generar("Read", in.nombre, tipo, "");
}



REG_EXPRESION GenInfijo(REG_EXPRESION izq, char *op, REG_EXPRESION der) {
    REG_EXPRESION res;
    static int numTemp = 1;
    char nombreTemp[20];

    // Crear nombre de temporal (Temp&1, Temp&2, etc.)
    sprintf(nombreTemp, "Temp&%d", numTemp++);
    strcpy(res.nombre, nombreTemp);
    res.clase = ID;

    // *******************************
    // 1) Determinar tipo del resultado
    // *******************************
    if (izq.tipo == T_FLOAT || der.tipo == T_FLOAT) {
        res.tipo = T_FLOAT;     // Si alguno es float → resultado float
        res.valor = izq.valor + der.valor; // Ejemplo para suma
    } 
    else if (izq.tipo == T_CHAR || der.tipo == T_CHAR) {
        res.tipo = T_ENTERO;    // char se considera entero (ASCII)
                res.valor = izq.valor + der.valor; // Ejemplo para suma

    }
    else {
        res.tipo = T_ENTERO;    // int + int
                res.valor = izq.valor + der.valor; // Ejemplo para suma

    }

    // ***********************************************
    // 2) Generar código EXACTAMENTE como Micro original
    // ***********************************************
    if (strcmp(op, "+") == 0) {
        printf("Sumar %s,%s,%s\n", izq.nombre, der.nombre, res.nombre);
    }
    else if (strcmp(op, "-") == 0) {
        printf("Restar %s,%s,%s\n", izq.nombre, der.nombre, res.nombre);
    }

    return res;
}



/* ------------------ Auxiliares ------------------ */

void Match(TOKEN t) {
    if (!(t == ProximoToken())) ErrorSintactico();
    flagToken = 0;
}

TOKEN ProximoToken() {
    if (!flagToken) {
        tokenActual = scanner();
        if (tokenActual == ERRORLEXICO) ErrorLexico();
        flagToken = 1;
        if (tokenActual == ID) {
            Buscar(buffer, TS, &tokenActual);
        }
    }
    return tokenActual;
}

void ErrorLexico() { printf("Error Lexico\n"); }
void ErrorSintactico() { printf("Error Sintactico\n"); }

void Generar(char *co, char *a, char *b, char *c) {
    printf("%s %s,%s,%s\n", co, a, b, c);
}

char *Extraer(REG_EXPRESION *preg) { return preg->nombre; }





void Comenzar(void) { }
void Terminar(void) { Generar("Detiene", "", "", ""); }


void AsignarConTipo(REG_EXPRESION izq, REG_EXPRESION der) {
    // Almacena <Tipo> <Valor> <Variable>,
    if (der.tipo == T_ENTERO) {
        printf("Almacena Entera %.0f %s,\n", der.valor, izq.nombre);
    }
    else if (der.tipo == T_FLOAT) {
        // %.2f → muestra 2 decimales (podés ajustar a gusto)
        printf("Almacena Real %.2f %s,\n", der.valor, izq.nombre);
    }
    else if (der.tipo == T_CHAR) {
        // Mostramos el carácter real en vez del código ASCII
        printf("Almacena Caracter '%c' %s,\n", (char)der.valor, izq.nombre);
    }
    else {
        printf("Almacena Desconocido %f %s,\n", der.valor, izq.nombre);
    }
    actualizarID(izq.nombre, der.tipo);
}


void actualizarID(const char *id, TipoDato tipo) {
    for(int i = 0; i < lengthTS; i++) {
        if(strcmp(id, TS[i].identifi) == 0) {
            TS[i].tipo = tipo;
            return;
        }
    }
}





void Asignar(REG_EXPRESION izq, REG_EXPRESION der) {
    Generar("Almacena", Extraer(&der), izq.nombre, "");
}

/* ------------------ Scanner ------------------ */

// TOKEN scanner() {
// int tabla[NUMESTADOS][NUMCOLS] = {
// /* a  d  +  -  (  )  ,  ;  :  = EOF sp  .  '  otro */
//   { 1, 3, 5, 6, 7, 8, 9,10,11,14,13, 0,15,17,14 }, // 0 inicio
//   { 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 }, // 1 letras → ID
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 2 fin ID
//   { 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4 }, // 3 dígitos ('.'→15)
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 4 fin entero
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 5 '+'
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 6 '-'
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 7 '('
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 8 ')'
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 9 ','
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, //10 ';'
//   {14,14,14,14,14,14,14,14,14,12,14,14,14,14,14}, //11 ':'
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, //12 ':='
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, //13 FDT
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, //14 error
//   {14,16,14,14,14,14,14,14,14,14,14,14,14,14,14}, //15 vio '.' espera dígito
//   {14,16,14,14,14,14,14,14,14,14,14,14,14,14,14}, //16 parte fraccionaria (float)
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,19,14}, //17 leí `'`, espero carácter
//   {19,19,19,19,19,19,19,19,19,19,19,19,19,19,19}, //18 leí carácter, espero `'` cierre
//   {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}  //19 fin char (se maneja en switch)
// };

//     int car, col, estado = 0, i = 0;
//     do {
//         car = fgetc(in);
//         col = columna(car);
//         estado = tabla[estado][col];
//         if (col != 11 && i < TAMLEX-1) buffer[i++] = (char)car;
//     } while (!estadoFinal(estado) && !(estado == 14));
//     buffer[i] = '\0';



// /* ... inside scanner() after the do-while loop ... */

// switch (estado) {
//     case 2:
//     case 4:
// case 16: // CONSTANTE_REAL (número con parte decimal)
//     if (col != 11 && car != EOF) {
//         ungetc(car, in);
//         buffer[i-1] = '\0';
//     }

//     // Si el número termina con un punto, es un error léxico (ej: 123.)
//     size_t len = strlen(buffer);
//     if (len > 0 && buffer[len - 1] == '.')
//         return ERRORLEXICO;

//     // Si no termina en punto, es un número flotante válido
//     return CONSTANTE_REAL;

//     case 19: // 👈 CONSTANTE_CARACTER ends here
//         // The token is now in buffer: `'x'`
//         return CONSTANTE_CARACTER;

//     case 5:  return SUMA;
//     case 6:  return RESTA;
//     case 7:  return PARENIZQUIERDO;
//     case 8:  return PARENDERECHO;
//     case 9:  return COMA;
//     case 10: return PUNTOYCOMA;
//     case 12: return ASIGNACION;
//     case 13: return FDT;
//     case 14: return ERRORLEXICO;
//     default: return ERRORLEXICO;
// }




// }

// int estadoFinal(int e) {
//     // estados NO finales (se sigue leyendo)
//     if (e == 0 || e == 1 || e == 3 || e == 11 || e == 15 || e == 17 || e == 18)
//         return 0;
//     // todos los demás son finales
//     return 1; // Includes states 2, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 16, 19
// }

// int columna(int c) {
//     if (isalpha(c)) return 0;
//     if (isdigit(c)) return 1;
//     if (c == '+') return 2;
//     if (c == '-') return 3;
//     if (c == '(') return 4;
//     if (c == ')') return 5;
//     if (c == ',') return 6;
//     if (c == ';') return 7;
//     if (c == ':') return 8;
//     if (c == '=') return 9;
//     if (c == EOF) return 10;
//     if (isspace(c)) return 11;
//     if (c == '.') return 12;
//     if (c == '\'') return 13;
//     if (c == '<' || c == '>') return 14;
//     return 14; // "otro"
// }


TOKEN scanner(void) {
    int c;
    int i = 0;

    // Saltar espacios y saltos de línea
    do {
        c = fgetc(in);
    } while (isspace(c));

    // Fin del archivo
    if (c == EOF)
        return FDT;

    // 🔹 Identificador o palabra reservada
    if (isalpha(c)) {
        do {
            buffer[i++] = c;
            c = fgetc(in);
        } while (isalnum(c));
        buffer[i] = '\0';
        ungetc(c, in);

        // Chequear si es palabra reservada
if (!strcmp(buffer, "inicio")) return INICIO;
if (!strcmp(buffer, "fin")) return FIN;
if (!strcmp(buffer, "leer")) return LEER;
if (!strcmp(buffer, "escribir")) return ESCRIBIR;
if (!strcmp(buffer, "si")) return SI;
if (!strcmp(buffer, "entonces")) return ENTONCES;
if (!strcmp(buffer, "cortarCondicional")) return FIN_SI;   // ✅ AGREGAR ESTO
if (!strcmp(buffer, "mientras")) return MIENTRAS;
if (!strcmp(buffer, "hacer")) return HACER;
if (!strcmp(buffer, "cortarMientras")) return FIN_MIENTRAS;
if(!strcmp(buffer,"repetir")) return REPETIR;
if(!strcmp(buffer,"cortarRepetir")) return REPETIR_FIN;

        // if (!strcmp(buffer, "repetir")) return REPETIR;
        // ... (y otras si querés después)

        return ID;
    }

    // 🔹 Constante numérica (entera o real)
    if (isdigit(c)) {
        int tienePunto = 0;
        do {
            if (c == '.') {
                if (tienePunto) {
                    // Dos puntos = error léxico
                    buffer[i++] = c;
                    buffer[i] = '\0';
                    return ERRORLEXICO;
                }
                tienePunto = 1;
            }
            buffer[i++] = c;
            c = fgetc(in);
        } while (isdigit(c) || c == '.');

        buffer[i] = '\0';
        ungetc(c, in);

        if (buffer[strlen(buffer) - 1] == '.')
            return ERRORLEXICO;

        return (tienePunto ? CONSTANTE_REAL : CONSTANTE_ENTERO);
    }

    // 🔹 Constante de carácter
    if (c == '\'') {
        buffer[i++] = c;
        c = fgetc(in);
        if (!isprint(c) || c == '\'') return ERRORLEXICO;  // No puede estar vacío
        buffer[i++] = c;
        if (fgetc(in) != '\'') return ERRORLEXICO; // Debe cerrar con '
        buffer[i++] = '\'';
        buffer[i] = '\0';
        return CONSTANTE_CARACTER;
    }

    // 🔹 Operadores y símbolos
    // 🔹 Operadores y símbolos
switch (c) {
    case '+': return SUMA;
    case '-': return RESTA;
    case '(': return PARENIZQUIERDO;
    case ')': return PARENDERECHO;
    case ',': return COMA;
    case ';': return PUNTOYCOMA;

    // Asignación ":="
    case ':':
        c = fgetc(in);
        if (c == '=') return ASIGNACION;
        ungetc(c, in);
        return ERRORLEXICO;

    // ✅ MENOR "<"
    case '<':
        return MENOR;

    // ✅ MAYOR ">"
    case '>':
        return MAYOR;

    // ✅ IGUAL "=="
    case '=':
        c = fgetc(in);
        if (c == '=') return IGUAL;  // detecta ==
        ungetc(c, in);
        return ERRORLEXICO;

    default:
        return ERRORLEXICO;
}

}
