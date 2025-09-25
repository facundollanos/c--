/* Compilador del Lenguaje Micro (Fischer) */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NUMESTADOS 15
#define NUMCOLS 13
#define TAMLEX 32+1
#define TAMNOM 20+1

/****************** Declaraciones Globales *************************/
FILE *in;






// ----------------- TIPOS -------------------
typedef enum { 
    T_ENTERO, 
    T_FLOAT, 
    T_CHAR 
} TipoDato;


int numTemp = 1; // para generar nombres de variables temporales

// Tokens que devuelve el scanner
typedef enum {
    ID, 
    CONSTANTE_ENTERO, 
    CONSTANTE_REAL, 
    CONSTANTE_CARACTER,
    SUMA, RESTA,
    PARENIZQUIERDO, PARENDERECHO,
    COMA, PUNTOYCOMA,
    ASIGNACION,
    LEER, ESCRIBIR,
    SI, SINO, FIN_SI,
    MIENTRAS, HACER, FIN_MIENTRAS,
    REPETIR, HASTA,
    INICIO, FIN,
    FDT, ERRORLEXICO,ENTONCES
} TOKEN;

// Registro de tabla de símbolos
typedef struct {
    char identifi[TAMLEX]; // nombre del símbolo
    TOKEN t;               // token
    TipoDato tipo;         // tipo de dato
} RegTS;

// Expresión intermedia
typedef struct {
    char nombre[TAMLEX];
    TipoDato tipo;
    union {
        int valorEntero;
        float valorReal;
        char valorChar;
    };
} REG_EXPRESION;


RegTS TS[1000] = {
   {"inicio", INICIO},
   {"fin", FIN},
   {"leer", LEER},
   {"escribir", ESCRIBIR},
   {"si", SI},
   {"entonces", ENTONCES},
   {"sino", SINO},
   {"fin_si", FIN_SI},
   {"mientras", MIENTRAS},
   {"fin_mientras", FIN_MIENTRAS},
   {"repetir", REPETIR},
   {"hasta", HASTA},
   {"$", 99},
   {"hacer", HACER}
};

// typedef struct {
//     TOKEN clase;
//     char nombre[TAMLEX];
//     int valor;
// } REG_EXPRESION;






/* Duplicate typedef for REG_EXPRESION removed */


char buffer[TAMLEX];
TOKEN tokenActual;
int flagToken = 0;

/********************** Prototipos ************************/
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

/// MIAS
TipoDato BuscarTipo(const char *id, TOKEN *tok);
REG_EXPRESION ProcesarCteEntero(void);
REG_EXPRESION ProcesarCteReal(void);
REG_EXPRESION ProcesarCteCaracter(void);
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
void Colocar(char *id, RegTS *TS, TipoDato tipo);
void Chequear(char *s, TipoDato tipo);
void Comenzar(void);
void Terminar(void);
void Asignar(REG_EXPRESION izq, REG_EXPRESION der);

