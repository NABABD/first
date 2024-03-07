Processus fils en largeur
March 6, 2024
fork() avec les fils du  parent en largeur
ChatGPT
Le système de fork() en C crée un nouveau processus en duplicant le processus appelant. Cependant, la manière dont les processus sont organisés après la création dépend de la logique de votre programme. Si vous souhaitez organiser les fils d'un parent en largeur, vous devrez utiliser une structure de données appropriée pour cela, comme une file (queue) ou un tableau dynamique.

Voici un exemple simple en C pour créer des processus fils en largeur à partir d'un processus parent en utilisant fork() et une file:

c
Copy code
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_CHILDREN 10

typedef struct {
    pid_t pid;
    int level; // Niveau dans l'arbre (profondeur)
} ChildInfo;

ChildInfo children[MAX_CHILDREN]; // Tableau pour stocker les informations sur les fils
int front = 0, rear = 0; // Indices pour la file

void enqueue(pid_t pid, int level) {
    if (rear == MAX_CHILDREN) {
        printf("Queue is full\n");
        exit(1);
    }
    children[rear].pid = pid;
    children[rear].level = level;
    rear++;
}

ChildInfo dequeue() {
    if (front == rear) {
        printf("Queue is empty\n");
        exit(1);
    }
    return children[front++];
}

int main() {
    pid_t pid;
    int current_level = 0;

    pid = getpid();
    enqueue(pid, current_level);

    while (front != rear) {
        ChildInfo current = dequeue();
        printf("Process ID: %d, Level: %d\n", current.pid, current.level);

        if (current.level < 2) { // Niveau maximum dans l'exemple : 2
            for (int i = 0; i < 2; i++) {
                pid = fork();
                if (pid < 0) {
                    printf("Fork failed\n");
                    exit(1);
                } else if (pid == 0) { // Processus fils
                    current_level = current.level + 1;
                    pid_t child_pid = getpid();
                    enqueue(child_pid, current_level);
                    break; // Sortie de la boucle pour éviter la création supplémentaire de processus fils
                } else { // Processus parent
                    enqueue(pid, current_level + 1);
                }
            }
        }
    }

    return 0;
}
Cet exemple crée un arbre de processus avec une profondeur maximale de 2 (modifiable). À chaque niveau, deux fils sont créés. La fonction enqueue() et dequeue() sont utilisées pour ajouter et supprimer des éléments de la file. Chaque processus affiche son PID et son niveau dans l'arbre.

avec fils sur un sesul niveau
ChatGPT
Si vous souhaitez créer plusieurs processus fils à partir d'un seul processus parent sur un seul niveau, sans hiérarchie supplémentaire, vous pouvez le faire sans utiliser de structures de données complexes comme une file. Voici un exemple simple :

c
Copy code
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CHILDREN 5

int main() {
    pid_t pid;
    int i;

    printf("Parent process PID: %d\n", getpid());

    // Création des processus fils
    for (i = 0; i < NUM_CHILDREN; i++) {
        pid = fork();

        if (pid < 0) {
            // Erreur lors de la création du processus fils
            fprintf(stderr, "Fork failed\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Code exécuté par le processus fils
            printf("Child process PID: %d, Parent PID: %d\n", getpid(), getppid());
            exit(EXIT_SUCCESS);
        }
    }

    // Code exécuté par le processus parent après avoir créé tous les fils
    for (i = 0; i < NUM_CHILDREN; i++) {
        // Attente de la terminaison de chaque processus fils
        wait(NULL);
    }

    return 0;
}
Dans cet exemple, le processus parent crée plusieurs processus fils sur un même niveau. Chaque processus fils affiche son propre PID ainsi que le PID de son parent. Ensuite, le processus parent attend la terminaison de chacun de ses processus fils à l'aide de la fonction wait(). Notez que les processus fils seront tous créés en parallèle et leur ordre d'exécution dépendra de l'ordonnancement du système d'exploitation.

protocole tcp en utilisant un serveur fork
ChatGPT
Voici un exemple simple dun serveur TCP en C qui utilise la fonction fork() pour gérer les connexions des clients de manière concurrente :

c
Copy code
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int sockfd, newsockfd, portno, client_len;
    pid_t pid;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;

    // Création de la socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Erreur lors de l'ouverture de la socket");
        exit(EXIT_FAILURE);
    }

    // Initialisation de la structure sockaddr_in
    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    portno = PORT;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Attachement de la socket au port
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Erreur lors de la liaison");
        exit(EXIT_FAILURE);
    }

    // Mise en attente de connexions entrantes
    listen(sockfd, 5);
    client_len = sizeof(cli_addr);

    while (1) {
        // Acceptation de la connexion entrante
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &client_len);
        if (newsockfd < 0) {
            perror("Erreur lors de l'acceptation de la connexion");
            exit(EXIT_FAILURE);
        }

        // Création d'un nouveau processus pour gérer la connexion du client
        pid = fork();
        if (pid < 0) {
            perror("Erreur lors du fork");
            exit(EXIT_FAILURE);
        }
        
        if (pid == 0) {  // Processus fils
            close(sockfd);  // Le processus fils n'a pas besoin de la socket d'écoute

            // Lecture et écriture avec le client
            memset(buffer, 0, BUFFER_SIZE);
            int n = read(newsockfd, buffer, BUFFER_SIZE - 1);
            if (n < 0) {
                perror("Erreur lors de la lecture depuis le socket");
                exit(EXIT_FAILURE);
            }
            printf("Message du client: %s\n", buffer);

            n = write(newsockfd, "Message bien reçu", 18);
            if (n < 0) {
                perror("Erreur lors de l'écriture sur le socket");
                exit(EXIT_FAILURE);
            }

            close(newsockfd);  // Fermeture du socket du client dans le processus fils
            exit(EXIT_SUCCESS);
        } else {  // Processus parent
            close(newsockfd);  // Le processus parent n'a pas besoin du socket du client
        }
    }

    close(sockfd);
    return 0;
}
Ce serveur TCP écoute sur le port 8888 et accepte les connexions entrantes des clients. Lorsqu'un client se connecte, le serveur crée un nouveau processus fils pour gérer cette connexion. Le processus fils lit le message envoyé par le client, l'affiche et envoie une réponse. Ensuite, le processus fils se termine. Le processus parent continue d'écouter de nouvelles connexions.

