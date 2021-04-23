#include <fstream>
#include "prob.h"
#include <iomanip>
PROBLEM::PROBLEM(char nombrefichero[85])
{
  unsigned i, j;
  double dummy;
  ifstream textfile;
  
textfile.open(nombrefichero);
  if (textfile.is_open()) {
           textfile >> (char *) clase >> (unsigned &) n >>(unsigned &) m;
       A.resize(m); // m filas
    //leemos la fila de los n costes
       for (i=0;i<n; i++) {
          textfile >> (double &) dummy;
      c.push_back(dummy);
    }
    // si el problema es de máximo cambiamos de signo los costes
      if (strncmp(clase,"max",3) == 0)
          for(i=0;i<n;i++) 
            c[i] *= -1.0;
      // extendemos el vector c a tamaño n + nh
    nh = m;
    for(i=0;i<nh;i++) 
       c.push_back(0.0);
    // leemos las m restricciones y asignamos A, b, ivb
    for (i=0;i<m;i++) {
        for (j=0;j<n;j++) {
	  textfile >> (double &) dummy;
	  A[i].push_back(dummy);
      }
      textfile >> (double &) dummy;
      b.push_back(dummy);
      if (dummy < CERONEG){
	cout << "Lado derecho negativo, Modifique problema"<< endl;
	exit(0);
      }
      A[i].resize(n+nh); // extendemos la fila a tamaño n+nh
      A[i][n+i] = 1.0; // ponemos un 1 en la columna n+i fila i correspondiente a la v. de holgura de la fila i
      ivb.push_back(n+i); // la v. básica asociada a la fila i es n+i
    }
    textfile.close();
  }
  else{
    cout<<endl<<"No se reconoce el fichero"<<endl;
    cout<<"el formato es: Lsimplex <nombre_fichero>"<<endl;
    cout<<"el formato del fichero es el siguiente"<<endl;
    cout<<"------------------------------------------------"<<endl;
    cout<<"tipo_problema     n_variables    m_restricciones"<<endl;
    cout<<"c_1   c_2                                     c_n"<<endl;
    cout<<"a_11  a_12........a_1n                         b_1"<<endl;
    cout<<endl<<endl;
    cout<<"a_m1  a_m2........a_mn                         b_m"<<endl;
    cout<<"------------------------------------------------"<<endl<<endl;
    exit(0);
  }
}


void PROBLEM::Volcar_problema(void){
  char a[80];
  cout<<endl<<"               PROBLEMA DE OPTIMIZACIÓN LINEAL CON RESTRICCIONES <="<<endl<<endl;
  cout<<" Z =  "<<clase;
  if(strncmp(clase,"min",3) == 0)
   for(int i=0;i<n;i++)
   {
    if(c[i]>CERONEG)
      sprintf(a," +%8.2lf",c[i]);
    else
      sprintf(a," %8.2lf",c[i]);
    cout<<a<<"x"<<i+1;
   } 
  else
    for(int i=0;i<n;i++)
    {
      if(-c[i]>CERONEG)
	sprintf(a," +%8.2lf",-c[i]);
      else
	sprintf(a," %8.2lf",-c[i]);
      cout<<a<<"x"<<i+1;
    } 
  cout<<endl<<" sujeto a:"<<endl;
  for(int i=0; i<nh;i++){
    cout<<"    ";
    for(int j=0;j<n;j++){
      if(A[i][j]>CERONEG)
	sprintf(a," +%8.2lf",A[i][j]);
      else
	sprintf(a," %8.2lf",A[i][j]);
      cout<<a<<"x"<<j+1;
    }
    sprintf(a," %8.2lf",b[i]);
    cout<<" <="<<a<<endl;
  }
  cout<<endl<<endl<<endl<<endl;
  cout<<" Pulsa cualquier tecla para continuar";
  cin.get();
  cin.get();
}

