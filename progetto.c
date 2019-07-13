#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 100
#define MAX_ENT_ID 50

typedef struct ent_nodo t_ent_nodo;

typedef struct ent_head{
  t_ent_nodo *root;
  int nil;
} t_ent_head;

struct ent_nodo{
  char id[MAX_ENT_ID];
  int color; // 0 -> black - 1 -> red
  t_ent_nodo *p;
  t_ent_nodo *left;
  t_ent_nodo *right;
};

typedef struct rel_head{
  int *root;
} t_rel_head;

typedef struct rel_nodo{
  int id;
  int *members;
  int code;
  int ranking;
} t_rel_nodo;

typedef struct ent_ranking_nodo{
  int id;
  int code;
  int ranking;
} t_ent_ranking_nodo;

// VARIABILI GLOBALI


// ################################
// ##### DEFAULT OPERATIONS #######
// ################################
int LEFT_ROTATE(t_ent_head *T, t_ent_nodo *x){
  t_ent_nodo *y = x->right;
  x->right = y->left;
  if(y->left = T->nil)
    y->left->p = x;
  y->p = x->p;
  if(x->p == T->nil)
    T->root = y;
  else if(x == x->p->left)
    x->p->left = y;
  else
    x->p->right = y;
  y->left = x;
  x->p = y;
}



int getcmd(char *input){
  int i = 0;
  char input_cmd[6];
  char cmdlist[6][7] = {"addent", "delent", "addrel", "delrel", "report", "end"};
  while(input[i] != ' ' && input[i] != '\0'){
    input_cmd[i] = input[i];
    i++;
  }
  input_cmd[i] = '\0';
  for(i=0; i<6; i++)
    if(strcmp(input_cmd, cmdlist[i]) == 0)
      return i;
  return -1;
}

int retrive_addent_delent_id(char *input, char *ent){
  int i = 8; //sappiamo come è fatto il comando e parte da 7 il nome della nuova identità
  while(input[i] != '"' && input[i] != '\0' && input[i] != ' '){
    ent[i-8] = input[i];
    i++;
  }
  // printf("\n");
  ent[i-8] = '\0';
  return 1;
}

int retrive_addrel_delrel_args(char *input, char *orig, char *dest, char *rel){
  int i = 8; //sappiamo come è fatto il comando e parte da 7 il nome della nuova relazione
  int c = 8;
  // ORIG
  while(input[i] != '"' && input[i] != '\0' && input[i] != ' '){
    orig[i-c] = input[i];
    i++;
  }
  orig[i-c] = '\0';
  // ORIG
  i = i + 3;
  c = i;
  while(input[i] != '"' && input[i] != '\0' && input[i] != ' '){
    dest[i-c] = input[i];
    i++;
  }
  dest[i-c] = '\0';
  // REL
  i = i + 3;
  c = i;
  while(input[i] != '"' && input[i] != '\0' && input[i] != ' '){
    rel[i-c] = input[i];
    i++;
  }
  rel[i-c] = '\0';

  return 1;
}

int check_existence_ent(t_ent_nodo *x, char *id, int i){
  if(x == NULL)
    return 0;
  else if(id[i] == x->id[i] && id[i+1] == '\0' && x->id[i+1] == '\0'){
    return 1;
  }
  else if(id[i] == x->id[i] && id[i+1] != '\0' && x->id[i+1] != '\0'){
    return check_existence_ent(x, id, i+1);
  }
  else if(id[i] < x->id[i]){
    return check_existence_ent((x->left), id, i+1);
  }
  else{
    return check_existence_ent((x->right), id, i+1);
  }
}

int addEntInTree(t_ent_nodo *x, char *id, int i){

}

// FUNZIONE PRINCIPALE COMANDO addent
int addent(t_ent_head *T, char *comando){
  char addent_id[50];

  retrive_addent_delent_id(comando, &addent_id[0]);

  if(check_existence_ent(T->root, addent_id, 0)){
    printf("Entitla' esiste gia'! No add\n");
  }
  else{
    printf("Entitla' non esiste. Da add!\n");
  }

  return 1;
}

int delent(t_ent_head *T, char *comando){
  char delent_id[50];

  retrive_addent_delent_id(comando, &delent_id[0]);

  printf("ent to delete is: %s \n", delent_id);

  return 1;
}

int addrel(t_ent_head *T, char *comando){
  char addrel_orig[50];
  char addrel_dest[50];
  char addrel_rel[50];

  retrive_addrel_delrel_args(comando, addrel_orig, addrel_dest, addrel_rel);

  printf("add new rel: \nOrig: %s \nDest: %s \nRel: %s\n", addrel_orig, addrel_dest, addrel_rel);
}

int delrel(t_ent_head *T, char *comando){
  char delrel_orig[50];
  char delrel_dest[50];
  char delrel_rel[50];

  retrive_addrel_delrel_args(comando, delrel_orig, delrel_dest, delrel_rel);

  printf("del rel: \nOrig: %s \nDest: %s \nRel: %s\n", delrel_orig, delrel_dest, delrel_rel);
}

int report(){
  printf("Report\n");
}




// IL MAIN È FATTO!!
// NON DOVREBBE ESSERE PIÙ DA TOCCARE!
int main(){
  char *comando = NULL;
  int cmd;
  int i = 0;
  int size_cmd;
  size_t len = 0;
  t_ent_head T_ent; //head delle entità
  t_rel_head T_rel; //head delle relazioni

  // init entity tree
  T_ent.root = NULL;
  T_ent.nil = NULL;

  // init relations tree
  T_rel.root = NULL;
  T_rel.nil = NULL;

  // RICEZIONE PRIMO COMANDO
  // fgets(&comando[0], MAX_INPUT, stdin);

  size_cmd = getline(&comando, &len, stdin);
  i = len-1;
  if(comando[i] == '\n')
      comando[i] = '\0';
  cmd = getcmd(comando);

  // FINO A CHE NON SCRIVE "end" RESTIAMO NEL CICLO
  while(cmd != 5){
    if(cmd == 0){ // addent
      addent(&T_ent, comando);
    }
    else if(cmd == 1){ // delent
      delent(&T_ent, comando);
    }
    else if(cmd == 2){ // addrel
      addrel(&T_ent, comando);
    }
    else if(cmd == 3){ // delrel
      addrel(&T_ent, comando);
    }
    else if(cmd == 4){ // report
      report();
    }
    else{ // end
       // non dovrebbe mai entrare qui!
       // se ci entra meglio non scrivere niente!
       // printf("ERROR\n");
    }

    // RICEZIONE COMANDI SUCCESSIVI AL PRIMO
    // scanf("%s", comando);
    comando = NULL;
    len = 0;
    size_cmd = getline(&comando, &len, stdin);
    i = strlen(comando)-1;
    if(comando[i] == '\n')
        comando[i] = '\0';
    cmd = getcmd(comando);
  }

  return 0;
}
