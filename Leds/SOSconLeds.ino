#include <vector>
#define Up 4
#define Right 5
#define Down 6
#define Left 7
#define LED 1


void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  pinMode(Up,INPUT_PULLUP);
  pinMode(Down,INPUT_PULLUP);
  pinMode(Right,INPUT_PULLUP);
  pinMode(Left,INPUT_PULLUP);
}

int actionled=0;
int partition=10;
int timeS=200/partition;//mantener como entero mayor que 2*particion
int timePForReference=timeS*2; //la pausa entre S y O dura un toque más
int timeP=timePForReference;
int timeO=timeS*3;//La O dura el doble que la S
int SOSstep=1;
int Ssteps=0;
int Osteps=0;
int timesSOSsteps=0;
int timeBetwenSOS=150;//en milisegundos pero entre 10 o sea 1seg=100
int cycleCompleted=false;


std::vector<int> S(){//vamos a hacer la S
  timesSOSsteps+=1; //llevamos una accion
  int led; //que hacer con el led 
  int end=0; //terminar? (La S)
  if ((Ssteps%2)==0){ //si estamos en un paso par (o 0) estaremos en punto
    led=1; //por lo tanto encendemos el led
  } else { //si no etamos en una pausa 
    led=0; //apagaremos el led
  }
  if (timesSOSsteps==timeS){ //si hemos hecho una accion la duracion establecida para S veces (cada accion dura "partition" milisegundos, si el tiempo de S es 300milis y partition es 10 entonces 300/10=30, cuando hayamos hecho 30 acciones...)
    Ssteps+=1; //completamos un paso de los 5 en la S (punto pausa punto pausa punto)
    timesSOSsteps=0;//volvemos a empezar a contar para hacer el siguiente paso
  }
  if (Ssteps==5){//si ya hicimos los 5 pasos 
    end=1; //depaso diremos que ya hemos acabado para que el proximo bucle entre a la Pausa entre S y O
    Ssteps=0; //para lo proxima vez que hagamos S
  }
  std::vector<int> answer={end,led};//devolver el vector de acciones
  return answer;
}

int P(){//haremos la pausa entre S y O en esta funcion solo se lleva la cuenta
  timesSOSsteps+=1; 
  int end=0; 
  if (timesSOSsteps==timeP){ //si llevamos las acciones necesarias para completar el tiempo de la pausa
    end=1; //acabamos
    timesSOSsteps=0;//volvemos a 0 el contador de acciones
  }
  return end;
}

std::vector<int> O(){//vamos a hacer la O
  timesSOSsteps+=1; //llevamos una accion
  int led; //que hacer con el led 
  int end=0; //terminar? (La O)
  if ((Osteps%2)==0){ //si estamos en un paso par (o 0) estaremos en una raya
    led=1; //por lo tanto encendemos el led
  } else { //si no etamos en una pausa 
    led=0; //apagaremos el led
  }
  if ((led)&&(timesSOSsteps==timeO)){ // si estamos en una raya y ademas hemos acabado de hacer los pasos para su duracion
    Osteps+=1; //completamos un paso de los 5 en la O
    timesSOSsteps=0;//volvemos a empezar a contar para hacer el siguiente paso

  } else if ((!led)&&(timesSOSsteps==timeS)){ //si en cambio estamos en una pausa debemos haber completado un tiempo más corto, el de S
    Osteps+=1;
    timesSOSsteps=0;
  }

  if (Osteps==5){//si ya hicimos los 5 pasos 
    end=1; //depaso diremos que ya hemos acabado para que el proximo bucle entre a la Pausa entre S y O
    Osteps=0;//para la proxima vez
  }
  std::vector<int> answer={end,led};//devolver el vector de acciones
  return answer;
}



int SOS() {//haremos un SOS
  if (SOSstep==1){//si vamos en el paso 1 entonces haremos una S
    std::vector<int> answerS=S(); //La respuesta de la funcion que hace una S es un vector con si debemos termianr o no y la accion del led para esta accion
    if (answerS[0]){//si la respuesta es que termino la S
    SOSstep=2;//Pasamos al paso 2 que es la pausa entre S y O
    }
    return answerS[1];
  } else if (SOSstep==2){//si el paso es el 2 es una pausa

    if (cycleCompleted){//si completamos un ciclo en realidad esperaremos un poco más
      timeP=timeBetwenSOS;
    }
    int answerP=P(); 
    if (answerP){//si la funcion P termino
      if (cycleCompleted){//si estabamos cerrado un ciclo
        timeP=timePForReference;//volvemos a hacer pausas normales
        cycleCompleted=false;//ya no estamso cerrando un ciclo
        SOSstep=1;//vamos a la S
      }else//si hicimos una pasua normal
      SOSstep=3;//vamos a la O
    }  
    return 0;
  } else if (SOSstep==3){//si vamos por la O
    std::vector<int> answerO=O(); //La respuesta de la funcion que hace una S es un vector con si debemos termianr o no y la accion del led para esta accion
    if (answerO[0]){//si la respuesta es que termino la S
    SOSstep=4;//Pasamos al paso 2 que es la pausa entre S y O
    }
    return answerO[1];
  } else if (SOSstep==4){//si el paso es el 4 hacemos una pausa y seria la ultima del ciclo porque podemos ir a la S de nuevo 
    int answerP=P();
    if (answerP){//si la funcion P termino
      cycleCompleted=true;
      SOSstep=1;//vamos a la S de nuevo 
    }
    return 0;
  }
}


void loop() {
  if (!(digitalRead(Up))) {
    actionled=SOS();
  } else {
    actionled=0;
    SOSstep=1;
    Ssteps=0;
    Osteps=0;
    timesSOSsteps=0;
    cycleCompleted=false;
    timeP=timePForReference;
  }
  Serial.println(SOSstep);
  digitalWrite(LED,actionled);

  delay(partition);
}



