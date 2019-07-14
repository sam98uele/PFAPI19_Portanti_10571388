#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 200
#define MAX_ENT_ID 50
#define MAX_REL_ID 50

// DEBUG_DEFINE
#define DISABLE_ADD 0


typedef struct ent_node t_ent_node;
typedef struct rel_node t_rel_node;

typedef struct rel_list t_rel_list;

struct rel_list{
  t_rel_node *rel;
  t_rel_list *next;
};

typedef struct ent_head{
  t_ent_node *root;
  t_ent_node *nil;
} t_ent_head;

struct ent_node{
  char id[MAX_ENT_ID];
  int color; // 0 -> black - 1 -> red
  int v; //valid bit = 0->deleted , 1->alive
  t_rel_list *in_rel; //rel in
  t_rel_list *out_rel; //rel out
  t_ent_node *p;
  t_ent_node *left;
  t_ent_node *right;
};

typedef struct rel_head{
  t_rel_node *root;
  t_rel_node *nil;
} t_rel_head;

typedef struct rel_node{
  char id[MAX_REL_ID];
  int color; // 0 -> black - 1 -> red
  int v; //valid bit = 0->deleted , 1->alive
  t_ent_node *orig;
  t_ent_node *dest;
  t_rel_node *p;
  t_rel_node *left;
  t_rel_node *right;
} t_rel_node;

typedef struct ent_ranking_node{
  int id;
  int code;
  int ranking;
} t_ent_ranking_node;

// START Global Vars
t_ent_node NIL_ENT = {.id="vv", .color=0, .in_rel = NULL, .out_rel = NULL, .p = NULL, .left = NULL, .right = NULL};
t_rel_node NIL_REL = {.id="vv", .color=0, .p = NULL, .left = NULL, .right = NULL};

// END Global Vars

// ################################
// ##### DEFAULT OPERATIONS #######
// ################################

// This function is like id_1 > id_2 ?
// It compares ASCII
// Return values:
// 1 -> id_1 > id_2
// 0 -> altrimenti
int IS_GREATER_ID(char *id_1, char *id_2){
  int i = 0;
  while (id_1[i] != '\0' && id_2[i] != '\0') {
    if(id_1[i] > id_2[i]) return 1;
    else if(id_1[i] == id_2[i]) i++;
    else return 0;
  }
  return 0; // equal. this is only >!
}

// This function is like id_1 < id_2 ?
// It compares ASCII
// Return values:
// 1 -> id_1 > id_2
// 0 -> altrimenti
int IS_LOWER_ID(char *id_1, char *id_2){
  int i = 0;
  while (id_1[i] != '\0' && id_2[i] != '\0') {
    if(id_1[i] < id_2[i]) return 1;
    else if(id_1[i] == id_2[i]) i++;
    else return 0;
  }
  return 0; // equal. this is only >!
}

// LIST REL ENT
int ADD_TO_LIST(t_rel_list **HEAD, t_rel_list *rel){
  rel->next = *HEAD;
  *HEAD = rel;
  return 1;
}

// ###############################
// ##### ENT OPERATIONS ##########
// #####      START     ##########
// ###############################

int ENT_LEFT_ROTATE(t_ent_head *T, t_ent_node *x){
  t_ent_node *y = x->right;
  x->right = y->left;
  if(y->left == T->nil)
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

  return 1;
}

int ENT_TREE_WALK(t_ent_head *T, t_ent_node *x){
  // printf("a\n");
  if(x != T->nil){
    ENT_TREE_WALK(T, x->left);
    printf("%s ", x->id);
    ENT_TREE_WALK(T, x->right);
  }
}

int ENT_RIGHT_ROTATE(t_ent_head *T, t_ent_node *x){
  t_ent_node *y = x->left;
  x->left = y->right;
  if(y->right == T->nil)
    y->right->p = x;
  y->p = x->p;
  if(x->p == T->nil)
    T->root = y;
  else if(x == x->p->right)
    x->p->right = y;
  else
    x->p->left = y;
  y->right = x;
  x->p = y;

  return 1;
}

t_ent_node *ENT_TREE_SUCCESSOR(t_ent_head *T, t_ent_node *x){
  t_ent_node *y;
  if(x->right != T->nil)
    return ENT_TREE_SUCCESSOR(T, x->right);
  y = x->p;
  while (y != T->nil && x == y->right) {
    x = y;
    y = y->p;
  }
  return y;
}

