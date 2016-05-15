#include<unistd.h>
#include<stdio.h>

static int count=1; //cout of probe
static char list[20][255];

int get_the_probe_num(char  *arg)
{
char *ar = arg;
int i =0;
int j=0;

	while(*ar!='\0')
	{
        list[i][j] = *ar;
		if(*ar ==',')
		{
			count++;
            list[i][j] = '\0';//one func is end 
            i++;
            j=0;
            ar++;
		}
        else
        {
            ar++;
            j++;
        }
	}
    list[i][j]='\0';
	
	printf("number of probe is %d \n", count);

    for(i=0;i<count;i++)
    {

        printf("list: = %s\n",list[i]);

     }       
     return 0;

}

int main(int argc, char **argv)
{
    
    
    printf("%s\n",argv[1]);
    get_the_probe_num(argv[1]);
    return 0;
}

