#ifndef GTKSTRUCT_H
#define GTKSTRUCT_H

#include <gtk/gtk.h>

typedef struct DbWindow DbWindow;
typedef struct MainWindow MainWindow;
typedef struct CallbackMessage CallbackMessage;
typedef struct AttackStack AttackStack;
typedef struct LogsWindow LogsWindow;
typedef struct LogEntry LogEntry;

struct LogEntry
{
  char attackId [500];
  char time [500];
  char host [500];
  char port [500];
  char method [1000];
  char success [1000];
  char pwdDump [255];

  FILE * exportFile;
};

struct LogsWindow
{
  GtkWidget * closeButton;
  GtkWidget * scrolledWindow;
  GtkWidget * window;
  GtkWidget * label;
  GtkWidget * box;

  GtkWidget * logsTreeView;
  GtkListStore * model;
};

struct AttackStack {
  char ** passwordList;
  long offset;
  struct MainWindow * handler;
  long numberOfPasswords;
  FILE * file;
  long port;
  const gchar * host;
  char username [255];
  char destination [255];
  char errorMessage [1000];
  int attackId;

  unsigned long long bruteIter;
  unsigned long long bruteCount;
  char brutePwd [255];

  char dic;
  char brute;
};

struct DbWindow {
  // parties de la fenêtre de création de db.
  GtkWidget * window;
  GtkWidget * box;
  GtkWidget * label;
  GtkWidget * inputs[2];
  GtkWidget * button;
};

struct CallbackMessage {

  struct MainWindow * handle;
  char message [500];
  char tagname[500];
};

struct MainWindow{

  GtkWidget * window;

  // fenêtre de logs
  struct LogsWindow * logsWindow;

  // parties de la fenêtre de login.
  GtkWidget * box;
  GtkWidget * button;
  GtkWidget * input;
  GtkWidget * label;

  // parties de la fenêtre principale.

  // conteneur principal.

  GtkWidget * grid;

  // target options widgets.

  GtkWidget * targetLabel;
  GtkWidget * protoLabel;
  GtkWidget * protoList;
  GtkWidget * hostLabel;
  GtkWidget * hostInput;
  GtkWidget * portLabel;
  GtkWidget * portInput;
  GtkWidget * threadsLabel;
  GtkWidget * threadsScale;

  // method options widgets.

  GtkWidget * methodLabel;
  GtkWidget * dicRadio;
  GtkWidget * fileButton;
  GtkWidget * fileInputLabel;

  GtkWidget * bruteRadio;
  GtkWidget * bruteLengthStart;
  GtkWidget * bruteLengthEnd;
  GtkWidget * bruteUpperCase;
  GtkWidget * bruteNumbers;
  GtkWidget * bruteSpecialChars;

  // console
  GtkWidget * console;
  GtkWidget * consoleWindow;
  GtkTextBuffer * consoleBuffer;
  GtkAdjustment * consoleAdjustment;

  // boutons de contrôle
  GtkWidget * startButton;
  GtkWidget * stopButton;
  GtkWidget * exportButton;
  GtkWidget * logsButton;

  // data du contexte GUI
  char optMethodBrute;
  char optMethodDic;
  char optMethodBruteUppercase;
  char optMethodBruteNumbers;
  char optMethodBruteSpecialChars;

  //filename
  gchar * filename;
  long crCount;

  // état de l'attaque
  char stopped;

  char minLength;
  char maxLength;
  gint threads;

  long neededThreads;
  long threadsLaunched;

  // on stock les ID des threads dans ces variables, le thread principal de création de threads,
  // et les n threads que l'utilisateur a choisi, on met 1024 en maximum car en théorie pthread peut
  // gérer jusqu'à 1024 threads, bien sûr le maximum de threads fixé dans l'application est bien moins
  // elevé.
  pthread_t mainThread;
  pthread_t threadsId [1024];
  int runningThreads;

  char  dbPassword [255];

  char authorizedChars [500];

  unsigned long long bruteIter;
  unsigned long long bruteCount;
};

#endif
