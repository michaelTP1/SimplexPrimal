#include "prob.h"
#include <cstdio>
#include <cmath>
using namespace std;
int main (void)
{
  char nombre[85];
  char ans;
  cout<<endl<<"Se ha de especificar un fichero.¿Deseas dar el nombre del Fichero(y/n)? ";
  cin>>ans;
  if(ans=='y'){
    cout<<endl<<"Introduzca el nombre del fichero en el directorio actual: ";
    cin>> (char *) nombre;
    PROBLEM p(nombre);
    p.Volcar_problema();
    p.Simplex_Light();
  }
}
