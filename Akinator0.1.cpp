#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "Tree.h"



#define INFO " ====================================================\n \
Akinator v0.1. Small version of en.akinator.com with\n \
MIPT teachers.                                      \n \
Soshin Konstantin, 2017, soshinkonstantinv@gmail.com\n \
====================================================\n"
                                                                              //just for lulz
enum CONST
  {
  DSCR_CAP = 1024,
  INPUT_CAP = 64,
  GETS_SIZE = 128,
  ANS_SIZE = 32,
  REFSIZE = 4096,
  };


enum AKINATOR_MENU
  {
  #define AKINATOR_MENU
  #define AKINATOR(choice, act) \
    choice,
  #include "Akinator_menu.txt"
  #undef AKINATOR
  #undef AKINATOR_MENU
    LAST_CHOICE,
  };


class Akinator
  {
  public:
    Akinator();
    ~Akinator();
    int menu();
    void play();
    void newchar();
    void dump();
    void definition();
    void compare();
    void reference();
    void exit();
   
    bool Akinator_global_exit;
  private:
    char** getdescription(const char* name);
    char* gets();
    Tree Base_;
  };

Akinator::Akinator():
  Akinator_global_exit(false)
  {
  FILE* f = fopen("Akinator.txt", "rt");
  if(!f)
    {
    printf("Akinator:ERROR:lost base\n");
    assert(0);
    }
  fclose(f);
  Base_.fscan("Akinator.txt");
  if(!(Base_.Root()->data())){printf("Akinator:ERROR:bad base(Akinator.txt)(damaged) - data doesnt exist\n");assert(0);}
  if((!(Base_.Root()->right())) || (!(Base_.Root()->left()))){printf("Akinator:ERROR:bad base(Akinator.txt)(damaged)\
 - root's leaves dont exist\n"); assert(0);}
  }

Akinator::~Akinator()
  {
  Base_.mcroot();
  Base_.fprint("Akinator.txt");
  }

void Akinator::reference()
  {
  FILE* f = fopen("AboutAkinator.txt", "rt");
  if(!f)printf("Akinator:ERROR:Reference (AboutAkinator.txt) not exist\n");
  else
    {
    char ref[REFSIZE];
    fread(ref, sizeof(char), REFSIZE, f);
    printf("%s", ref);
    }
  }

  

#define INVITE \
  do{ \
    printf("Enter what to do[0-%d]:", LAST_CHOICE - 1); \
    scanf("%20s", ans); \
    inp = ans[0] - '0'; \
    } \
  while(!((strlen(ans) == 1) && inp >= 0 && inp < LAST_CHOICE))

