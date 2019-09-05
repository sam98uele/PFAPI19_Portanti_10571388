#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define MAX_COMANDO 6
#define MAX_INPUT 100
#define MAX_ENT_ID 50
#define MAX_REL_ID 50

// #define HASH_SIZE 64 // 4096
// #define HASH_SIZE 1 // 4096

// DEBUG_DEFINE

// #define DEBUG 1
// #define DEBUG_2 1
// #define DEBUG_ADDENT 1
// #define DEBUG_REPORT 1
// #define DEBUG_ADDREL 1
// #define DEBUG_DELREL 1
// #define DEBUG_DELENT 1
// #define DEBUG_ENT_DELETE 1
// #define DEBUG_REL_DELETE 1
// #define DEBUG_TREE_WALK 0
// #define DEBUG_3 0

// #define DEBUG_ADDENT_WALK 0

// #define DEBUG_HASH_1 0
// #define DEBUG_HASH_2 0

// #define DISABLE_ADD 0

// #define DISABLE_FREE_ENT_NODE 0
// #define DISABLE_FREE_REL_TREE 0
// #define DISABLE_FREE_REL_TREE_ID 0
// #define DISABLE_FREE_REL_NODE 0
// #define DISABLE_FREE_REL_NODE_DELENT 0
// #define DISABLE_FREE_REL_NODE_DELREL 0
// #define DISABLE_FREE_REL_NODE_ID 0
// #define DISABLE_FREE_RANK_NODE 0 //rank node è puntato solo da rel
// #define DISABLE_FREE_RANK_NODE_ID 0 //rank node è puntato solo da rel

// #define DISABLE_FREE_REL_LIST 0
// #define DISABLE_FREE_REL_LIST_DELREL 0
// #define DISABLE_FREE_REL_LIST_ADDENT 0


// #define NO_FIND 0

// #define TEST_1 0

// #define DISABLE_REPORT 0

#define INSERT_SLIDE 0
#define DELETE_SLIDE 0

// #define ITERATIVE_SEARCH 0
// #define ITERATIVE_REPORT 0


typedef struct ent_head t_ent_head;
typedef struct ent_node t_ent_node;
typedef struct rel_tree_head t_rel_tree_head;
typedef struct rel_tree_node t_rel_tree_node;
typedef struct rel_head t_rel_head;
typedef struct rel_node t_rel_node;
typedef struct rank_head t_rank_head;
typedef struct rank_node t_rank_node;

typedef struct rel_list t_rel_list;

// #################
// START ENTITY TREE
// #################
struct ent_head{
  t_ent_node *root;
  t_ent_node *nil;
};

#pragma pack(1)
struct ent_node{
  int color : 8;
  char *id; // [MAX_ENT_ID]
  t_rel_list *in_rel; // list of relations in
  t_rel_list *out_rel; // list of relations out
  t_ent_node *p;
  t_ent_node *left;
  t_ent_node *right;
};
// #################
// END OF ENTITY TREE
// #################

// #################
// START REL LIST
// #################
// Questa va messa nel nodo della relazione
#pragma pack(1)
struct rel_list{
  t_rel_node *rel;
  t_rel_list *prev;
  t_rel_list *next;
};
// #################
// END REL LIST TREE
// #################

// #########################
// START RELATIONS "ID" TREE
// #########################
struct rel_tree_head{
  t_rel_tree_node *root;
  t_rel_tree_node *nil;
};

#pragma pack(1)
struct rel_tree_node{
  int color : 8;
  char *id; //[MAX_REL_ID]
  t_rel_head *relations;
  t_rank_head *ranking;
  t_rel_tree_node *p;
  t_rel_tree_node *left;
  t_rel_tree_node *right;
};
// #########################
//  END RELATIONS "ID" TREE
// #########################

// ####################################
// START RELATIONS WITH "IN - OUT" TREE
// ####################################
struct rel_head{
  t_rel_node *root;
  t_rel_node *nil;
};

#pragma pack(1)
struct rel_node{
  int color : 8;
  t_rel_list *orig_rel_list_pointer;
  t_rel_list *dest_rel_list_pointer;
  t_ent_node *orig;
  t_rank_node *rank_pointer;
  t_rel_node *p;
  t_rel_node *left;
  t_rel_node *right;
};
// ####################################
//  END RELATIONS WITH "IN - OUT" TREE
// ####################################


// ##################
// START RANKING TREE
// ##################
struct rank_head{
  t_rank_node *root;
  t_rank_node *nil;
};

#pragma pack(1)
struct rank_node{
  int color : 8;
  unsigned short int n; // number of entrance rels
  t_ent_node *dest;
  t_rel_tree_node *rel_tree_pointer;
  t_rank_node *p;
  t_rank_node *left;
  t_rank_node *right;
};
// ##################
// END RANKING TREE
// ##################


// START Global Vars
t_ent_node NIL_ENT = {.id = "NIL_END", .color = 0, .in_rel = NULL, .out_rel = NULL, .p = NULL, .left = NULL, .right = NULL};
t_rel_tree_node NIL_REL_TREE = {.id = "NIL_REL_TREE", .color = 0, .ranking = NULL, .relations = NULL, .p = NULL, .left = NULL, .right = NULL};
t_rel_node NIL_REL = {.color = 0, .orig = &NIL_ENT, .p = NULL, .left = NULL, .right = NULL};
t_rank_node NIL_RANK = {.n = 0, .dest = &NIL_ENT, .color = 0, .p = NULL, .left = NULL, .right = NULL};


int vuoto = 1;
int printed_1 = 0;

// END Global Vars


// ################################
// ##### DEFAULT OPERATIONS #######
// ################################

// come sopra ma con strcmp
// Return values:
// 1 -> id_1 < id_2
// 0 -> altrimenti
int IS_LOWER_ID_RANK_STRCMP(int n1, char *id_1, int n2, char *id_2){
  if(n1 == n2){ // se gli n sono uguali confrontiamo le stringhe
    if(strcmp(id_1, id_2) < 0) return 1;
    else return 0;
  }
  else if(n1 < n2){
    return 1;
  }
  return 0; // equal. this is only >!
}


int REL_CMP(char *orig, char *dest, t_rel_node *x){
  int uno;
  uno = strcmp(orig, x->orig->id);
  if(uno != 0) return uno;
  return strcmp(dest, x->rank_pointer->dest->id);
}

int REL_CMP_NODES(t_rel_node *x, t_rel_node *y){
  int uno;
  uno = strcmp(x->orig->id, y->orig->id);
  if(uno != 0) return uno;
  return strcmp(x->rank_pointer->dest->id, y->rank_pointer->dest->id);
}


// ###############################
// #### REL LIST OPERATIONS ######
// #####      START     ##########
// ###############################

// void ADD_TO_LIST(t_rel_list **HEAD, t_rel_list *rel){
//   rel->next = *HEAD;
//   *HEAD = rel;
// }
//
// t_rel_list *REMOVE_FROM_LIST(t_rel_list **HEAD, t_rel_node *rel){
//   t_rel_list *temp = *HEAD;
//   t_rel_list *prev;
//   if(temp != NULL && temp->rel == rel){
//     *HEAD = temp->next;
//     return temp;
//   }
//   else{
//     while (temp != NULL && temp->rel != rel) {
//       prev = temp;
//       temp = temp->next;
//     }
//     if(temp != NULL){
//       prev->next = temp->next;
//       return temp;
//     }
//     else{
//       return NULL;
//     }
//   }
// }


void ADD_TO_LIST(t_rel_list **HEAD, t_rel_list *rel){
  rel->next = *HEAD;
  if(*HEAD != NULL)
    (*HEAD)->prev = rel;
  *HEAD = rel;
  rel->prev = NULL;
}

void REMOVE_FROM_LIST(t_rel_list **HEAD, t_rel_list *rel){
  if(rel->prev != NULL)
    rel->prev->next = rel->next;
  else
    *HEAD = rel->next;
  if(rel->next != NULL)
    rel->next->prev = rel->prev;
  // return rel;
}

// ###############################
// #### REL LIST OPERATIONS ######
// #####       END      ##########
// ###############################


// ###############################
// ##### ENT OPERATIONS ##########
// #####      START     ##########
// ###############################

void ENT_LEFT_ROTATE(t_ent_head *T, t_ent_node *x){
  t_ent_node *y = x->right;
  x->right = y->left;
  if(y->left != T->nil)
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

void ENT_RIGHT_ROTATE(t_ent_head *T, t_ent_node *x){
  t_ent_node *y = x->left;
  x->left = y->right;
  if(y->right != T->nil)
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
}

void ENT_REL_WALK(t_rel_list *x){
  if(x != NULL){
    printf("%s ", x->rel->rank_pointer->rel_tree_pointer->id);
    ENT_REL_WALK(x->next);
  }
}

void ENT_TREE_WALK(t_ent_head *T, t_ent_node *x){
  if(x != T->nil){
    ENT_TREE_WALK(T, x->left);
    printf(" %s ", x->id);
    ENT_TREE_WALK(T, x->right);
  }
  else{
    printf(" %s ", x->id);
  }
}

t_ent_node *ENT_TREE_MINIMUM(t_ent_head *T, t_ent_node *x){
  while (x->left != T->nil) {
    x = x->left;
  }
  return x;
}

t_ent_node *ENT_TREE_SUCCESSOR(t_ent_head *T, t_ent_node *x){
  t_ent_node *y;
  if(x->right != T->nil)
    return ENT_TREE_MINIMUM(T, x->right);
  y = x->p;
  while (y != T->nil && x == y->right) {
    x = y;
    y = y->p;
  }
  return y;
}

#ifndef INSERT_SLIDE
void ENT_INSERT_FIXUP(t_ent_head *T, t_ent_node *z){
  t_ent_node *y;
  while (z->p->color == 1) {
    if(z->p == z->p->p->left){
      y = z->p->p->right;
      if(y->color == 1){
        z->p->color = 0;
        y->color = 0;
        z->p->p->color = 1;
        z = z->p->p;
      }
      else{
        if(z == z->p->right){
          z = z->p;
          ENT_LEFT_ROTATE(T, z);
        }
        z->p->color = 0;
        z->p->p->color = 1;
        ENT_RIGHT_ROTATE(T, z->p->p);
      }
    }
    else{
      y = z->p->p->left;
      if(y->color == 1){
        z->p->color = 0;
        y->color = 0;
        z->p->p->color = 1;
        z = z->p->p;
      }
      else{
        if(z == z->p->left){
          z = z->p;
          ENT_RIGHT_ROTATE(T, z);
        }
        z->p->color = 0;
        z->p->p->color = 1;
        ENT_LEFT_ROTATE(T, z->p->p);
      }
    }
  }
  T->root->color = 0;
}
#endif

#ifdef INSERT_SLIDE
void ENT_INSERT_FIXUP(t_ent_head *T, t_ent_node *z){
  t_ent_node *x;
  t_ent_node *y;
  if(z == T->root){
    T->root->color = 0;
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
      // return 1;
    }
  }
  // return 1;
}
#endif

int ENT_INSERT(t_ent_head *T, t_ent_node *z){
  t_ent_node *y = T->nil;
  t_ent_node *x = T->root;
  while(x != T->nil){
    y = x;
    if(strcmp(z->id, x->id) < 0) // IS_LOWER_ID(z->id, x->id)
      x = x->left;
    else
      x = x->right;
  }
  z->p = y;
  if(y == T->nil)
    T->root = z;
  else if(strcmp(z->id, y->id) < 0) // IS_LOWER_ID(z->id, y->id)
    y->left = z;
  else
    y->right = z;
  z->left = T->nil;
  z->right = T->nil;
  z->color = 1;
  #ifdef DEBUG_ADDENT
    printf("!!Tree prima fixup insert: \n");
    ENT_TREE_WALK(T, T->root);
    printf("\n");
  #endif
  ENT_INSERT_FIXUP(T, z);
  return 1;
}

#ifndef DELETE_SLIDE
void ENT_TRANSPLANT(t_ent_head *T, t_ent_node *u, t_ent_node *v){
  if(u->p == T->nil)
    T->root = v;
  else if(u == u->p->left)
    u->p->left = v;
  else
    u->p->right = v;
  v->p = u->p;
}

