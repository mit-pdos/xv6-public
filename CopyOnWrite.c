#include "types.h"
#include "stat.h"
#include "user.h"

int a = 1;

void tutorial1()
{
    printf(1,"在還沒fork前free page數量：%d\n",getNumFreePages());
    printf(1,"Parent 和 Child 共同使用一個全域變數 a \n");
    printf(1,"========執行第一次fork========\n");
    int pid = fork();
    if(pid==0)
    {
        printf(1,"Child: a = %d\n",a);
        printf(1,"改變子行程a前free page數量：%d\n",getNumFreePages());
        a = 2;
        printf(1,"Child: a = %d\n",a);
        printf(1,"第一次改變子行程a後free page數量：%d\n",getNumFreePages());
        a = 4;
        printf(1,"Child: a = %d\n",a);
        printf(1,"第二次改變子行程a後free page數量：%d\n",getNumFreePages());
        exit();
    }
    printf(1,"Parent: a = %d\n",a);
    wait();
    printf(1,"Parent: a = %d\n",a);
    printf(1,"在等待子行程結束後free page數量：%d\n",getNumFreePages());
    return ;
}

void tutorial2()
{
    printf(1,"在還沒fork前free page數量：%d\n",getNumFreePages());
    printf(1,"========執行第一次fork========\n");
    if(fork()==0)
    {
	printf(1,"第一個子行程");
        exit();
    }
    else
    {
        printf(1,"在第二次fork前free page數量：%d\n",getNumFreePages());
        printf(1,"========執行第二次fork========\n");
        if(fork()==0)
        {
	    printf(1,"第二個子行程");
            printf(1,"改變第二個子行程的a前free page數量：%d\n",getNumFreePages());
            a = 5;
       	    printf(1,"Child: a = %d\n",a);
            printf(1,"改變第二個子行程的a後free page數量：%d\n",getNumFreePages());
            exit();
        }
        wait();
        printf(1,"在第一個子行程結束後free page數量：%d\n",getNumFreePages());
    }
    wait();
    printf(1,"在第二個子行程結束後free page數量：%d\n",getNumFreePages());
    return ;
}

void tutorial3()
{
    printf(1,"在還沒fork前free page數量：%d\n",getNumFreePages());
    printf(1,"========執行第一次fork========\n");
    int pid = fork();
    if(pid==0)
    {
        printf(1,"讓子行程sleep 4秒\n");
        sleep(4);
        printf(1,"第一次改變子行程a前free page數量：%d\n",getNumFreePages());
        a = 5;
        printf(1,"Child: a = %d\n",a);
        printf(1,"第一次改變子行程a後free page數量：%d\n",getNumFreePages());
        exit();
    }
    printf(1,"在父行程結束前free page數量：%d\n",getNumFreePages());
    wait();
    return ;
}


int main(void)
{

//1
    printf(1,"1.\n\
   int a = 1;          #宣告一個全域變數a\n\
   if(fork()==0)       #複製一個一子行程,fork==0代表子行程,fork==1代表父行程,使free page減少\n\
   {  #此為子行程的內容\n\
        a = 2;         #由於要寫入a,根據copy-on-write的概念,此時會複製出一份a,使free page減少\n\
        a = 4;         #由於前一行已複製出一份a,此時修改並不會額外複製,free page不變\n\
        exit();        #結束子行程\n\
   }\n\
    wait();            #等待子行程結束,若在此後輸出a,a的值並不會被改變\n\n");

//2
    printf(1,"2.\n\
    int a = 1;         #宣告一個全域變數a\n\
    if(fork()==0)      #複製一個一子行程,fork==0代表子行程,fork==1代表父行程,使free page減少\n\
    {  #此為子行程的內容\n\
        exit();        #結束第一個子行程,釋出free page\n\
    }\n\
    else  #執行第一個父行程\n\
    {  #此為父行程的內容\n\
        if(fork()==0)  #複製一個一子行程,此時原來的父行程將會被複製出第二份,因此會優先執行第一個子行程\n\
        {\n\
            a = 5;     #由於要寫入a,根據copy-on-write的概念,此時會複製出一份a,使free page減少\n\
            exit();    #結束第二個子行程,釋出free page\n\
        }\n\
        wait();        #由於第一個子行程已經執行完畢,此時的wait沒有任何作用\n\
    }\n\
    wait();            #等待第二個子行程結束\n\n");

//3
    printf(1,"3.\n\
    int a = 1;         #宣告一個全域變數a\n\
    int pid = fork();  #複製一個一子行程,fork==0代表子行程,fork==1代表父行程\n\
    if(pid==0)\n\
    {  #此為子行程的內容\n\
	sleep(4);      #sleep(4) 讓子行程先停止4秒再執行,因此先執行父行程\n\
        a = 5;         #由於要寫入a,根據copy-on-write的概念,此時會複製出一份a,使free page減少\n\
        exit();        #結束子行程,釋出free page\n\
    }\n\
    wait();            #等待子行程結束\n\n");

printf(1,"請輸入數字1~3選擇詳細範例的教程,輸入4離開程式.\n\n");

char *inputC;
while (1)
{
	inputC = gets("9",9);
        printf(1,"\n");
	int input = atoi(inputC);
	if(input==1)
	{
            printf(1,"******************教程一******************\n\n");
	    tutorial1();
            printf(1,"\n****************教程結束******************\n\n");
	}
	else if(input==2)
	{
 	    printf(1,"******************教程二******************\n\n");
	    tutorial2();
 	    printf(1,"\n****************教程結束******************\n\n");
	}
	else if(input==3)
	{
 	    printf(1,"******************教程三******************\n\n");
	    tutorial3();
 	    printf(1,"\n****************教程結束******************\n\n");
	} 
	else if(input==4)
	{
	    break;
	}

	    printf(1,"選擇教程,請重新輸入：\n");
}
exit();
}