int Akinator::menu()
  {
  printf("\n");
  #define AKINATOR_MENU
  #define AKINATOR(choice, act) \
    printf("[%d]%s\n", choice, #choice);
  #include "Akinator_menu.txt"
  #undef AKINATOR
  #undef AKINATOR_MENU
  char ans[INPUT_CAP];
  int inp;
  INVITE;
  switch(inp)
    {
    #define AKINATOR_MENU
    #define AKINATOR(choice, act) \
      case choice: \
        act; \
        break;
    #include "Akinator_menu.txt" 
    #undef AKINATOR
    #undef AKINATOR_MENU
    default:
      break;
    }
  return inp;
  }
      
#undef INVITE

#define INVITE(msg, object) \
    do { \
    printf(msg, object); \
    scanf("%100s", inp); \
    }while((strcmp(inp, "y") && strcmp(inp, "n")))
    

#define CHARACTER \
      if(!Base_.getcursor()->left()) \
        { \
        INVITE("Oh, I know! It's %s\nDid I guess?[y/n]:", Base_.getcursor()->data()); \
        if(!strcmp(inp, "y")) \
          { \
          printf("I like to play with You ^-^\n"); \
          break; \
          } \
        if(!strcmp(inp, "n")) \
          { \
          getchar(); \
          newchar(); \
          break; \
          } \
        }

char* Akinator::gets()        
  {
  int size = GETS_SIZE;
  int index = 0;
  char c = 1;
  char* str = (char*)malloc(sizeof(char) * size);
  while(!isalpha(c))
    {
    c = getchar();
    }
  while(c != '\n')
    {
    str[index++] = c;
    if(index == size)
      {
      size *= 2;
      str = (char*)realloc(str, sizeof(char) * size);
      }
    c = getchar();
    }
  str[index] = '\0';
  return str;
  }

void Akinator::newchar()
  {
  printf("Ok, I give up. Whom did you think?[Enter the name]:");
  char* name = gets();                                         //gets
  Node* Feature = new Node;
  Node* Character = new Node;
  Node* cur = Base_.getcursor();
  Base_.mcroot();
  if(Base_.search(name))
    {
    printf("Hey, I know that person!(you probably made a mistake)\n");
    delete Feature;
    delete Character;
    }
  else
    {
    printf("I'll remember. And what his feature?[Enter feature]:");
    char* feature = gets();                                      //gets
    Base_.mcnode(cur);
    Character->setdata(name);
    Feature->setdata(feature);
    Feature->setright(Character);
    Base_.mcabove();
    Feature->setleft(cur);
    if(Base_.getcursor()->left() == cur)Base_.getcursor()->setleft(Feature);
    else if(Base_.getcursor()->right() == cur)Base_.getcursor()->setright(Feature);
    delete [] feature;
    }
  delete [] name;
  }

void Akinator::play()
  {
  printf("\n");
  while(true)
    {
    printf("%s?[y/n]:", Base_.getcursor()->data());
    char inp[INPUT_CAP];
    scanf("%s", inp);
    if(!strcmp(inp, "n"))
      {
      Base_.mcleft();
      CHARACTER;
      }
    if(!strcmp(inp, "y"))
      {
      Base_.mcright();
      CHARACTER;
      }
    }
  printf("Thx for playing! See u later:*\n");
  Base_.mcroot();
  }

#undef CHARACTER

void Akinator::exit()
  {
  char inp[INPUT_CAP];
  INVITE("\nDo you want to exit?[y/n%c:", ']' ); //le kostyl
  if(!strcmp(inp, "y"))Akinator_global_exit = true;
  }

#undef INVITE



void Akinator::dump()
  {
  Base_.mcroot();
  Base_.dotdump("Akinator.gv");
  Base_.mcroot();
  system("xdot Akinator.gv");
  }

#define DELETEDESCR(descr) \
  for(int i = 0; descr[i] != NULL; ++i) \
    if(!descr[i])delete [] descr[i]; \
  if(!descr)delete [] descr

void Akinator::definition()
  {
  printf("Enter name to search:[Enter name]:");
  char* name;
  name = gets();                                    //gets
  Base_.mcroot();
  char ** descr = NULL;
  descr = getdescription(name);
  if(!descr)printf("'%s' not found\n", name);
  else
    {
    printf("Description of '%s':", name);
    for(int i = 0; descr[i] != NULL; ++i)
      printf("%s, ", descr[i]);
    DELETEDESCR(descr);
    }
  if(!name)delete [] name;
  }

void Akinator::compare()
  {
  printf("Please, enter first name to compare[Enter name]:");
  char* name1 = gets();                                    //gets
  char** descr1 = getdescription(name1);
  if(!descr1)printf("'%s' not found", name1);
  else
    {
    printf("Please, enter another one[Enter name]:");
    char* name2 = gets();                                   //gets
    char** descr2 = getdescription(name2);
    if(!descr2)printf("'%s' not found", name2);
    else
      {
      printf("%s's description: ", name1); 
      int i = 0;
      for(i = 0; descr1[i] != NULL; i++)
        printf("%s, ", descr1[i]);
      int size1 = i;
      printf("\n%s`s description: ", name2);
      for(i = 0; descr2[i] != NULL; i++)
        printf("%s, ", descr2[i]);
      int size2 = i;
      if((size1 * size2 != 0) && strcmp(descr1[size1 - 1], descr2[size2 - 1]))
        {
        printf("\nSeems they have nothing in common\n");
        }
      else if(size1 * size2 != 0)
        {
        printf("\nThey have some similar features: ");
        while(true)
          {
          if(size1 <= 0 || size2 <= 0) break;
          if(strcmp(descr1[size1 - 1], descr2[size2 - 1])) break;
          else 
            {
            printf("%s, ", descr1[size1 - 1]);
            --size1;
            --size2;
            }
          }
        printf("....and seems thats all\n");
        }
//--------------------------------------
      DELETEDESCR(descr2);
      }
   DELETEDESCR(descr1);
   if(!name2)delete [] name2;
   }
  if(!name1)delete [] name1;
  }

#undef DELETEDESCR
char** Akinator::getdescription(const char* name)
  {
  Node* wanted = Base_.search(name);
  if(!wanted)return NULL;
  else
    {
    int capacity = DSCR_CAP;
    int size = 0;
    char** descr = new char*[capacity];
    Node* prev;
    while(Base_.getcursor() != Base_.Root())
      {
      prev = Base_.getcursor();
      Base_.mcabove();
      if(Base_.getcursor()->left() == prev)
        {
        char* buf = (char*)malloc((strlen(Base_.getcursor()->data()) + 10)* sizeof(char));
        buf[0] = 'n';
        buf[1] = 'o';
        buf[2] = 't';
        buf[3] = ' ';
        buf[4] = '\0';
        strcat(buf, Base_.getcursor()->data());
        descr[size++] = buf;
        }
      else
        {
        char* buf = (char*)malloc((strlen(Base_.getcursor()->data()) + 5)* sizeof(char));
        strcpy(buf, Base_.getcursor()->data());
        descr[size++] = buf;
        }
      if(size == capacity)
        {
        capacity *= 2;
        descr = (char**)realloc(descr, capacity * sizeof(char*));
        }
      }
    descr[size] = NULL;
    return descr;
    }
  }

int main()
  {
  Akinator aki;
  printf(INFO);
  while(true)
    {
    if(aki.menu() != EXIT)
      {
      aki.exit();
      }
    if(aki.Akinator_global_exit) break;
    }
  return 0;  
  }