void ENT_DELETE_FIXUP(t_ent_head *T, t_ent_node *x){
  t_ent_node *w;
  while (x != T->root && x->color == 0) {
    if(x == x->p->left){
      w = x->p->right;
      if(w->color == 1){
        w->color = 0;
        x->p->color = 1;
        ENT_LEFT_ROTATE(T, x->p);
        w = x->p->right;
      }
      if(w->left->color == 0 && w->right->color == 0){
        w->color = 1;
        x = x->p;
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
        x = T->root;
      }
    }
    else{
      w = x->p->left;
      if(w->color == 1){
        w->color = 0;
        x->p->color = 1;
        ENT_RIGHT_ROTATE(T, x->p);
        w = x->p->left;
      }
      if(w->right->color == 0 && w->left->color == 0){
        w->color = 1;
        x = x->p;
      }
      else{
        if(w->left->color == 0){
          w->right->color = 0;
          w->color = 1;
          ENT_LEFT_ROTATE(T, w);
          w = x->p->left;
        }
        w->color = x->p->color;
        x->p->color = 0;
        w->left->color = 0;
        ENT_RIGHT_ROTATE(T, x->p);
        x = T->root;
      }
    }
  }
  x->color = 0;
}

t_ent_node *ENT_DELETE(t_ent_head *T, t_ent_node *z){
  t_ent_node *y = z;
  t_ent_node *x;
  int y_original_color = y->color;

  if(z->left == T->nil){
    x = z->right;
    ENT_TRANSPLANT(T, z, z->right);
  }
  else if(z->right == T->nil){
    x = z->left;
    ENT_TRANSPLANT(T, z, z->left);
  }
  else{
    y = ENT_TREE_MINIMUM(T, z->right);
    y_original_color = y->color;
    x = y->right;
    if(y->p == z)
      x->p = y;
    else{
      ENT_TRANSPLANT(T, y, y->right);
      y->right = z->right;
      y->right->p = y;
    }
    ENT_TRANSPLANT(T, z, y);

    y->left = z->left;

    y->left->p = y;
    y->color = z->color;
  }
  if(y_original_color == 0)
    ENT_DELETE_FIXUP(T, x);

  return z;
}
#endif

