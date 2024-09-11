#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///ESTRUCTURA GENERAL///
typedef struct
{
    int idSector;
    char sector[30];
    int dni;
    char nombreYapellido[30];
    int ventasDiarias;
}stRegistro;

///VENDEDORES///
//lista
typedef struct
{
    int dni;
    char nombreYapellido[30];
    int ventasDiarias;
}vendedor;

typedef struct
{
    vendedor datos;
    struct nodoL * sig;
}nodoL;

///SECTORES///
//arbol
typedef struct
{
    int idSector;
    char sector[30];
}sector;

typedef struct
{
    sector datos;
    nodoL * lista;
    struct nodoA * izq;
    struct nodoA * der;
}nodoA;

nodoL * crearNodoL (vendedor vendedor);
nodoL * agregarPrincipio (nodoL * lista, vendedor vendedor);
void mostrarLista (nodoL * lista);
nodoA * crearNodoA (sector sector);
nodoA * insertarSectorOrdenado (nodoA * arbol, sector sector);
nodoA * buscarSector (nodoA * arbol, int idSectorBuscado);
nodoA * pasarDatos (const char *archivo);
void mostrarADL (nodoA * arbol);
int ventasVendedores (nodoL * lista);
int ventasTotales (nodoA * arbol);
nodoL * buscarVendedor (nodoL * lista, const char * nombreBuscado);
int buscarVendedorEnSector (nodoA * arbol, const char *nombreBuscado);


int main()
{
    nodoA * arbol = NULL;
    nodoL * lista = NULL;

    const char * archivo = "archivoRegistrosVendedor.bin";

    arbol = pasarDatos(archivo);
    mostrarADL(arbol);

    int totalVentas = ventasTotales(arbol);

    char nombreBuscado[30];
    printf("Ingrese el nombre\n");
    fflush(stdin);
    gets(nombreBuscado);

    int vendedor = buscarVendedorEnSector(arbol, nombreBuscado);



    return 0;
}

//////////////////// FUNCIONES //////////////////////

////////////////////// FUNCIONES PARA MANEJAR LA LDA //////////////////////
///LISTA
nodoL * crearNodoL (vendedor vendedor)
{
    nodoL * nuevoNodo = (nodoL*)malloc(sizeof(nodoL));
    nuevoNodo->datos.dni = vendedor.dni;
    nuevoNodo->datos.ventasDiarias = vendedor.ventasDiarias;
    strcpy(nuevoNodo->datos.nombreYapellido, vendedor.nombreYapellido);
    nuevoNodo->sig = NULL;
    return nuevoNodo;
}

nodoL * agregarPrincipio (nodoL * lista, vendedor vendedor)
{
    nodoL * nuevo = crearNodoL(vendedor);
    if(lista == NULL)
    {
        lista = nuevo;
    }
    else
    {
        nuevo->sig = lista;
        lista = nuevo;
    }

    return lista;
}

void mostrarLista (nodoL * lista)
{
    while(lista!=NULL)
    {
        printf("Nombre %s\n", lista->datos.nombreYapellido);
        printf("DNI %i\n", lista->datos.dni);
        printf("Ventas diarias %i\n", lista->datos.ventasDiarias);
        lista = lista->sig;
    }
}

///ARBOL
nodoA * crearNodoA (sector sector)
{
    nodoA * nuevoNodo = (nodoA*)malloc(sizeof(nodoA));
    nuevoNodo->datos.idSector = sector.idSector;
    strcpy(nuevoNodo->datos.sector, sector.sector);
    nuevoNodo->lista = NULL;
    nuevoNodo->izq = NULL;
    nuevoNodo->der = NULL;
    return nuevoNodo;
}

nodoA * insertarSectorOrdenado (nodoA * arbol, sector sector)
{
    nodoA * nuevo = crearNodoA(sector);
    if (arbol==NULL)
    {
        arbol = nuevo;
    }
    else if (strcmp(sector.sector, arbol->datos.sector)<0)
    {
        arbol->izq = insertarSectorOrdenado(arbol->izq, sector);
    }
    else
    {
        arbol->der = insertarSectorOrdenado(arbol->der, sector);
    }
    return arbol;
}