int ENT_INSERT_FIXUP(t_ent_head *T, t_ent_node *z){
  t_ent_node *x;
  t_ent_node *y;
  if(z == T->root){
    T->root->color = 0;
    return 1;
  }
  else{
    x = z->p;
    if(x->color == 1){
      if(x == x->p->left){ // se x è figlio sinistro
        y = x->p->right;
        if(y->color == 1){
          x->color = 0;
          y->color = 0;
          x->p->color = 1;
          ENT_INSERT_FIXUP(T, x->p);
        }
        else{
          if(z == x->right){
            z = x;
            ENT_LEFT_ROTATE(T, z);
            x = z->p;
          }
          x->color = 0;
          x->p->color = 1;
          ENT_RIGHT_ROTATE(T, x->p);
        }
      }
      else{ // se x == x->p->right (se x è figlio destro)
        y = x->p->left;
        if(y->color == 1){
          x->color = 0;
          y->color = 0;
          x->p->color = 1;
          ENT_INSERT_FIXUP(T, x->p);
        }
        else{
          if(z == x->left){
            z = x;
            ENT_RIGHT_ROTATE(T, z);
            x = z->p;
          }
          x->color = 0;
          x->p->color = 1;
          ENT_LEFT_ROTATE(T, x->p);
        }
      }
    }
    else{ //if the node is black. do nothing
      return 1;
    }
  }
  return 1;
}

int ENT_INSERT(t_ent_head *T, t_ent_node *z){
  t_ent_node *y = T->nil;
  t_ent_node *x = T->root;
  while(x != T->nil){
    y = x;
    if(IS_LOWER_ID(z->id, x->id))
      x = x->left;
    else
      x = x->right;
  }
  z->p = y;
  if(y == T->nil)
    T->root = z;
  else if(IS_LOWER_ID(z->id, y->id))
    y->left = z;
  else
    y->right = z;
  z->left = T->nil;
  z->right = T->nil;
  z->color = 1;
  ENT_INSERT_FIXUP(T, z);
  return 1;
}

int ENT_DELETE_FIXUP(t_ent_head *T, t_ent_node *x){
  t_ent_node *w;
  if(x->color == 1 || x->p == T->nil)
    x->color = 0; // case 0
  else if(x == x->p->left){
    w = x->p->right;
    if(w->color == 1){
      w->color = 0;
      x->p->color = 1;
      ENT_LEFT_ROTATE(T, x->p);
      w = x->p->right;
    }
    if(w->left->color == 0 && w->right->color == 0){
      w->color = 1;
      ENT_DELETE_FIXUP(T, x->p);
    }
    else{
      if(w->right->color == 0){
        w->left->color = 0;
        w->color = 1;
        ENT_RIGHT_ROTATE(T, w);
        w = x->p->right;
      }
      w->color = x->p->color;
      x->p->color = 0;
      w->right->color = 0;
      ENT_LEFT_ROTATE(T, x->p);
    }
  }
  else{ // if x == x->p->right
    w = x->p->left;
    if(w->color == 1){
      w->color = 0;
      x->p->color = 1;
      ENT_RIGHT_ROTATE(T, x->p);
      w = x->p->left;
    }
    if(w->right->color == 0 && w->left->color == 0){
      w->color = 1;
      ENT_DELETE_FIXUP(T, x->p);
    }
    else{
      if(w->right->color == 0){
        w->right->color = 0;
        w->color = 1;
        ENT_LEFT_ROTATE(T, w);
        w = x->p->left;
      }
      w->color = x->p->color;
      x->p->color = 0;
      w->left->color = 0;
      ENT_RIGHT_ROTATE(T, x->p);
    }
  }
}

t_ent_node *ENT_DELETE(t_ent_head *T, t_ent_node *z){
  t_ent_node *x;
  t_ent_node *y;
  if(z->left == T->nil || z->right == T->nil)
    y = z;
  else
    y = ENT_TREE_SUCCESSOR(T, z);
  if(y->left != T->nil)
    x = y->left;
  else
    x = y->right;
  x->p = y->p;
  if(y->p == T->nil)
    T->root = x;
  else if(y == y->p->left)
    y->p->left = x;
  else
    y->p->right = x;
  if(y != z)
    strcpy(z->id, y->id);
  if(y->color = 0)
    ENT_DELETE_FIXUP(T, x);
  return y; //return the deleted node
}