#ifdef DELETE_SLIDE
void ENT_DELETE_FIXUP(t_ent_head *T, t_ent_node *x){
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
      if(w->left->color == 0){
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
  int color;
  t_ent_node y_temp; //temporaneo per salvare i dati durante lo scambio
  t_ent_node z_temp; //temporaneo per salvare i dati durante lo scambio

  t_ent_node *x;
  t_ent_node *y;

  #ifdef DEBUG_ENT_DELETE
    printf("Deletion of ent: %s\n", z->id);
  #endif

  if(z->left == T->nil || z->right == T->nil)
    y = z;
  else
    y = ENT_TREE_SUCCESSOR(T, z);

  // START MIO ALGORITMO
  color = y->color; // salviamo il colore di y

  // prima di eliminare il nodo, scambiamo y con z (se bisogna farlo)
  //    serve per evitare conflitti
  if(y != z){
    y_temp.color = y->color;
    y_temp.p = y->p;
    y_temp.left = y->left;
    y_temp.right = y->right;

    z_temp.color = z->color;
    z_temp.p = z->p;
    z_temp.left = z->left;
    z_temp.right = z->right;

    // INIZIO INVERSIONE Y CON Z

    // metto y al posto di z
    y->color = z_temp.color;
    if(z_temp.p != y)
      y->p = z_temp.p;
    else
      y->p = z;
    if(z_temp.left != y)
      y->left = z_temp.left;
    else
      y->left = z;
    if(z_temp.right != y)
      y->right = z_temp.right;
    else
      y->right = z;

    // metto z al posto di y
    z->color = y_temp.color;
    if(y_temp.p != z)
      z->p = y_temp.p;
    else
      z->p = y;
    if(y_temp.left != z)
      z->left = y_temp.left;
    else
      z->left = y;
    if(y_temp.right != z)
      z->right = y_temp.right;
    else
      z->right = y;


    // /////////////// FIX PER Y ////////////////////////
    // sistemiamo i figli del padre
    if(y->p == T->nil) // se è la radice
      T->root = y; // andiamo ad aggiornare la radice
    else if(z == y->p->left) // se z era il figlio sinistro
      y->p->left = y; // poniamo figlio sinistro y
    else if(z == y->p->right) // se z era il figlio sinistro
      y->p->right = y; // poniamo figlio destro y

    // ora andiamo a sistemare il padre dei figli
    if(y->left != T->nil) // && y->left != y
      y->left->p = y;
    if(y->right != T->nil) // && y->right != y
      y->right->p = y;


    // /////////////// FIX PER Z ////////////////////////
    if(z->p == T->nil)
      T->root = z;
    else if(y == z->p->left)
      z->p->left = z;
    else if(y == z->p->right)
      z->p->right = z;

    if(z->left != T->nil)
      z->left->p = z;
    if(z->right != T->nil)
      z->right->p = z;

    // ///////////////////////////////////////


    // FINE INVERSIONE Y CON Z

    y = z;  // ora quello vero da eliminare torna ad essere z!
              // che ha preso il posto di y (e viceversa)
  }

  if(y->left != T->nil)
    x = y->left;
  else
    x = y->right;

  // START eliminazione
  x->p = y->p;
  // printf("x id: \n", );
  if(y->p == T->nil)
    T->root = x;
  else if(y == y->p->left)
    y->p->left = x;
  else
    y->p->right = x;
  // END eliminazione
  // END NEWW


  if(color == 0)
    ENT_DELETE_FIXUP(T, x);
  return y; //return the deleted node
}
#endif
// ###############################
// ##### ENT OPERATIONS ##########
// #####       END      ##########
// ###############################


// ###############################
// ##### REL OPERATIONS ##########
// #####      START     ##########
// ###############################
void REL_LEFT_ROTATE(t_rel_head *T, t_rel_node *x){
  t_rel_node *y = x->right;
  x->right = y->left;
  if(y->left != T->nil)
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

void REL_RIGHT_ROTATE(t_rel_head *T, t_rel_node *x){
  t_rel_node *y = x->left;
  x->left = y->right;
  if(y->right != T->nil)
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
}

void REL_TREE_WALK(t_rel_head *T, t_rel_node *x){
  if(x != T->nil){
    REL_TREE_WALK(T, x->left);
    printf("Rel: \"%s\" From: \"%s\" To: \"%s\"", x->rank_pointer->rel_tree_pointer->id, x->orig->id, x->rank_pointer->dest->id);
    REL_TREE_WALK(T, x->right);
  }
}

t_rel_node *REL_TREE_MINIMUM(t_rel_head *T, t_rel_node *x){
  while (x->left != T->nil) {
    x = x->left;
  }
  return x;
}

t_rel_node *REL_TREE_SUCCESSOR(t_rel_head *T, t_rel_node *x){
  t_rel_node *y;
  if(x->right != T->nil)
    return REL_TREE_MINIMUM(T, x->right);
  y = x->p;
  while (y != T->nil && x == y->right) {
    x = y;
    y = y->p;
  }
  return y;
}

#ifndef INSERT_SLIDE
void REL_INSERT_FIXUP(t_rel_head *T, t_rel_node *z){
  t_rel_node *y;
  while (z->p->color == 1) {
    if(z->p == z->p->p->left){
      y = z->p->p->right;
      if(y->color == 1){
        z->p->color = 0;
        y->color = 0;
        z->p->p->color = 1;
        z = z->p->p;
      }
      else{
        if(z == z->p->right){
          z = z->p;
          REL_LEFT_ROTATE(T, z);
        }
        z->p->color = 0;
        z->p->p->color = 1;
        REL_RIGHT_ROTATE(T, z->p->p);
      }
    }
    else{
      y = z->p->p->left;
      if(y->color == 1){
        z->p->color = 0;
        y->color = 0;
        z->p->p->color = 1;
        z = z->p->p;
      }
      else{
        if(z == z->p->left){
          z = z->p;
          REL_RIGHT_ROTATE(T, z);
        }
        z->p->color = 0;
        z->p->p->color = 1;
        REL_LEFT_ROTATE(T, z->p->p);
      }
    }
  }
  T->root->color = 0;
}
#endif

#ifdef INSERT_SLIDE
void REL_INSERT_FIXUP(t_rel_head *T, t_rel_node *z){
  t_rel_node *x;
  t_rel_node *y;
  if(z == T->root){
    T->root->color = 0;
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
      // return 1;
    }
  }
  // return 1;
}
#endif

int REL_INSERT(t_rel_head *T, t_rel_node *z){
  t_rel_node *y = T->nil;
  t_rel_node *x = T->root;
  while(x != T->nil){
    y = x;
    if(REL_CMP_NODES(z, x) < 0) // strcmp(z->id, x->id) // IS_LOWER_ID(z->id, x->id)
      x = x->left;
    else
      x = x->right;
  }
  z->p = y;
  if(y == T->nil)
    T->root = z;
  else if(REL_CMP_NODES(z, y) < 0) // strcmp(z->id, x->id) // IS_LOWER_ID(z->id, y->id)
    y->left = z;
  else
    y->right = z;
  z->left = T->nil;
  z->right = T->nil;
  z->color = 1;
  REL_INSERT_FIXUP(T, z);
  return 1;
}

#ifndef DELETE_SLIDE
void REL_TRANSPLANT(t_rel_head *T, t_rel_node *u, t_rel_node *v){
  if(u->p == T->nil)
    T->root = v;
  else if(u == u->p->left)
    u->p->left = v;
  else
    u->p->right = v;
  v->p = u->p;
}

void REL_DELETE_FIXUP(t_rel_head *T, t_rel_node *x){
  t_rel_node *w;
  while (x != T->root && x->color == 0) {
    if(x == x->p->left){
      w = x->p->right;
      if(w->color == 1){
        w->color = 0;
        x->p->color = 1;
        REL_LEFT_ROTATE(T, x->p);
        w = x->p->right;
      }
      if(w->left->color == 0 && w->right->color == 0){
        w->color = 1;
        x = x->p;
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
        x = T->root;
      }
    }
    else{
      w = x->p->left;
      if(w->color == 1){
        w->color = 0;
        x->p->color = 1;
        REL_RIGHT_ROTATE(T, x->p);
        w = x->p->left;
      }
      if(w->right->color == 0 && w->left->color == 0){
        w->color = 1;
        x = x->p;
      }
      else{
        if(w->left->color == 0){
          w->right->color = 0;
          w->color = 1;
          REL_LEFT_ROTATE(T, w);
          w = x->p->left;
        }
        w->color = x->p->color;
        x->p->color = 0;
        w->left->color = 0;
        REL_RIGHT_ROTATE(T, x->p);
        x = T->root;
      }
    }
  }
  x->color = 0;
}

t_rel_node *REL_DELETE(t_rel_head *T, t_rel_node *z){
  t_rel_node *y = z;
  t_rel_node *x;
  int y_original_color = y->color;

  if(z->left == T->nil){
    x = z->right;
    REL_TRANSPLANT(T, z, z->right);
  }
  else if(z->right == T->nil){
    x = z->left;
    REL_TRANSPLANT(T, z, z->left);
  }
  else{
    y = REL_TREE_MINIMUM(T, z->right);
    y_original_color = y->color;
    x = y->right;
    if(y->p == z)
      x->p = y;
    else{
      REL_TRANSPLANT(T, y, y->right);
      y->right = z->right;
      y->right->p = y;
    }
    REL_TRANSPLANT(T, z, y);

    y->left = z->left;

    y->left->p = y;
    y->color = z->color;
  }
  if(y_original_color == 0)
    REL_DELETE_FIXUP(T, x);

  return z;
}
#endif

#ifdef DELETE_SLIDE
void REL_DELETE_FIXUP(t_rel_head *T, t_rel_node *x){
  t_rel_node *w;
  if(x->color == 1 || x->p == T->nil)
    x->color = 0; // case 0
  else if(x == x->p->left){
    #ifdef DEBUG_REL_DELETE
      // printf("strada 1\n");
    #endif
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
    #ifdef DEBUG_REL_DELETE
      // printf("strada 2\n");
    #endif
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
      if(w->left->color == 0){
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
  int color;
  t_rel_node y_temp; //temporaneo per salvare i dati durante lo scambio
  t_rel_node z_temp; //temporaneo per salvare i dati durante lo scambio

  t_rel_node *x;
  t_rel_node *y;

  #ifdef DEBUG_REL_DELETE
    printf("Deletion of rel: %s\n", z->id);
  #endif

  if(z->left == T->nil || z->right == T->nil)
    y = z;
  else
    y = REL_TREE_SUCCESSOR(T, z);

  // START NEWW
  color = y->color; // salviamo il colore di y

  // prima di eliminare il nodo, scambiamo y con z (se bisogna farlo)
  //    serve per evitare conflitti
  if(y != z){
    y_temp.color = y->color;
    y_temp.p = y->p;
    y_temp.left = y->left;
    y_temp.right = y->right;

    z_temp.color = z->color;
    z_temp.p = z->p;
    z_temp.left = z->left;
    z_temp.right = z->right;

    // INIZIO INVERSIONE Y CON Z

    // metto y al posto di z
    y->color = z_temp.color;
    if(z_temp.p != y)
      y->p = z_temp.p;
    else
      y->p = z;
    if(z_temp.left != y)
      y->left = z_temp.left;
    else
      y->left = z;
    if(z_temp.right != y)
      y->right = z_temp.right;
    else
      y->right = z;

    // metto z al posto di y
    z->color = y_temp.color;
    if(y_temp.p != z)
      z->p = y_temp.p;
    else
      z->p = y;
    if(y_temp.left != z)
      z->left = y_temp.left;
    else
      z->left = y;
    if(y_temp.right != z)
      z->right = y_temp.right;
    else
      z->right = y;


    // /////////////// FIX PER Y ////////////////////////
    // sistemiamo i figli del padre
    if(y->p == T->nil) // se è la radice
      T->root = y; // andiamo ad aggiornare la radice
    else if(z == y->p->left) // se z era il figlio sinistro
      y->p->left = y; // poniamo figlio sinistro y
    else if(z == y->p->right) // se z era il figlio sinistro
      y->p->right = y; // poniamo figlio destro y

    // ora andiamo a sistemare il padre dei figli
    if(y->left != T->nil) // && y->left != y
      y->left->p = y;
    if(y->right != T->nil) // && y->right != y
      y->right->p = y;


    // /////////////// FIX PER Z ////////////////////////
    if(z->p == T->nil)
      T->root = z;
    else if(y == z->p->left)
      z->p->left = z;
    else if(y == z->p->right)
      z->p->right = z;

    if(z->left != T->nil)
      z->left->p = z;
    if(z->right != T->nil)
      z->right->p = z;

    // ///////////////////////////////////////


    // FINE INVERSIONE Y CON Z

    y = z;  // ora quello vero da eliminare torna ad essere z!
              // che ha preso il posto di y (e viceversa)
  }

  if(y->left != T->nil)
    x = y->left;
  else
    x = y->right;

  // START eliminazione
  x->p = y->p;
  // printf("x id: \n", );
  if(y->p == T->nil)
    T->root = x;
  else if(y == y->p->left)
    y->p->left = x;
  else
    y->p->right = x;
  // END eliminazione
  // END NEWW

  if(color == 0)
    REL_DELETE_FIXUP(T, x);
  return y; //return the deleted node
}
#endif
// ###############################
// ##### REL OPERATIONS ##########
// #####       END      ##########
// ###############################


// ####################################
// ##### REL TREE OPERATIONS ##########
// #####         START       ##########
// ####################################
void REL_TREE_LEFT_ROTATE(t_rel_tree_head *T, t_rel_tree_node *x){
  t_rel_tree_node *y = x->right;
  x->right = y->left;
  if(y->left != T->nil)
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

void REL_TREE_RIGHT_ROTATE(t_rel_tree_head *T, t_rel_tree_node *x){
  t_rel_tree_node *y = x->left;
  x->left = y->right;
  if(y->right != T->nil)
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
}

void REL_TREE_TREE_WALK(t_rel_tree_head *T, t_rel_tree_node *x){
  if(x != T->nil){
    REL_TREE_TREE_WALK(T, x->left);
    // printf("Rel: \"%s\" From: \"%s\" To: \"%s\"", x->id, x->orig->id, x->dest->id);
    REL_TREE_TREE_WALK(T, x->right);
  }
}

t_rel_tree_node *REL_TREE_TREE_MINIMUM(t_rel_tree_head *T, t_rel_tree_node *x){
  while (x->left != T->nil) {
    x = x->left;
  }
  return x;
}

t_rel_tree_node *REL_TREE_TREE_MAXIMUM(t_rel_tree_head *T, t_rel_tree_node *x){
  while (x->right != T->nil) {
    x = x->right;
  }
  return x;
}

t_rel_tree_node *REL_TREE_TREE_SUCCESSOR(t_rel_tree_head *T, t_rel_tree_node *x){
  t_rel_tree_node *y;
  if(x->right != T->nil)
    return REL_TREE_TREE_MINIMUM(T, x->right);
  y = x->p;
  while (y != T->nil && x == y->right) {
    x = y;
    y = y->p;
  }
  return y;
}

t_rel_tree_node *REL_TREE_TREE_PREDECESSOR(t_rel_tree_head *T, t_rel_tree_node *x){
  t_rel_tree_node *y;
  if(x->left != T->nil)
    return REL_TREE_TREE_MAXIMUM(T, x->left);
  y = x->p;
  while (y != T->nil && x == y->left) {
    x = y;
    y = y->p;
  }
  return y;
}

#ifndef INSERT_SLIDE
void REL_TREE_INSERT_FIXUP(t_rel_tree_head *T, t_rel_tree_node *z){
  t_rel_tree_node *y;
  while (z->p->color == 1) {
    if(z->p == z->p->p->left){
      y = z->p->p->right;
      if(y->color == 1){
        z->p->color = 0;
        y->color = 0;
        z->p->p->color = 1;
        z = z->p->p;
      }
      else{
        if(z == z->p->right){
          z = z->p;
          REL_TREE_LEFT_ROTATE(T, z);
        }
        z->p->color = 0;
        z->p->p->color = 1;
        REL_TREE_RIGHT_ROTATE(T, z->p->p);
      }
    }
    else{
      y = z->p->p->left;
      if(y->color == 1){
        z->p->color = 0;
        y->color = 0;
        z->p->p->color = 1;
        z = z->p->p;
      }
      else{
        if(z == z->p->left){
          z = z->p;
          REL_TREE_RIGHT_ROTATE(T, z);
        }
        z->p->color = 0;
        z->p->p->color = 1;
        REL_TREE_LEFT_ROTATE(T, z->p->p);
      }
    }
  }
  T->root->color = 0;
}
#endif

#ifdef INSERT_SLIDE
void REL_TREE_INSERT_FIXUP(t_rel_tree_head *T, t_rel_tree_node *z){
  t_rel_tree_node *x;
  t_rel_tree_node *y;
  if(z == T->root){
    T->root->color = 0;
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
          REL_TREE_INSERT_FIXUP(T, x->p);
        }
        else{
          if(z == x->right){
            z = x;
            REL_TREE_LEFT_ROTATE(T, z);
            x = z->p;
          }
          x->color = 0;
          x->p->color = 1;
          REL_TREE_RIGHT_ROTATE(T, x->p);
        }
      }
      else{ // se x == x->p->right (se x è figlio destro)
        y = x->p->left;
        if(y->color == 1){
          x->color = 0;
          y->color = 0;
          x->p->color = 1;
          REL_TREE_INSERT_FIXUP(T, x->p);
        }
        else{
          if(z == x->left){
            z = x;
            REL_TREE_RIGHT_ROTATE(T, z);
            x = z->p;
          }
          x->color = 0;
          x->p->color = 1;
          REL_TREE_LEFT_ROTATE(T, x->p);
        }
      }
    }
    else{ //if the node is black. do nothing
      // return 1;
    }
  }
  // return 1;
}
#endif

int REL_TREE_INSERT(t_rel_tree_head *T, t_rel_tree_node *z){
  t_rel_tree_node *y = T->nil;
  t_rel_tree_node *x = T->root;
  while(x != T->nil){
    y = x;
    if(strcmp(z->id, x->id) < 0) // IS_LOWER_ID(z->id, x->id)
      x = x->left;
    else
      x = x->right;
  }
  z->p = y;
  if(y == T->nil)
    T->root = z;
  else if(strcmp(z->id, y->id) < 0) // IS_LOWER_ID(z->id, y->id)
    y->left = z;
  else
    y->right = z;
  z->left = T->nil;
  z->right = T->nil;
  z->color = 1;
  REL_TREE_INSERT_FIXUP(T, z);
  return 1;
}

#ifndef DELETE_SLIDE
void REL_TREE_TRANSPLANT(t_rel_tree_head *T, t_rel_tree_node *u, t_rel_tree_node *v){
  if(u->p == T->nil)
    T->root = v;
  else if(u == u->p->left)
    u->p->left = v;
  else
    u->p->right = v;
  v->p = u->p;
}

void REL_TREE_DELETE_FIXUP(t_rel_tree_head *T, t_rel_tree_node *x){
  t_rel_tree_node *w;
  while (x != T->root && x->color == 0) {
    if(x == x->p->left){
      w = x->p->right;
      if(w->color == 1){
        w->color = 0;
        x->p->color = 1;
        REL_TREE_LEFT_ROTATE(T, x->p);
        w = x->p->right;
      }
      if(w->left->color == 0 && w->right->color == 0){
        w->color = 1;
        x = x->p;
      }
      else{
        if(w->right->color == 0){
          w->left->color = 0;
          w->color = 1;
          REL_TREE_RIGHT_ROTATE(T, w);
          w = x->p->right;
        }
        w->color = x->p->color;
        x->p->color = 0;
        w->right->color = 0;
        REL_TREE_LEFT_ROTATE(T, x->p);
        x = T->root;
      }
    }
    else{
      w = x->p->left;
      if(w->color == 1){
        w->color = 0;
        x->p->color = 1;
        REL_TREE_RIGHT_ROTATE(T, x->p);
        w = x->p->left;
      }
      if(w->right->color == 0 && w->left->color == 0){
        w->color = 1;
        x = x->p;
      }
      else{
        if(w->left->color == 0){
          w->right->color = 0;
          w->color = 1;
          REL_TREE_LEFT_ROTATE(T, w);
          w = x->p->left;
        }
        w->color = x->p->color;
        x->p->color = 0;
        w->left->color = 0;
        REL_TREE_RIGHT_ROTATE(T, x->p);
        x = T->root;
      }
    }
  }
  x->color = 0;
}

t_rel_tree_node *REL_TREE_DELETE(t_rel_tree_head *T, t_rel_tree_node *z){
  t_rel_tree_node *y = z;
  t_rel_tree_node *x;
  int y_original_color = y->color;

  if(z->left == T->nil){
    x = z->right;
    REL_TREE_TRANSPLANT(T, z, z->right);
  }
  else if(z->right == T->nil){
    x = z->left;
    REL_TREE_TRANSPLANT(T, z, z->left);
  }
  else{
    y = REL_TREE_TREE_MINIMUM(T, z->right);
    y_original_color = y->color;
    x = y->right;
    if(y->p == z)
      x->p = y;
    else{
      REL_TREE_TRANSPLANT(T, y, y->right);
      y->right = z->right;
      y->right->p = y;
    }
    REL_TREE_TRANSPLANT(T, z, y);

    y->left = z->left;

    y->left->p = y;
    y->color = z->color;
  }
  if(y_original_color == 0)
    REL_TREE_DELETE_FIXUP(T, x);

  return z;
}
#endif

#ifdef DELETE_SLIDE
void REL_TREE_DELETE_FIXUP(t_rel_tree_head *T, t_rel_tree_node *x){
  t_rel_tree_node *w;
  if(x->color == 1 || x->p == T->nil)
    x->color = 0; // case 0
  else if(x == x->p->left){
    w = x->p->right;
    if(w->color == 1){
      w->color = 0;
      x->p->color = 1;
      REL_TREE_LEFT_ROTATE(T, x->p);
      w = x->p->right;
    }
    if(w->left->color == 0 && w->right->color == 0){
      w->color = 1;
      REL_TREE_DELETE_FIXUP(T, x->p);
    }
    else{
      if(w->right->color == 0){
        w->left->color = 0;
        w->color = 1;
        REL_TREE_RIGHT_ROTATE(T, w);
        w = x->p->right;
      }
      w->color = x->p->color;
      x->p->color = 0;
      w->right->color = 0;
      REL_TREE_LEFT_ROTATE(T, x->p);
    }
  }
  else{ // if x == x->p->right
    w = x->p->left;
    if(w->color == 1){
      w->color = 0;
      x->p->color = 1;
      REL_TREE_RIGHT_ROTATE(T, x->p);
      w = x->p->left;
    }
    if(w->right->color == 0 && w->left->color == 0){
      w->color = 1;
      REL_TREE_DELETE_FIXUP(T, x->p);
    }
    else{
      if(w->left->color == 0){
        w->right->color = 0;
        w->color = 1;
        REL_TREE_LEFT_ROTATE(T, w);
        w = x->p->left;
      }
      w->color = x->p->color;
      x->p->color = 0;
      w->left->color = 0;
      REL_TREE_RIGHT_ROTATE(T, x->p);
    }
  }
}

t_rel_tree_node *REL_TREE_DELETE(t_rel_tree_head *T, t_rel_tree_node *z){
  int color;
  t_rel_tree_node y_temp;
  t_rel_tree_node z_temp;

  t_rel_tree_node *x;
  t_rel_tree_node *y;

  if(z->left == T->nil || z->right == T->nil)
    y = z;
  else
    y = REL_TREE_TREE_SUCCESSOR(T, z);

  color = y->color; // salviamo il colore di y

  // prima di eliminare il nodo, scambiamo y con z (se bisogna farlo)
  //    serve per evitare conflitti
  if(y != z){
    y_temp.color = y->color;
    y_temp.p = y->p;
    y_temp.left = y->left;
    y_temp.right = y->right;

    z_temp.color = z->color;
    z_temp.p = z->p;
    z_temp.left = z->left;
    z_temp.right = z->right;

    // INIZIO INVERSIONE Y CON Z

    // metto y al posto di z
    y->color = z_temp.color;
    if(z_temp.p != y)
      y->p = z_temp.p;
    else
      y->p = z;
    if(z_temp.left != y)
      y->left = z_temp.left;
    else
      y->left = z;
    if(z_temp.right != y)
      y->right = z_temp.right;
    else
      y->right = z;

    // metto z al posto di y
    z->color = y_temp.color;
    if(y_temp.p != z)
      z->p = y_temp.p;
    else
      z->p = y;
    if(y_temp.left != z)
      z->left = y_temp.left;
    else
      z->left = y;
    if(y_temp.right != z)
      z->right = y_temp.right;
    else
      z->right = y;


    // /////////////// FIX PER Y ////////////////////////
    // sistemiamo i figli del padre
    if(y->p == T->nil) // se è la radice
      T->root = y; // andiamo ad aggiornare la radice
    else if(z == y->p->left) // se z era il figlio sinistro
      y->p->left = y; // poniamo figlio sinistro y
    else if(z == y->p->right) // se z era il figlio sinistro
      y->p->right = y; // poniamo figlio destro y

    // ora andiamo a sistemare il padre dei figli
    if(y->left != T->nil) // && y->left != y
      y->left->p = y;
    if(y->right != T->nil) // && y->right != y
      y->right->p = y;


    // /////////////// FIX PER Z ////////////////////////
    if(z->p == T->nil)
      T->root = z;
    else if(y == z->p->left)
      z->p->left = z;
    else if(y == z->p->right)
      z->p->right = z;

    if(z->left != T->nil)
      z->left->p = z;
    if(z->right != T->nil)
      z->right->p = z;

    // ///////////////////////////////////////


    // FINE INVERSIONE Y CON Z

    y = z;  // ora quello vero da eliminare torna ad essere z!
              // che ha preso il posto di y (e viceversa)
  }

  if(y->left != T->nil)
    x = y->left;
  else
    x = y->right;

  // START eliminazione
  x->p = y->p;
  // printf("x id: \n", );
  if(y->p == T->nil)
    T->root = x;
  else if(y == y->p->left)
    y->p->left = x;
  else
    y->p->right = x;
  // END eliminazione


  if(color == 0)
    REL_TREE_DELETE_FIXUP(T, x);
  return y; //return the deleted node
}
#endif
// ####################################
// ##### REL TREE OPERATIONS ##########
// #####          END        ##########
// ####################################


// ####################################
// ###### RANKING OPERATIONS ##########
// ######       START        ##########
// ####################################
void RANK_LEFT_ROTATE(t_rank_head *T, t_rank_node *x){
  t_rank_node *y = x->right;
  x->right = y->left;
  if(y->left != T->nil)
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

void RANK_RIGHT_ROTATE(t_rank_head *T, t_rank_node *x){
  t_rank_node *y = x->left;
  x->left = y->right;
  if(y->right != T->nil)
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
}

void RANK_TREE_WALK(t_rank_head *T, t_rank_node *x){
  if(x != T->nil){
    RANK_TREE_WALK(T, x->left);
    printf("\t\tid: \"%s\" n: \"%d\" color: %d", x->dest->id, x->n, x->color);
    if(x->p == T->nil) {
      printf(" !radice!");
    }
    printf("\n");
    RANK_TREE_WALK(T, x->right);
  }
}

t_rank_node *RANK_TREE_MINIMUM(t_rank_head *T, t_rank_node *x){
  while (x->left != T->nil) {
    x = x->left;
  }
  return x;
}

t_rank_node *RANK_TREE_MAXIMUM(t_rank_head *T, t_rank_node *x){
  while (x->right != T->nil) {
    x = x->right;
  }
  return x;
}

t_rank_node *RANK_TREE_SUCCESSOR(t_rank_head *T, t_rank_node *x){
  t_rank_node *y;
  if(x->right != T->nil)
    return RANK_TREE_MINIMUM(T, x->right);
  y = x->p;
  while (y != T->nil && x == y->right) {
    x = y;
    y = y->p;
  }
  return y;
}

t_rank_node *RANK_TREE_PREDECESSOR(t_rank_head *T, t_rank_node *x){
  t_rank_node *y;
  if(x->left != T->nil)
    return RANK_TREE_MAXIMUM(T, x->left);
  y = x->p;
  while (y != T->nil && x == y->left) {
    x = y;
    y = y->p;
  }
  return y;
}

#ifndef INSERT_SLIDE
void RANK_INSERT_FIXUP(t_rank_head *T, t_rank_node *z){
  t_rank_node *y;
  while (z->p->color == 1) {
    if(z->p == z->p->p->left){
      y = z->p->p->right;
      if(y->color == 1){
        z->p->color = 0;
        y->color = 0;
        z->p->p->color = 1;
        z = z->p->p;
      }
      else{
        if(z == z->p->right){
          z = z->p;
          RANK_LEFT_ROTATE(T, z);
        }
        z->p->color = 0;
        z->p->p->color = 1;
        RANK_RIGHT_ROTATE(T, z->p->p);
      }
    }
    else{
      y = z->p->p->left;
      if(y->color == 1){
        z->p->color = 0;
        y->color = 0;
        z->p->p->color = 1;
        z = z->p->p;
      }
      else{
        if(z == z->p->left){
          z = z->p;
          RANK_RIGHT_ROTATE(T, z);
        }
        z->p->color = 0;
        z->p->p->color = 1;
        RANK_LEFT_ROTATE(T, z->p->p);
      }
    }
  }
  T->root->color = 0;
}
#endif

#ifdef INSERT_SLIDE
int RANK_INSERT_FIXUP(t_rank_head *T, t_rank_node *z){
  t_rank_node *x;
  t_rank_node *y;
  if(z == T->root){
    T->root->color = 0;
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
          RANK_INSERT_FIXUP(T, x->p);
        }
        else{
          if(z == x->right){
            z = x;
            RANK_LEFT_ROTATE(T, z);
            x = z->p;
          }
          x->color = 0;
          x->p->color = 1;
          RANK_RIGHT_ROTATE(T, x->p);
        }
      }
      else{ // se x == x->p->right (se x è figlio destro)
        y = x->p->left;
        if(y->color == 1){
          x->color = 0;
          y->color = 0;
          x->p->color = 1;
          RANK_INSERT_FIXUP(T, x->p);
        }
        else{
          if(z == x->left){
            z = x;
            RANK_RIGHT_ROTATE(T, z);
            x = z->p;
          }
          x->color = 0;
          x->p->color = 1;
          RANK_LEFT_ROTATE(T, x->p);
        }
      }
    }
    else{ //if the node is black. do nothing
      return 1;
    }
  }
  return 1;
}
#endif