DIFFERENCE ENTRE FGETS() ET SCANF()
fgets() et scanf() sont deux fonctions en C utilisées pour lire l'entrée depuis la console, mais elles ont des différences importantes dans leur fonctionnement et leur utilisation.

fgets() :

fgets() est utilisée pour lire une chaîne de caractères (ligne par ligne) à partir de l'entrée standard (généralement le clavier) et la stocker dans un tampon.
Syntaxe : char *fgets(char *str, int n, FILE *stream);
La fonction prend trois arguments :
str : un pointeur vers un tampon où stocker la chaîne lue.
n : le nombre maximum de caractères à lire (y compris le caractère de fin de chaîne \0).
stream : le flux de fichier à partir duquel lire (généralement stdin pour l'entrée standard).
La fonction renvoie un pointeur vers la chaîne lue si elle réussit, sinon elle renvoie NULL.
scanf() :

scanf() est utilisée pour lire des entrées formatées à partir de l'entrée standard.
Syntaxe : int scanf(const char *format, ...);
La fonction prend une chaîne de format et une liste d'adresses de variables où stocker les valeurs lues.
Elle retourne le nombre de valeurs lues avec succès, ce qui peut être utile pour vérifier si l'entrée a été correctement interprétée.
scanf() peut être utilisée pour lire différents types de données, tels que des entiers, des flottants, des caractères, etc.
Il peut être plus difficile à utiliser correctement que fgets() en raison de problèmes potentiels liés à la synchronisation des entrées/sorties et à la gestion des caractères de nouvelle ligne.
Principales différences :

fgets() est généralement utilisée pour lire des lignes de texte entières, tandis que scanf() est utilisée pour lire des données formatées telles que des nombres ou des chaînes de caractères spécifiques.
fgets() est souvent préférée pour la lecture de lignes de texte complètes car elle est plus sûre et plus facile à utiliser pour cette tâche.
scanf() est plus flexible car elle peut interpréter différents types de données en fonction des spécificateurs de format fournis, mais elle peut être plus sujette à des erreurs si elle est mal utilisée, notamment en ce qui concerne les caractères de nouvelle ligne restants dans le flux d'entrée.

FONCTION BLOQUANT EN UDP
recvfrom() :

La fonction recvfrom() est utilisée pour recevoir des données sur un socket UDP.
Elle peut être bloquante si aucun paquet n'a été reçu et que l'option de blocage est activée sur le socket.
Si aucun paquet n'est disponible et que le socket est configuré en mode bloquant, l'appel à recvfrom() sera bloqué jusqu'à ce qu'un paquet soit reçu ou qu'un délai d'attente (timeout) soit spécifié.
sendto() :

La fonction sendto() est utilisée pour envoyer des données sur un socket UDP.
Elle peut également être bloquante si le buffer d'envoi est plein et que le socket est configuré en mode bloquant.
Dans ce cas, l'appel à sendto() sera bloqué jusqu'à ce qu'il y ait de la place dans le buffer d'envoi pour accepter les données à envoyer.

FONCTION BLOQUANTE EN TCP:
accept() :

La fonction accept() est utilisée sur le côté serveur pour accepter une nouvelle connexion entrante.
Si aucun client n'essaye de se connecter et que le socket est configuré en mode bloquant, l'appel à accept() sera bloqué jusqu'à ce qu'une connexion entrante soit détectée.
recv() :

La fonction recv() est utilisée pour recevoir des données sur un socket TCP.
Elle peut être bloquante si aucun paquet n'a été reçu et que l'option de blocage est activée sur le socket.
Comme pour UDP, si aucun paquet n'est disponible et que le socket est configuré en mode bloquant, l'appel à recv() sera bloqué jusqu'à ce qu'un paquet soit reçu ou qu'un délai d'attente (timeout) soit spécifié.
send() :

La fonction send() est utilisée pour envoyer des données sur un socket TCP.
Elle peut également être bloquante si le buffer d'envoi est plein et que le socket est configuré en mode bloquant.
Dans ce cas, l'appel à send() sera bloqué jusqu'à ce qu'il y ait de la place dans le buffer d'envoi pour accepter les données à envoyer.
connect() :

La fonction connect() est utilisée pour établir une connexion TCP vers un serveur.
Si le serveur n'est pas disponible ou si la connexion est retardée pour une raison quelconque, l'appel à connect() peut être bloquant jusqu'à ce que la connexion soit établie ou qu'un délai d'attente (timeout) soit atteint.