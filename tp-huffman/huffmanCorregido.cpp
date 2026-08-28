
#ifndef _MAIN
#define _MAIN

#include <cstring>
#include <iostream>
#include <bitset>
#include <string.h>
#include "C:\Users\Sebastian\Desktop\AlgoritmosAFondo_FULL_v2.7.15\src\biblioteca\funciones\strings.hpp"
#include "C:\Users\Sebastian\Desktop\AlgoritmosAFondo_FULL_v2.7.15\src\biblioteca\funciones\tokens.hpp"
#include "C:\Users\Sebastian\Desktop\AlgoritmosAFondo_FULL_v2.7.15\src\biblioteca\funciones\files.hpp"
#include "C:\Users\Sebastian\Desktop\AlgoritmosAFondo_FULL_v2.7.15\src\biblioteca\funciones\arrays.hpp"
#include "C:\Users\Sebastian\Desktop\AlgoritmosAFondo_FULL_v2.7.15\src\biblioteca\tads\parte2\List.hpp"
#include "C:\Users\Sebastian\Desktop\AlgoritmosAFondo_FULL_v2.7.15\src\biblioteca\tads\parte2\Map.hpp"
#include "C:\Users\Sebastian\Desktop\AlgoritmosAFondo_FULL_v2.7.15\src\biblioteca\tads\parte2\Queue.hpp"
#include "C:\Users\Sebastian\Desktop\AlgoritmosAFondo_FULL_v2.7.15\src\biblioteca\tads\parte2\Stack.hpp"
#include "C:\Users\Sebastian\Desktop\AlgoritmosAFondo_FULL_v2.7.15\src\biblioteca\tads\huffman\HuffmanSetup.hpp"
#include "C:\Users\Sebastian\Desktop\AlgoritmosAFondo_FULL_v2.7.15\src\biblioteca\tads\huffman\BitWriter.hpp"
#include "C:\Users\Sebastian\Desktop\AlgoritmosAFondo_FULL_v2.7.15\src\biblioteca\tads\huffman\BitReader.hpp"



string intToBin(int decimal)
{
   string binario;
   while( decimal>0 )
   {
      if( decimal%2==0 )
      {
         binario = "0"+binario;
      }
      else
      {
         binario = "1"+binario;
      }

      decimal /= 2;
   }

   return binario;
}

int cmpInt(int a, int b)
{
   return a-b;
}

int cmpHuffmanTreeInfo(HuffmanTreeInfo c1, HuffmanTreeInfo c2)
{
   int a = cmpInt(c1.n,c2.n);

   if( a==0 )
   {
      int byte1 = c1.c;
      int byte2 = c2.c;

      a = cmpInt(byte1,byte2);

   }

   return a;
}





void contarOcurrencias(FILE* f, HuffmanTable tabla[])
{
   // Creo la tabla del �rbol vac�a.
   for( int i = 0; i<256; i++ )
   {
      tabla[i].n = 0;
      tabla[i].cod = "";
   }

   int c;

   while( (c = fgetc(f))!=EOF)
   {
      if( c>=0&&c<256 )
      {
         unsigned char ch = static_cast<unsigned char>(c);
         tabla[ch].n++;
      }
   }

}

List<HuffmanTreeInfo> crearLista(List<HuffmanTreeInfo>& lista, HuffmanTable tabla[256])
{

   for( int i = 0; i<256; i++ )
   {
      int cByte = i; // Vemos qu� Byte vamos a trabajar.

      if( tabla[i].n>0 )
      {
         // Creamos un HuffmanTreeInfo con los datos de cada Byte y sus ocurrencias.
         HuffmanTreeInfo *carac = huffmanTreeInfo(cByte,tabla[i].n,NULL,NULL);          // Agregamos el caracter a la lista (usando la funci�n listAdd si est� definida).
         listAdd<HuffmanTreeInfo>(lista,*carac);
      }
   }

   // Ordenamos la lista (usando la funci�n listSort si est� definida).
   listSort<HuffmanTreeInfo>(lista,cmpHuffmanTreeInfo);

   return lista;
}

