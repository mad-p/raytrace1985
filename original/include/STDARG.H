/**************************************************

		    "stdarg.h"

	STANDARD VARIABLE ARGUMENT HEADER
		for  Optimizing C86
		   by   K.Maeda

		Version 1.0  1-Feb-86

**************************************************/

typedef	struct	{
	bool	 va_active;
	void	*va_next;
}	va_list;

#define	va_start(ap, paraN)	{ ap.va_active = true;  \
				  ap.va_next = (void *)(&paraN + 1); }

#define va_arg(ap, type)	(ap.va_active ? *((type *)ap.va_next)++ : \
				 abort("va_arg : Illegal va_list") )

#define va_end(ap)		  if(ap.va_active)			   \
					ap.va_active = false;		   \
				    else				   \
				  	abort("va_end : Illegal va_list");

/*********************************

	End of "stdarg.h"

*********************************/
