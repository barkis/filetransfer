#include <cstdio>
#include <errno.h>

int main()	{
	int ret = rename("/home/matthew/temp/gl.h","/home/matthew/projects/cpp/linbak/rengl.h");
	if(ret)	{
		char mess[51];
		printf("rename failed, %d\n",errno);		
	}
	else	{
		printf("Success\n");
	}
}
