#include <stdlib.h>
#include <ctype.h>
#include "lib/stringx.h"
#include <string.h>

double
strtod (char *str, char **ptr)
{
  char *p;

  if (ptr == (char **)0){
    //printf("0:%s\n",str);
    return atof (str);
  }
  p = str;
  
  while (isspace (*p))
    ++p;
  
  // negatif işaret kontrolü başında yer alması
  // - 5 ise string kabul edilir, negatif olmaz.
  int neg = 1;
  if (*p == '-' && str[1] != ' ')
  {   
	  //printf("1:%s\n",str);
	  neg = -1;
	  str[0]='0';
  }
  
  if (*p == '+' || *p == '-')
    ++p;
  
  

  /* INF or INFINITY.  */
  if ((p[0] == 'i' || p[0] == 'I')
      && (p[1] == 'n' || p[1] == 'N')
      && (p[2] == 'f' || p[2] == 'F'))
    {
      if ((p[3] == 'i' || p[3] == 'I')
	  && (p[4] == 'n' || p[4] == 'N')
	  && (p[5] == 'i' || p[5] == 'I')
	  && (p[6] == 't' || p[6] == 'T')
	  && (p[7] == 'y' || p[7] == 'Y'))
	{
	  *ptr = p + 8;
	  //printf("2:%s\n",str);
	  return atof (str) * neg;
	}
      else
	{
	  *ptr = p + 3;
	  //printf("3:%s\n",str);
	  return atof (str) * neg;
	}
    }

  /* NAN or NAN(foo).  */
  if ((p[0] == 'n' || p[0] == 'N')
      && (p[1] == 'a' || p[1] == 'A')
      && (p[2] == 'n' || p[2] == 'N'))
    {
      p += 3;
      if (*p == '(')
	{
	  ++p;
	  while (*p != '\0' && *p != ')')
	    ++p;
	  if (*p == ')')
	    ++p;
	}
      *ptr = p;
      //printf("4:%s\n",str);
      return atof (str) *neg;
    }

  /* digits, with 0 or 1 periods in it.  */
  if (isdigit (*p) || *p == '.')
    {
      int got_dot = 0;
      while (isdigit (*p) || (!got_dot && *p == '.'))
	{
	  if (*p == '.')
	    got_dot = 1;
	  ++p;
	}

      /* Exponent.  */
      if (*p == 'e' || *p == 'E')
	{
	  int i;
	  i = 1;
	  if (p[i] == '+' || p[i] == '-')
	    ++i;
	  if (isdigit (p[i]))
	    {
	      while (isdigit (p[i]))
		++i;
	      *ptr = p + i;
	      //printf("5:%s\n",str);
	      return atof (str) * neg;
	    }
	}
      *ptr = p;
      //printf("6:%s:%s:\n",str,p);
      return atof (str) *neg;
    }
  /* Didn't find any digits.  Doesn't look like a number.  */
  *ptr = str;
  return 0.0;
}

size_t
strcspn (const char *string, const char *reject)
{
  size_t count = 0;
  while (strchr (reject, *string) == 0)
    {
      ++count, ++string;
    }

  return count;
}