HuffmanTreeInfo* crearArbol(List<HuffmanTreeInfo>& lista)
{
   HuffmanTreeInfo* huffInfoNuevo= NULL;

   int i = 255;

   //Mientras el tama�o de la lista sea mayor a 0...
   while( listSize(lista)>1 )
   {
      HuffmanTreeInfo* huffInfoDer = huffmanTreeInfo(0,0,NULL,NULL); //Removemos el primer elemento de la lista / hijo derecho.
      *huffInfoDer = listRemoveFirst<HuffmanTreeInfo>(lista); //Removemos el primer elemento de la lista / hijo derecho.
      HuffmanTreeInfo* huffInfoIzq = huffmanTreeInfo(0,0,NULL,NULL); //Removemos el primer elemento de la lista / hijo izquierdo.
      *huffInfoIzq = listRemoveFirst<HuffmanTreeInfo>(lista); //Removemos el primer elemento de la lista / hijo izquierdo.
      int ocurrencia = huffInfoIzq->n+huffInfoDer->n; //Contamos las ocurrencias totales de los hijos.
       huffInfoNuevo = huffmanTreeInfo(i,ocurrencia,huffInfoIzq,huffInfoDer); //Hacemos el HuffmanTreeInfo*.
      listOrderedInsert(lista,*huffInfoNuevo,cmpHuffmanTreeInfo); //Ordenamos la lista.
      i++;
   }


   return huffInfoNuevo;
}




void cargarCodigoEnTabla(HuffmanTreeInfo* raiz, HuffmanTable tabla[]){
   //Obtengo el �rbol a partir de su ra�z.
   HuffmanTree arbol = huffmanTree(raiz);

   while(huffmanTreeHasNext(arbol)){  //Recorremos el arbol hasta que no queden hojas.
      string x = "";
      HuffmanTreeInfo* t = huffmanTreeNext(arbol, x); //Obtenemos un puntero para las hojas.

      cout << t->c << ", ("<<t->n <<"), " << "[" << x <<"]" << endl;

      int pos = t->c; //Usaremos el numero en ASCII del Byte para acceder a la posicion del array de tablas.
      tabla[pos].cod = x; //Obtenemos la fila de la tabla del Byte que necesitamos.
   }
}

struct Registros
{
   unsigned int c;
   unsigned int h;
   string m;
};

Registros registros()
{
   Registros regis;
   regis.c = 0;
   regis.h = 0;
   regis.m = "";
   return regis;
}

FILE* crearFile(string fName)
{
   //Creamos el nombre del archivo nuevo.
   string nuevo = fName+".huf";
   int sizeNuevo = length(nuevo);
   char newName[sizeNuevo];
//   fName.copy(newName, fName.length() + 1);
   strcpy(newName,nuevo.c_str());
   //Creamos el archivo sobre el cual escribiremos la compresi�n.
   FILE* fComprimido = fopen(newName,"w+b");

   return fComprimido;
}

FILE* obtenerOriginalFile(string fName)
{
   //Obtengo el nombre del archivo original.
   int sizeOriginal = length(fName);
   char fNameOriginal[sizeOriginal];
   strcpy(fNameOriginal,fName.c_str());
   //Abrimos para leer el archivo original.
   FILE* fOriginal = fopen(fNameOriginal,"r+b");

   return fOriginal;
}

string escribirM(string code)
{
   int largo = length(code);

   while( largo%8!=0 )
   {
      code += "0";
      largo = length(code);
   }

   return code;
}

void sizeOriginalToChar(int sizeOriginal, unsigned char size[4])
{
   string c = intToBin(sizeOriginal);
   string aux = "";

   while( length(c)%32!=0 )
   {
      c = "0"+c;
   }

   for( int i = 0,j = 0; i<32; i++ )
   {
      aux += c[i];
      if( (i+1)%8==0 )
      {
         int numByte = stringToInt(aux,2);
         unsigned char cByte = numByte;
         size[j] = cByte;
         j++;
      }
   }
}

void bytesToChar(string code, unsigned char size[4])
{
   string aux = "";

   for( int i = 0,j = 0; i<length(code); i++ )
   {
      aux += code[i];
      if( (i+1)%8==0 )
      {
         int numByte = stringToInt(aux,2);
         unsigned char cByte = numByte;
         size[j] = cByte;
         j++;
      }
   }
}