/*************************** Programa Principal ************************/
int main(int argc, char *argv[]) {
    char nomArchi[TAMNOM];
    int l;

    if (argc != 2) {
        printf("Uso: %s <archivo.m>\n", argv[0]);
        return -1;
    }

    strcpy(nomArchi, argv[1]);
    l = strlen(nomArchi);
    if (l > TAMNOM || nomArchi[l-1] != 'm' || nomArchi[l-2] != '.') {
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

/******************* PAS - Análisis Sintáctico *******************/
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
            case ID: 
            case LEER: 
            case ESCRIBIR:
            case SI: 
            case MIENTRAS: 
            case REPETIR:
                Sentencia();
                break;
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
            Asignar(izq, der);
            Match(PUNTOYCOMA);
            break;
        case LEER:
            Match(LEER); Match(PARENIZQUIERDO);
            ListaIdentificadores();
            Match(PARENDERECHO); Match(PUNTOYCOMA);
            break;
        case ESCRIBIR:
            Match(ESCRIBIR); Match(PARENIZQUIERDO);
            ListaExpresiones();
            Match(PARENDERECHO); Match(PUNTOYCOMA);
            break;
case SI:
    Match(SI);
    Match(PARENIZQUIERDO);
    REG_EXPRESION cond;
    Expresion(&cond);
    Match(PARENDERECHO);
    Match(ENTONCES);

    ListaSentencias();   // en vez de Sentencia(), porque puede haber varias

    if (ProximoToken() == SINO) {
        Match(SINO);
        ListaSentencias();
    }

    Match(FIN_SI);
    break;

case MIENTRAS: {
    Match(MIENTRAS);
    Match(PARENIZQUIERDO);
    REG_EXPRESION condMientras;
    Expresion(&condMientras);
    Match(PARENDERECHO);
    Match(HACER);

    // Bloque de sentencias dentro del mientras
    ListaSentencias();

    Match(FIN_MIENTRAS);
    break;
}

        case REPETIR:   // NUEVO
            Match(REPETIR);
            ListaSentencias();
            Match(HASTA);
            Match(PARENIZQUIERDO);
            REG_EXPRESION condRepetir;
            Expresion(&condRepetir);
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
    REG_EXPRESION izq, der;
    char op[TAMLEX];
    TOKEN t;

    Primaria(&izq);
    for (t = ProximoToken(); t == SUMA || t == RESTA; t = ProximoToken()) {
        OperadorAditivo(op);
        Primaria(&der);
        izq = GenInfijo(izq, op, der);
    }
    *presul = izq;
}

void Primaria(REG_EXPRESION *presul) {
    TOKEN tok = ProximoToken();
    switch (tok) {
        case ID: Identificador(presul); break;
case CONSTANTE_ENTERO:
    Match(CONSTANTE_ENTERO);
    *presul = ProcesarCteEntero();
    break;

case CONSTANTE_REAL:
    Match(CONSTANTE_REAL);
    *presul = ProcesarCteReal();
    break;

case CONSTANTE_CARACTER:
    Match(CONSTANTE_CARACTER);
    *presul = ProcesarCteCaracter();
    break;


        case PARENIZQUIERDO:
            Match(PARENIZQUIERDO);
            Expresion(presul);
            Match(PARENDERECHO);
            break;
        default: return;
    }
}

void OperadorAditivo(char *presul) {
    TOKEN t = ProximoToken();
    if (t == SUMA || t == RESTA) {
        Match(t);
        strcpy(presul, ProcesarOp());
    } else ErrorSintactico();
}

// REG_EXPRESION GenInfijo(REG_EXPRESION izq, char *op, REG_EXPRESION der) {
//     REG_EXPRESION res;
//     char cadOp[20];
//     char temp[TAMLEX];

//     // Promoción de tipo
//     if (izq.tipo == T_FLOAT || der.tipo == T_FLOAT)
//         res.tipo = T_FLOAT;
//     else
//         res.tipo = T_ENTERO;

//     // Crear variable temporal
//     sprintf(temp, "Temp&%d", numTemp++);
//     strcpy(res.nombre, temp);

//     // Decidir operación según tipo
//     if (strcmp(op, "+") == 0) {
//         if (res.tipo == T_FLOAT) strcpy(cadOp, "SumarReal");
//         else strcpy(cadOp, "SumarInt");
//     } else if (strcmp(op, "-") == 0) {
//         if (res.tipo == T_FLOAT) strcpy(cadOp, "RestarReal");
//         else strcpy(cadOp, "RestarInt");
//     }

//     // Generar instrucción
//     Generar(cadOp, izq.nombre, der.nombre, res.nombre);

//     return res;
// }




/******************* Rutinas Semánticas *******************/
// REG_EXPRESION ProcesarCte(void) {
//     REG_EXPRESION reg;
//     reg.clase = CONSTANTE;
//     strcpy(reg.nombre, buffer);
//     sscanf(buffer, "%d", &reg.valor);
//     return reg;
// }


// REG_EXPRESION ProcesarCte(void) {
//     REG_EXPRESION reg;
    
//     if (esEntero(buffer)) {
//         reg.tipo = T_ENTERO;
//         sscanf(buffer, "%d", &reg.valorEntero);
//     } 
//     else if (esFloat(buffer)) {
//         reg.tipo = T_FLOAT;
//         sscanf(buffer, "%f", &reg.valorReal);
//     } 
//     else if (esChar(buffer)) {
//         reg.tipo = T_CHAR;
//         reg.valorChar = buffer[1];   // ej: 'a' en buffer = "'a'"
//     } 
//     else {
//         ErrorSemantico("Constante no reconocida");
//     }

//     strcpy(reg.nombre, buffer); // útil para imprimir o generar código
//     return reg;
// }


REG_EXPRESION ProcesarCteEntero(void) {
    REG_EXPRESION reg;
    reg.tipo = T_ENTERO;
    sscanf(buffer, "%d", &reg.valorEntero);
    strcpy(reg.nombre, buffer);
    return reg;
}

REG_EXPRESION ProcesarCteReal(void) {
    REG_EXPRESION reg;
    reg.tipo = T_FLOAT;
    sscanf(buffer, "%f", &reg.valorReal);
    strcpy(reg.nombre, buffer);
    return reg;
}

REG_EXPRESION ProcesarCteCaracter(void) {
    REG_EXPRESION reg;
    reg.tipo = T_CHAR;
    reg.valorChar = buffer[1];   // ej: 'a'
    strcpy(reg.nombre, buffer);
    return reg;
}



TipoDato BuscarTipo(const char *id, TOKEN *tok) {
    int i = 0;
    while (strcmp(TS[i].identifi, "$") != 0) {
        if (strcmp(TS[i].identifi, id) == 0) {
            *tok = TS[i].t;
            return TS[i].tipo;
        }
        i++;
    }
    return T_ENTERO; // valor por defecto o error
}


REG_EXPRESION ProcesarId(void) {
    REG_EXPRESION reg;
    TOKEN t;
    TipoDato tipo = BuscarTipo(buffer, &t); // BuscarTipo consulta la TS
    Chequear(buffer,tipo);                       // lo declara si no estaba

    reg.tipo = tipo;                        // ahora sí el tipo real
    strcpy(reg.nombre, buffer);

    return reg;
}

char *ProcesarOp(void) { return buffer; }
void Leer(REG_EXPRESION in) { Generar("Read", in.nombre, "Entera", ""); }
void Escribir(REG_EXPRESION out) { Generar("Write", Extraer(&out), "Entera", ""); }

REG_EXPRESION GenInfijo(REG_EXPRESION e1, char *op, REG_EXPRESION e2) {
    REG_EXPRESION reg;
    static unsigned int numTemp = 1;
    char cadTemp[TAMLEX] = "Temp&", cadNum[TAMLEX], cadOp[TAMLEX];

    sprintf(cadNum, "%d", numTemp++);
    strcat(cadTemp, cadNum);

    // Promoción de tipos
    if (e1.tipo == T_FLOAT || e2.tipo == T_FLOAT)
        reg.tipo = T_FLOAT;
    else
        reg.tipo = T_ENTERO;

    // Operador
    if (op[0] == '-') {
        strcpy(cadOp, (reg.tipo == T_FLOAT) ? "RestarReal" : "RestarInt");
    } else {
        strcpy(cadOp, (reg.tipo == T_FLOAT) ? "SumarReal" : "SumarInt");
    }

    // if (e1.tipo == ID) Chequear(Extraer(&e1), e1.tipo);
    // if (e2.tipo == ID) Chequear(Extraer(&e2), e2.tipo);
    Chequear(cadTemp, reg.tipo);

    Generar(cadOp, Extraer(&e1), Extraer(&e2), cadTemp);
    strcpy(reg.nombre, cadTemp);

    return reg;
}

/******************* Auxiliares *******************/
void Match(TOKEN t) {
    if (!(t == ProximoToken())) ErrorSintactico();
    flagToken = 0;
}

TOKEN ProximoToken() {
    if (!flagToken) {
        tokenActual = scanner();
        if (tokenActual == ERRORLEXICO) ErrorLexico();
        flagToken = 1;
        if (tokenActual == ID) Buscar(buffer, TS, &tokenActual);
    }
    return tokenActual;
}

void ErrorLexico() { printf("Error Lexico\n"); }
void ErrorSintactico() { printf("Error Sintactico\n"); }
void Generar(char *co, char *a, char *b, char *c) { printf("%s %s,%s,%s\n", co, a, b, c); }
char *Extraer(REG_EXPRESION *preg) { return preg->nombre; }

int Buscar(char *id, RegTS *TS, TOKEN *t) {
    int i = 0;
    while (strcmp("$", TS[i].identifi)) {
        if (!strcmp(id, TS[i].identifi)) { *t = TS[i].t; return 1; }
        i++;
    }
    return 0;
}

void Colocar(char *id, RegTS *TS, TipoDato tipo) {
    int i = 4; // los primeros slots son palabras reservadas
    while (strcmp("$", TS[i].identifi)) i++;
    if (i < 999) {
        strcpy(TS[i].identifi, id);
        TS[i].t = ID;
        TS[i].tipo = tipo;   
        strcpy(TS[++i].identifi, "$");
    }
}

// void Chequear(char *s) {
//     TOKEN t;
//     if (!Buscar(s, TS, &t)) {
//         Colocar(s, TS);
//         Generar("Declara", s, "Entera", "");
//     }
// }


void Chequear(char *s, TipoDato tipo) {
    TOKEN t;
    if (!Buscar(s, TS, &t)) {
        Colocar(s, TS, tipo);
        if (tipo == T_ENTERO)
            Generar("Declara", s, "Entera", "");
        else if (tipo == T_FLOAT)
            Generar("Declara", s, "Real", "");
        else if (tipo == T_CHAR)
            Generar("Declara", s, "Caracter", "");
    }
}


void Comenzar(void) {}
void Terminar(void) { Generar("Detiene", "", "", ""); }
void Asignar(REG_EXPRESION izq, REG_EXPRESION der) {
    Generar("Almacena", Extraer(&der), izq.nombre, "");
}

/******************* Scanner *******************/
TOKEN scanner() {
    int tabla[NUMESTADOS][NUMCOLS] = {
        {1,3,5,6,7,8,9,10,11,14,13,0,14},
        {1,1,2,2,2,2,2,2,2,2,2,2,2},
        {14,14,14,14,14,14,14,14,14,14,14,14,14},
        {4,3,4,4,4,4,4,4,4,4,4,4,4},
        {14,14,14,14,14,14,14,14,14,14,14,14,14},
        {14,14,14,14,14,14,14,14,14,14,14,14,14},
        {14,14,14,14,14,14,14,14,14,14,14,14,14},
        {14,14,14,14,14,14,14,14,14,14,14,14,14},
        {14,14,14,14,14,14,14,14,14,14,14,14,14},
        {14,14,14,14,14,14,14,14,14,14,14,14,14},
        {14,14,14,14,14,14,14,14,14,14,14,14,14},
        {14,14,14,14,14,14,14,14,14,12,14,14,14},
        {14,14,14,14,14,14,14,14,14,14,14,14,14},
        {14,14,14,14,14,14,14,14,14,14,14,14,14},
        {14,14,14,14,14,14,14,14,14,14,14,14,14}
    };

    int car, col, estado = 0, i = 0;
    do {
        car = fgetc(in);
        col = columna(car);
        estado = tabla[estado][col];
        if (col != 11) buffer[i++] = car;
    } while (!estadoFinal(estado) && estado != 14);
    buffer[i] = '\0';

switch (estado) {
    case 2: 
        if (col != 11) { ungetc(car, in); buffer[i-1] = '\0'; }
        return ID;

    case 4: 
        if (col != 11) { ungetc(car, in); buffer[i-1] = '\0'; }
        if (buffer[0] == '\'' && buffer[2] == '\'' && buffer[3] == '\0')
            return CONSTANTE_CARACTER;   // ej: 'a'
        else if (strchr(buffer, '.') != NULL)
            return CONSTANTE_REAL;       // ej: 3.14
        else
            return CONSTANTE_ENTERO;     // ej: 123

    case 5: return SUMA;
    case 6: return RESTA;
    case 7: return PARENIZQUIERDO;
    case 8: return PARENDERECHO;
    case 9: return COMA;
    case 10: return PUNTOYCOMA;
    case 12: return ASIGNACION;
    case 13: return FDT;
    case 14: return ERRORLEXICO;
}
    return 0;
}

int estadoFinal(int e) { return !(e == 0 || e == 1 || e == 3 || e == 11 || e == 14); }

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
    return 12;
}
