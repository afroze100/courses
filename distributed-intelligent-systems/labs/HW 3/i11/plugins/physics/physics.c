/* This file is necessary to make the shared library work under Windows and Mac OS X */
#ifndef LINUX
dGeomID  (*dWebotsGetGeomFromDEFProc)(const char *)=NULL;
void     (*dWebotsSendProc)(int,void *,int)=NULL;
void*    (*dWebotsReceiveProc)(int *)=NULL;

DLLEXPORT void physics_special_init(
  dGeomID (*dWebotsGetGeomFromDEFProcA)(const char *),
  void    (*dWebotsSendProcA)(int,void *,int),
  void*   (*dWebotsReceiveProcA)(int *)) {
  dWebotsGetGeomFromDEFProc=dWebotsGetGeomFromDEFProcA;
  dWebotsSendProc=dWebotsSendProcA;
  dWebotsReceiveProc=dWebotsReceiveProcA;
}
#endif