int RANK_INSERT(t_rank_head *T, t_rank_node *z){
  t_rank_node *y = T->nil;
  t_rank_node *x = T->root;
  while(x != T->nil){
    y = x;
    if(IS_LOWER_ID_RANK_STRCMP(z->n, z->dest->id, x->n, x->dest->id))
      x = x->left;
    else
      x = x->right;
  }
  z->p = y;
  if(y == T->nil)
    T->root = z;
  else if(IS_LOWER_ID_RANK_STRCMP(z->n, z->dest->id, y->n, y->dest->id))
    y->left = z;
  else
    y->right = z;
  z->left = T->nil;
  z->right = T->nil;
  z->color = 1;
  RANK_INSERT_FIXUP(T, z);
  return 1;
}

#ifndef DELETE_SLIDE
void RANK_TRANSPLANT(t_rank_head *T, t_rank_node *u, t_rank_node *v){
  if(u->p == T->nil)
    T->root = v;
  else if(u == u->p->left)
    u->p->left = v;
  else
    u->p->right = v;
  v->p = u->p;
}

void RANK_DELETE_FIXUP(t_rank_head *T, t_rank_node *x){
  t_rank_node *w;
  while (x != T->root && x->color == 0) {
    if(x == x->p->left){
      w = x->p->right;
      if(w->color == 1){
        w->color = 0;
        x->p->color = 1;
        RANK_LEFT_ROTATE(T, x->p);
        w = x->p->right;
      }
      if(w->left->color == 0 && w->right->color == 0){
        w->color = 1;
        x = x->p;
      }
      else{
        if(w->right->color == 0){
          w->left->color = 0;
          w->color = 1;
          RANK_RIGHT_ROTATE(T, w);
          w = x->p->right;
        }
        w->color = x->p->color;
        x->p->color = 0;
        w->right->color = 0;
        RANK_LEFT_ROTATE(T, x->p);
        x = T->root;
      }
    }
    else{
      w = x->p->left;
      if(w->color == 1){
        w->color = 0;
        x->p->color = 1;
        RANK_RIGHT_ROTATE(T, x->p);
        w = x->p->left;
      }
      if(w->right->color == 0 && w->left->color == 0){
        w->color = 1;
        x = x->p;
      }
      else{
        if(w->left->color == 0){
          w->right->color = 0;
          w->color = 1;
          RANK_LEFT_ROTATE(T, w);
          w = x->p->left;
        }
        w->color = x->p->color;
        x->p->color = 0;
        w->left->color = 0;
        RANK_RIGHT_ROTATE(T, x->p);
        x = T->root;
      }
    }
  }
  x->color = 0;
}

t_rank_node *RANK_DELETE(t_rank_head *T, t_rank_node *z){
  t_rank_node *y = z;
  t_rank_node *x;
  int y_original_color = y->color;

  if(z->left == T->nil){
    x = z->right;
    RANK_TRANSPLANT(T, z, z->right);
  }
  else if(z->right == T->nil){
    x = z->left;
    RANK_TRANSPLANT(T, z, z->left);
  }
  else{
    y = RANK_TREE_MINIMUM(T, z->right);
    y_original_color = y->color;
    x = y->right;
    if(y->p == z)
      x->p = y;
    else{
      RANK_TRANSPLANT(T, y, y->right);
      y->right = z->right;
      y->right->p = y;
    }
    RANK_TRANSPLANT(T, z, y);

    y->left = z->left;

    y->left->p = y;
    y->color = z->color;
  }
  if(y_original_color == 0)
    RANK_DELETE_FIXUP(T, x);

  return z;
}
#endif

