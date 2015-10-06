#include <stdio.h>
#include <ode/ode.h>
#include <plugins/physics.h>
#include "../physics.c" /* makes the shared library work under Windows & Mac */
#include <GL/gl.h>
#include <GL/glu.h>

//extern robot *robots;
//extern struct eventlist *events;
//extern int n_activerobots;
int n_activerobots = 1;

void webots_physics_init(dWorldID w,dSpaceID s,dJointGroupID j) {}
void webots_physics_step() {}
int webots_physics_collide(dGeomID g1,dGeomID g2) { return 0;}
void webots_physics_cleanup() {}

void webots_physics_draw(){
  int size, nfloats, i;
  void *buff=NULL;
  float *fbuff;

  //receive line strip from supervisor
  if ((buff = dWebotsReceive(&size)) == NULL)
    return;
  nfloats = (size/sizeof(float));
  //printf("Physics: received %d floats\n", nfloats);
  fbuff = (float *) buff;

  glDisable(GL_LIGHTING);
  glLineWidth(5);
  glColor3f(1,0,0);

  glBegin(GL_LINE_STRIP);
  for (i=0; i<nfloats;){
    if (fbuff[i] == -1){
      //printf("NEW STRIP\n");
      glEnd();
      glBegin(GL_LINE_STRIP);
      i++;
    }else{
      glVertex3f(fbuff[i], 0, fbuff[i+1]);
      //printf("\t %.2f %.2f\n", fbuff[i], fbuff[i+1]);
      i+=2;
    }
  }
  glEnd();
  glColor3f(1,1,1);
  glEnable(GL_LIGHTING);
}