void PROBLEM::Simplex_Light(){
  unsigned s,r;
  while ((s = entrante())!= UERROR) {
    volcar_tabla();
    r = saliente(s);
    if (r != UERROR)
      actualizar_valores(s,r);
    else{
      cout<<endl<<"PROBLEMA NO ACOTADO"<<endl;
      exit(0);
    }
  }
  volcar_tabla();
  mostrar_solucion();
}
void PROBLEM::volcar_tabla (void){
  char a[90];
  cout<<"                                SIMPLEX PRIMAL"<<endl<<endl<<endl;
  cout<<"          ";
  //Imprime X1...Xn(variables) h1...hm(variables de holgura) b(términos independientes)
for(int i=1;i<=n;i++)
    cout<<"x"<<i<<"       ";
for(int i=1;i<=nh;i++)
    cout<<"h"<<i<<"       ";
  cout<<"  b";
cout<<endl;
  cout<<"------";
for(int i=0;i<n+nh;i++)
    cout<<"---------";
  cout<<"-------";
  cout<<endl;
for(int i=0; i<m;i++){
  if(ivb[i]<n)                                  //Imprime la columna de las variables básicas
      cout<<"x"<<ivb[i]+1<<" |";
  else
      cout<<"h"<<ivb[i]-n+1<<" |";
  for(int j=0;j<n+nh;j++){
     if((A[i][j]<CERONEG)||(A[i][j]>CEROPOS))
	sprintf(a,"%+8.1lf ",A[i][j]);
     else{
	sprintf(a,"%8.1lf ",0.0);
      } 
      cout<<a;
    }
    if((b[i]<CERONEG)||(b[i]>CEROPOS))
      sprintf(a,"%+8.1lf ",b[i]);
    else
      sprintf(a,"%8.1lf ",0.0);
      
    cout<<a<<endl;
  }
  cout<<"------";
  for(int i=0;i<n+nh;i++)
    cout<<"---------";
  cout<<"-------";
  cout<<endl;
  cout<<"-Z  ";
  for(int i=0;i<n+nh;i++){
    if((c[i]<CERONEG)||(c[i]>CEROPOS))
      sprintf(a,"%+8.1lf ",c[i]);
    else{
      sprintf(a,"%8.1lf ",0.0);
    }   
    cout<<a;
  }
  if((Vo<CERONEG)||(Vo>CEROPOS))
      sprintf(a,"%+8.1lf ",Vo);
    else{
      sprintf(a,"%8.1lf ",0.0);
    }   
  cout<<a;
  cout<<endl<<endl<<endl<<endl;
  cout<<"                PULSA INTRO PARA CONTINUAR"<<endl<<endl<<endl;
  cin.get();
  
}

void PROBLEM::actualizar_valores(unsigned s, unsigned r){
  double temp=0;
  ivb[r] = s; // actualiza la base 
  temp = A[r][s];
  for(int i=0; i<n+nh;i++)
    A[r][i] /= temp;
  b[r] /= temp;
  for(int i=0;i<m;i++)
     if((A[i][s]!=0)&&(i!=r)){ 
       temp = A[i][s];
       for(int j=0;j<n+nh;j++)
	  A[i][j] -= A[r][j] * temp; // la fila entera de i
          b[i] -= b[r] * temp;
    }
  Vo-= b[r] * c[s]; //actualizamos Vo
  for(int i=0;i<n+nh;i++)
    if(i!=s)
      c[i] -= c[s] * A[r][i]; /*para todo i<>s*/ 
  c[s] = 0.0;
}
unsigned PROBLEM::entrante(){
  
  unsigned s;
  s=UERROR;
  double min;
  min=CERONEG;
  for(int i=0; i<n+nh;i++){
    if(c[i]<min){
     for(int j=0; j<nh;j++)
      if(i==ivb[j])
	
        min=c[i];
        s=i;
      }
    }
  return(s);
}



unsigned PROBLEM::saliente(unsigned s)
{
  double min=UERROR;
  unsigned r= UERROR;
  for(int i=0; i<nh;i++){
    if((min>(b[i]/A[i][s]))&&(A[i][s]>CEROPOS)){
      min=b[i]/A[i][s];
      r=i;
    }
  }
  return(r);
}





void PROBLEM::mostrar_solucion(void)
{
  char a[80];
  cout<<"*****************************************************************************"<<endl;
  cout<<"                            SOLUCIÓN"<<endl;
  for(int i=0;i<m;i++){
  
    if(ivb[i]<n)
      
      cout<<" x"<<ivb[i]+1<<" = ";
    else{
      cout<<" h"<<ivb[i]-n+1<<" = ";
    }
    sprintf(a,"%.3lf",b[i]);
    cout<<a;
  }
  if(strncmp(clase,"min",3) == 0)
    sprintf(a,"%.3lf",-Vo);
  else
    sprintf(a,"%.3lf",Vo);
  cout<<endl<<endl<<"                        Z =   "<<a<<endl;
  cout<<"*****************************************************************************"<<endl;
}







