/* MACRO-CODED FAST FIXED AVL TREES IMPLEMENTATION IN C              */
/* COPYRIGHT (C) 1998 VALERY SHCHEDRIN                               */
/* IT IS DISTRIBUTED UNDER GLPL (GNU GENERAL LIBRARY PUBLIC LICENSE) */

#define balance(objname, pr, root, ht_changed) \
  { \
    objname *p; \
    p = *root; \
    if (p->bal_##pr < -1) \
    { \
      if (p->l_##pr->bal_##pr == 1) \
      { \
	objname *pp; \
	pp=p->l_##pr; *root=p->l_##pr->r_##pr; p->l_##pr = (*root)->r_##pr; \
	(*root)->r_##pr = p; pp->r_##pr = (*root)->l_##pr; \
	p = *root; p->l_##pr = pp; \
	if (p->bal_##pr > 0) p->l_##pr ->bal_##pr = -p->bal_##pr ; \
	else p->l_##pr ->bal_##pr = 0; \
	if (p->bal_##pr < 0) p->r_##pr ->bal_##pr = -p->bal_##pr ; \
	else p->r_##pr ->bal_##pr = 0; \
	p->bal_##pr = 0; \
	ht_changed = 1; \
      } \
      else \
      { \
	ht_changed = (p->l_##pr ->bal_##pr)?1:0; \
	*root = p->l_##pr ; \
	p->l_##pr = (*root)->r_##pr ; (*root)->r_##pr = p; \
	p->bal_##pr = - (++((*root)->bal_##pr )); \
      } \
    } \
    else if (p->bal_##pr > 1) \
    { \
      if (p->r_##pr->bal_##pr == -1) \
      { \
	objname *pp; \
	pp=p->r_##pr ; *root=p->r_##pr ->l_##pr ; p->r_##pr =(*root)->l_##pr ; \
	(*root)->l_##pr = p; pp->l_##pr = (*root)->r_##pr ; \
	p = *root; p->r_##pr = pp; \
	if (p->bal_##pr > 0) p->l_##pr ->bal_##pr = -p->bal_##pr ; \
	else p->l_##pr ->bal_##pr = 0; \
	if (p->bal_##pr < 0) p->r_##pr ->bal_##pr = -p->bal_##pr ; \
	else p->r_##pr ->bal_##pr = 0; \
	p->bal_##pr = 0; \
	ht_changed = 1; \
      } \
      else \
      { \
	ht_changed = (p->r_##pr ->bal_##pr)?1:0; \
	*root = p->r_##pr ; \
	p->r_##pr = (*root)->l_##pr ; (*root)->l_##pr = p; \
	p->bal_##pr = - (--((*root)->bal_##pr )); \
      } \
    } else ht_changed = 0; \
  }

#define Avl_r_insert_proto(objname, pr, COMPARE) \
  static int Avl_##objname##pr##_r_insert(objname **root) \
  { \
    int i; /* height increase */ \
    if (!*root) \
    { \
      *root = Avl_##objname##pr##_new_node; \
      Avl_##objname##pr##_new_node = NULL; \
      return 1; \
    } \
    COMPARE(i, Avl_##objname##pr##_new_node, *root); \
    \
    if (i < 0) \
    { /* insert into the left subtree */ \
      i = -Avl_##objname##pr##_r_insert(&((*root)->l_##pr)); \
      if (Avl_##objname##pr##_new_node != NULL) return 0; /* already there */ \
    } \
    else if (i > 0) \
    { /* insert into the right subtree */ \
      i = Avl_##objname##pr##_r_insert(&((*root)->r_##pr)); \
      if (Avl_##objname##pr##_new_node != NULL) return 0; /* already there */ \
    } \
    else \
    { /* found */ \
      Avl_##objname##pr##_new_node = *root; \
      return 0; \
    } \
    if (!i) return 0; \
    (*root)->bal_##pr += i; /* update balance factor */ \
    if ((*root)->bal_##pr) \
    { \
      balance(objname,pr,root,i); \
      return 1-i; \
    } \
    else return 0; \
  }

#define Avl_r_delete_proto(objname,pr,COMPARE) \
  static int Avl_##objname##pr##_r_delete(objname **root) \
  { \
    int i; /* height decrease */ \
    \
    if (!*root) return 0; /* not found */ \
    \
    COMPARE(i, Avl_##objname##pr##_new_node, *root); \
    \
    if (i < 0) \
      i = -Avl_##objname##pr##_r_delete(&((*root)->l_##pr)); \
    else if (i > 0) \
      i =  Avl_##objname##pr##_r_delete(&((*root)->r_##pr)); \
    else \
    { \
      if (!(*root)->l_##pr) \
      { \
	*root = (*root)->r_##pr; \
	return 1; \
      } \
      else if (!(*root)->r_##pr) \
      { \
	*root = (*root)->l_##pr; \
	return 1; \
      } \
      else \
      { \
	i = Avl_##objname##pr##_r_delfix(&((*root)->r_##pr)); \
	Avl_##objname##pr##_new_node->l_##pr = (*root)->l_##pr; \
	Avl_##objname##pr##_new_node->r_##pr = (*root)->r_##pr; \
	Avl_##objname##pr##_new_node->bal_##pr = (*root)->bal_##pr; \
	*root = Avl_##objname##pr##_new_node; \
      } \
    } \
    if (!i) return 0; \
    (*root)->bal_##pr -= i; \
    if ((*root)->bal_##pr) \
    { \
      balance(objname,pr,root,i); \
      return i; \
    } \
    return 1; \
  }

#define Avl_r_delfix_proto(objname,pr) \
  static int Avl_##objname##pr##_r_delfix(objname **root) \
  { \
    int i; /* height decrease */ \
    \
    if (!(*root)->l_##pr) \
    { \
      Avl_##objname##pr##_new_node = *root; \
      *root = (*root)->r_##pr; \
      return 1; \
    } \
    i = -Avl_##objname##pr##_r_delfix(&((*root)->l_##pr)); \
    if (!i) return 0; \
    (*root)->bal_##pr -= i; \
    if ((*root)->bal_##pr) \
    { \
      balance(objname,pr,root,i); \
      return i; \
    } \
    return 1; \
  }

#define Avl_ins_proto(alias,objname,pr) \
  static objname *##alias##_ins(objname *data) \
  { \
    Avl_##objname##pr##_new_node = data; \
    (data)->l_##pr = NULL; \
    (data)->r_##pr = NULL; \
    (data)->bal_##pr = 0; \
    Avl_##objname##pr##_r_insert(&Avl_##objname##pr##_tree); \
    if (Avl_##objname##pr##_new_node) \
      return Avl_##objname##pr##_new_node; \
    return NULL; \
  }

#define Avl_del_proto(alias,objname,pr) \
  static void alias##_del(objname *data) \
  { \
    Avl_##objname##pr##_new_node = data; \
    Avl_##objname##pr##_r_delete(&Avl_##objname##pr##_tree); \
  }

#define Avl_replace_proto(alias,objname,pr,COMPARE) \
  static void alias##_replace(objname *data) \
  { \
    objname **p = &Avl_##objname##pr##_tree; \
    int cmp; \
    while (*p) \
    { \
      COMPARE(cmp, data, *p); \
      if (cmp < 0) \
	p = &((*p)->l_##pr); \
      else if (cmp > 0) \
	p = &((*p)->r_##pr); \
      else \
      { \
	(data)->l_##pr = (*p)->l_##pr; \
	(data)->r_##pr = (*p)->r_##pr; \
	(data)->bal_##pr = (*p)->bal_##pr; \
	*p = data; \
	return; \
      } \
    } \
  }

#define Avl_Root(objname,pr) Avl_##objname##pr##_tree

#define Avl_Tree(alias,objname,pr,COMPARE) \
static objname *Avl_##objname##pr##_tree = NULL; \
static objname *Avl_##objname##pr##_new_node; \
Avl_r_insert_proto(objname,pr,COMPARE) \
Avl_r_delfix_proto(objname,pr) \
Avl_r_delete_proto(objname,pr,COMPARE) \
Avl_ins_proto(alias,objname,pr) \
Avl_del_proto(alias,objname,pr) \
Avl_replace_proto(alias,objname,pr,COMPARE)

