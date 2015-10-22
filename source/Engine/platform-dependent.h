/* These are the functions which have different versions for 
 * the different operating systems.
 */
#include <stdint.h>

#ifdef _WIN32
#define PATHSLASH '\\'
#else
#define PATHSLASH '/'
#endif

char * grabFileName ();
int showSetupWindow();

void msgBox (const char * head, const char * msg);
int msgBoxQuestion (const char * head, const char * msg);

void changeToUserDir ();
uint32_t launch(char * filename);

bool defaultUserFullScreen();