void comprimirArchivoOriginal(FILE*& fComprimido, FILE* fOriginal, HuffmanTable arrTabla[])
{
   //Me posiciono al inicio del archivo original.
   seek<unsigned char>(fOriginal,0);
   BitWriter bw = bitWriter(fComprimido);
   //Leo el byte correspondiente.
   unsigned char cByteLeido = read<unsigned char>(fOriginal);

   //Mientras el archivo tenga elementos...
   while( !feof(fOriginal) )
   {
      //Obtengo la posici�n del byte le�do en el array.
      unsigned int pos = cByteLeido;
      //Obtengo el c�digo Huffman correspondiente al byte.
      string codeHuffman = arrTabla[pos].cod;
      bitWriterWrite(bw,codeHuffman);
      //Leo el siguiente byte del archivo.
      cByteLeido = read<unsigned char>(fOriginal);
   }

   bitWriterFlush(bw);
}

void escribirArchivoComprimido(FILE*& fComprimido, FILE* fOriginal, int sizeOriginal, int contHojas, Registros arrRegistros[], HuffmanTable arrTabla[], int lenRegistro)
{
   //Convertimos la cantidad de hojas de un int a un unsigned char.
   unsigned char hojas = contHojas;

   write<unsigned char>(fComprimido,hojas); //Escribimos en el archivo cuantas hojas hay
   for( int i = lenRegistro,pos = 0; i>0; i--, pos++ )
   { //Grabaremos en el archivo todos los registros que contienen c, h y m.
      Registros reg = arrRegistros[pos];
      unsigned char c = reg.c;
      unsigned char h = reg.h;
      unsigned char m[10];
      bytesToChar(reg.m,m); //Transformamos el code de binario a decimal
      write<unsigned char>(fComprimido,c);
      write<unsigned char>(fComprimido,h);
      for( int j = 0,hCont = reg.h; j<10&&hCont>0; j++, hCont -= 8 )
      {
         write<unsigned char>(fComprimido,m[j]);
      }
   }

   //Obtenemos el tama�o del archivo original.
   unsigned int tamanio = fileSize<unsigned char>(fOriginal);
   write<unsigned int>(fComprimido,tamanio); //Lo escribimos en el archivo comprimido.
   comprimirArchivoOriginal(fComprimido,fOriginal,arrTabla); //Comprimimos el archivo original.
}

void grabarArchivoComprimido(HuffmanTable arrTabla[], FILE*& fComprimido, FILE* fOriginal)
{
   int sizeOriginal = fileSize<unsigned char>(fOriginal);
   Registros arrRegistros[256]; //Creamos un array que va a guardar los registros por cada hoja del �rbol.
   int contHojas = 0;
   int len = 0;
   for( int i = 0; i<256; i++ )
   {
      int cByte = i; //Vemos qu� Byte vamos a trabajar.

      if( arrTabla[i].n>0 )
      { //Si el Byte tiene ocurrencia, entra; sino, pasamos al siguiente.
        //Obtenemos el registro.
         Registros registro = registros();
         registro.c = cByte;
         registro.h = length(arrTabla[i].cod);
         registro.m = escribirM(arrTabla[i].cod);
         add(arrRegistros,len,registro); //Agregamos el registro al array.
         contHojas++;
      }

   }

   escribirArchivoComprimido(fComprimido,fOriginal,sizeOriginal,contHojas,arrRegistros,arrTabla,len);
}

void comprimir(string fName )
{

   HuffmanTable tabla[256];
   FILE* fComprimido = crearFile(fName); //Creamos un archivo para escribir la compresi�n.
   FILE* fOriginal = obtenerOriginalFile(fName); //Obtenemos el archivo original.
   contarOcurrencias(fOriginal,tabla);

   List<HuffmanTreeInfo> lista = list<HuffmanTreeInfo>();
   crearLista(lista,tabla);

   HuffmanTreeInfo* raiz = crearArbol(lista);

   cargarCodigoEnTabla(raiz,tabla);


   grabarArchivoComprimido(tabla,fComprimido,fOriginal); //Comprimimos el archivo original.

   fclose(fOriginal);
   fclose(fComprimido);

   cout<<"llego hasta el final 2";

}