// ###############################
// ##### REL OPERATIONS ##########
// #####      START     ##########
// ###############################


// ###############################
// ##### REL OPERATIONS ##########
// #####      START     ##########
// ###############################

int REL_LEFT_ROTATE(t_rel_head *T, t_rel_node *x){
  t_rel_node *y = x->right;
  x->right = y->left;
  if(y->left == T->nil)
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

  return 1;
}

int REL_RIGHT_ROTATE(t_rel_head *T, t_rel_node *x){
  t_rel_node *y = x->left;
  x->left = y->right;
  if(y->right == T->nil)
    y->right->p = x;
  y->p = x->p;
  if(x->p == T->nil)
    T->root = y;
  else if(x == x->p->right)
    x->p->right = y;
  else
    x->p->left = y;
  y->right = x;
  x->p = y;

  return 1;
}

int REL_TREE_WALK(t_rel_head *T, t_rel_node *x){
  // printf("a\n");
  if(x != T->nil){
    REL_TREE_WALK(T, x->left);
    printf("Rel: \"%s\" From: \"%s\" To: \"%s\"", x->id, x->orig->id, x->dest->id);
    REL_TREE_WALK(T, x->right);
  }
}

t_rel_node *REL_TREE_SUCCESSOR(t_rel_head *T, t_rel_node *x){
  t_rel_node *y;
  if(x->right != T->nil)
    return REL_TREE_SUCCESSOR(T, x->right);
  y = x->p;
  while (y != T->nil && x == y->right) {
    x = y;
    y = y->p;
  }
  return y;
}

int REL_INSERT_FIXUP(t_rel_head *T, t_rel_node *z){
  t_rel_node *x;
  t_rel_node *y;
  if(z == T->root){
    T->root->color = 0;
    return 1;
  }
  else{
    x = z->p;
    if(x->color == 1){
      if(x == x->p->left){ // se x è figlio sinistro
        y = x->p->right;
        if(y->color == 1){
          x->color = 0;
          y->color = 0;
          x->p->color = 1;
          REL_INSERT_FIXUP(T, x->p);
        }
        else{
          if(z == x->right){
            z = x;
            REL_LEFT_ROTATE(T, z);
            x = z->p;
          }
          x->color = 0;
          x->p->color = 1;
          REL_RIGHT_ROTATE(T, x->p);
        }
      }
      else{ // se x == x->p->right (se x è figlio destro)
        y = x->p->left;
        if(y->color == 1){
          x->color = 0;
          y->color = 0;
          x->p->color = 1;
          REL_INSERT_FIXUP(T, x->p);
        }
        else{
          if(z == x->left){
            z = x;
            REL_RIGHT_ROTATE(T, z);
            x = z->p;
          }
          x->color = 0;
          x->p->color = 1;
          REL_LEFT_ROTATE(T, x->p);
        }
      }
    }
    else{ //if the node is black. do nothing
      return 1;
    }
  }
  return 1;
}

int REL_INSERT(t_rel_head *T, t_rel_node *z){
  t_rel_node *y = T->nil;
  t_rel_node *x = T->root;
  while(x != T->nil){
    y = x;
    if(IS_LOWER_ID(z->id, x->id))
      x = x->left;
    else
      x = x->right;
  }
  z->p = y;
  if(y == T->nil)
    T->root = z;
  else if(IS_LOWER_ID(z->id, y->id))
    y->left = z;
  else
    y->right = z;
  z->left = T->nil;
  z->right = T->nil;
  z->color = 1;
  REL_INSERT_FIXUP(T, z);
  return 1;
}

