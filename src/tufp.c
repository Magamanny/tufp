#include "tufp.h"

// process the tufp frame byte by byte
void tufp_rcv(tufp_st *tufp_s,char ch)
{
  switch((*tufp_s).state)
  {
  case 0:
    if(ch=='@')
    {
      (*tufp_s).state=10;
      (*tufp_s).i=0;
      (*tufp_s).j=0;
      (*tufp_s).check_sum_calc=0;
    }
    break;
  case 10:
    if(ch==',')
    {
      (*tufp_s).check_sum_calc += ch;
      (*tufp_s).buf[(*tufp_s).i]=0;
      (*tufp_s).i=0;
      (*tufp_s).rcode = strToInt((*tufp_s).buf);
      (*tufp_s).state=20;
    }
    // some packets only have a code
    else if(ch=='|')
    {
      (*tufp_s).buf[(*tufp_s).i]=0;
      (*tufp_s).i=0;
      (*tufp_s).rcode = strToInt((*tufp_s).buf);
      (*tufp_s).state=30;
    }
    // if start of packet restart the reciving state
    else if(ch=='@')
    {
      (*tufp_s).state=10;
      (*tufp_s).i=0;
      (*tufp_s).j=0;
      (*tufp_s).check_sum_calc=0;
    }
    // if end of packet, stop the state,
    else if(ch=='#')
    {
      (*tufp_s).state=0;
    }
    else
    {
      (*tufp_s).check_sum_calc += ch;
      (*tufp_s).buf[(*tufp_s).i]=ch;
      (*tufp_s).i++;
      if((*tufp_s).i>=TUGP_MAX_FIELD_SIZE)
      {
        (*tufp_s).overflow=1;
        (*tufp_s).state=0;
      }
    }
    break;
  case 20:
    if(ch==',')
    {
      (*tufp_s).check_sum_calc += ch;
      (*tufp_s).field[(*tufp_s).j][(*tufp_s).i]=0;
      (*tufp_s).i=0;
      (*tufp_s).j++;
      if((*tufp_s).j>=TUFP_MAX_FIELDS)
      {
        (*tufp_s).overflow=2;
        (*tufp_s).state=0;
      }
    }
    else if(ch=='|')
    {
      (*tufp_s).field[(*tufp_s).j][(*tufp_s).i]=0;
      (*tufp_s).i=0;
      (*tufp_s).j++;
      (*tufp_s).state=30;
    }
    else if(ch=='@')
    {
      (*tufp_s).state=10;
      (*tufp_s).i=0;
      (*tufp_s).j=0;
      (*tufp_s).check_sum_calc=0;
    }
    else
    {
      (*tufp_s).check_sum_calc += ch;
      (*tufp_s).field[(*tufp_s).j][(*tufp_s).i]=ch;
      (*tufp_s).i++;
      if((*tufp_s).i>=TUGP_MAX_FIELD_SIZE)
      {
        (*tufp_s).overflow=3;
        (*tufp_s).state=0;
      }
    }
    break;
  case 30:
    if(ch=='#')
    {
      (*tufp_s).state=0;
      (*tufp_s).check_sum_rcv[(*tufp_s).i]=0;
      intToStr((*tufp_s).check_sum_calc,(*tufp_s).buf,0);
      
      for(int i=0;i<10;i++)
      {
        if((*tufp_s).buf[i]==0)
        {
          break;
        }
        else if((*tufp_s).buf[i]!=(*tufp_s).check_sum_rcv[i])
        {
          (*tufp_s).error=1;
          break;
        }
      }
      (*tufp_s).pending=1;
    }
    else if(ch=='@')
    {
      (*tufp_s).state=10;
      (*tufp_s).i=0;
      (*tufp_s).j=0;
      (*tufp_s).check_sum_calc=0;
    }
    else
    {
      (*tufp_s).check_sum_rcv[(*tufp_s).i]=ch;
      (*tufp_s).i++;
      if((*tufp_s).i >=TUGP_MAX_FIELD_SIZE)
      {
        (*tufp_s).overflow=4;
        (*tufp_s).state=0;
      }
    }
    break;
  }
}
// tufp frame create
void tufp_frame(tufp_st *tufp_s, char frame[100])
{
  int n=0;
  intToStr((*tufp_s).scode,(*tufp_s).buf,0);
  (*tufp_s).check_sum_calc = 0;
  frame[n++] = '@';
  for(int i=0;i<10;i++)
  {
    if((*tufp_s).buf[i]==0)
    {
      break;
    }
    else
    {
      frame[n++] = (*tufp_s).buf[i];
      (*tufp_s).check_sum_calc += (*tufp_s).buf[i];
    }
  }
  for(int j=0;j<(*tufp_s).snum;j++)
  {
    frame[n++] = ',';
    (*tufp_s).check_sum_calc += ',';
    for(int i=0;i<15;i++)
    {
      if((*tufp_s).field[j][i]==0)
      {
        break;
      }
      else
      {
         frame[n++] = (*tufp_s).field[j][i];
        (*tufp_s).check_sum_calc += (*tufp_s).field[j][i];
      }
    }
  }
  frame[n++] = '|';
  intToStr((*tufp_s).check_sum_calc,(*tufp_s).buf,0);
  for(int i=0;i<10;i++)
  {
    if((*tufp_s).buf[i]==0)
    {
      break;
    }
    else
    {
      frame[n++] = (*tufp_s).buf[i];
    }
  }
  frame[n++] = '#';
  frame[n++] = 0;
}