#define SPTR_SIZE   20

char   *sPtr [SPTR_SIZE];             // this is reused each call
char *strData = NULL;                 // this is allocated in separate and needs to be free( ) eventually
size_t numberOfStr = 0;               // number of valid elements in sPtr[  ]

// use this free the memory after use
void freeData(char **pdata) {
  free(*pdata);
  *pdata = NULL;
  numberOfStr = 0;
}

int separate (String& str,                                  // pass as a reference to avoid double memory usage
              char **p,  int size, char **pdata ) {
  int  n = 0;
  free(*pdata);                                             // clean up last data as we are reusing sPtr[ ]
  // BE CAREFUL not to free it twice
  // calling free on NULL is OK
  *pdata = strdup(str.c_str());                             // allocate new memory for the result.
  if (*pdata == NULL) {
    Serial.println("OUT OF MEMORY");
    return 0;
  }
  *p++ = strtok (*pdata, " ");
  for (n = 1; NULL != (*p++ = strtok (NULL, " ")); n++) {
    if (size == n) {
      break;
    }
  }
  return n;
}