#ifdef DELETE_SLIDE
int RANK_DELETE_FIXUP(t_rank_head *T, t_rank_node *x){
  t_rank_node *w;
  if(x->color == 1 || x->p == T->nil)
    x->color = 0; // case 0
  else if(x == x->p->left){
    w = x->p->right;
    if(w->color == 1){
      w->color = 0;
      x->p->color = 1;
      RANK_LEFT_ROTATE(T, x->p);
      w = x->p->right;
    }
    if(w->left->color == 0 && w->right->color == 0){
      w->color = 1;
      RANK_DELETE_FIXUP(T, x->p);
    }
    else{
      if(w->right->color == 0){
        w->left->color = 0;
        w->color = 1;
        RANK_RIGHT_ROTATE(T, w);
        w = x->p->right;
      }
      w->color = x->p->color;
      x->p->color = 0;
      w->right->color = 0;
      RANK_LEFT_ROTATE(T, x->p);
    }
  }
  else{ // if x == x->p->right
    w = x->p->left;
    if(w->color == 1){
      w->color = 0;
      x->p->color = 1;
      RANK_RIGHT_ROTATE(T, x->p);
      w = x->p->left;
    }
    if(w->right->color == 0 && w->left->color == 0){
      w->color = 1;
      RANK_DELETE_FIXUP(T, x->p);
    }
    else{
      if(w->left->color == 0){
        w->right->color = 0;
        w->color = 1;
        RANK_LEFT_ROTATE(T, w);
        w = x->p->left;
      }
      w->color = x->p->color;
      x->p->color = 0;
      w->left->color = 0;
      RANK_RIGHT_ROTATE(T, x->p);
    }
  }
}

t_rank_node *RANK_DELETE(t_rank_head *T, t_rank_node *z){
  int color;
  t_rank_node y_temp; //temporaneo per salvare i dati durante lo scambio
  t_rank_node z_temp; //temporaneo per salvare i dati durante lo scambio

  t_rank_node *x;
  t_rank_node *y;

  if(z->left == T->nil || z->right == T->nil)
    y = z;
  else
    y = RANK_TREE_SUCCESSOR(T, z);

  color = y->color; // salviamo il colore di y

  // prima di eliminare il nodo, scambiamo y con z (se bisogna farlo)
  //    serve per evitare conflitti
  if(y != z){
    y_temp.color = y->color;
    y_temp.p = y->p;
    y_temp.left = y->left;
    y_temp.right = y->right;

    z_temp.color = z->color;
    z_temp.p = z->p;
    z_temp.left = z->left;
    z_temp.right = z->right;

    // INIZIO INVERSIONE Y CON Z

    // metto y al posto di z
    y->color = z_temp.color;
    if(z_temp.p != y)
      y->p = z_temp.p;
    else
      y->p = z;
    if(z_temp.left != y)
      y->left = z_temp.left;
    else
      y->left = z;
    if(z_temp.right != y)
      y->right = z_temp.right;
    else
      y->right = z;

    // metto z al posto di y
    z->color = y_temp.color;
    if(y_temp.p != z)
      z->p = y_temp.p;
    else
      z->p = y;
    if(y_temp.left != z)
      z->left = y_temp.left;
    else
      z->left = y;
    if(y_temp.right != z)
      z->right = y_temp.right;
    else
      z->right = y;


    // /////////////// FIX PER Y ////////////////////////
    // sistemiamo i figli del padre
    if(y->p == T->nil) // se è la radice
      T->root = y; // andiamo ad aggiornare la radice
    else if(z == y->p->left) // se z era il figlio sinistro
      y->p->left = y; // poniamo figlio sinistro y
    else if(z == y->p->right) // se z era il figlio sinistro
      y->p->right = y; // poniamo figlio destro y

    // ora andiamo a sistemare il padre dei figli
    if(y->left != T->nil) // && y->left != y
      y->left->p = y;
    if(y->right != T->nil) // && y->right != y
      y->right->p = y;


    // /////////////// FIX PER Z ////////////////////////
    if(z->p == T->nil)
      T->root = z;
    else if(y == z->p->left)
      z->p->left = z;
    else if(y == z->p->right)
      z->p->right = z;

    if(z->left != T->nil)
      z->left->p = z;
    if(z->right != T->nil)
      z->right->p = z;

    // ///////////////////////////////////////


    // FINE INVERSIONE Y CON Z

    y = z;  // ora quello vero da eliminare torna ad essere z!
              // che ha preso il posto di y (e viceversa)
  }

  if(y->left != T->nil)
    x = y->left;
  else
    x = y->right;

  // START eliminazione
  x->p = y->p;
  // printf("x id: \n", );
  if(y->p == T->nil)
    T->root = x;
  else if(y == y->p->left)
    y->p->left = x;
  else
    y->p->right = x;
  // END eliminazione

  #ifdef DEBUG_3
    printf("allcolor: %d\n", color);
    printf("appena dopo el (no fix)\n");
    RANK_TREE_WALK(T, T->root);
  #endif

  if(color == 0)
    RANK_DELETE_FIXUP(T, x);

  return y; //return the deleted node
}
#endif
// ################################
// ##### RANK OPERATIONS ##########
// #####      END        ##########
// ################################



int getcmd(char *input){
  int i = 0;
  // char input_cmd[6];
  char cmdlist[6][7] = {"addent", "delent", "addrel", "delrel", "report", "end"};
  // while(input[i] != ' ' && (input[i] != '\0' || input[i] != '\n' || input[i] != EOF)){
    // input_cmd[i] = input[i];
    // i++;
  // }
  // input_cmd[i] = '\0';
  // printf("%s\n", input);
  for(i=0; i<6; i++)
    if(strcmp(input, cmdlist[i]) == 0)
      return i;
  return 5;
}

int retrive_addent_delent_id(char *input, char *ent){
  int i = 2; //sappiamo come è fatto il comando e parte da 7 il nome della nuova identità
  int c = 2;
  // while(input[i] != '"' && input[i] != '\0' && input[i] != ' '){
  while(input[i] != '"'){
    ent[i-c] = input[i];
    i++;
  }
  // printf("\n");
  ent[i-c] = '\0';
  return 1;
}

int retrive_addrel_delrel_args(char *input, char *orig, char *dest, char *rel){
  int i = 2; //sappiamo come è fatto il comando e parte da 7 il nome della nuova relazione
  int c = 2;
  // ORIG
  // while(input[i] != '"' && input[i] != '\0' && input[i] != ' '){ // OLD
  while(input[i] != '"'){
    orig[i-c] = input[i];
    i++;
  }
  orig[i-c] = '\0';
  // ORIG
  i = i + 3;
  c = i;
  // while(input[i] != '"' && input[i] != '\0' && input[i] != ' '){ // OLD
  while(input[i] != '"'){
    dest[i-c] = input[i];
    i++;
  }
  dest[i-c] = '\0';
  // REL
  i = i + 3;
  c = i;
  // while(input[i] != '"' && input[i] != '\0' && input[i] != ' '){ // OLD
  while(input[i] != '"'){
    rel[i-c] = input[i];
    i++;
  }
  rel[i-c] = '\0';
  // printf("end get retrive_addrel_delrel_args\n");
  return 1;
}

#ifdef ITERATIVE_SEARCH
// ITERATIVE
int check_existence_ent(t_ent_head *T, t_ent_node *x, char *id){
  while (x != T->nil) {
    if(strcmp(id, x->id) == 0) return 1;
    if(strcmp(id, x->id) < 0) x = x->left;
    else x = x->right;
  }
  return 0;
}
#endif

#ifndef ITERATIVE_SEARCH
// RECURSIVE
int check_existence_ent(t_ent_head *T, t_ent_node *x, char *id){
  if(x == T->nil){
    return 0;
  }
  else if(strcmp(id, x->id) == 0){
    return 1;
  }
  if(strcmp(id, x->id) < 0) //IS_LOWER_ID(id, x->id)
    return check_existence_ent(T, (x->left), id);
  else
    return check_existence_ent(T, (x->right), id);
}
#endif

// int check_existence_ent_hash(t_ent_head* hash_a, char *id, int v){
//   return check_existence_ent((hash_a+v), (hash_a+v)->root, id);
// }

#ifdef ITERATIVE_SEARCH
// ITERATIVE
int find_ent(t_ent_head *T, t_ent_node *x, t_ent_node **z, char *id){
  while (x != T->nil) {
    if(strcmp(id, x->id) == 0){
      *z = x;
      return 1;
    }
    if(strcmp(id, x->id) < 0) x = x->left;
    else x = x->right;
  }
  *z = x;
  return 0;
}
#endif

#ifndef ITERATIVE_SEARCH
// RECURSIVE
int find_ent(t_ent_head *T, t_ent_node *x, t_ent_node **z, char *id){
  if(x == T->nil){ // not found
    *z = x;
    return 0;
  }
  else if(strcmp(id, x->id) == 0){ // found
    *z = x;
    return 1;
  }
  if(strcmp(id, x->id) < 0) // IS_LOWER_ID(id, x->id)
    return find_ent(T, (x->left), z, id);
  else
    return find_ent(T, (x->right), z, id);
}
#endif

// int find_ent_hash(t_ent_head* hash_a, t_ent_node **z, char *id, int v){
//   return find_ent((hash_a+v), (hash_a+v)->root, z, id);
// }

#ifdef ITERATIVE_SEARCH
// ITERATIVE
int find_rel(t_rel_head *T, t_rel_node *x, t_rel_node **z, char *id_orig, char *id_dest){
  while (x != T->nil) {
    if(REL_CMP(id_orig, id_dest, x) == 0){
      *z = x;
      return 1;
    }
    if(REL_CMP(id_orig, id_dest, x) < 0) x = x->left;
    else x = x->right;
  }
  *z = x;
  return 0;
}
#endif

#ifndef ITERATIVE_SEARCH
// RECURSIVE
int find_rel(t_rel_head *T, t_rel_node *x, t_rel_node **z, char *id_orig, char *id_dest){
  if(x == T->nil){
    *z = x;
    return 0;
  }
  else if(REL_CMP(id_orig, id_dest, x) == 0){
    *z = x;
    return 1;
  }
  if(REL_CMP(id_orig, id_dest, x) < 0) // IS_LOWER_ID(id, x->id)
    return find_rel(T, (x->left), z, id_orig, id_dest);
  else
    return find_rel(T, (x->right), z, id_orig, id_dest);
}
#endif

#ifdef ITERATIVE_SEARCH
// ITERATIVE
int find_rel_tree(t_rel_tree_head *T, t_rel_tree_node *x, t_rel_tree_node **z, char *id){
  while (x != T->nil) {
    if(strcmp(id, x->id) == 0){
      *z = x;
      return 1;
    }
    if(strcmp(id, x->id) < 0) x = x->left;
    else x = x->right;
  }
  *z = x;
  return 0;
}
#endif

#ifndef ITERATIVE_SEARCH
// RECURSIVE
int find_rel_tree(t_rel_tree_head *T, t_rel_tree_node *x, t_rel_tree_node **z, char *id){
  if(x == T->nil){
    *z = x;
    return 0;
  }
  else if(strcmp(id, x->id) == 0){
    *z = x;
    return 1;
  }
  if(strcmp(id, x->id) < 0) // IS_LOWER_ID(id, x->id)
    return find_rel_tree(T, (x->left), z, id);
  else
    return find_rel_tree(T, (x->right), z, id);
}
#endif

// int find_rel_tree_hash(t_rel_tree_head *hash_rt, t_rel_tree_node **z, char *id, int v){
//   return find_rel_tree((hash_rt+v), (hash_rt+v)->root, z, id);
// }

// non usato
// void find_rank(t_rank_head *T, t_rank_node *x, t_rank_node **z, char *id){ // NON USATO
//   if(x != T->nil){
//     find_rank(T, x->left, z, id);
//     if(strcmp(id, x->dest->id) == 0){
//       *z = x;
//     }
//     #ifdef DEBUG
//     // printf("Rel: \"%s\" From: \"%s\" To: \"%s\"", x->id, x->orig->id, x->dest->id);
//     #endif
//     find_rank(T, x->right, z, id);
//   }
// }
// non usato

t_rank_node *find_rank_v2(t_rel_list *x, t_rel_tree_node *rel_tree){
  while (x != NULL) {
    // if(x->rel->v == 1 && x->rel->rank_pointer->v == 1 && strcmp(id_rel, x->rel->rank_pointer->rel_tree_pointer->id) == 0)
    // if(x->rel->v == 1 && x->rel->rank_pointer->v == 1 && rel_tree == x->rel->rank_pointer->rel_tree_pointer)
    // if(x->rel->v == 1 && x->rel->rank_pointer->v == 1 && x->rel->rank_pointer->rel_tree_pointer->v == 1 && rel_tree == x->rel->rank_pointer->rel_tree_pointer)
    if(rel_tree == x->rel->rank_pointer->rel_tree_pointer)
      break;
    x = x->next;
  }
  if(x != NULL)
    return x->rel->rank_pointer;
  return NULL;
}


