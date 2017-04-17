#ifndef TREE_H
#define TREE_H

#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>


//!~~~~NODE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Node
  {
  public:
    Node();
    explicit Node(const char* data);
    ~Node();

    char* data();                              //! returns data
    Node* right();                             //! returns right leaf
    Node* left();                              //! ....
    Node* parent();                            //! returns parent
    
    void setright(Node* right);                //! change/set leaves
    void setparent(Node* parent);   
    void setleft(Node* left);
    void setdata(const char* data);            //! change/set data
    
    void kill(FILE*);                          //! calls delete for leaves(used by visitor in
                                               //! Tree::~Tree())

    void dotprint(FILE* out);                  //! put in *.gv file node descrition
                                               //! (used by visitor in Tree::dotdump())
 
    void nodeprint(FILE* out);                 //! writes in file current node

    void dump(FILE*);                          //! dumping current node(used by visitor in
                                               //! Tree::dump())
  
    void fnodescan(FILE*);                     //! reads from file current node

  private:
    Node* Parent_;
    Node* Left_;
    Node* Right_;
    char* Data_;
  };

//!macro to verify data
#define IS_CORRECT \
  if(!Data_) \
    { \
    printf("Node:ERROR:Lost data\n"); \
    dump(NULL); \
    assert(0); \
    }

Node::Node():
  Parent_(NULL),
  Left_(NULL),
  Right_(NULL),
  Data_(NULL)
  {}

Node::Node(const char* data):
  Parent_(NULL),
  Left_(NULL),
  Right_(NULL)
  {
  Data_ = new char[strlen(data)];
  Data_ = strcpy(Data_, data);
  IS_CORRECT;
  }

Node::~Node()
  {
  Parent_ = NULL;
  Left_ = NULL;
  Right_ = NULL;
  if(!Data_)delete [] Data_;
  }

void Node::kill(FILE*)
  {
  if(left())delete left();
  if(right())delete right();
  }

#define NODEPRINT(leaf) \
  if(leaf)leaf->nodeprint(out); \
  else fprintf(out, "N");

void Node::nodeprint(FILE* out)
  {
  fprintf(out, "(");
  fprintf(out, "%c%s%c", '\'', Data_, '\'');
  NODEPRINT(Left_);
  NODEPRINT(Right_);
  fprintf(out, ")");
  }
#undef NODEPRINT


char* Node::data(){return Data_; IS_CORRECT;}

Node* Node::right(){return Right_; IS_CORRECT;}

Node* Node::left(){return Left_; IS_CORRECT;}

Node* Node::parent(){return Parent_; IS_CORRECT;}

void Node::setright(Node* right)
  {
  Right_ = right;
  right->setparent(this); 
  IS_CORRECT;
  }

void Node::setparent(Node* parent)
  {
  Parent_ = parent; 
  IS_CORRECT;
  }

void Node::setleft(Node* left)
  {
  Left_ = left; 
  left->setparent(this); 
  IS_CORRECT;
  }

void Node::setdata(const char* newdata)
  {
  delete [] Data_;
  Data_ = new char [strlen(newdata)];
  Data_ = strcpy(Data_, newdata);
  IS_CORRECT;
  }

void Node::dump(FILE* f)
  {
  printf("~~~~\n");
  printf("Node:Dump has been called\n");
  printf("That node = %p\n", this);
  if(Data_)printf("Data_ = %s\n", Data_);
  printf("Parent_ = %p\n", Parent_);
  printf("Left_ leaf = %p\n", Left_);
  printf("Right_ leaf = %p\n", Right_);
  printf("~~~~\n");
  //.....
  }

void Node::dotprint(FILE* in)
  {
  fprintf(in, "TreeNode_%p [label = ", this);
  fprintf(in, "%c", '"');
  fprintf(in, "TreeNode_%p\\l parent = %p\\l data = '%s'\\l left = %p\\l right = %p",
                           this, parent(), data(), left(), right());
  fprintf(in, "%c]\n", '"');
  if(left())fprintf(in, "TreeNode_%p->TreeNode_%p\n", this, left());
  if(right())fprintf(in, "TreeNode_%p->TreeNode_%p\n", this, right());
  } 

#define IFEOF \
  if(c == EOF) \
  { \
    printf("Node:fnodescan:getstr:ERROR:bad inputfile(EOF reached)\n"); \
    assert(0); \
  }


char* getstr(FILE* in)                          //! local function to read data
  {
  int size = 20;
  int index = 0;
  char c = 1;
  char* str = (char*)malloc(sizeof(char) * size);
  while(c != '\'')
    {
    c = fgetc(in);
    IFEOF;
    }
  c = fgetc(in);
  IFEOF;
  while(c != '\'')
    {
    str[index++] = c;
    if(index == size)
      {
      size *= 2;
      str = (char*)realloc(str, sizeof(char) * size);
      }
    c = fgetc(in);
    IFEOF;
    }
  str[index] = '\0';
  return str;
  }

#define LEAF(side) \
  c = fgetc(file); \
  if(c == '(') \
    { \
    fseek(file, -1, SEEK_CUR); \
    Node* side = new Node; \
    side->fnodescan(file); \
    set##side(side); \
    } \
  else if(c != 'N') \
    { \
    printf("Node:fnodscan:ERROR:bad input(wrong symbol)\n"); \
    assert(0); \
    }


void Node::fnodescan(FILE* file)
  {
  char c = fgetc(file);
  if(c != '(')
    {
    printf("Node:fnodscan:ERROR:bad input(brace is lost)\n");
    assert(0);
    }
  char* str = getstr(file);
  Data_ = str;
  LEAF(left);
  LEAF(right);
  c = fgetc(file);
  if(c != ')')
    {
    printf("Node:fnodscan:ERROR:bad input(brace is lost)\n");
    assert(0);
    }
  }