//void addHijoDerecho(HuffmanTreeInfo*& aux, int registroC, int registroH)
//{
//   //Vemos si el hijo derecho no tiene ningun HuffmanTreeInfo creado.
//   if( registroH==1 )
//   { //Si el registroH == 1, entonces estamos por colocar la hoja.
//      aux->left = huffmanTreeInfo(0,0,NULL,NULL);
//      aux->right->c = registroC; //Agregamos el Byte o car�cter que le corresponde a la hoja.
//      aux = aux->left; //Nos posicionamos en el siguiente nodo del �rbol.
//   }
//   else
//      if( aux->right==NULL )
//      { //Si el regsitro H != 1 es porque todavia hay que seguir
//        //agregando huffmanTreeInfo antes de llegar a la hoja.
//         aux->right = huffmanTreeInfo(0,0,NULL,NULL);
//         aux = aux->right; //Nos posicionamos en el siguiente nodo del �rbol.
//      }
//      else
//      { //Si aux->der != NULL quiere decir que el hijo derecho ya tiene un huffManTree agregado.
//         aux = aux->right; //Nos posicionamos en el siguiente nodo del �rbol.
//      }
//}
//
//void addHijoIzquierdo(HuffmanTreeInfo*& aux, int registroC, int registroH)
//{
//   //Vemos si el hijo izquierdo no tiene ningun HuffmanTreeInfo creado.
//   if( registroH==1 )
//   { //Si el registroH == 1, entonces estamos por colocar la hoja.
//      aux->left = huffmanTreeInfo(0,0,NULL,NULL);
//      aux->left->c = registroC; //Agregamos el Byte o car�cter que le corresponde a la hoja.
//      aux = aux->left; //Nos posicionamos en el siguiente nodo del �rbol.
//   }
//   else
//      if( aux->left==NULL )
//      { //Si el regsitro H != 1 es porque todavia hay que seguir
//        //agregando huffmanTreeInfo antes de llegar a la hoja.
//         aux->left = huffmanTreeInfo(0,0,NULL,NULL);
//         aux = aux->left; //Nos posicionamos en el siguiente nodo del �rbol.
//      }
//      else
//      { //Si aux->izq != NULL quiere decir que el hijo izquierdo ya tiene un huffManTree agregado.
//         aux = aux->left; //Nos posicionamos en el siguiente nodo del �rbol.
//      }
//}


void addHijoDerecho(HuffmanTreeInfo*& aux, int registroC, int registroH)
{
   if( registroH==1 )
   {
      if(aux->right == NULL)
      {
         aux->right = huffmanTreeInfo(0,0,NULL,NULL);
      }
      aux->right->c = registroC;
      aux = aux->right;
   }
   else
   {
      if( aux->right==NULL )
      {
         aux->right = huffmanTreeInfo(0,0,NULL,NULL);
      }
      aux = aux->right;
   }
}

void addHijoIzquierdo(HuffmanTreeInfo*& aux, int registroC, int registroH)
{
   if( registroH==1 )
   {
      if(aux->left == NULL)
      {
         aux->left = huffmanTreeInfo(0,0,NULL,NULL);
      }
      aux->left->c = registroC;
      aux = aux->left;
   }
   else
   {
      if( aux->left==NULL )
      {
         aux->left = huffmanTreeInfo(0,0,NULL,NULL);
      }
      aux = aux->left;
   }
}

HuffmanTreeInfo*armarArbolHuffman(FILE*& fHuffman)
{
   seek<unsigned char>(fHuffman,0); //Nos posicionamos al inicio del archivo comprimido.
   unsigned char cH = read<unsigned char>(fHuffman); //Obtengo el cByte con la cantidad de hojas
   int cantHojas = cH;

   if( cantHojas==0 )
   {
      cantHojas = 256;
   }
   HuffmanTreeInfo* raiz = huffmanTreeInfo(0,0,NULL,NULL);

   //Mientras la cantidad de hojas sea mayor a 0...
   while( cantHojas>0 )
   {
      HuffmanTreeInfo* aux = raiz;
      unsigned char rC = read<unsigned char>(fHuffman); //Leemos el cByte correspondiente al car�cter que est� en el registro t.
      int registroC = rC;
      unsigned char rH = read<unsigned char>(fHuffman); //Leemos el cByte correspondiente a la longitud que est� en el registro t.
      int registroH = rH;
      BitReader br = bitReader(fHuffman);

      //Mientras la longitud del byte sea mayor a 0...
      while( registroH>0 )
      {
         int bit = bitReaderRead(br); //Ahora leeremos bit a bit correspondientes al Byte de m.

         if( bit==1 )
         { //Si el bit es 1...
            addHijoDerecho(aux,registroC,registroH); //Agregamos el hijo derecho.
         }
         if( bit==0 )
         { //Si el bit es 0...
            addHijoIzquierdo(aux,registroC,registroH); //Agregamos el hijo izquierdo.
         }

         registroH--;
      }

      cantHojas--;
   }

   return raiz;
}