// ##################################
// ##########   COMANDI    ##########
// ##################################

// FUNZIONE PRINCIPALE COMANDO "addent"
int addent(t_ent_head *E){
  char input[MAX_INPUT];
  char addent_id[MAX_ENT_ID];
  // int hash_v;
  char *r; // temp tanto alla fine della funzione viene distrutto

  t_ent_node *ent;

  // input = (char*) calloc(MAX_INPUT+1, sizeof(char));
  // addent_id = (char*) calloc(MAX_ENT_ID+1, sizeof(char));

  // ADDENT !!
  r = fgets(input, MAX_INPUT, stdin);
  retrive_addent_delent_id(input, addent_id);
  // sscanf(input," \"%49[^\"]\"", addent_id);


  if(check_existence_ent(E, E->root, addent_id) != 0){
    #ifdef DEBUG_ADDENT
      printf("esiste gia' %s\n", addent_id);
    #endif
    return 1; //if the ent already exists, do nothing
  }

  // crea nuova ent
  ent = (t_ent_node*) malloc(sizeof(t_ent_node));
  // ent = (t_ent_node*) memalign(2, sizeof(t_ent_node));

  // copy id
  ent->id = (char*) calloc(strlen(addent_id) + 1, sizeof(char));
  ent->id = strcpy(ent->id, addent_id);
  ent->in_rel = NULL;
  ent->out_rel = NULL;

  if(ENT_INSERT(E, ent)){
    #ifdef DEBUG_ADDENT
      printf("ent inserted: %s\n", ent->id);
    #endif
  }

  #ifdef DEBUG_ADDENT_WALK
    printf("!!Tree dopo insert: \n");
    ENT_TREE_WALK(E, E->root);
    printf("\n");
  #endif

  return 1;
}

// FUNZIONE PRINCIPALE COMANDO "delent"
int delent(t_ent_head *E, t_rel_tree_head *R){
  char input[MAX_INPUT];
  char delent_id[MAX_ENT_ID];
  char *r;

  t_ent_node *ent;
  // t_ent_node *deleted_ent;

  t_rel_list *in;
  // t_rel_list *in_tofree;
  t_rel_list *out;
  // t_rel_list *out_tofree;
  t_rel_list *rel_list_tofree;

  // t_rel_list *deleted_list;

  // t_rel_tree_node *deleted_rel_tree;
  // t_rel_node *deleted_rel;
  // t_rank_node *deleted_rank;

  t_rel_tree_node *rel_tree_tofree;

  r = fgets(input, MAX_INPUT, stdin);
  retrive_addent_delent_id(input, delent_id);
  // sscanf(input," \"%49[^\"]\"", delent_id);


  #ifdef DEBUG_DELENT
    printf("ent to delete: %s\n", delent_id);
  #endif

  // hash_e_v = hash_value(delent_id[0]);

  if(find_ent(E, E->root, &ent, delent_id) == 0){
    #ifdef DEBUG_DELENT
      printf("Ent NON presente, nulla da eliminare\n");
    #endif
    return 0;
  }

  #ifdef DEBUG_DELENT
    printf("found ent to delete : %s\n", ent->id);
  #endif

  // qui dovrebbe aver cancellato solo quella che dico io
  // non l'algoritmo del prof che ne cancella un'altra
  // deleted_ent = ENT_DELETE(E, ent);
  ENT_DELETE(E, ent);
  #ifdef DEBUG_DELENT
    printf("Deleted %s\n", deleted_ent->id);
  #endif

  #ifdef TEST_1
    if(deleted_ent != ent){
        printf("Deleted an other ent\n");
    }
  #endif

  // CANCELLO TUTTE LE RELAZIONI IN "IN"
  in = ent->in_rel;
  while (in != NULL) {
    rel_tree_tofree = NULL;
    // deleted_rel = REL_DELETE(in->rel->rank_pointer->rel_tree_pointer->relations, in->rel);
    REL_DELETE(in->rel->rank_pointer->rel_tree_pointer->relations, in->rel);

    #ifdef TEST_1
      if(deleted_rel != in->rel){
        printf("deleted an other rel\n");
      }
    #endif

    // POSSIBILE BUG_!!
    // se rank_h_p è vuoto, vuol dire che devo eliminare REL_TREE
    // if(in->rel->rank_h_p->root == in->rel->rank_h_p->nil){
    //   REL_TREE_DELETE(R, in->rel->rank_pointer->rel_tree_pointer);
    // }
    // devo cancellare la rel tree solo se non ci sono più rel!
    if(in->rel->rank_pointer->rel_tree_pointer->relations->root == in->rel->rank_pointer->rel_tree_pointer->relations->nil){
      // hash_rt_v = hash_value(in->rel->rank_pointer->rel_tree_pointer->id[0]);

      // deleted_rel_tree = REL_TREE_DELETE(R, in->rel->rank_pointer->rel_tree_pointer);
      REL_TREE_DELETE(R, in->rel->rank_pointer->rel_tree_pointer);

      #ifdef TEST_1
        if(deleted_rel_tree != in->rel->rank_pointer->rel_tree_pointer)
          printf("deleted an other rel tree 2\n");
      #endif

      rel_tree_tofree = in->rel->rank_pointer->rel_tree_pointer;
    }

    if((in->rel->rank_pointer->n - 1) == 0){
      // deleted_rank = RANK_DELETE(in->rel->rank_pointer->rel_tree_pointer->ranking, in->rel->rank_pointer);
      RANK_DELETE(in->rel->rank_pointer->rel_tree_pointer->ranking, in->rel->rank_pointer);

      #ifdef TEST_1
        if(deleted_rank != in->rel->rank_pointer)
          printf("deleted an other rank\n");
      #endif

      // in->rel->rank_pointer->n = in->rel->rank_pointer->n - 1;

      #ifndef DISABLE_FREE_RANK_NODE
        free(in->rel->rank_pointer);
      #endif
    }
    else{
      // update rank (lo reinseriamo)
      // deleted_rank = RANK_DELETE(in->rel->rank_pointer->rel_tree_pointer->ranking, in->rel->rank_pointer);
      RANK_DELETE(in->rel->rank_pointer->rel_tree_pointer->ranking, in->rel->rank_pointer);

      #ifdef TEST_1
        if(deleted_rank != in->rel->rank_pointer)
          printf("deleted an other rank\n");
      #endif

      in->rel->rank_pointer->n = in->rel->rank_pointer->n - 1;

      RANK_INSERT(in->rel->rank_pointer->rel_tree_pointer->ranking, in->rel->rank_pointer);
    }

    // deleted_list = REMOVE_FROM_LIST(&(in->rel->orig->out_rel), in->rel->orig_rel_list_pointer);
    // if(deleted_list != NULL)
    //   free(deleted_list);
    REMOVE_FROM_LIST(&(in->rel->orig->out_rel), in->rel->orig_rel_list_pointer);
    free(in->rel->orig_rel_list_pointer);


    if(rel_tree_tofree != NULL){
      #ifndef DISABLE_FREE_REL_TREE
        free(rel_tree_tofree->id); // così perchè ho modificato l'algoritmo!
        free(rel_tree_tofree->relations);
        free(rel_tree_tofree->ranking);
        free(rel_tree_tofree); // così perchè ho modificato l'algoritmo!
      #endif
    }

    rel_list_tofree = in;
    in = in->next;


    #ifndef DISABLE_FREE_REL_NODE_DELENT
      // free(in->rel->id);
      free(rel_list_tofree->rel);
    #endif

    #ifndef DISABLE_FREE_REL_LIST
      REMOVE_FROM_LIST(&(ent->in_rel), rel_list_tofree);
      free(rel_list_tofree);
    #endif

  }

  // CANCELLO TUTTE LE RELAZIONI IN "OUT"
  out = ent->out_rel;
  while (out != NULL) {
    rel_tree_tofree = NULL;
    // deleted_rel = REL_DELETE(out->rel->rank_pointer->rel_tree_pointer->relations, out->rel);
    REL_DELETE(out->rel->rank_pointer->rel_tree_pointer->relations, out->rel);

    #ifdef TEST_1
      if(deleted_rel != out->rel){
        printf("deleted an other rel\n");
      }
    #endif

    // POSSIBILE BUG_!!
    // se rank h_p è vuoto, vuol dire che devo eliminare REL_TREE
    // if(out->rel->rank_h_p->root == out->rel->rank_h_p->nil){
    //   REL_TREE_DELETE(R, out->rel->rank_pointer->rel_tree_pointer);
    // }
    if(out->rel->rank_pointer->rel_tree_pointer->relations->root == out->rel->rank_pointer->rel_tree_pointer->relations->nil){
      // hash_rt_v = hash_value(out->rel->rank_pointer->rel_tree_pointer->id[0]);

       REL_TREE_DELETE(R, out->rel->rank_pointer->rel_tree_pointer);

      #ifdef TEST_1
        if(deleted_rel_tree != out->rel->rank_pointer->rel_tree_pointer)
          printf("deleted an other rank\n");
      #endif

      rel_tree_tofree = out->rel->rank_pointer->rel_tree_pointer;
      // free(out->rel->rank_pointer->rel_tree_pointer->id);
      // free(out->rel->rank_pointer->rel_tree_pointer->relations);
      // free(out->rel->rank_pointer->rel_tree_pointer->ranking);
      // free(out->rel->rank_pointer->rel_tree_pointer);
    }

    // deleted_list = REMOVE_FROM_LIST(&(out->rel->rank_pointer->dest->in_rel), out->rel->dest_rel_list_pointer);
    // if(deleted_list != NULL)
    //   free(deleted_list);
    REMOVE_FROM_LIST(&(out->rel->rank_pointer->dest->in_rel), out->rel->dest_rel_list_pointer);
    free(out->rel->dest_rel_list_pointer);

    if((out->rel->rank_pointer->n - 1) == 0){
      // deleted_rank = RANK_DELETE(out->rel->rank_pointer->rel_tree_pointer->ranking, out->rel->rank_pointer);
      RANK_DELETE(out->rel->rank_pointer->rel_tree_pointer->ranking, out->rel->rank_pointer);

      #ifdef TEST_1
        if(deleted_rank != out->rel->rank_pointer)
          printf("deleted an other rank\n");
      #endif

      // out->rel->rank_pointer->n = out->rel->rank_pointer->n - 1;

      #ifndef DISABLE_FREE_RANK_NODE
        // free(out->rel->rank_pointer->id);
        free(out->rel->rank_pointer);
      #endif
    }
    else{
      // update (lo reinseriamo)
      RANK_DELETE(out->rel->rank_pointer->rel_tree_pointer->ranking, out->rel->rank_pointer);

      #ifdef TEST_1
        if(deleted_rank != out->rel->rank_pointer)
          printf("deleted an other rank\n");
      #endif

      out->rel->rank_pointer->n = out->rel->rank_pointer->n - 1;

      RANK_INSERT(out->rel->rank_pointer->rel_tree_pointer->ranking, out->rel->rank_pointer);
    }

    if(rel_tree_tofree != NULL){
      #ifndef DISABLE_FREE_REL_TREE
      free(rel_tree_tofree->id); // così perchè ho modificato l'algoritmo!
      free(rel_tree_tofree->relations);
      free(rel_tree_tofree->ranking);
      free(rel_tree_tofree); // così perchè ho modificato l'algoritmo!
      #endif
    }


    rel_list_tofree = out;
    out = out->next;

    #ifndef DISABLE_FREE_REL_NODE_DELENT
      // free(out->rel->id);
      free(rel_list_tofree->rel);
    #endif

    #ifndef DISABLE_FREE_REL_LIST
      REMOVE_FROM_LIST(&(ent->out_rel), rel_list_tofree);
      free(rel_list_tofree);
    #endif
  }

  #ifdef DEBUG
    printf("ent deleted is: %s \n", delent_id);
  #endif


  #ifdef DEBUG_TREE_WALK
    printf("\t\tENT walk after delent: ");
    ENT_TREE_WALK(T, T->root);
  #endif

  #ifndef DISABLE_FREE_ENT_NODE
    // possiamo tranquillamente farlo, perché se cancello
    // una entità, poi non ci saranno più relazioni con
    // quella entità
    free(ent->id);
    free(ent);
  #endif

  return 1;
}

