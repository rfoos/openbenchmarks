#include	"config.h"

#if 	!HAVE_TIME && HAVE_CLOCK
#if 	HAVE_TIME_H
#include <time.h>
#else
typedef long time_t
time_t	time(time_t *_timer);
#endif

time_t	   time(time_t *_timer)
{
	return(clock()/1000);
}
#endif