int descomprimirSizeOriginal(FILE*& fHuffman)
{
   string binTotal = "";

   for( int i = 0; i<4; i++ )
   {
      unsigned char c = read<unsigned char>(fHuffman);
      string cByteLeido = _binToString(c);
      binTotal = binTotal+cByteLeido;
   }
   unsigned int size = stringToInt(binTotal,2);

   return size;
}

//Obtenemos el nombre del archivo original.
string crearArchivoDescomprimir(string fName)
{
   string nombre = "";
   int i = length(fName)-4;
   int j = 0;

   while( i>0 )
   {
      nombre += fName[j];
      j++;
      i--;
   }

   return nombre;
}

//Funci�n que permite recorrer el �rbol.
int recorrerArbol(HuffmanTreeInfo*& aux, int bit)
{
   if( bit==1 )
   { //Si el bit es 1...
      aux = aux->right;
      if( aux->right==NULL&&aux->left==NULL )
      {
         return aux->c;
      }
   }
   if( bit==0 )
   { //Si el bit es 0...
      aux = aux->left;
      if( aux->right==NULL&&aux->left==NULL )
      {
         return aux->c;
      }
   }

   return -1;
}

void restaurarArchivoOrignal(FILE* fHuffman, FILE*& fDescomprimdo, HuffmanTreeInfo* raiz, unsigned int sizeOriginal)
{
   BitReader br = bitReader(fHuffman);
   HuffmanTreeInfo* aux = raiz;
   int bit = bitReaderRead(br); //Leemos el primer bit.
   int i=0;
   //Mientras el archivo comprimido tenga elementos...
   while( !feof(fHuffman) )
   {
      int cByteEnAscii = recorrerArbol(aux,bit); //Recorremos el �rbol.

      if( cByteEnAscii>=0&&cByteEnAscii<257 )
      { //Si el byte pertenece al rango de combinaciones de bits posibles...
         unsigned char cByte = cByteEnAscii;
         write<unsigned char>(fDescomprimdo,cByte); //Escribimos el byte en el archivo que va a simular ser el original.
         cout << cByte << endl;
         aux = raiz;
         i++;
         if (i==sizeOriginal)
         {
            break;
         }
      }

      bit = bitReaderRead(br); //Leemos el siguiente bit.
   }
}

void descomprimir(string fName)
{

   string sNameOriginal = crearArchivoDescomprimir(fName); //Obtengo el nombre del archivo original.

   int sizeFileHuff = length(fName); //Obtengo la longitud del nombre del archivo comprimido.

   char fNameHuff[sizeFileHuff]; //Creo un array con la longitud del nombre del archivo comprimido.

   char fNameOriginal[sizeFileHuff-4]; //Creo un array con la longitud del nombre del archivo original.

   strcpy(fNameHuff,fName.c_str()); //Obtengo el nombre del archivo comprimido como string.

//   fName.copy(fNameHuff, fName.length() + 1);

   strcpy(fNameOriginal,sNameOriginal.c_str());

   FILE* fHuffman = fopen(fNameHuff,"r+b"); //Abrimos el archivo comprimido.

   FILE* fDescomprimido = fopen(fNameOriginal,"w+b"); //Creamos un archivo nuevo para restaurar aqu� el original.

   HuffmanTreeInfo* raiz = armarArbolHuffman(fHuffman); //Armamos el �rbol a partir del archivo comprimido.

   unsigned int sizeOriginal = read<unsigned int>(fHuffman); //Leemos el tama�o del archivo original.

   restaurarArchivoOrignal(fHuffman,fDescomprimido,raiz,sizeOriginal); //Restauramos el archivo original.
   fclose(fHuffman);
   fclose(fDescomprimido);
}

int main ()
{
   string fName;
   cout << "ingrese el nombre del archivo a comprimir o descomprimir" << endl;
   fName = "prueba.txt.huf";

   if(!endsWith(fName, ".huf") )
   {
      cout << "comprimir" << endl;
      comprimir(fName);
   }
   else
   {
      cout << "descomprimir" << endl;
     descomprimir(fName);
   }
}


#endif