// FUNZIONE PRINCIPALE COMANDO "addrel"
int addrel(t_ent_head *E, t_rel_tree_head *R){
  char input[MAX_INPUT];
  char addrel_orig[MAX_ENT_ID];
  char addrel_dest[MAX_ENT_ID];
  char addrel_rel_id[MAX_REL_ID];

  char *r; // temp. tanto alla fine della funzione viene distrutto

  t_rel_tree_node *rel_tree;
  t_rel_node *rel_node = NULL;

  t_ent_node *orig;
  t_ent_node *dest;
  t_rel_list *out;
  t_rel_list *in;

  t_rank_node *rank = NULL;

  // t_rank_node *deleted_rank;


  // input = (char*) calloc(MAX_INPUT+1, sizeof(char));
  // addrel_orig = (char*) calloc(MAX_ENT_ID+1, sizeof(char));
  // addrel_dest = (char*) calloc(MAX_ENT_ID+1, sizeof(char));
  // addrel_rel_id = (char*) calloc(MAX_REL_ID+1, sizeof(char));

  r = fgets(input, MAX_INPUT, stdin);
  retrive_addrel_delrel_args(input, addrel_orig, addrel_dest, addrel_rel_id);
  // scanf(" \"%s\" \"%s\" \"%s\"", addrel_orig, addrel_dest, addrel_rel_id);
  // sscanf(input," \"%49[^\"]\" \"%49[^\"]\" \"%49[^\"]\"", addrel_orig, addrel_dest, addrel_rel_id);

  // free(input);

  // printf("%s - %s - %s\n", addrel_orig, addrel_dest, addrel_rel_id);

  // hash_e_v_orig = hash_value(addrel_orig[0]);
  // hash_e_v_dest = hash_value(addrel_dest[0]);

  // cerchiamo l'ent di ORIGINE
  if(find_ent(E, E->root, &orig, addrel_orig) == 0){ //search for ent 1
    #ifdef DEBUG_ADDREL
      printf("Ent 1 not found\n");
    #endif
    // free(addrel_orig);
    // free(addrel_dest);
    // free(addrel_rel_id);
    return 0; //ent orig not found
  }

  // free(addrel_orig);

  // cerchiamo l'ent di DESTINAZIONE
  if(find_ent(E, E->root, &dest, addrel_dest) == 0){ //search for ent 2
    #ifdef DEBUG_ADDREL
      printf("Ent 2 not found\n");
    #endif

    #ifdef DEBUG_TREE_WALK
      ENT_TREE_WALK(E, E->root);
      printf("\n");
    #endif

    // free(addrel_dest);
    // free(addrel_rel_id);

    return 0; //ent dest not found
  }

  // free(addrel_dest);

  #ifdef DEBUG_ADDREL
    printf("\n\nAddRel: %s - %s - %s\n", addrel_orig, addrel_dest, addrel_rel_id);
  #endif

  // hash_rt_v = hash_value(addrel_rel_id[0]);

  if(find_rel_tree(R, R->root, &rel_tree, addrel_rel_id)){
    // rel_tree è già presente!
    // e in questo modo l'ho già
    #ifdef DEBUG_ADDREL
      printf("\tRel Tree Presente\n");
    #endif

    // free(addrel_rel_id);

    // cerchiamo se questa relazione fra i due è presente
    if(find_rel(rel_tree->relations, rel_tree->relations->root, &rel_node, orig->id, dest->id)){
      #ifdef DEBUG_ADDREL
        printf("\tRelazione Gia' Presente\n");
      #endif

      return 0;
    }
    // se la rel orig dest non è presente viene inserita sotto
    // fuori dall'if
  }
  else{
    #ifdef DEBUG_ADDREL
      printf("\tRel Tree Da creare\n");
    #endif
    rel_tree = (t_rel_tree_node*) malloc(sizeof(t_rel_tree_node));
    // rel_tree = (t_rel_tree_node*) memalign(2, sizeof(t_rel_tree_node));
    rel_tree->id = (char*) calloc(strlen(addrel_rel_id) + 1, sizeof(char));
    strcpy(rel_tree->id, addrel_rel_id);
    // free(addrel_rel_id);
    rel_tree->color = 0;
    rel_tree->relations = (t_rel_head*) malloc(sizeof(t_rel_head));
    // rel_tree->relations = (t_rel_head*) memalign(2, sizeof(t_rel_head));
    rel_tree->relations->nil = &NIL_REL;
    rel_tree->relations->root = rel_tree->relations->nil;
    rel_tree->ranking = (t_rank_head*) malloc(sizeof(t_rank_head));
    // rel_tree->ranking = (t_rank_head*) memalign(2, sizeof(t_rank_head));
    rel_tree->ranking->nil = &NIL_RANK;
    rel_tree->ranking->root = rel_tree->ranking->nil;
    if(REL_TREE_INSERT(R, rel_tree)){
      #ifdef DEBUG_2
        printf("\tadded new REL TREE\n");
      #endif
    }
  }

  #ifdef DEBUG_ADDREL
    printf("-------Rank repo prima insert: ---------\n");
    if(rel_tree->ranking->root != rel_tree->ranking->nil)
      RANK_TREE_WALK(rel_tree->ranking, rel_tree->ranking->root);
    else
      printf("rank vuoto\n");
  #endif

  // vuol dire che non esiste nemmeno con v = 0
  // allochiamo il rel "che deve essere perforza presente"
  rel_node = (t_rel_node*) malloc(sizeof(t_rel_node));
  // rel_node = (t_rel_node*) memalign(2, sizeof(t_rel_node));

  // rel_node->id = (char*) calloc(strlen(addrel_orig_dest) + 1, sizeof(char));
  // strcpy(rel_node->id, addrel_orig_dest);
  rel_node->orig = orig;
  // rel_node->dest = dest;
  // rel_node->color = 0;
  // rel_node->rel_head = rel_tree->relations;

  // cercare se l'in relation è già monitorata
  // se si update
  //            elimina
  //            aggiorna contatore
  //            inserisci
  #ifdef DEBUG_ADDREL
    printf("\tDest id to rank: %s\n", dest->id);
  #endif
  // find_rank(rel_tree->ranking, rel_tree->ranking->root, &rank, dest->id);
  // find_rank(rel_tree->ranking, rel_tree->ranking->root, &rank_test, dest->id);
  rank = find_rank_v2(dest->in_rel, rel_tree);
  // if(rank == NULL){
  //   rank = find_rank_v2(rel_node->orig->out_rel, rel_tree);
  // }
  if(rank != NULL){
    // #ifdef TEST_1
    //   if(rank != rank_test){
    //       printf("found an other rank\n");
    //       printf("v2: %s :-: v1: %s\n", rank->id, rank_test->id);
    //   }
    // #endif

    #ifdef DEBUG_ADDREL
      printf("\tRanking da aggiornare\n");
    #endif
    #ifdef DEBUG_ADDREL
      printf("\tElimino: %s\n", rank->id);
    #endif

    RANK_DELETE(rel_tree->ranking, rank);

    #ifdef TEST_1
      if(deleted_rank != rank){
        printf("deleted an other rank\n");
      }
    #endif

    #ifdef DEBUG_ADDREL
      printf("\tRank eliminato\n");
    #endif

    rank->n = rank->n + 1;

    #ifdef DEBUG_ADDREL
      printf("\tRank incrementato\n");
      printf("\tRank n: %d - Rank id: %s\n", rank->n, rank->id);
    #endif

    RANK_INSERT(rel_tree->ranking, rank);

    #ifdef DEBUG_ADDREL
      printf("\tRank reinserito\n");
    #endif
    #ifdef DEBUG_ADDREL
      printf("\tRanking aggiornato\n");
    #endif
  }
  else{
    // INSERT NEW RANK
    #ifdef DEBUG_ADDREL
      printf("\tRank da inserire da capo\n");
      // RANK_TREE_WALK(rel_tree->ranking, rel_tree->ranking->root);
    #endif
    rank = (t_rank_node*) malloc(sizeof(t_rank_node));
    // rank = (t_rank_node*) memalign(2, sizeof(t_rank_node));
    // rank->id = (char*) calloc(strlen(addrel_dest) + 1, sizeof(char));
    // strcpy(rank->id, addrel_dest);
    rank->dest = dest;
    rank->n = 1;
    rank->rel_tree_pointer = rel_tree;
    RANK_INSERT(rel_tree->ranking, rank);
    #ifdef DEBUG_ADDREL
      printf("\tRank inserito da capo\n");
      // RANK_TREE_WALK(rel_tree->ranking, rel_tree->ranking->root);
    #endif
  }

  // inseriamo il collegamento da ent al nodo rank
  rel_node->rank_pointer = rank; // puntatore al nodo rank
  // rank->rank_h_p = rel_tree->ranking; // inseriamo la head del rank (serve per il delete)

  #ifdef DEBUG_ADDREL
    printf("-------Rank repo dopo insert: ---------\n");
    RANK_TREE_WALK(rel_tree->ranking, rel_tree->ranking->root);
  #endif

  out = (t_rel_list*) malloc(sizeof(t_rel_list));
  // out->rel_head = rel_tree->relations;
  out->rel = rel_node;
  in = (t_rel_list*) malloc(sizeof(t_rel_list));
  // in->rel_head = rel_tree->relations;
  in->rel = rel_node;

  ADD_TO_LIST(&(orig->out_rel), out);
  rel_node->orig_rel_list_pointer = out;

  ADD_TO_LIST(&(dest->in_rel), in);
  rel_node->dest_rel_list_pointer = in;

  if(REL_INSERT(rel_tree->relations, rel_node)){
    #ifdef DEBUG_2
      printf("\tadded REL\n");
    #endif
  }

  return 1;
}