int REL_DELETE_FIXUP(t_rel_head *T, t_rel_node *x){
  t_rel_node *w;
  if(x->color == 1 || x->p == T->nil)
    x->color = 0; // case 0
  else if(x == x->p->left){
    w = x->p->right;
    if(w->color == 1){
      w->color = 0;
      x->p->color = 1;
      REL_LEFT_ROTATE(T, x->p);
      w = x->p->right;
    }
    if(w->left->color == 0 && w->right->color == 0){
      w->color = 1;
      REL_DELETE_FIXUP(T, x->p);
    }
    else{
      if(w->right->color == 0){
        w->left->color = 0;
        w->color = 1;
        REL_RIGHT_ROTATE(T, w);
        w = x->p->right;
      }
      w->color = x->p->color;
      x->p->color = 0;
      w->right->color = 0;
      REL_LEFT_ROTATE(T, x->p);
    }
  }
  else{ // if x == x->p->right
    w = x->p->left;
    if(w->color == 1){
      w->color = 0;
      x->p->color = 1;
      REL_RIGHT_ROTATE(T, x->p);
      w = x->p->left;
    }
    if(w->right->color == 0 && w->left->color == 0){
      w->color = 1;
      REL_DELETE_FIXUP(T, x->p);
    }
    else{
      if(w->right->color == 0){
        w->right->color = 0;
        w->color = 1;
        REL_LEFT_ROTATE(T, w);
        w = x->p->left;
      }
      w->color = x->p->color;
      x->p->color = 0;
      w->left->color = 0;
      REL_RIGHT_ROTATE(T, x->p);
    }
  }
}

t_rel_node *REL_DELETE(t_rel_head *T, t_rel_node *z){
  t_rel_node *x;
  t_rel_node *y;
  if(z->left == T->nil || z->right == T->nil)
    y = z;
  else
    y = REL_TREE_SUCCESSOR(T, z);
  if(y->left != T->nil)
    x = y->left;
  else
    x = y->right;
  x->p = y->p;
  if(y->p == T->nil)
    T->root = x;
  else if(y == y->p->left)
    y->p->left = x;
  else
    y->p->right = x;
  if(y != z)
    strcpy(z->id, y->id);
  if(y->color = 0)
    REL_DELETE_FIXUP(T, x);
  return y; //return the deleted node
}

// ###############################
// ##### REL OPERATIONS ##########
// #####       END      ##########
// ###############################



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

int check_existence_ent(t_ent_head *T, t_ent_node *x, char *id, int i){
  if(x == T->nil){
    // printf("aa\n");
    return 0;
  }
  else if(id[i] == x->id[i] && id[i+1] == '\0' && x->id[i+1] == '\0'){
    return 1;
  }
  else if(id[i] == x->id[i] && id[i+1] != '\0' && x->id[i+1] != '\0'){
    return check_existence_ent(T, x, id, i+1);
  }
  else if(id[i] != '\0' && x->id[i] == '\0')
    return 0;
  else if(id[i] == '\0' && x->id[i] != '\0')
    return 0;
  else if(id[i] < x->id[i]){
    return check_existence_ent(T, (x->left), id, i);
  }
  else{
    return check_existence_ent(T, (x->right), id, i);
  }
}

int find_ent(t_ent_head *T, t_ent_node *x, t_ent_node **z, char *id, int i){
  if(x == T->nil){
    return 0; //ent not found
  }
  else if(id[i] == x->id[i] && id[i+1] == '\0' && x->id[i+1] == '\0'){
    *z = x;
    return 1;
  }
  else if(id[i] == x->id[i] && id[i+1] != '\0' && x->id[i+1] != '\0'){
    return find_ent(T, x, z, id, i+1);
  }
  else if(id[i] != '\0' && x->id[i] == '\0')
    return 0;
  else if(id[i] == '\0' && x->id[i] != '\0')
    return 0;
  else if(id[i] < x->id[i]){
    return find_ent(T, (x->left), z, id, i);
  }
  else{
    return find_ent(T, (x->right), z, id, i);
  }
}

int addEntInTree(t_ent_node *x, char *id, int i){

}

// FUNZIONE PRINCIPALE COMANDO addent
int addent(t_ent_head *T, char *comando, t_ent_node *ent){
  char addent_id[MAX_ENT_ID];
  // t_ent_node ent;

  // for debug
  #if DISABLE_ADD == 0
  #else
  #endif

  // ADDENT !!
  retrive_addent_delent_id(comando, &addent_id[0]);

  if(check_existence_ent(T, T->root, addent_id, 0) != 0){
    printf("Esiste gia\n");
    return 1; //if the ent already exists, do nothing
  }

  // copy id
  strcpy(ent->id, addent_id);
  ent->in_rel = NULL;
  ent->out_rel = NULL;

  if(ENT_INSERT(T, ent)){
    // printf("added\n");
  }

  return 1;
}

