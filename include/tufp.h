#ifndef TUFP_H
#define TUFP_H
#define TUFP_MAX_FIELDS 15
#define TUGP_MAX_FIELD_SIZE 25
// "TUFP" (Text-based UART Framed Protocol)
typedef struct tufp_st{
  int state;
  int i,j;
  char field[TUFP_MAX_FIELDS][TUGP_MAX_FIELD_SIZE];
  char buf[10];
  char check_sum_rcv[TUGP_MAX_FIELD_SIZE];
  int check_sum_calc;
  int pending;
  int error;
  int overflow;
  int rcode;
  int scode;
  int snum;
} tufp_st;
// process the frame byte by byte
void tufp_rcv(tufp_st *tufp_s,char ch);
// create the frame and load it in the fram buffer provided by user
void tufp_frame(tufp_st *tufp_s, char frame[100]);
#endif