#undef LEAF

#undef IS_CORRECT
//!~~~~/NODE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//!~~~~TREE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum TRAVERSAL                                    //! taversal modes
  {
  PRE_ORDER,                                      //! data - left leaf - right leaf
  IN_ORDER,                                       //! leaft leaf - data - right leaf
  POST_ORDER,                                     //! leaft leaf - right leaf - data
  };


class Tree
  {
  public:
    Tree();
    Tree(Node* root);
    explicit Tree(const char* filename);
    ~Tree();
    Node* Root();                                 //! return root
    Node* getcursor();                            //! returns current cursor position
    bool mcleft();                                //! moves cursor to the left leaf
    bool mcright();                              
    bool mcabove();                               //! moves to parent
    bool mcroot();
    bool mcnode(Node* dest);
    void dump();
    bool dotdump(const char* filename);           //! put commands in *.gv file for Dotter

    void addl(Node* newnode);                     //! add new left leaf(old one 
                                                  //! wont be deleted)
     
    void addr(Node* newnode);

    bool fscan(const char* filename);             //! reads tree from the file

    bool fprint(const char* filename);            //! writes tree to the file
    
    void visitor/*your mum*/(TRAVERSAL mode,      //! visits every node according to
         Node* curnode, void (Node::*func)(FILE*),//! traversal mode
         FILE* file);
    
    Node* search(const char* data);               //! seatch node with specified data,
                                                  //! put cursor on it and returns its address

  private:
    Node* Root_;                                  //! Root
    Node* Cursor_;                                //! Cursor (thx, Cap)
  };

Tree::Tree():
  Root_(new Node),
  Cursor_(Root_)
  {}


Tree::Tree(Node* root):
  Root_(root),
  Cursor_(Root_)
  {}

Tree::Tree(const char* filename):
  Root_(new Node),
  Cursor_(new Node)
  {
  if(!fscan(filename))
    {
    printf("Tree:ERROR:cant open/read input file");
    dump();
    assert(0);
    }
  Root_ = Cursor_;
  }

Tree::~Tree()
  {
  visitor(POST_ORDER, Root_, &Node::kill, NULL);
  Root_->~Node();
  Cursor_->~Node(); //
  }

void Tree::dump()
  {
  printf("========\n");
  printf("Root_ = %p\n", Root_);
  printf("Cursor_ = %p\n", Cursor_);
  visitor(PRE_ORDER, Root_, &Node::dump, NULL);
  printf("========\n");
  }

bool Tree::dotdump(const char*filename)
  {
  FILE* file = fopen(filename, "wt");
  if(!file)
    {
    printf("Tree:dotdump:ERROR:cant open file\n");
    return 0;
    }
  fprintf(file, "digraph G{\ngraph [ dpi = 300]\n");
  visitor(POST_ORDER, Cursor_, &Node::dotprint, file);
  fprintf(file, "}");
  fclose(file);
  return true;
  }

Node* Tree::Root()
  {
  return Root_;
  }

bool Tree::fprint(const char* filename)
  {
  FILE* file = fopen(filename, "wt");
  if(!file)
    {
    printf("Tree:fprint:ERROR:cant open file\n");
    return false;
    }
  Cursor_->nodeprint(file);
  fclose(file);
  return true;
  }

bool Tree::fscan(const char* filename)
  {
  FILE* file = fopen(filename, "rt");
  if(!file)
    {
    printf("Tree:fscan:ERROR:cant open/read input file\n");
    return false;
    }
  Cursor_->fnodescan(file); 
  fclose(file);  
  return true;
  }
 
void Tree::addr(Node* newnode)
  {
  Cursor_->setright(newnode);
  }

void Tree::addl(Node* newnode)
  {
  Cursor_->setleft(newnode);
  }


Node* Tree::getcursor(){return Cursor_;}

bool Tree::mcleft()
  {
  if(Cursor_->left())
    {
    Cursor_ = Cursor_->left();
    return true;
    }
  return false;
  }

bool Tree::mcright()
  {
  if(Cursor_->right())
    {
    Cursor_ = Cursor_->right();
    return true;
    }
  return false;
  }

bool Tree::mcabove()
  {
  if(Cursor_->parent())
    {
    Cursor_ = Cursor_->parent();
    return true;
    }
  return false;
  }

bool Tree::mcroot()
  {
  if(!Root_)return false;
  Cursor_ = Root_;
  }

bool Tree::mcnode(Node* dest)
  {
  Cursor_ = dest;
  }

#define LEFT \
  if(curnode->left())visitor(mode, curnode->left(), func, file);

#define RIGHT \
  if(curnode->right())visitor(mode, curnode->right(), func, file);

void Tree::visitor(TRAVERSAL mode, Node* curnode, void (Node::*func)(FILE* f), FILE* file)
  {
  switch(mode)
    {
    case PRE_ORDER:
      (curnode->*func)(file);
      LEFT;
      RIGHT;
      break;      
    case IN_ORDER:
      LEFT;
      (curnode->*func)(file);
      RIGHT;
      break;
    case POST_ORDER:
      LEFT;
      RIGHT;
      (curnode->*func)(file);
      break;
    default:
      break;
    }
  }
#undef LEFT
#undef RIGHT
  

Node* Tree::search(const char* data)
  {
  if(!data)return NULL;
  if(!strcmp(data, Cursor_->data()))return Cursor_;
  Node* wanted = NULL;
  if(Cursor_->left())
    {
    mcleft();  
    wanted = search(data);
    if(wanted)return wanted;
    }
  if(Cursor_->right())
    {
    mcright();
    wanted = search(data);
    if(wanted)return wanted;
    }
  mcabove();
  return NULL;
  }


//!~~~~/TREE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#endif

