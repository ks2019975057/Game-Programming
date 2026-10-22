#include <stdio.h>
#include <math.h>
#include <windows.h>
int calc frequency(int octave, int inx);
int main (void)
{
	//int index[]={0, 2, 4, 5, 7, 9, 11, 12};
int song[] = { 7, 7, 9, 9, 7, 7, 4,
            	7, 7, 4, 4, 2,
                7, 7, 9, 9, 7, 7, 4,
                7, 4, 2, 4, 0 };


int duration[] = { 200, 200, 200, 200, 200, 200, 400,
                       200, 200, 200, 200, 400,
                       200, 200, 200, 200, 200, 200, 400,
                       200, 200, 200, 200, 400 };

    int freq[24];
    int i;

    for (i=0; i<24; i++)
        freq[i] = calc_frequency(4, song[i]);

    for (i=0; i<24; i++)
    {
        Beep(freq[i], duration[i]);
        Sleep(50);
    }
    return 0;
}

int calc_frequency (int octave, int inx)
{
	double do_scale=32.7032;
	double ratio=pow (2., 1/12.), temp;
	int i;
	temp=do_scale*pow (2, octave-1);
	for(i=0;i<inx;i++)
	{
		temp=(int) (temp+0.5);
		temp*=ratio;
	}
	return (int) temp;
}
int calc_frequency (int octave, int inx);
void practice_piano (void);