int delent(t_ent_head *T, char *comando){
  char delent_id[MAX_ENT_ID];
  t_ent_node *ent;
  t_ent_node *deleted_ent;

  retrive_addent_delent_id(comando, &delent_id[0]);

  if(find_ent(T, T->root, &ent, delent_id, 0) == 0){
    return 0;
  }

  // t_ent_node *ENT_DELETE(t_ent_head *T, t_ent_node *z){
  deleted_ent = ENT_DELETE(T, ent);

  if(deleted_ent != ent){
    printf("Deleted an other ent\n");
  }

  // printf("ent to delete is: %s \n", delent_id);

  return 1;
}

int addrel(t_ent_head *E, t_rel_head *R, char *comando, t_rel_node *rel){
  char addrel_orig[MAX_ENT_ID];
  char addrel_dest[MAX_ENT_ID];
  char addrel_rel[MAX_REL_ID];
  t_ent_node *orig;
  t_ent_node *dest;
  t_rel_list *out;
  t_rel_list *in;

  retrive_addrel_delrel_args(comando, addrel_orig, addrel_dest, addrel_rel);

  if(find_ent(E, E->root, &orig, addrel_orig, 0) == 0){ //search for ent 1
    return 0; //ent orig not found
  }
  if(find_ent(E, E->root, &dest, addrel_dest, 0) == 0){ //search for ent 2
    return 0; //ent dest not found
  }

  strcpy(rel->id, addrel_rel);
  rel->orig = orig;
  rel->dest = dest;

  if(REL_INSERT(R, rel)){
    // printf("added\n");
  }

  out = malloc(sizeof(t_rel_list));
  out->rel = rel;
  in = malloc(sizeof(t_rel_list));
  in->rel = rel;

  ADD_TO_LIST(&(orig->out_rel), out);
  ADD_TO_LIST(&(dest->in_rel), in);

  return 1;
  // printf("add new rel: Orig: %s Dest: %s Rel: %s\n", addrel_orig, addrel_dest, addrel_rel);
}

int delrel(t_ent_head *T, char *comando){
  char delrel_orig[50];
  char delrel_dest[50];
  char delrel_rel[50];

  retrive_addrel_delrel_args(comando, delrel_orig, delrel_dest, delrel_rel);

  printf("del rel: Orig: %s Dest: %s Rel: %s\n", delrel_orig, delrel_dest, delrel_rel);
}

int report(){
  printf("Report\n");
}




int main(){
  char *comando = NULL;
  int cmd;
  int i = 0;
  int size_cmd;
  size_t len = 0;
  t_ent_head T_ent; //head delle entità
  t_rel_head T_rel; //head delle relazioni
  t_ent_node *new_ent;
  t_rel_node *new_rel;

  // init entity tree
  T_ent.nil = &NIL_ENT;
  T_ent.root = T_ent.nil;

  // init relations tree
  T_rel.nil = &NIL_REL;
  T_rel.root = T_rel.nil;

  // RICEZIONE PRIMO COMANDO
  // fgets(&comando[0], MAX_INPUT, stdin);

  size_cmd = getline(&comando, &len, stdin);
  i = strlen(comando)-1;
  if(comando[i] == '\n')
      comando[i] = '\0';
  cmd = getcmd(comando);

  // FINO A CHE NON SCRIVE "end" RESTIAMO NEL CICLO
  while(cmd != 5){
    if(cmd == 0){ // addent
      new_ent = malloc(sizeof(t_ent_node));
      addent(&T_ent, comando, new_ent);
    }
    else if(cmd == 1){ // delent
      delent(&T_ent, comando);
    }
    else if(cmd == 2){ // addrel
      new_rel = malloc(sizeof(t_rel_node));
      addrel(&T_ent, &T_rel, comando, new_rel);
    }
    else if(cmd == 3){ // delrel
      delrel(&T_ent, comando);
    }
    else if(cmd == 4){ // report
      // report();
      printf("Ent list: ");
      ENT_TREE_WALK(&T_ent, T_ent.root);
      printf("\n");
      printf("Rel list: ");
      REL_TREE_WALK(&T_rel, T_rel.root);
      printf("\n");
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