// FUNZIONE PRINCIPALE COMANDO "delrel"
int delrel(t_rel_tree_head *R){
  char input[MAX_INPUT];
  char delrel_orig[MAX_ENT_ID];
  char delrel_dest[MAX_ENT_ID];
  char delrel_id[MAX_REL_ID];
  // char delrel_orig_dest[MAX_ENT_ID+MAX_ENT_ID+1];
  char *r;

  t_rel_tree_node *rel_tree;
  t_rel_node *rel;

  // t_rel_tree_node *deleted_rel_tree;
  // t_rel_node *deleted_rel;
  // t_rank_node *deleted_rank;

  // t_rel_list *deleted_list;

  // int hash_rt_v;

  #ifdef DEBUG_DELREL
    printf("Inizio eliminazione relazione\n");
  #endif

  // if(retrive_addrel_delrel_args(comando, delrel_orig, delrel_dest, delrel_id) != 1){
  //   #ifdef DEBUG_DELREL
  //     printf("Errore nella retrive\n");
  //   #endif
  //   return 0;
  // }
  // #ifdef DEBUG_DELREL
  //   printf("Presi i dati in input\n");
  // #endif


  r = fgets(input, MAX_INPUT, stdin);
  retrive_addrel_delrel_args(input, delrel_orig, delrel_dest, delrel_id);

  // fgets(input, MAX_INPUT, stdin);
  // sscanf(input," \"%49[^\"]\" \"%49[^\"]\" \"%49[^\"]\"", delrel_orig, delrel_dest, delrel_id);

  // strcpy(delrel_orig_dest, delrel_orig);
  // strcat(delrel_orig_dest, l);
  // strcat(delrel_orig_dest, delrel_dest);
  // printf("delrel_orig_dest: %s\n", delrel_orig_dest);

  if(find_rel_tree(R, R->root, &rel_tree, delrel_id) == 0){
    #ifdef DEBUG_DELREL
      printf("Relazione in Rel Tree non trovata\n");
    #endif
    return 0;
  }
  if(find_rel(rel_tree->relations, rel_tree->relations->root, &rel, delrel_orig, delrel_dest) == 0){
    #ifdef DEBUG_DELREL
      printf("Relazione in Relations non trovata\n");
    #endif
    return 0;
  }

  // deleted_rel = REL_DELETE(rel_tree->relations, rel);
  REL_DELETE(rel_tree->relations, rel);

  #ifdef TEST_1
    if(deleted_rel != rel){
      printf("deleted an other rel\n");
    }
  #endif

  #ifdef DEBUG_DELREL
    printf("Eliminata relazione\n");
  #endif

  // if(deleted_rel != rel){
  //   #ifdef DEBUG
  //   printf("Deleted an other rel\n");
  //   #endif
  // }

  // sistemo le liste.
  // per evitare conflitti
  // deleted_list = REMOVE_FROM_LIST(&(rel->orig->out_rel), rel->orig_rel_list_pointer);
  // if(deleted_list != NULL)
  //   free(deleted_list);
  REMOVE_FROM_LIST(&(rel->orig->out_rel), rel->orig_rel_list_pointer);
  free(rel->orig_rel_list_pointer);

  // deleted_list = REMOVE_FROM_LIST(&(rel->rank_pointer->dest->in_rel), rel->dest_rel_list_pointer);
  // if(deleted_list != NULL)
  //   free(deleted_list);
  REMOVE_FROM_LIST(&(rel->rank_pointer->dest->in_rel), rel->dest_rel_list_pointer);
  free(rel->dest_rel_list_pointer);

  #ifndef DISABLE_FREE_REL_LIST_DELREL
  #endif

  if((rel->rank_pointer->n - 1) == 0){
    #ifdef DEBUG_DELREL
      printf("Voglio eliminare il nodo rank che è a 0\n");
    #endif

    // deleted_rank = RANK_DELETE(rel->rank_pointer->rel_tree_pointer->ranking, rel->rank_pointer);
    RANK_DELETE(rel->rank_pointer->rel_tree_pointer->ranking, rel->rank_pointer);

    #ifdef TEST_1
      if(deleted_rank != rel->rank_pointer){
        printf("deleted an other rank\n");
      }
    #endif

    // rel->rank_pointer->n = rel->rank_pointer->n - 1;

    #ifndef DISABLE_FREE_RANK_NODE
      // free(rel->rank_pointer->id);
      free(rel->rank_pointer);
    #endif
  }
  else{
    // UPDATE RANK
    #ifdef DEBUG_DELREL
      printf("Voglio aggiornare il nodo rank\n");
    #endif

    // deleted_rank = RANK_DELETE(rel->rank_pointer->rel_tree_pointer->ranking, rel->rank_pointer);
    RANK_DELETE(rel->rank_pointer->rel_tree_pointer->ranking, rel->rank_pointer);

    #ifdef TEST_1
      if(deleted_rank != rel->rank_pointer){
        printf("deleted an other rank\n");
      }
    #endif

    rel->rank_pointer->n = rel->rank_pointer->n - 1;

    #ifdef DEBUG_DELREL
      printf("Rank eliminato (per update)\n");
    #endif

    RANK_INSERT(rel->rank_pointer->rel_tree_pointer->ranking, rel->rank_pointer);

    #ifdef DEBUG_DELREL
      printf("Rank reinserito (per update)\n");
    #endif
  }

  // vuol dire che mi sono rimaste zero relazioni
  if(rel_tree->relations->root == rel_tree->relations->nil){
    // deleted_rel_tree = REL_TREE_DELETE(R, rel_tree);
    REL_TREE_DELETE(R, rel_tree);

    #ifdef TEST_1
      if(deleted_rel_tree != rel_tree){
        printf("deleted an other rel tree 1\n");
      }
    #endif

    free(rel_tree->id);
    free(rel_tree->relations);
    free(rel_tree->ranking);
    free(rel_tree);
  }

  #ifndef DISABLE_FREE_REL_NODE_DELREL
    // free(deleted_rel->id);
    // free(deleted_rel);
    free(rel);
  #endif

  return 1;
}

#ifdef ITERATIVE_REPORT
// ITERATIVE
void REPORT_RANK_TREE_WALK(t_rank_head *T, t_rank_node *x, int n){
  t_rank_node *prev, *max;
  while (x != T->nil && x->n == n){
    prev = x;
    x = RANK_TREE_PREDECESSOR(T, x);
  }
  x = prev;
  while (x != T->nil && x->n == n) {
    fputs("\"", stdout);
    fputs(x->dest->id, stdout);
    fputs("\" ", stdout);
    x = RANK_TREE_SUCCESSOR(T, x);
  }
}
#endif

#ifndef ITERATIVE_REPORT
// RECURSIVE
void REPORT_RANK_TREE_WALK(t_rank_head *T, t_rank_node *x, int n){
  if(x != T->nil && x->n == n){
    REPORT_RANK_TREE_WALK(T, RANK_TREE_PREDECESSOR(T, x), n);
    fputs("\"", stdout);
    fputs(x->dest->id, stdout);
    fputs("\" ", stdout);
    // printf("\"%s\" ", x->id);
  }
}
#endif


int no_of_digits(int num)
{
    int digit_count = 0;

    while(num > 0)
    {
        digit_count++;
        num /= 10;
    }

    return digit_count;
}

char *i_to_a(int num)
{
    char *str;
    int digit_count = 0;

    if(num < 0)
    {
        num = -1*num;
        digit_count++;
    }

    digit_count += no_of_digits(num);
    str = (char*) malloc(sizeof(char)*(digit_count+1));

    str[digit_count] = '\0';

    while(num > 0)
    {
        str[digit_count-1] = num%10 + '0';
        num = num/10;
        digit_count--;
    }

    if(digit_count == 1)
        str[0] = '-';

    return str;
}


// FUNZIONE SECONDARIA COMANDO "report"
int REPORT_RANK(t_rank_head *T, t_rank_node *x){
  // t_rank_node *y;
  int n;
  char *str;

  // andiamo al massimo
  #ifdef DEBUG_REPORT
    printf(" -- andiamo al max --\n");
  #endif
  #ifdef DEBUG_REPORT
    printf("- pp- %s\n -/pp-", x->id);
  #endif

  // cerco il max
  while (x->right != T->nil) {
    x = x->right;
  }

  n = x->n;

  #ifdef DEBUG_REPORT
    printf(" -- trovato il max --\n");
  #endif

  REPORT_RANK_TREE_WALK(T, x, n); // viene fatta una tree walk completa

  str = i_to_a(n);
  fputs(str, stdout);
  // printf("%d", n);
  fputs(";", stdout);
  free(str);
  // printf("%d;", n);
  return 1;
}

#ifdef ITERATIVE_REPORT
// ITERATIVE
void REPORT_WALK_REL_TREE(t_rel_tree_head *T, t_rel_tree_node *x){
  t_rel_tree_node *current, *pre;

  if(T->root == T->nil)
    return;

  // while (x != T->nil && x->n == n) {
  //   current = x;
  //   x = RANK_TREE_PREDECESSOR(T, x);
  // }

  current = T->root;

  while (current != T->nil) {
    if(current->left == T->nil){
      x = current;
      if(x->ranking->root != x->ranking->nil && x->relations->root != x->relations->nil){
        vuoto = 0;
        // printed_2 = 1;
        if(printed_1 == 0) fputs("\"", stdout);
        else fputs(" \"", stdout);
        fputs(x->id, stdout);
        fputs("\" ", stdout);
        // printf("\"%s\" ", x->id); // print ent name

        REPORT_RANK(x->ranking, x->ranking->root); // print rank

        printed_1 = 1;
      }
      current = current->right;
    }
    else{
      pre = current->left;
      while (pre->right != T->nil && pre->right != current)
        pre = pre->right;

      // if(pre->right == current){
      //   pre->right = T->nil;
      //   // fputs("\"", stdout);
      //   // fputs(current->dest->id, stdout);
      //   // fputs("\" ", stdout);
      //   current = current->right;
      // }
      // else{
      //   pre->right = current;
      //   // fputs("\"", stdout);
      //   // fputs(current->dest->id, stdout);
      //   // fputs("\" ", stdout);
      //   current = current->left;
      // }
      if(pre->right == T->nil){
        pre->right = current;
        current = current->left;
      }
      else{
        pre->right = T->nil;
        x = current;
        if(x->ranking->root != x->ranking->nil && x->relations->root != x->relations->nil){
          vuoto = 0;
          // printed_2 = 1;
          if(printed_1 == 0) fputs("\"", stdout);
          else fputs(" \"", stdout);
          fputs(x->id, stdout);
          fputs("\" ", stdout);
          // printf("\"%s\" ", x->id); // print ent name
          REPORT_RANK(x->ranking, x->ranking->root); // print rank

          printed_1 = 1;
        }
        current = current->right;
      }
    }
  }
}
#endif

#ifndef ITERATIVE_REPORT
// RECURSIVE
// FUNZIONE SECONDARIA COMANDO "report"
void REPORT_WALK_REL_TREE(t_rel_tree_head *T, t_rel_tree_node *x){
  if(x != T->nil){
    REPORT_WALK_REL_TREE(T, x->left);

    // faccio la report solo se ranking non è vuoto
    if(x->ranking->root != x->ranking->nil && x->relations->root != x->relations->nil){
      vuoto = 0;
      // printed_2 = 1;
      if(printed_1 == 0) fputs("\"", stdout);
      else fputs(" \"", stdout);
      fputs(x->id, stdout);
      fputs("\" ", stdout);
      // printf("\"%s\" ", x->id); // print ent name

      REPORT_RANK(x->ranking, x->ranking->root); // print rank

      printed_1 = 1;
    }

    REPORT_WALK_REL_TREE(T, x->right);
  }
}
#endif


// FUNZIONE PRINCIPALE COMANDO "report"
int report(t_rel_tree_head *R){
  char *r;

  // #######################
  // praticamente probabilemnte c'è uno \n o \0 alla fine che bisona leggere.
  //    perchè il gets del main non lo legge
  char input[MAX_INPUT];
  r = fgets(input, MAX_INPUT, stdin);
  // #######################

  #ifdef DISABLE_REPORT
    return 1;
  #endif

  vuoto = 1; // variabile globale
  printed_1 = 0; // variabile globale

  REPORT_WALK_REL_TREE(R, R->root);

  // vuoto rimane 0 se non ho trovato niente da stampare
  if(vuoto == 1){
    fputs("none", stdout);
    // printf("none");
  }

  fputs("\n", stdout);
  // printf("\n");

  return 1;
}


int main(){
  char comando[MAX_COMANDO + 1]; // 6 + 1 (\0)
  int cmd;
  char *r;

  // array delle entità
  // t_ent_head *T_ent = init_hash_ent();
  // t_rel_tree_head *T_rel_tree = init_hash_rel_tree();

  t_ent_head *T_ent; //head delle entità
  t_rel_tree_head *T_rel_tree; //head delle relazioni

  // init entity tree
  T_ent = (t_ent_head*) malloc(sizeof(t_ent_head));
  T_ent->nil = &NIL_ENT;
  T_ent->root = T_ent->nil;

  // init relations tree
  T_rel_tree = (t_rel_tree_head*) malloc(sizeof(t_rel_tree_head));
  T_rel_tree->nil = &NIL_REL_TREE;
  T_rel_tree->root = T_rel_tree->nil;

  // RICEZIONE PRIMO COMANDO
  r = fgets(comando, (MAX_COMANDO + 1), stdin);
  // scanf("%s", comando);
  cmd = getcmd(comando);

  // FINO A CHE NON SCRIVE "end" RESTIAMO NEL CICLO
  while(cmd != 5){
    if(cmd == 0){ // addent
      addent(T_ent);
    }
    else if(cmd == 1){ // delent
      delent(T_ent, T_rel_tree);
    }
    else if(cmd == 2){ // addrel
      addrel(T_ent, T_rel_tree);
    }
    else if(cmd == 3){ // delrel
      // delrel(&T_ent, &T_rel_tree, comando);
      delrel(T_rel_tree);
    }
    else if(cmd == 4){ // report
      report(T_rel_tree);
      #ifdef DEBUG
        printf("Ent list: ");
        ENT_TREE_WALK(&T_ent, T_ent.root);
        printf("\n");
        printf("Rel list: ");
        REL_TREE_WALK(&T_rel_tree, T_rel.root);
        printf("\n");
      #endif
    }
    else{
      // ESCO
    }

    // RICEZIONE COMANDI SUCCESSIVI AL PRIMO
    r = fgets(comando, (MAX_COMANDO + 1), stdin);
    // scanf("%s", comando);
    cmd = getcmd(comando);
  }

  // free(T_ent);
  // free(T_rel_tree);

  return 0;
}
