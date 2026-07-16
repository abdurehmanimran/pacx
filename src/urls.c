#include "urls.h"
#include "colors.h"
#include "packageattr.h"
#include "packagelist.h"
#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MIRROR_URLS 5

String *getRawMirrors(char *path, packageAttr *attrs) {
  String *output;
  int count = 0;
  allocString(&output, 512);
  FILE *mirrorlist = fopen(path, "r");

  if (mirrorlist == NULL) {
    puts("Error: failed to open the mirrorlist !!");
    exit(1);
  }

  char temp_buffer[512];
  memset(temp_buffer, 0, sizeof(temp_buffer));

  while (fgets(temp_buffer, sizeof(temp_buffer), mirrorlist) != NULL &&
         count < MIRROR_URLS) {
    if (strcspn(temp_buffer, "http") == 9) {
      char *url = temp_buffer + 9;

      int line_end_index = strcspn(url, "\n");
      if (line_end_index >= 511)
        line_end_index = 510;
      url[line_end_index] = '\0';

      stringAppend(&output, url);
      fillURLPlaceholders(&output, attrs);
      stringAppend(&output, "/");
      stringAppend(&output, attrs->fileName);
      stringAppend(&output, " ");

      memset(temp_buffer, 0, sizeof(temp_buffer));
      count++;
    }
  }

  output->str[output->size - 1] = '\0';
  fclose(mirrorlist);
  return output;
}

void fillURLPlaceholders(String **urls, packageAttr *attrs) {
  if (!urls || (*urls)->size == 0)
    return;

  replaceInString(urls, "$arch", ARCHITECTURE);
  replaceInString(urls, "$repo", attrs->repo);
}

mirrorTable *initMirrorTable() {
  mirrorTable *table = (mirrorTable *)malloc(sizeof(mirrorTable));
  if (!table) {
    printf(RED "Error: " WHITE
               "failed to allocate memory for mirrorTable !!\n");
    exit(1);
  }

  table->n = 0;
  table->capacity = 8;
  table->repos = (repoURLs **)malloc(sizeof(repoURLs *) * table->capacity);

  if (!table->repos) {
    printf(RED "Error: " WHITE
               "failed to allocate memory for mirrorTable's repoList !!\n");
    free(table);
    exit(1);
  }

  return table;
}

// Also reposible for freeing the repoURLs inside it
void freeTable(mirrorTable *table) {

  for (unsigned int i = 0; i < table->n; i++) {
    freeString(table->repos[i]->repoName);
    freeString(table->repos[i]->mirrorUrls);
    free(table->repos[i]);
  }

  if (table->repos)
    free(table->repos);

  if (table)
    free(table);
}

void expandTable(mirrorTable **table) {
  (*table)->capacity += 8;

  repoURLs **temp =
      realloc((*table)->repos, sizeof(repoURLs *) * (*table)->capacity);

  if (!temp) {
    printf(RED "Error: " WHITE "failed to expand mirrorTable's repoList !!\n");
    freeTable(*table);
    exit(1);
  }

  (*table)->repos = temp;
}

void addRepoInTable(mirrorTable **table, String *name, String *mirrors) {
  repoURLs *repoEntry = (repoURLs *)malloc(sizeof(repoURLs));
  repoEntry->repoName = name; // Already allocated
  repoEntry->mirrorUrls = mirrors;

  while ((*table)->n + 1 > (*table)->capacity)
    expandTable(table);

  (*table)->repos[(*table)->n] = repoEntry;
  (*table)->n++;
}

void createMirrorTable(packageInfoList *dbList, mirrorTable **table) {
  *table = initMirrorTable();

  for (unsigned int i = 0; i < dbList->n; i++) {
    packageAttr *attrs = initPackageAttr();

    String *repoName = createString(dbList->packages[i]->packageName);
    replaceInString(&repoName, ".db", "\0");

    attrs->fileName = strdup("${name}$");
    attrs->repo = strdup(repoName->str);
    attrs->url = strdup("repo");
    freeString(repoName);

    String *mirrorListPath = getMirrorListPath(attrs);
    String *name = createString(attrs->repo);
    String *urls = getRawMirrors(mirrorListPath->str, attrs);

    addRepoInTable(table, name, urls);

    // Cleanup
    // name & urls will be freed by the func dealing with mirrorTable
    freeString(mirrorListPath);
    freePackageAttr(attrs);
  }
}

String *getUrls(mirrorTable *table, char *packageName) {
  String *urls = NULL;
  packageAttr *attrs = NULL;
  if (strstr(packageName, ".db") == NULL)
    attrs = getPackageAttr(packageName);
  else {
    attrs = initPackageAttr();
    String *repoName = createString(packageName);
    replaceInString(&repoName, ".db", "\0");

    attrs->fileName = strdup(packageName);
    attrs->repo = strdup(repoName->str);
    attrs->url = strdup("repo");
    freeString(repoName);
  }

  for (unsigned i = 0; i < table->n; i++)
    if (strcmp(table->repos[i]->repoName->str, attrs->repo) == 0) {
      if (strstr(attrs->url, "file")) {
        urls = createString("file");
        break;
      }

      urls = createString(table->repos[i]->mirrorUrls->str);
      replaceInString(&urls, "${name}$", attrs->fileName);

      break;
    }

  if (attrs)
    freePackageAttr(attrs);
  if (!urls) {
    printf(RED
           "Error: " WHITE
           "nothing matched in the mirror table for this package or repo !!\n");
    exit(1);
  }
  return urls;
}
