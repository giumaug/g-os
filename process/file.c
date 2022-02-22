#include "lib/lib.h"  
#include "file.h"

int main(int argc, char* argv[])
{
	int tot_read = 0;
	int f;
	int i;

	const char * files[] = {
    "/yyy/xxx1.txt",
	"/yyy/xxx2.txt",
	"/yyy/xxx3.txt",
	"/yyy/xxx4.txt",
	"/yyy/xxx5.txt",
	"/yyy/xxx6.txt",
	"/yyy/xxx7.txt",
	"/yyy/xxx8.txt",
	"/yyy/xxx9.txt",
	"/yyy/xxx10.txt",
	"/yyy/xxx11.txt",
	"/yyy/xxx12.txt",
	"/yyy/xxx13.txt",
	"/yyy/xxx14.txt",
	"/yyy/xxx15.txt",
	"/yyy/xxx16.txt",
	"/yyy/xxx17.txt",
	"/yyy/xxx18.txt",
	"/yyy/xxx19.txt",
	"/yyy/xxx20.txt",
	"/yyy/xxx21.txt",
	"/yyy/xxx22.txt",
	"/yyy/xxx23.txt",
	"/yyy/xxx24.txt",
	"/yyy/xxx25.txt",
	"/yyy/xxx26.txt",
	"/yyy/xxx27.txt",
	"/yyy/xxx28.txt",
	"/yyy/xxx29.txt",
	"/yyy/xxx30.txt",
	"/yyy/xxx31.txt",
	"/yyy/xxx32.txt",
	"/yyy/xxx33.txt",
	"/yyy/xxx34.txt",
	"/yyy/xxx35.txt",
	"/yyy/xxx36.txt",
	"/yyy/xxx37.txt",
	"/yyy/xxx38.txt",
	"/yyy/xxx39.txt",
	"/yyy/xxx40.txt",
	"/yyy/xxx41.txt",
	"/yyy/xxx42.txt",
	"/yyy/xxx43.txt",
	"/yyy/xxx44.txt",
	"/yyy/xxx45.txt",
	"/yyy/xxx46.txt",
	"/yyy/xxx47.txt",
	"/yyy/xxx48.txt",
	"/yyy/xxx49.txt",
	"/yyy/xxx50.txt",
	"/yyy/xxx51.txt",
	"/yyy/xxx52.txt",
	"/yyy/xxx53.txt",
	"/yyy/xxx54.txt",
	"/yyy/xxx55.txt",
	"/yyy/xxx56.txt",
	"/yyy/xxx57.txt",
	"/yyy/xxx58.txt",
	"/yyy/xxx59.txt",
	"/yyy/xxx60.txt",
	"/yyy/xxx61.txt",
	"/yyy/xxx62.txt",
	"/yyy/xxx63.txt",
	"/yyy/xxx64.txt",
	"/yyy/xxx65.txt",
	"/yyy/xxx66.txt",
	"/yyy/xxx67.txt",
	"/yyy/xxx68.txt",
	"/yyy/xxx69.txt",
	"/yyy/xxx70.txt",
	"/yyy/xxx71.txt",
	"/yyy/xxx72.txt",
	"/yyy/xxx73.txt",
	"/yyy/xxx74.txt",
	"/yyy/xxx75.txt",
	"/yyy/xxx76.txt",
	"/yyy/xxx77.txt",
	"/yyy/xxx78.txt",
	"/yyy/xxx79.txt",
	"/yyy/xxx80.txt",
	"/yyy/xxx81.txt",
	"/yyy/xxx82.txt",
	"/yyy/xxx83.txt",
	"/yyy/xxx84.txt",
	"/yyy/xxx85.txt",
	"/yyy/xxx86.txt",
	"/yyy/xxx87.txt",
	"/yyy/xxx88.txt",
	"/yyy/xxx89.txt",
	"/yyy/xxx90.txt",
	"/yyy/xxx91.txt",
	"/yyy/xxx92.txt",
	"/yyy/xxx93.txt",
	"/yyy/xxx94.txt",
	"/yyy/xxx95.txt",
	"/yyy/xxx96.txt",
	"/yyy/xxx97.txt",
	"/yyy/xxx98.txt",
	"/yyy/xxx99.txt",
	"/yyy/xxx100.txt",
	"/yyy/xxx101.txt",
	"/yyy/xxx102.txt",
	"/yyy/xxx103.txt",
	"/yyy/xxx104.txt",
	"/yyy/xxx105.txt",
	"/yyy/xxx106.txt",
	"/yyy/xxx107.txt",
	"/yyy/xxx108.txt",
	"/yyy/xxx109.txt",
	"/yyy/xxx110.txt",
	"/yyy/xxx111.txt",
	"/yyy/xxx112.txt",
	"/yyy/xxx113.txt",
	"/yyy/xxx114.txt",
	"/yyy/xxx115.txt",
	"/yyy/xxx116.txt",
	"/yyy/xxx117.txt",
	"/yyy/xxx118.txt",
	"/yyy/xxx119.txt",
	"/yyy/xxx120.txt",
	"/yyy/xxx121.txt",
	"/yyy/xxx122.txt",
	"/yyy/xxx123.txt",
	"/yyy/xxx124.txt",
	"/yyy/xxx125.txt",
	"/yyy/xxx126.txt",
	"/yyy/xxx127.txt",
	"/yyy/xxx128.txt",
	"/yyy/xxx129.txt",
	"/yyy/xxx130.txt",
	"/yyy/xxx131.txt",
	"/yyy/xxx132.txt",
	"/yyy/xxx133.txt",
	"/yyy/xxx134.txt",
	"/yyy/xxx135.txt",
	"/yyy/xxx136.txt",
	"/yyy/xxx137.txt",
	"/yyy/xxx138.txt",
	"/yyy/xxx139.txt",
	"/yyy/xxx140.txt",
	"/yyy/xxx141.txt",
	"/yyy/xxx142.txt",
	"/yyy/xxx143.txt",
	"/yyy/xxx144.txt",
	"/yyy/xxx145.txt",
	"/yyy/xxx146.txt",
	"/yyy/xxx147.txt",
	"/yyy/xxx148.txt",
	"/yyy/xxx149.txt",
	"/yyy/xxx150.txt",
	"/yyy/xxx151.txt",
	"/yyy/xxx152.txt",
	"/yyy/xxx153.txt",
	"/yyy/xxx154.txt",
	"/yyy/xxx155.txt",
	"/yyy/xxx156.txt",
	"/yyy/xxx157.txt",
	"/yyy/xxx158.txt",
	"/yyy/xxx159.txt",
	"/yyy/xxx160.txt",
	"/yyy/xxx161.txt",
	"/yyy/xxx162.txt",
	"/yyy/xxx163.txt",
	"/yyy/xxx164.txt",
	"/yyy/xxx165.txt",
	"/yyy/xxx166.txt",
	"/yyy/xxx167.txt",
	"/yyy/xxx168.txt",
	"/yyy/xxx169.txt",
	"/yyy/xxx170.txt",
	"/yyy/xxx171.txt",
	"/yyy/xxx172.txt",
	"/yyy/xxx173.txt",
	"/yyy/xxx174.txt",
	"/yyy/xxx175.txt",
	"/yyy/xxx176.txt",
	"/yyy/xxx177.txt",
	"/yyy/xxx178.txt",
	"/yyy/xxx179.txt",
	"/yyy/xxx180.txt",
	"/yyy/xxx181.txt",
	"/yyy/xxx182.txt",
	"/yyy/xxx183.txt",
	"/yyy/xxx184.txt",
	"/yyy/xxx185.txt",
	"/yyy/xxx186.txt",
	"/yyy/xxx187.txt",
	"/yyy/xxx188.txt",
	"/yyy/xxx189.txt",
	"/yyy/xxx190.txt",
	"/yyy/xxx191.txt",
	"/yyy/xxx192.txt",
	"/yyy/xxx193.txt",
	"/yyy/xxx194.txt",
	"/yyy/xxx195.txt",
	"/yyy/xxx196.txt",
	"/yyy/xxx197.txt",
	"/yyy/xxx198.txt",
	"/yyy/xxx199.txt"
	};

	char *io_buffer = "Look Here!!!!";
	
	mkdir("/yyy");
	for ( i = 0 ; i < 199; i++)
	{
		f = open(files[i], O_CREAT | O_RDWR);                                      
		write(f,io_buffer, 13);
		close(f);
	}

//	for ( i = 51 ; i < 102; i++)
//	{
//		remove(files[i]);
//	}

	for ( i = 0 ; i < 199; i++)
	{
		remove(files[i]);
	}
	remove("/yyy");

	exit(0);
}
