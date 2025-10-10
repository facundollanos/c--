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
    CONSTANTE_REAL, CONSTANTE_CARACTER, MIENTRAS, SI, ENTONCES

} TOKEN;


typedef enum {
    T_ENTERO,
    T_FLOAT,
    T_CHAR
} TipoDato;

typedef struct {
    char identifi[TAMLEX];
    TOKEN t;
} RegTS;

RegTS TS[1000] = {
    {"inicio", INICIO}, {"fin", FIN}, {"leer", LEER}, {"escribir", ESCRIBIR}, {"$", 99}
};

typedef struct {
    TOKEN clase;
    char nombre[TAMLEX];
    union {
        int valorEntero;
        float valorReal;
        char valorChar;
    } valor;
} REG_EXPRESION;

char buffer[TAMLEX];
TOKEN tokenActual;
int flagToken = 0;

/* Prototipos */
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
REG_EXPRESION ProcesarReal(void);
REG_EXPRESION ProcesarChar(void);
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
void Colocar(char *id, RegTS *TS);
void Chequear(char *s, TipoDato tipo);
void Comenzar(void);
void Terminar(void);
void Asignar(REG_EXPRESION izq, REG_EXPRESION der);

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
    Sentencia();
    while (1) {
        switch (ProximoToken()) {
            case ID: case LEER: case ESCRIBIR:
                Sentencia();
                break;
            default: return;
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
            Asignar(izq, der);
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

void Identificador(REG_EXPRESION *presul) {
    Match(ID);
    *presul = ProcesarId();
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
            Match(CONSTANTE_ENTERO);
            *presul = ProcesarCte();
            break;
            case CONSTANTE_REAL:
            Match(CONSTANTE_REAL);
            *presul = ProcesarReal();
            break;
            case CONSTANTE_CARACTER:
            Match(CONSTANTE_CARACTER);
            *presul = ProcesarChar();
            break;
        case PARENIZQUIERDO:
            Match(PARENIZQUIERDO);
            Expresion(presul);
            Match(PARENDERECHO);
            break;
        default: ErrorSintactico(); break;
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

REG_EXPRESION ProcesarCte(void) {
    REG_EXPRESION reg;
    reg.clase = CONSTANTE;
    strcpy(reg.nombre, buffer);
    sscanf(buffer, "%d", &reg.valor.valorEntero);
    return reg;
}



/////FUNCION MIA
REG_EXPRESION ProcesarChar(void) {
    REG_EXPRESION reg;
    reg.clase = CONSTANTE_CARACTER;
    strcpy(reg.nombre, buffer);
    reg.valor.valorChar = buffer[0]; // el buffer tiene solo 'c'
    return reg;
}

//////FUNCION MIA
REG_EXPRESION ProcesarReal(void) {
    REG_EXPRESION reg;
    reg.clase = CONSTANTE_REAL;
    strcpy(reg.nombre, buffer);
    sscanf(buffer, "%f", &reg.valor.valorReal);
    return reg;
}



REG_EXPRESION ProcesarId(void) {
    REG_EXPRESION reg;
    Chequear(buffer, T_ENTERO);
    reg.clase = ID;
    strcpy(reg.nombre, buffer);
    reg.valor.valorEntero = 0;
    return reg;
}

/////////// MIS FUNCIONES
REG_EXPRESION ProcesarFlotante(void) {
    REG_EXPRESION reg;
    reg.clase = T_FLOAT;
    strcpy(reg.nombre, buffer);
    sscanf(buffer, "%f", &reg.valor.valorReal);
    return reg;
}

char *ProcesarOp(void) { return buffer; }

void Escribir(REG_EXPRESION out) {
    char tipo[20];

    switch (out.clase) {
        case CONSTANTE:      strcpy(tipo, "Entera"); break;
        case CONSTANTE_REAL: strcpy(tipo, "Real"); break;
        case CONSTANTE_CARACTER: strcpy(tipo, "Caracter"); break;
        case ID:             strcpy(tipo, "Entera"); break; // o buscar tipo en tabla
        default:             strcpy(tipo, "Desconocido"); break;
    }

    Generar("Write", Extraer(&out), tipo, "");
}

void Leer(REG_EXPRESION in) {
    char tipo[16];

    switch (in.clase) {
        case ID:           strcpy(tipo, "Entera"); break; // o según la tabla de símbolos
        case CONSTANTE:     strcpy(tipo, "Entera"); break;
        case CONSTANTE_REAL: strcpy(tipo, "Real"); break;
        case CONSTANTE_CARACTER: strcpy(tipo, "Caracter"); break;
        default:             strcpy(tipo, "Desconocido"); break;
    }

    Generar("Read", in.nombre, tipo, "");
}



REG_EXPRESION GenInfijo(REG_EXPRESION e1, char *op, REG_EXPRESION e2) {
    REG_EXPRESION reg;
    static unsigned int numTemp = 1;
    char cadTemp[TAMLEX] = "Temp&";
    char cadNum[TAMLEX];
    char cadOp[TAMLEX];
    if (op[0] == '-') strcpy(cadOp, "Restar");
    else strcpy(cadOp, "Sumar");
    sprintf(cadNum, "%u", numTemp++);
    strcat(cadTemp, cadNum);
    if (e1.clase == ID) Chequear(Extraer(&e1), T_ENTERO);
    if (e2.clase == ID) Chequear(Extraer(&e2), T_ENTERO);
    Chequear(cadTemp, T_ENTERO);
    Generar(cadOp, Extraer(&e1), Extraer(&e2), cadTemp);
    strcpy(reg.nombre, cadTemp);
    reg.clase = ID;
    reg.valor.valorEntero = 0;
    return reg;
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

int Buscar(char *id, RegTS *TS, TOKEN *t) {
    int i = 0;
    while (strcmp("$", TS[i].identifi)) {
        if (!strcmp(id, TS[i].identifi)) {
            *t = TS[i].t;
            return 1;
        }
        i++;
    }
    return 0;
}

void Colocar(char *id, RegTS *TS) {
    int i = 4;
    while (strcmp("$", TS[i].identifi)) i++;
    if (i < 999) {
        strcpy(TS[i].identifi, id);
        TS[i].t = ID;
        strcpy(TS[++i].identifi, "$");
        TS[i].t = 99;
    }
}

void Chequear(char *s, TipoDato tipo) {
    TOKEN t;
    if (!Buscar(s, TS, &t)) {
        Colocar(s, TS);
        if (tipo == T_ENTERO)
            Generar("Declara", s, "Entera", "");
        else if (tipo == T_FLOAT)
            Generar("Declara", s, "Real", "");
        else if (tipo == T_CHAR)
            Generar("Declara", s, "Caracter", "");
    }
}

void Comenzar(void) { }
void Terminar(void) { Generar("Detiene", "", "", ""); }
void Asignar(REG_EXPRESION izq, REG_EXPRESION der) {
    Generar("Almacena", Extraer(&der), izq.nombre, "");
}

/* ------------------ Scanner ------------------ */

TOKEN scanner() {
int tabla[NUMESTADOS][NUMCOLS] = {
/* a  d  +  -  (  )  ,  ;  :  = EOF sp  .  '  otro */
  { 1, 3, 5, 6, 7, 8, 9,10,11,14,13, 0,15,17,14 }, // 0 inicio
  { 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 }, // 1 letras → ID
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 2 fin ID
  { 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,15, 4, 4 }, // 3 dígitos ('.'→15)
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 4 fin entero
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 5 '+'
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 6 '-'
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 7 '('
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 8 ')'
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, // 9 ','
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, //10 ';'
  {14,14,14,14,14,14,14,14,14,12,14,14,14,14,14}, //11 ':'
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, //12 ':='
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, //13 FDT
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}, //14 error
  {14,16,14,14,14,14,14,14,14,14,14,14,14,14,14}, //15 vio '.' espera dígito
  {14,16,14,14,14,14,14,14,14,14,14,14,14,14,14}, //16 parte fraccionaria (float)
  {14,14,14,14,14,14,14,14,14,14,14,14,14,19,14}, //17 leí `'`, espero carácter
  {19,19,19,19,19,19,19,19,19,19,19,19,19,19,19}, //18 leí carácter, espero `'` cierre
  {14,14,14,14,14,14,14,14,14,14,14,14,14,14,14}  //19 fin char (se maneja en switch)
};

    int car, col, estado = 0, i = 0;
    do {
        car = fgetc(in);
        col = columna(car);
        estado = tabla[estado][col];
        if (col != 11 && i < TAMLEX-1) buffer[i++] = (char)car;
    } while (!estadoFinal(estado) && !(estado == 14));
    buffer[i] = '\0';



/* ... inside scanner() after the do-while loop ... */

switch (estado) {
    case 2:
    case 4:
case 16: // CONSTANTE_REAL (número con parte decimal)
    if (col != 11 && car != EOF) {
        ungetc(car, in);
        buffer[i-1] = '\0';
    }

    // Si el número termina con un punto, es un error léxico (ej: 123.)
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '.')
        return ERRORLEXICO;

    // Si no termina en punto, es un número flotante válido
    return CONSTANTE_REAL;

    case 19: // 👈 CONSTANTE_CARACTER ends here
        // The token is now in buffer: `'x'`
        return CONSTANTE_CARACTER;

    case 5:  return SUMA;
    case 6:  return RESTA;
    case 7:  return PARENIZQUIERDO;
    case 8:  return PARENDERECHO;
    case 9:  return COMA;
    case 10: return PUNTOYCOMA;
    case 12: return ASIGNACION;
    case 13: return FDT;
    case 14: return ERRORLEXICO;
    default: return ERRORLEXICO;
}




}

int estadoFinal(int e) {
    // estados NO finales (se sigue leyendo)
    if (e == 0 || e == 1 || e == 3 || e == 11 || e == 15 || e == 17 || e == 18)
        return 0;
    // todos los demás son finales
    return 1; // Includes states 2, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 16, 19
}

int columna(int c) {
    if (isalpha(c)) return 0;
    if (isdigit(c)) return 1;
    if (c == '+') return 2;
    if (c == '-') return 3;
    if (c == '(') return 4;
    if (c == ')') return 5;
    if (c == ',') return 6;
    if (c == ';') return 7;
    if (c == ':') return 8;
    if (c == '=') return 9;
    if (c == EOF) return 10;
    if (isspace(c)) return 11;
    if (c == '.') return 12;
    if (c == '\'') return 13;
    if (c == '<' || c == '>') return 14;
    return 14; // "otro"
}
