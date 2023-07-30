#include <stdio.h>
#include <stdlib.h>

typedef struct {
  void (*update)(int);
} Observer;

typedef struct {
  Observer** observers;
  int observer_count;
  int state;
} Subject;

void add_observer(Subject* subject, Observer* observer) {
  printf("add observer count %d %p\n", subject->observer_count, observer);
  subject->observers[subject->observer_count++] = observer;
  
  (void) (reallocarray( 
		       subject->observers, 
		       subject->observer_count, 
		       sizeof(Observer*))); 

  printf("--after add observer count %d %p %p\n",
	 subject->observer_count,
	 subject,
	 subject->observers[subject->observer_count - 1]
	 );
}

void set_state(Subject* subject, int state) {
  printf("set state %d, subject %p, observer count: %d\n",state, subject, subject->observer_count);
  subject->state = state;
  for(int i = 0; i < subject->observer_count; i++) {
    printf("notify observer %p\n", &(subject->observers[i]));
    subject->observers[i]->update(state);
  }
}

void print_on_update(int state) {
  printf("state: %d\n", state);
}

void log_on_update(int state) {
  printf("log \n");
  FILE *f = fopen("abc.txt", "a");
  fprintf(f, "state: %d\n", state);
  fclose(f);
}