///Buscar el sector al cual corresponde el vendedor a cargar en la ADL
nodoA * buscarSector (nodoA * arbol, int idSectorBuscado)
{
   if (arbol == NULL)
   {
       return NULL;
   }

   if (arbol->datos.idSector == idSectorBuscado)
   {
       return arbol;
   }
   else if(idSectorBuscado < arbol->datos.idSector)
   {
       return buscarSector(arbol->izq, idSectorBuscado);
   }
   else
   {
       return buscarSector(arbol->der, idSectorBuscado);
   }
}

///Leer el archivo adjuntado y copiar los datos correspondientes a la estructura stSector y a
///la estructura stVendedor.
nodoA * pasarDatos (const char *archivo)
{
    stRegistro registro;
    nodoA * arbol = NULL;
    FILE * archivito = fopen(archivo, "rb");

    if (archivito!=NULL)
    {
        while (fread(&registro, sizeof(stRegistro),1,archivito)>0)
        {
            sector sector;
            sector.idSector = registro.sector;
            strcpy(sector.sector, registro.sector);

            vendedor vendedor;
            vendedor.dni = registro.dni;
            vendedor.ventasDiarias = registro.ventasDiarias;
            strcpy(vendedor.nombreYapellido, registro.nombreYapellido);

            nodoA * encontrado = buscarSector(arbol, sector.idSector);
            if (encontrado == NULL)
            {
                arbol = insertarSectorOrdenado(arbol, sector);
                encontrado = arbol;
            }

            encontrado->lista = agregarPrincipio(encontrado->lista, vendedor);
        }
        fclose(archivito);
    }
    return arbol;
}

///Realizar las funciones necesarias para mostrar todos los elementos de la estructura
///compuesta, permitiendo visualizar cada sector con sus vendedores.
void mostrarADL (nodoA * arbol)
{
    if (arbol!=NULL)
    {
        mostrarADL(arbol->izq); ///ES ARBOL
        printf("Sector %s\n", arbol->datos.sector);
        printf("ID del sector %i\n", arbol->datos.idSector);
        mostrarLista(arbol->lista);
        mostrarADL(arbol->der);
    }
}

///Realizar las funciones necesarias para calcular las ventas diarias TOTALES de la tienda (es
///decir: la suma de las ventas de todos los vendedores de todos los sectores).
int ventasVendedores (nodoL * lista)
{
    int sumaTotal = 0; //inicializa las variables que acumularan las ventas totales
    while(lista!=NULL) //recorre la lista
    {
        sumaTotal += lista->datos.ventasDiarias; //suma las ventas diarias del vendedor
        lista = lista->sig; //avanza al siguiente nodo
    }
    return sumaTotal; //devuelve el total de ventas diarias
}

int ventasTotales (nodoA * arbol)
{
    if (arbol == NULL)
    {
        return NULL;
    }

    int total = ventasTotales(arbol->izq); //calcula las ventas diarias totales del sub izq
    total += ventasVendedores(arbol->lista); //suma las ventas del sector actual
    total += ventasTotales(arbol->der); //calcula el sub derecho

    return total; //devuelve el total de ventas diarias de todos los sectores
}

///Realizar las funciones necesarias para buscar en la estructura compuesta a un vendedor
///cuyo nombre y apellido será elegido por el usuario del sistema.
nodoL * buscarVendedor (nodoL * lista, const char * nombreBuscado)
{
    while(lista!=NULL && strcmp(nombreBuscado, lista->datos.nombreYapellido)!=0)
    {
        lista = lista->sig;
    }

    return lista;
}

int buscarVendedorEnSector (nodoA * arbol, const char *nombreBuscado)
{
    if (arbol == NULL)
    {
        return NULL;
    }

    nodoL * encontrado = buscarVendedor(arbol->lista, nombreBuscado);

    if (encontrado!=NULL)
    {
        printf("Sector %s, para el vendedor %s", arbol->datos.sector, nombreBuscado);
        return 1;
    }
    else if (buscarVendedorEnSector(arbol->izq,nombreBuscado))
    {
        return 1;
    }
    else
    {
        return buscarVendedorEnSector(arbol->der, nombreBuscado);
    }
}
