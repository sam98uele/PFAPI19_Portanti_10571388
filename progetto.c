#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 200
#define MAX_ENT_ID 50
#define MAX_REL_ID 50

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

// #define DISABLE_ADD 0

// #define DISABLE_FREE 0

//#define NO_FIND 0

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

struct ent_node{
  char id[MAX_ENT_ID];
  int v; //valid bit = 0->deleted , 1->alive
  t_rel_list *in_rel; // list of relations in
  t_rel_list *out_rel; // list of relations out
  int color; // 0 -> black - 1 -> red
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
struct rel_list{
  t_rel_head *rel_head;
  t_rel_node *rel;
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

struct rel_tree_node{
  char id[MAX_REL_ID];
  int v; //valid bit = 0->deleted , 1->alive
  t_rel_head *relations;
  t_rank_head *ranking;
  int color; // 0 -> black - 1 -> red
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

struct rel_node{
  // t_rel_tree_head rel; //head of rel_tree_node
  // char id[MAX_REL_ID];
  char id[MAX_ENT_ID+MAX_ENT_ID]; //id made by "from_ent"+"to_ent" !UNIQUE!
  int v; //valid bit = 0->deleted , 1->alive
  t_ent_node *orig;
  t_ent_node *dest;
  t_rank_head *rank_h_p;
  t_rank_node *rank_pointer;
  int color; // 0 -> black - 1 -> red
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

struct rank_node{
  // t_rel_tree_head rel; //head of rel_tree_node
  // char id[MAX_REL_ID];
  int n; // number of entrance rels
  char id[MAX_ENT_ID]; //id made by "to_ent" !UNIQUE! (relazioni entranti)
  t_rel_tree_node *rel_tree_pointer;
  int v; //valid bit = 0->deleted , 1->alive
  int color; // 0 -> black - 1 -> red
  t_rank_node *p;
  t_rank_node *left;
  t_rank_node *right;
};
// ##################
// END RANKING TREE
// ##################

typedef struct ent_ranking_node{
  int id;
  int code;
  int ranking;
} t_ent_ranking_node;

// START Global Vars
t_ent_node NIL_ENT = {.id = "NIL_END", .color = 0, .v = 1, .in_rel = NULL, .out_rel = NULL, .p = NULL, .left = NULL, .right = NULL};
t_rel_tree_node NIL_REL_TREE = {.id = "NIL_REL_TREE", .color = 0, .v = 1, .ranking = NULL, .relations = NULL, .p = NULL, .left = NULL, .right = NULL};
t_rel_node NIL_REL = {.id = "NIL_REL", .v = 1, .color = 0, .orig = NULL, .dest = NULL, .p = NULL, .left = NULL, .right = NULL};
t_rank_node NIL_RANK = {.n = 0, .id = "NIL_RANK", .color = 0, .v = 1, .p = NULL, .left = NULL, .right = NULL};
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
  if(id_1[i] == '\0' && id_2[i] != '\0')
    return 0;
  else if(id_1[i] != '\0' && id_2[i] == '\0')
    return 1;
  return 0; // equal. this is only >!
}

// This function is like id_1 < id_2
// It compares ASCII
// Return values:
// 1 -> id_1 < id_2
// 0 -> altrimenti
int IS_LOWER_ID(char *id_1, char *id_2){
  int i = 0;
  while (id_1[i] != '\0' && id_2[i] != '\0') {
    if(id_1[i] < id_2[i]) return 1;
    else if(id_1[i] == id_2[i]) i++;
    else return 0;
  }
  // se uno è il prefisso dell'altro
  if(id_1[i] == '\0' && id_2[i] != '\0')
    return 1;
  else if(id_1[i] != '\0' && id_2[i] == '\0')
    return 0;
  return 0; // equal. this is only >!
}

// This function is like id_1 < id_2 ?
// First it compares n1 and n2
// Than it compares ASCII
// Return values:
// 1 -> id_1 < id_2
// 0 -> altrimenti
int IS_LOWER_ID_RANK(int n1, char *id_1, int n2, char *id_2){
  int i = 0;
  if(n1 == n2){ // se gli n sono uguali confrontiamo le stringhe
    while (id_1[i] != '\0' && id_2[i] != '\0') {
      if(id_1[i] < id_2[i]) return 1;
      else if(id_1[i] == id_2[i]) i++;
      else return 0;
    }
    if(id_1[i] == '\0' && id_2[i] != '\0')
      return 1;
    else if(id_1[i] != '\0' && id_2[i] == '\0')
      return 0;
  }
  else if(n1 < n2){
    return 1;
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

  return 1;
}

int ENT_REL_WALK(t_rel_list *x){
  // printf("a\n");
  if(x != NULL){
    if(x->rel->v == 1){
      printf("%s ", x->rel->id);
    }
    ENT_REL_WALK(x->next);
  }
}

int ENT_TREE_WALK(t_ent_head *T, t_ent_node *x){
  // printf("a\n");
  if(x != T->nil){
    ENT_TREE_WALK(T, x->left);
    printf("%s - ", x->id);
    // printf("Rel in: ");
    // ENT_REL_WALK(x->in_rel);
    // printf("\n");
    // printf("Rel out: ");
    // ENT_REL_WALK(x->out_rel);
    // printf("\n");
    ENT_TREE_WALK(T, x->right);
  }
}


int ENT_RIGHT_ROTATE(t_ent_head *T, t_ent_node *x){
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

  return 1;
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
  #ifdef DEBUG_ADDENT
    printf("!!Tree prima fixup insert: \n");
    ENT_TREE_WALK(T, T->root);
    printf("\n");
  #endif
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
  t_ent_node temp; //temporaneo per salvare i dati durante lo scambio

  t_ent_node *x;
  t_ent_node *y;

  #ifdef DEBUG_ENT_DELETE
    printf("Deletion of ent: %s\n", z->id);
  #endif

  if(z->left == T->nil || z->right == T->nil)
    y = z;
  else
    y = ENT_TREE_SUCCESSOR(T, z);
  if(y->left != T->nil)
    x = y->left;
  else
    x = y->right;

  // START NEWW
  color = y->color; // salviamo il colore di y

  // prima di eliminare il nodo, scambiamo y con z (se bisogna farlo)
  //    serve per evitare conflitti
  if(y != z){
    temp.color = y->color;
    temp.p = y->p;
    temp.left = y->left;
    temp.right = y->right;

    // metto y al posto di z
    y->color = z->color;
    if(z->p != y)
      y->p = z->p;
    else
      y->p = z;
    if(z->left != y)
      y->left = z->left;
    else
      y->left = z;
    if(z->right != y)
      y->right = z->right;
    else
      y->right = z;

    if(y->p == T->nil)
      T->root = y;
    else if(z == y->p->left)
      y->p->left = y;
    else
      y->p->right = y;

    if(y->left != T->nil && y->left != y)
      y->left->p = y;
    else if(y->left != T->nil && y->left == y)
      z->p = y;
    if(y->right != T->nil && y->right != y)
      y->right->p = y;
    else if(y->right != T->nil && y->right == y)
      z->p = y;


    // metto z al posto di y
    z->color = temp.color;
    if(temp.p != z){
      z->p = temp.p;
    }
    else{
      z->p = y;
    }
    z->left = temp.left;
    z->right = temp.right;
    if(z->p == T->nil)
      T->root = z;
    else if(y == z->p->left && z->p->left != z)
      z->p->left = z;
    else if(y == z->p->left && z->p->left == z)
      y->left = z;
    else if(y == z->p->right && z->p->right != z)
      z->p->right = z;
    else if(y == z->p->right && z->p->right == z)
      y->right = z;

    if(z->left != T->nil)
      z->left->p = z;
    if(z->right != T->nil)
      z->right->p = z;

    y = z;  // ora quello vero da eliminare torna ad essere z!
              // che ha preso il posto di y (e viceversa)
  }
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


  if(y->color == 0)
    ENT_DELETE_FIXUP(T, x);
  return y; //return the deleted node
}
// ###############################
// ##### ENT OPERATIONS ##########
// #####       END      ##########
// ###############################


// ###############################
// ##### REL OPERATIONS ##########
// #####      START     ##########
// ###############################
int REL_LEFT_ROTATE(t_rel_head *T, t_rel_node *x){
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

  return 1;
}

int REL_RIGHT_ROTATE(t_rel_head *T, t_rel_node *x){
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
    // printf("aaaa1\n");
    if(w->color == 1){
      // printf("aaaa2\n");
      w->color = 0;
      x->p->color = 1;
      REL_RIGHT_ROTATE(T, x->p);
      w = x->p->left;
    }
    if(w->right->color == 0 && w->left->color == 0){
      w->color = 1;
      // printf("aaaa3\n");
      REL_DELETE_FIXUP(T, x->p);
    }
    else{
      // printf("aaaa4\n");
      if(w->left->color == 0){
        // printf("aaaa5\n");
        w->right->color = 0;
        // printf("aaaa6\n");
        w->color = 1;
        REL_LEFT_ROTATE(T, w);
        w = x->p->left;
      }
      // printf("aaaa7\n");
      w->color = x->p->color;
      x->p->color = 0;
      w->left->color = 0;
      REL_RIGHT_ROTATE(T, x->p);
      // printf("aaaa8\n");
    }
  }
  // printf("aaaa9\n");
  return 1;
}

t_rel_node *REL_DELETE(t_rel_head *T, t_rel_node *z){
  int color;
  t_rel_node temp; //temporaneo per salvare i dati durante lo scambio

  t_rel_node *x;
  t_rel_node *y;

  #ifdef DEBUG_REL_DELETE
    printf("Deletion of rel: %s\n", z->id);
  #endif

  if(z->left == T->nil || z->right == T->nil)
    y = z;
  else
    y = REL_TREE_SUCCESSOR(T, z);
  if(y->left != T->nil)
    x = y->left;
  else
    x = y->right;

  // START NEWW
  color = y->color; // salviamo il colore di y

  // prima di eliminare il nodo, scambiamo y con z (se bisogna farlo)
  //    serve per evitare conflitti
  if(y != z){
    temp.color = y->color;
    temp.p = y->p;
    temp.left = y->left;
    temp.right = y->right;

    // metto y al posto di z
    y->color = z->color;
    if(z->p != y)
      y->p = z->p;
    else
      y->p = z;
    if(z->left != y)
      y->left = z->left;
    else
      y->left = z;
    if(z->right != y)
      y->right = z->right;
    else
      y->right = z;

    if(y->p == T->nil)
      T->root = y;
    else if(z == y->p->left)
      y->p->left = y;
    else
      y->p->right = y;

    if(y->left != T->nil && y->left != y)
      y->left->p = y;
    else if(y->left != T->nil && y->left == y)
      z->p = y;
    if(y->right != T->nil && y->right != y)
      y->right->p = y;
    else if(y->right != T->nil && y->right == y)
      z->p = y;


    // metto z al posto di y
    z->color = temp.color;
    if(temp.p != z){
      z->p = temp.p;
    }
    else{
      z->p = y;
    }
    z->left = temp.left;
    z->right = temp.right;
    if(z->p == T->nil)
      T->root = z;
    else if(y == z->p->left && z->p->left != z)
      z->p->left = z;
    else if(y == z->p->left && z->p->left == z)
      y->left = z;
    else if(y == z->p->right && z->p->right != z)
      z->p->right = z;
    else if(y == z->p->right && z->p->right == z)
      y->right = z;

    if(z->left != T->nil)
      z->left->p = z;
    if(z->right != T->nil)
      z->right->p = z;

    y = z;  // ora quello vero da eliminare torna ad essere z!
              // che ha preso il posto di y (e viceversa)
  }
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
  // printf("aaaa10\n");
  return y; //return the deleted node
}

// ###############################
// ##### REL OPERATIONS ##########
// #####       END      ##########
// ###############################


// ####################################
// ##### REL TREE OPERATIONS ##########
// #####         START       ##########
// ####################################
int REL_TREE_LEFT_ROTATE(t_rel_tree_head *T, t_rel_tree_node *x){
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

  return 1;
}

int REL_TREE_RIGHT_ROTATE(t_rel_tree_head *T, t_rel_tree_node *x){
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

  return 1;
}

int REL_TREE_TREE_WALK(t_rel_tree_head *T, t_rel_tree_node *x){
  // printf("a\n");
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

int REL_TREE_INSERT_FIXUP(t_rel_tree_head *T, t_rel_tree_node *z){
  t_rel_tree_node *x;
  t_rel_tree_node *y;
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
      return 1;
    }
  }
  return 1;
}

int REL_TREE_INSERT(t_rel_tree_head *T, t_rel_tree_node *z){
  t_rel_tree_node *y = T->nil;
  t_rel_tree_node *x = T->root;
  while(x != T->nil){
    y = x;
    if(IS_LOWER_ID(z->id, x->id)) //OK
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
  REL_TREE_INSERT_FIXUP(T, z);
  return 1;
}

int REL_TREE_DELETE_FIXUP(t_rel_tree_head *T, t_rel_tree_node *x){
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
  t_rel_tree_node *x;
  t_rel_tree_node *y;
  if(z->left == T->nil || z->right == T->nil)
    y = z;
  else
    y = REL_TREE_TREE_SUCCESSOR(T, z);
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
  if(y != z){
    strcpy(z->id, y->id);
    z->v = y->v;
    z->ranking = y->ranking;
    z->relations = y->relations;
  }
  if(y->color == 0)
    REL_TREE_DELETE_FIXUP(T, x);
  return y; //return the deleted node
}

// ####################################
// ##### REL TREE OPERATIONS ##########
// #####          END        ##########
// ####################################


// ####################################
// ###### RANKING OPERATIONS ##########
// ######       START        ##########
// ####################################
int RANK_LEFT_ROTATE(t_rank_head *T, t_rank_node *x){
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

  return 1;
}

int RANK_RIGHT_ROTATE(t_rank_head *T, t_rank_node *x){
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

  return 1;
}

void RANK_TREE_WALK(t_rank_head *T, t_rank_node *x){
  // printf("a\n");
  if(x != T->nil){
    RANK_TREE_WALK(T, x->left);
    printf("\t\tid: \"%s\" n: \"%d\" color: %d", x->id, x->n, x->color);
    if(x->p == T->nil) {
      printf(" !radice!");
    }
    printf(" - l: %s", x->left->id);
    printf(" - r: %s", x->right->id);
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

int RANK_INSERT_FIXUP(t_rank_head *T, t_rank_node *z){
  t_rank_node *x;
  t_rank_node *y;
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

int RANK_INSERT(t_rank_head *T, t_rank_node *z){
  t_rank_node *y = T->nil;
  t_rank_node *x = T->root;
  while(x != T->nil){
    y = x;
    if(IS_LOWER_ID_RANK(z->n, z->id, x->n, x->id)){
      x = x->left;
    }
    else{
      x = x->right;
    }
  }
  z->p = y;
  if(y == T->nil)
    T->root = z;
  else if(IS_LOWER_ID_RANK(z->n, z->id, y->n, y->id)){
    y->left = z;
  }
  else{
    y->right = z;
  }
  z->left = T->nil;
  z->right = T->nil;
  z->color = 1;
  RANK_INSERT_FIXUP(T, z);
  return 1;
}

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
  t_rank_node temp; //temporaneo per salvare i dati durante lo scambio

  t_rank_node *x;
  t_rank_node *y;

  if(z->left == T->nil || z->right == T->nil){
    y = z;
    #ifdef DEBUG_3
      printf("dir\n");
    #endif
  }
  else{
    y = RANK_TREE_SUCCESSOR(T, z);
  }
  if(y->left != T->nil)
    x = y->left;
  else{
    x = y->right;
  }

  color = y->color; // salviamo il colore di y

  // prima di eliminare il nodo, scambiamo y con z (se bisogna farlo)
  //    serve per evitare conflitti
  if(y != z){
    temp.color = y->color;
    temp.p = y->p;
    temp.left = y->left;
    temp.right = y->right;

    // metto y al posto di z
    y->color = z->color;
    if(z->p != y)
      y->p = z->p;
    else
      y->p = z;
    if(z->left != y)
      y->left = z->left;
    else
      y->left = z;
    if(z->right != y)
      y->right = z->right;
    else
      y->right = z;

    if(y->p == T->nil)
      T->root = y;
    else if(z == y->p->left)
      y->p->left = y;
    else
      y->p->right = y;

    if(y->left != T->nil && y->left != y)
      y->left->p = y;
    else if(y->left != T->nil && y->left == y)
      z->p = y;
    if(y->right != T->nil && y->right != y)
      y->right->p = y;
    else if(y->right != T->nil && y->right == y)
      z->p = y;


    // metto z al posto di y
    z->color = temp.color;
    if(temp.p != z){
      z->p = temp.p;
    }
    else{
      z->p = y;
    }
    z->left = temp.left;
    z->right = temp.right;
    if(z->p == T->nil)
      T->root = z;
    else if(y == z->p->left && z->p->left != z)
      z->p->left = z;
    else if(y == z->p->left && z->p->left == z)
      y->left = z;
    else if(y == z->p->right && z->p->right != z)
      z->p->right = z;
    else if(y == z->p->right && z->p->right == z)
      y->right = z;

    if(z->left != T->nil)
      z->left->p = z;
    if(z->right != T->nil)
      z->right->p = z;

    y = z;  // ora quello vero da eliminare torna ad essere z!
              // che ha preso il posto di y (e viceversa)
  }
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
  return z; //return the deleted node
}

// ################################
// ##### RANK OPERATIONS ##########
// #####      END        ##########
// ################################



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
  // printf("end get retrive_addrel_delrel_args\n");
  return 1;
}

// int check_existence_ent(t_ent_head *T, t_ent_node *x, char *id, int i){
//   if(x == T->nil){
//     // printf("aa\n");
//     return 0;
//   }
//   else if(id[i] == x->id[i] && id[i+1] == '\0' && x->id[i+1] == '\0'){
//     return 1;
//   }
//   else if(id[i] == x->id[i] && id[i+1] != '\0' && x->id[i+1] != '\0'){
//     return check_existence_ent(T, x, id, i+1);
//   }
//   else if(id[i] != '\0' && x->id[i] == '\0')
//     return 0;
//   else if(id[i] == '\0' && x->id[i] != '\0')
//     return 0;
//   else if(id[i] < x->id[i]){
//     return check_existence_ent(T, (x->left), id, i);
//   }
//   else{
//     return check_existence_ent(T, (x->right), id, i);
//   }
// }

int check_existence_ent(t_ent_head *T, t_ent_node *x, char *id, int i){
  if(x == T->nil){
    return 0;
  }
  else if(strcmp(id, x->id) == 0){
    return 1;
  }
  if(IS_LOWER_ID(id, x->id))
    return check_existence_ent(T, (x->left), id, i);
  else
    return check_existence_ent(T, (x->right), id, i);
}

// int find_ent_old(t_ent_head *T, t_ent_node *x, t_ent_node **z, char *id, int i){
//   if(x == T->nil){
//     return 0; //ent not found
//   }
//   else if(id[i] == x->id[i] && id[i+1] == '\0' && x->id[i+1] == '\0'){
//     *z = x;
//     return 1;
//   }
//   else if(id[i] == x->id[i] && id[i+1] != '\0' && x->id[i+1] != '\0'){
//     return find_ent(T, x, z, id, i+1);
//   }
//   else if(id[i] != '\0' && x->id[i] == '\0')
//     return 0;
//   else if(id[i] == '\0' && x->id[i] != '\0')
//     return 0;
//   else if(id[i] < x->id[i]){
//     return find_ent(T, (x->left), z, id, i);
//   }
//   else{
//     return find_ent(T, (x->right), z, id, i);
//   }
// }

int find_ent(t_ent_head *T, t_ent_node *x, t_ent_node **z, char *id, int i){
  if(x == T->nil){ // not found
    // printf("NF\n");
    *z = x;
    return 0;
  }
  else if(strcmp(id, x->id) == 0){ // found
    // printf("F\n");
    *z = x;
    return 1;
  }
  if(IS_LOWER_ID(id, x->id))
    return find_ent(T, (x->left), z, id, i);
  else
    return find_ent(T, (x->right), z, id, i);
}

// int find_rel(t_rel_head *T, t_rel_node *x, t_rel_node **z, char *id, int i){
//   if(x == T->nil){
//     return 0; //ent not found
//   }
//   else if(id[i] == x->id[i] && id[i+1] == '\0' && x->id[i+1] == '\0'){
//     *z = x;
//     if(x->v == 1){
//       return 1;
//     }
//     else{
//       return 0;
//     }
//   }
//   else if(id[i] == x->id[i] && id[i+1] != '\0' && x->id[i+1] != '\0'){
//     return find_rel(T, x, z, id, i+1);
//   }
//   else if(id[i] != '\0' && x->id[i] == '\0')
//     return 0;
//   else if(id[i] == '\0' && x->id[i] != '\0')
//     return 0;
//   else if(id[i] < x->id[i]){
//     return find_rel(T, (x->left), z, id, i);
//   }
//   else{
//     return find_rel(T, (x->right), z, id, i);
//   }
// }

int find_rel(t_rel_head *T, t_rel_node *x, t_rel_node **z, char *id, int i){
  if(x == T->nil){
    *z = x;
    return 0;
  }
  else if(strcmp(id, x->id) == 0){
    *z = x;
    return 1;
  }
  if(IS_LOWER_ID(id, x->id))
    return find_rel(T, (x->left), z, id, i);
  else
    return find_rel(T, (x->right), z, id, i);
}

// int find_rel_tree(t_rel_tree_head *T, t_rel_tree_node *x, t_rel_tree_node **z, char *id, int i){
//   if(x == T->nil){
//     return 0; //ent not found
//   }
//   else if(id[i] == x->id[i] && id[i+1] == '\0' && x->id[i+1] == '\0'){
//     *z = x;
//     return 1;
//   }
//   else if(id[i] == x->id[i] && id[i+1] != '\0' && x->id[i+1] != '\0'){
//     return find_rel_tree(T, x, z, id, i+1);
//   }
//   else if(id[i] != '\0' && x->id[i] == '\0')
//     return 0;
//   else if(id[i] == '\0' && x->id[i] != '\0')
//     return 0;
//   else if(id[i] < x->id[i]){
//     return find_rel_tree(T, (x->left), z, id, i);
//   }
//   else{
//     return find_rel_tree(T, (x->right), z, id, i);
//   }
// }

int find_rel_tree(t_rel_tree_head *T, t_rel_tree_node *x, t_rel_tree_node **z, char *id, int i){
  if(x == T->nil){
    *z = x;
    return 0;
  }
  else if(strcmp(id, x->id) == 0){
    *z = x;
    return 1;
  }
  if(IS_LOWER_ID(id, x->id))
    return find_rel_tree(T, (x->left), z, id, i);
  else
    return find_rel_tree(T, (x->right), z, id, i);
}


void find_rank(t_rank_head *T, t_rank_node *x, t_rank_node **z, char *id){
  if(x != T->nil){
    find_rank(T, x->left, z, id);
    if(strcmp(id, x->id) == 0){
      *z = x;
    }
    #ifdef DEBUG
    // printf("Rel: \"%s\" From: \"%s\" To: \"%s\"", x->id, x->orig->id, x->dest->id);
    #endif
    find_rank(T, x->right, z, id);
  }
}


// ##################################
// ##########   COMANDI    ##########
// ##################################

// FUNZIONE PRINCIPALE COMANDO "addent"
int addent(t_ent_head *T, char *comando){
  char addent_id[MAX_ENT_ID];
  t_ent_node *ent;

  // t_ent_node ent;

  // for debug
  #if DISABLE_ADD == 0
  #else
  #endif

  // ADDENT !!
  retrive_addent_delent_id(comando, addent_id);

  #ifndef NO_FIND
    if(check_existence_ent(T, T->root, addent_id, 0) != 0){
      #ifdef DEBUG_ADDENT
        printf("esiste gia' %s\n", addent_id);
      #endif
      return 1; //if the ent already exists, do nothing
    }
  #endif

  // crea nuova ent
  ent = malloc(sizeof(t_ent_node));

  // copy id
  strcpy(ent->id, addent_id);
  ent->v = 1;
  ent->in_rel = NULL;
  ent->out_rel = NULL;

  if(ENT_INSERT(T, ent)){
    #ifdef DEBUG_ADDENT
      printf("ent inserted: %s\n", ent->id);
    #endif
  }

  #ifdef DEBUG_ADDENT
    printf("!!Tree dopo insert: \n");
    ENT_TREE_WALK(T, T->root);
    printf("\n");
  #endif

  return 1;
}

// FUNZIONE PRINCIPALE COMANDO "delent"
int delent(t_ent_head *T, t_rel_tree_head *R, char *comando){
  char delent_id[MAX_ENT_ID];
  t_ent_node *ent;
  t_ent_node *deleted_ent;

  t_rel_list *in;
  t_rel_list *out;

  t_rel_node *deleted_rel;

  retrive_addent_delent_id(comando, delent_id);

  #ifdef DEBUG_DELENT
    printf("ent to delete: %s\n", delent_id);
  #endif

  if(find_ent(T, T->root, &ent, delent_id, 0) == 0){
    #ifdef DEBUG_DELENT
      printf("Ent NON presente, nulla da eliminare\n");
    #endif
    return 0;
  }

  #ifdef DEBUG_DELENT
    printf("found ent to delete : %s\n", ent->id);
  #endif

  deleted_ent = ENT_DELETE(T, ent);
  #ifdef DEBUG_DELENT
    printf("Deleted %s\n", deleted_ent->id);
  #endif

  // if(deleted_ent != ent){
  //   #ifdef DEBUG_DELENT
  //     printf("Deleted an other ent\n");
  //   #endif
  // }


  deleted_ent->v = 0;

  // disable all the rels with this ent
  in = deleted_ent->in_rel;
  while (in != NULL) {
    if(in->rel->v == 0){
      #ifndef DISABLE_FREE
        free(in->rel);
      #endif
      in = in->next;
      continue;
    }

    in->rel->v = 0;

    RANK_DELETE(in->rel->rank_h_p, in->rel->rank_pointer);
    in->rel->rank_pointer->n = in->rel->rank_pointer->n - 1;
    if(in->rel->rank_pointer->n == 0){
      #ifndef DISABLE_FREE
        free(in->rel->rank_pointer); // così perchè ho modificato l'algoritmo!
      #endif
    }
    else{
      // update rank (lo reinseriamo)
      RANK_INSERT(in->rel->rank_h_p, in->rel->rank_pointer);
    }

    // POSSIBILE BUG_!!
    // se rank h_p è vuoto, vuol dire che devo eliminare REL_TREE
    // if(in->rel->rank_h_p->root == in->rel->rank_h_p->nil){
    //   REL_TREE_DELETE(R, in->rel->rank_pointer->rel_tree_pointer);
    // }
    // devo cancellare la rel tree solo se non ci sono più rel!
    if(in->rel->rank_pointer->rel_tree_pointer->relations->root == in->rel->rank_pointer->rel_tree_pointer->relations->nil){
      REL_TREE_DELETE(R, in->rel->rank_pointer->rel_tree_pointer);
    }

    // printf("rel to delete: %s\n", in->rel->id);
    // cancelliamo la relazione
    deleted_rel = REL_DELETE(in->rel_head, in->rel);
    #ifndef DISABLE_FREE
      free(in->rel);
    #endif
    // printf("rel deleted:  %s\n", deleted_rel->id);

    #ifdef DEBUG_TREE_WALK
      printf("\t\tRANK walk after delent: ");
      RANK_TREE_WALK(in->rel->rank_h_p, in->rel->rank_h_p->root);
      printf("\n\n");
    #endif

    in = in->next;
  }

  out = deleted_ent->out_rel;
  while (out != NULL) {
    // se v = 0, allora è già stata eliminata, non dobbiamo toccarla
    if(out->rel->v == 0){
      #ifndef DISABLE_FREE
        free(out->rel);
      #endif
      out = out->next;
      continue;
    }

    out->rel->v = 0;

    RANK_DELETE(out->rel->rank_h_p, out->rel->rank_pointer);
    out->rel->rank_pointer->n = out->rel->rank_pointer->n - 1;
    if(out->rel->rank_pointer->n == 0){
      #ifndef DISABLE_FREE
        free(out->rel->rank_pointer); // così perchè ho modificato l'algoritmo!
      #endif
    }
    else{
      // update (lo reinseriamo)
      RANK_INSERT(out->rel->rank_h_p, out->rel->rank_pointer);
    }

    // POSSIBILE BUG_!!
    // se rank h_p è vuoto, vuol dire che devo eliminare REL_TREE
    // if(out->rel->rank_h_p->root == out->rel->rank_h_p->nil){
    //   REL_TREE_DELETE(R, out->rel->rank_pointer->rel_tree_pointer);
    // }
    if(out->rel->rank_pointer->rel_tree_pointer->relations->root == out->rel->rank_pointer->rel_tree_pointer->relations->nil){
      REL_TREE_DELETE(R, out->rel->rank_pointer->rel_tree_pointer);
    }

    REL_DELETE(out->rel_head, out->rel);
    #ifndef DISABLE_FREE
      free(out->rel);
    #endif

    #ifdef DEBUG_TREE_WALK
      printf("\t\tRANK walk after delent: ");
      RANK_TREE_WALK(out->rel->rank_h_p, out->rel->rank_h_p->root);
      printf("\n\n");
    #endif

    out = out->next;
  }

  #ifdef DEBUG
    printf("ent deleted is: %s \n", delent_id);
  #endif


  #ifdef DEBUG_TREE_WALK
    printf("\t\tENT walk after delent: ");
    ENT_TREE_WALK(T, T->root);
  #endif

  return 1;
}

// FUNZIONE PRINCIPALE COMANDO "addrel"
int addrel(t_ent_head *E, t_rel_tree_head *R, char *comando){
  char addrel_orig[MAX_ENT_ID];
  char addrel_dest[MAX_ENT_ID];
  char addrel_rel_id[MAX_REL_ID];
  char addrel_orig_dest[MAX_ENT_ID+MAX_ENT_ID];

  t_rel_tree_node *rel_tree;
  t_rel_node *rel_node = NULL;

  t_ent_node *orig;
  t_ent_node *dest;
  t_rel_list *out;
  t_rel_list *in;

  t_rank_node *rank = NULL;

  retrive_addrel_delrel_args(comando, addrel_orig, addrel_dest, addrel_rel_id);
  strcpy(addrel_orig_dest, addrel_orig);
  strcat(addrel_orig_dest, addrel_dest);

  // prima di tutto cerchiamo se orig e dest esistono
  if(find_ent(E, E->root, &orig, addrel_orig, 0) == 0){ //search for ent 1
    #ifdef DEBUG_ADDREL
      printf("Ent 1 not found\n");
    #endif
    return 0; //ent orig not found
  }
  if(find_ent(E, E->root, &dest, addrel_dest, 0) == 0){ //search for ent 2
    #ifdef DEBUG_ADDREL
      printf("Ent 2 not found\n");
    #endif

    #ifdef DEBUG_TREE_WALK
      ENT_TREE_WALK(E, E->root);
      printf("\n");
    #endif

    return 0; //ent dest not found
  }

  #ifdef DEBUG_ADDREL
    printf("\n\nAddRel: %s - %s - %s\n", addrel_orig, addrel_dest, addrel_rel_id);
  #endif

  if(find_rel_tree(R, R->root, &rel_tree, addrel_rel_id, 0)){
    // rel_tree è già presente!
    // e in questo modo l'ho già
    #ifdef DEBUG_ADDREL
      printf("\tRel Tree Presente\n");
    #endif

    // cerchiamo se questa relazione fra i due è presente
    if(find_rel(rel_tree->relations, rel_tree->relations->root, &rel_node, addrel_orig_dest, 0)){
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
    rel_tree = malloc(sizeof(t_rel_tree_node));
    // qui bisogna inizializzare rel tree e poi inserirlo se non è presente
    strcpy(rel_tree->id, addrel_rel_id);
    rel_tree->color = 0;
    rel_tree->v = 1;
    rel_tree->relations = malloc(sizeof(t_rel_head));
    rel_tree->relations->nil = &NIL_REL;
    rel_tree->relations->root = rel_tree->relations->nil;
    rel_tree->ranking = malloc(sizeof(t_rank_head));
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
  rel_node = malloc(sizeof(t_rel_node));

  strcpy(rel_node->id, addrel_orig_dest);
  rel_node->v = 1;
  rel_node->orig = orig;
  rel_node->dest = dest;
  // rel_node->color = 0;

  if(REL_INSERT(rel_tree->relations, rel_node)){
    #ifdef DEBUG_2
      printf("\tadded REL\n");
    #endif
  }

  out = malloc(sizeof(t_rel_list));
  out->rel_head = rel_tree->relations;
  out->rel = rel_node;
  in = malloc(sizeof(t_rel_list));
  in->rel_head = rel_tree->relations;
  in->rel = rel_node;

  ADD_TO_LIST(&(orig->out_rel), out);
  ADD_TO_LIST(&(dest->in_rel), in);

  // cercare se l'in relation è già monitorata
  // se si update
  //            elimina
  //            aggiorna contatore
  //            inserisci
  #ifdef DEBUG_ADDREL
    printf("\tDest id to rank: %s\n", dest->id);
  #endif
  find_rank(rel_tree->ranking, rel_tree->ranking->root, &rank, dest->id); //TODO: ottimizzare?? questa è O(n)!!
  if(rank != NULL){
    #ifdef DEBUG_ADDREL
      printf("\tRanking da aggiornare\n");
    #endif
    #ifdef DEBUG_ADDREL
      printf("\tElimino: %s\n", rank->id);
    #endif
    RANK_DELETE(rel_tree->ranking, rank);
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
    rank = malloc(sizeof(t_rank_node));
    strcpy(rank->id, addrel_dest);
    rank->n = 1;
    rank->rel_tree_pointer = rel_tree;
    RANK_INSERT(rel_tree->ranking, rank);
    #ifdef DEBUG_ADDREL
      printf("\tRank inserito da capo\n");
      // RANK_TREE_WALK(rel_tree->ranking, rel_tree->ranking->root);
    #endif
  }
  // inseriamo il collegamento da ent al nodo rank
  rel_node->rank_h_p = rel_tree->ranking; // inseriamo la testa del rank (serve per il delete)
  rel_node->rank_pointer = rank; // puntatore al nodo rank
  #ifdef DEBUG_ADDREL
    printf("-------Rank repo dopo insert: ---------\n");
    RANK_TREE_WALK(rel_tree->ranking, rel_tree->ranking->root);
  #endif

  return 1;
}

// FUNZIONE PRINCIPALE COMANDO "delrel"
int delrel(t_rel_tree_head *T, char *comando){
  char delrel_orig[MAX_ENT_ID];
  char delrel_dest[MAX_ENT_ID];
  char delrel_orig_dest[MAX_ENT_ID+MAX_ENT_ID];
  char delrel_id[MAX_REL_ID];
  t_rel_tree_node *rel_tree;
  t_rel_node *rel;
  // t_rel_node *deleted_rel;

  #ifdef DEBUG_DELREL
    printf("Inizio eliminazione relazione\n");
  #endif

  if(retrive_addrel_delrel_args(comando, delrel_orig, delrel_dest, delrel_id) != 1){
    #ifdef DEBUG_DELREL
      printf("Errore nella retrive\n");
    #endif
    return 0;
  }
  #ifdef DEBUG_DELREL
    printf("Presi i dati in input\n");
  #endif
  strcpy(delrel_orig_dest, delrel_orig);
  strcat(delrel_orig_dest, delrel_dest);
  // printf("delrel_orig_dest: %s\n", delrel_orig_dest);

  if(find_rel_tree(T, T->root, &rel_tree, delrel_id, 0) == 0){
    #ifdef DEBUG_DELREL
      printf("Relazione in Rel Tree non trovata\n");
    #endif
    return 0;
  }
  if(find_rel(rel_tree->relations, rel_tree->relations->root, &rel, delrel_orig_dest, 0) == 0){
    #ifdef DEBUG_DELREL
      printf("Relazione in Relations non trovata\n");
    #endif
    return 0;
  }

  REL_DELETE(rel_tree->relations, rel);
  rel->v = 0;
  #ifdef DEBUG_DELREL
    printf("Eliminata relazione\n");
  #endif

  // if(deleted_rel != rel){
  //   #ifdef DEBUG
  //   printf("Deleted an other rel\n");
  //   #endif
  // }

  // vuol dire che mi sono rimaste zero relazioni
  if(rel_tree->relations->root == rel_tree->relations->nil){
    REL_TREE_DELETE(T, rel_tree);
    #ifndef DISABLE_FREE
      free(rel_tree);
    #endif
    #ifdef DEBUG_DELREL
      printf("Eliminato Rel Tree\n");
    #endif
  }
  #ifdef DEBUG_DELREL
  else{
    printf("NON Eliminato Rel Tree\n");
  }
  #endif


  rel->rank_pointer->n = rel->rank_pointer->n - 1;
  if(rel->rank_pointer->n == 0){
    #ifdef DEBUG_DELREL
      printf("Voglio eliminare il nodo rank che è a 0\n");
    #endif
    RANK_DELETE(rel->rank_h_p, rel->rank_pointer);
    #ifndef DISABLE_FREE
      free(rel->rank_pointer); // così perchè ho modificato l'algoritmo!
    #endif
  }
  else{
    // update rank
    #ifdef DEBUG_DELREL
      printf("Voglio aggiornare il nodo rank\n");
    #endif
    RANK_DELETE(rel->rank_h_p, rel->rank_pointer);
    #ifdef DEBUG_DELREL
      printf("Rank eliminato (per update)\n");
    #endif
    RANK_INSERT(rel->rank_h_p, rel->rank_pointer);
    #ifdef DEBUG_DELREL
      printf("Rank reinserito (per update)\n");
    #endif
  }
  #ifdef DEBUG_DELREL
    printf("\t\tRank walk after delrel: ");
    RANK_TREE_WALK(rel->rank_h_p, rel->rank_h_p->root);
  #endif

  #ifdef DEBUG_TREE_WALK
    printf("\t\tRank walk after delrel: ");
    RANK_TREE_WALK(rel->rank_h_p, rel->rank_h_p->root);
  #endif

  return 1;
}

// // FUNZIONE SECONDARIA COMANDO "report"
// int RANK_TREE_WALK(t_rank_head *T, t_rank_node *x){
//   // printf("a\n");
//   if(x != T->nil){
//     RANK_TREE_WALK(T, x->right);
//     if(x->left != T->nil && x->n > T->nil){
//       return 1
//     }
//     else{
//       printf("%s\n", );
//     }
//   }
// }

// TO IMPROVE!!!!
void REPORT_RANK_TREE_WALK(t_rank_head *T, t_rank_node *x, int n){
  // printf("a\n");
  if(x != T->nil){
    REPORT_RANK_TREE_WALK(T, x->left, n);
    if(x->n == n){
      printf("\"%s\" ", x->id);
    }
    // else
    //   printf("\"\"%s\"\" ", x->id);
    REPORT_RANK_TREE_WALK(T, x->right, n);
  }
}

// FUNZIONE SECONDARIA COMANDO "report"
int REPORT_RANK(t_rank_head *T, t_rank_node *x){
  // t_rank_node *y;
  int n;
  t_rank_node *y = x;
  // andiamo al massimo
  #ifdef DEBUG_REPORT
    printf(" -- andiamo al max --\n");
  #endif
  #ifdef DEBUG_REPORT
    printf("- pp- %s\n -/pp-", x->id);
  #endif

  while (x->right != T->nil) {
    x = x->right;
  }
  n = x->n;
  #ifdef DEBUG_REPORT
    printf(" -- trovato il max --\n");
  #endif
  // TODO: da ottimizzare!
  REPORT_RANK_TREE_WALK(T, y, n); // viene fatta una tree walk completa
  // do {
  //   printf("\"%s\" ", x->id);
  //   if(x->left->n == n){ //TODO: da ottimizzare
  //     // se il figlio sx del nodo che stiamo analizzando ha n = n
  //     // allora facciamo un walk in quel sottoalbero
  //     REPORT_RANK_TREE_WALK(T, x->left, n);
  //   }
  //   x = x->p;
  // } while(x != T->nil && x->n == n);
  printf("%d;", n);
  return 1;
}

// FUNZIONE SECONDARIA COMANDO "report"
void REPORT_WALK_REL_TREE(t_rel_tree_head *T, t_rel_tree_node *x, t_rel_tree_node *max){
  if(x != T->nil){
    REPORT_WALK_REL_TREE(T, x->left, max);
    if(x->ranking->root != x->ranking->nil){
      printf("\"%s\" ", x->id); // print ent name
      #ifdef DEBUG_REPORT
        if(x->ranking->root == x->ranking->nil)
          printf("aaa\n");
      #endif
      REPORT_RANK(x->ranking, x->ranking->root); // print rank
      if(x != max)
        printf(" ");
    }
    REPORT_WALK_REL_TREE(T, x->right, max);
  }
}

// FUNZIONE PRINCIPALE COMANDO "report"
int report(t_rel_tree_head *T){
  t_rel_tree_node *x = T->root;

  if(T->root == T->nil){
    printf("none");
  }
  else{
    while (x->right != T->nil) {
      x = x->right;
    }
    REPORT_WALK_REL_TREE(T, T->root, x);
  }
  printf("\n");
  return 1;
}



int main(){
  char *comando = NULL;
  int cmd;
  int i = 0;
  int size_cmd;
  size_t len = 0;
  t_ent_head T_ent; //head delle entità
  t_rel_tree_head T_rel_tree; //head delle relazioni

  // init entity tree
  T_ent.nil = &NIL_ENT;
  T_ent.root = T_ent.nil;

  // init relations tree
  T_rel_tree.nil = &NIL_REL_TREE;
  T_rel_tree.root = T_rel_tree.nil;

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
      addent(&T_ent, comando);
    }
    else if(cmd == 1){ // delent
      delent(&T_ent, &T_rel_tree, comando);
    }
    else if(cmd == 2){ // addrel
      addrel(&T_ent, &T_rel_tree, comando);
    }
    else if(cmd == 3){ // delrel
      // delrel(&T_ent, &T_rel_tree, comando);
      delrel(&T_rel_tree, comando);
    }
    else if(cmd == 4){ // report
      report(&T_rel_tree);
      #ifdef DEBUG
        printf("Ent list: ");
        ENT_TREE_WALK(&T_ent, T_ent.root);
        printf("\n");
        printf("Rel list: ");
        REL_TREE_WALK(&T_rel_tree, T_rel.root);
        printf("\n");
      #endif
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
