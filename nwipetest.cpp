#include <iostream>
#include <parted/parted.h>

int main(){
	std::cout<<"yo"<<std::endl;
	r = write( c->device_fd, &b[w], blocksize );

		/* Check the result for a fatal error. */
		if( r < 0 )
		{
			nwipe_perror( errno, __FUNCTION__, "write" );
			nwipe_log( NWIPE_LOG_FATAL, "Unable to write to '%s'.", c->device_name );
			return -1;
}
	return 0;

}
