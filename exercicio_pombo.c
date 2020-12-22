// Código para o Trabalho Final de Sistemas Operacionais TT304A_2020S2 Unicamp
// Código por Mateus Nojima 184125 e Kleyton Rafael da Silva Siqueira 176803

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#define N 20

sem_t mutex;             // Evita acesso simultaneo a variavel contPostIt.
sem_t mochila_cheia;     // Indica se a mochila esta cheia, fazendo o pombo dormir enquanto nao esta.
sem_t enchendo;          // Indica se a mochila esta enchendo, fazendo os usuarios dormirem enquanto nao esta.

int contPostIt=0;        // Número de PostIt na mochila.

// As tres funções abaixo dormem por um tempo aleatorio entre 1 e 3.
void dorme_aleatorio(){
    srand(time(NULL));
    int n = rand()%((3+1)-1) + 1;
    sleep(n);
}

void leva_mochila_ate_B_e_volta(){
    srand(time(NULL));
    int n = rand()%((3+1)-1) + 1;
    sleep(n);
}

void ColaPostIt(int id){
    printf("O usuario %d colou um Post It.\n", id);
    
    srand(time(NULL));
    int n = rand()%((3+1)-1) + 1;
    sleep(n);
}

void* usuario(void *idU) {
    int id;
	id = *((unsigned int *) idU);
	
    while(1) {
        dorme_aleatorio();             // Dorme Aleatorio
        sem_wait(&enchendo);           // Down(enchendo)
        sem_wait(&mutex);              // Down(mutex)
        ColaPostIt(id);                // Usuario cola o PostIt
		contPostIt++;
		
        if(contPostIt == N){
		    sem_post(&mochila_cheia);   // Up(mochila_cheia)
            printf("mochila cheia\n");
	        }
        sem_post(&mutex);               // Up(mutex)
	}
    pthread_exit(0);
}

void* pombo() {
	while(1) {
	    sem_wait(&mochila_cheia);       // Down(mochila_cheia)
		sem_wait(&mutex);               // Down (mutex)
		leva_mochila_ate_B_e_volta();   // Pombo leva as cartas ate B e volta.
	    contPostIt = 0;
		
		for(int i=0; i<N; i++){
			sem_post(&enchendo);        // Up(enchendo)
			printf("Adic. msg %d\n", i);
		 }
	    sem_post(&mutex);               // Up(mutex)
	}
    pthread_exit(0);
}

/* Funcao para alocar um novo identificador.
   Funcao retirada de banheiro.c por Prof. Dr. André Leon S. Gradvohl*/
unsigned int *alocaId(unsigned int id){
	unsigned int *newId;  
	if ((newId = (unsigned int *)malloc(sizeof(unsigned int))) == NULL){
    	perror("Problemas na alocacao do identificador.");
    	exit(EXIT_FAILURE);
	}
	*newId = id;
	return newId;
}

int main() {
    int numero_usuario;
    printf("Informe o numero de usuarios: ");
    scanf("%d", &numero_usuario);
    
    // Inicializa os semaforos.
	sem_init(&mutex, 0, 1);
	sem_init(&mochila_cheia, 0, 0);
	sem_init(&enchendo, 0, N);
	
	pthread_t user[numero_usuario];
	pthread_t pigeon;

    pthread_create(&pigeon, NULL, pombo, NULL);    // Instanciando thread para o pombo.
    for (int i = 0; i < numero_usuario ; i++) {	   // Instanciando thread para cada usuário.
        pthread_create(&user[i], NULL, usuario, alocaId(i));
	}
    pthread_join(pigeon, NULL);
    return 0;
}
