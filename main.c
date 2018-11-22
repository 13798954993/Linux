/*main.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "GameLib.h"
#include "dice.h"
#include "unistd.h"
 
extern Player player; 		//玩家
extern Computer computer;	//电脑
extern Umpire umpire;		//裁判
 
int main()
{
    int c;
    char str[10];
 
    dice_init(); //游戏初始化
 
    while((c = getchar()) != 'Q')//按键输入
    {
    	if(c == 'r' && player.status == NoReady) //玩家每局游戏刚开始的时候状态为未准备，输入'r'表示准备完毕
		{ 
			 player.status = Ready;
			 WriteStr(PLAYER_STATUS_X,PLAYER_STATUS_Y,strlen(BLANK),BLANK);
			 WriteStr(PLAYER_STATUS_X,PLAYER_STATUS_Y,strlen(READY),READY);
			 refresh();
		}
		else if(player.status == Ready && c == ' ')//玩家每按一下空格键代表摇一个骰子
		{
			 if(!player.dice1.dice_flag)player.dice1.dice_flag = 1;      	//骰子1的标志，置1表示骰子数值确定
			 else if(!player.dice2.dice_flag)player.dice2.dice_flag = 1;	//骰子2的标志，置1表示骰子数值确定
			 else if(!player.dice3.dice_flag)player.dice3.dice_flag = 1;	//骰子3的标志，置1表示骰子数值确定
			 
			 //当三个骰子都确定后，玩家的状态为操作完毕
			 if(player.dice1.dice_flag &&
					player.dice2.dice_flag &&
						player.dice3.dice_flag)
			 {
				 player.status = Finish;
				 WriteStr(PLAYER_STATUS_X,PLAYER_STATUS_Y,strlen(BLANK),BLANK);
				 WriteStr(PLAYER_STATUS_X,PLAYER_STATUS_Y,strlen(FINISH),FINISH);
			 }
		}
		
		money_rule();//判断输赢
		if(player.status == Game_Win)//玩家胜利
		{
			ClearSCR(1,5,SCR_X/2-1,SCR_Y-11);
			ClearSCR(SCR_X/2+1,5,SCR_X/2-1,SCR_Y-11);
			ClearSCR(SCR_X*1/4+1,SCR_Y-7+3,SCR_X/2,7-3);
 
			WriteStr(PLAYER_GR_X,PLAYER_GR_Y,strlen(GAME_WIN),GAME_WIN);
			WriteStr(COMPUTER_GR_X,COMPUTER_GR_Y,strlen(GAME_FAIL),GAME_FAIL);
			WriteStr(SCR_X/2,SCR_Y-3,12,"'Q' to exit");
			
			refresh();
			stop_shake();
		}
		else if(player.status == Game_Fail)//玩家失败
		{
			ClearSCR(1,5,SCR_X/2-1,SCR_Y-11);
			ClearSCR(SCR_X/2+1,5,SCR_X/2-1,SCR_Y-11);
			ClearSCR(SCR_X*1/4+1,SCR_Y-7+3,SCR_X/2,7-3);
			
			WriteStr(PLAYER_GR_X,PLAYER_GR_Y,strlen(GAME_FAIL),GAME_FAIL);
			WriteStr(COMPUTER_GR_X,COMPUTER_GR_Y,strlen(GAME_WIN),GAME_WIN);
			WriteStr(SCR_X/2,SCR_Y-3,12,"'Q' to exit");
		
			refresh();
			stop_shake();
		}
 
		//当玩家和电脑都准备好时，裁判状态为开始
		if(player.status == Ready && computer.status == Ready)
		{
			umpire.status = Start;
			WriteStr(UMPIRE_STATUS_X,UMPIRE_STATUS_Y,strlen(BLANK),BLANK);
			WriteStr(UMPIRE_STATUS_X,UMPIRE_STATUS_Y,strlen(GAME_START),GAME_START);
			refresh();
		}
 
		//裁判状态为结束时，表示游戏结束，并重置各变量
		if(umpire.status == Over)
		{
			umpire.status = NoReady;
			player.dice1.dice_flag = 0;
			player.dice2.dice_flag = 0;
			player.dice3.dice_flag = 0;
			player.status = NoReady;
			computer.dice1.dice_flag = 0;
			computer.dice2.dice_flag = 0;
			computer.dice3.dice_flag = 0;
			computer.status = NoReady;
			computer.ready_ttg = computer.ready_ttm;
			DrawDice(player.dice1.pos_x,player.dice1.pos_y,'#','?');
			DrawDice(player.dice2.pos_x,player.dice2.pos_y,'#','?');
			DrawDice(player.dice3.pos_x,player.dice3.pos_y,'#','?');
			DrawDice(computer.dice1.pos_x,computer.dice1.pos_y,'#','?');
			DrawDice(computer.dice2.pos_x,computer.dice2.pos_y,'#','?');
			DrawDice(computer.dice3.pos_x,computer.dice3.pos_y,'#','?');
			WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(TBLANK),TBLANK);
			WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(TBLANK),TBLANK);
			WriteStr(PLAYER_TOTAL_X,PLAYER_TOTAL_Y,strlen(BLANK),BLANK);
			WriteStr(COMPUTER_TOTAL_X,COMPUTER_TOTAL_Y,strlen(BLANK),BLANK);
			WriteStr(UMPIRE_STATUS_X,UMPIRE_STATUS_Y,strlen(BLANK),BLANK);
			WriteStr(PLAYER_STATUS_X,PLAYER_STATUS_Y,strlen(NOREADY),NOREADY);
			WriteStr(COMPUTER_STATUS_X,COMPUTER_STATUS_Y,strlen(NOREADY),NOREADY);
		}
		
		//当玩家和电脑都操作完毕后，计算本局输赢
		if(player.status == Finish && computer.status == Finish)
		{
			dice_rule();//计算输赢并计算金钱收益
			if(player.status == Win)//玩家本局胜利
			{
				if(player.dice_status == Ordinary)//普通牌型
				{
					 WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(TBLANK),TBLANK);
					 WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(WIN),WIN);
					 WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(TBLANK),TBLANK);
					 WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(FAIL),FAIL);
				}
				else if(player.dice_status == Three_Times)//三倍
				{
					 WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(TBLANK),TBLANK);
							 WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(THREE_TIMES),THREE_TIMES);
							 WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(TBLANK),TBLANK);
					 WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(FAIL),FAIL);
				}
				else if(player.dice_status == Five_Times)//五倍
				{
					 WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(TBLANK),TBLANK);
					 WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(FIVE_TIMES),FIVE_TIMES);
					 WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(TBLANK),TBLANK);
					 WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(FAIL),FAIL);
				}
				sprintf(str,"%d",player.dice_total);
				WriteStr(PLAYER_TOTAL_X,PLAYER_TOTAL_Y,strlen(BLANK),BLANK);
				WriteStr(PLAYER_TOTAL_X,PLAYER_TOTAL_Y,strlen(str),str);
				sprintf(str,"%d",computer.dice_total);
				WriteStr(COMPUTER_TOTAL_X,COMPUTER_TOTAL_Y,strlen(BLANK),BLANK);
				WriteStr(COMPUTER_TOTAL_X,COMPUTER_TOTAL_Y,strlen(str),str);
 
				sprintf(str,"%d",player.money);
				WriteStr(PLAYER_MONEY_X,PLAYER_MONEY_Y,strlen(BLANK),BLANK);
				WriteStr(PLAYER_MONEY_X,PLAYER_MONEY_Y,strlen(str),str);
				sprintf(str,"%d",computer.money);
				WriteStr(COMPUTER_MONEY_X,COMPUTER_MONEY_Y,strlen(BLANK),BLANK);
				WriteStr(COMPUTER_MONEY_X,COMPUTER_MONEY_Y,strlen(str),str);
 
				refresh();
			}
			else if(computer.status == Win)//电脑本局胜利
			{
				if(computer.dice_status == Ordinary)//普通牌型
				{
					WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(TBLANK),TBLANK);
					WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(FAIL),FAIL);
					WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(TBLANK),TBLANK);
					WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(WIN),WIN);
				}
				else if(computer.dice_status == Three_Times)//三倍
				{
					WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(TBLANK),TBLANK);
					WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(FAIL),FAIL);
					WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(TBLANK),TBLANK);
					WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(THREE_TIMES),THREE_TIMES);
				}
				else if(computer.dice_status == Five_Times)//五倍
				{
					WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(TBLANK),TBLANK);
					WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(FAIL),FAIL);
					WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(TBLANK),TBLANK);
					WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(FIVE_TIMES),FIVE_TIMES);
				}
 
				sprintf(str,"%d",player.dice_total);
				WriteStr(PLAYER_TOTAL_X,PLAYER_TOTAL_Y,strlen(BLANK),BLANK);
				WriteStr(PLAYER_TOTAL_X,PLAYER_TOTAL_Y,strlen(str),str);
				sprintf(str,"%d",computer.dice_total);
				WriteStr(COMPUTER_TOTAL_X,COMPUTER_TOTAL_Y,strlen(BLANK),BLANK);
				WriteStr(COMPUTER_TOTAL_X,COMPUTER_TOTAL_Y,strlen(str),str);
							   
				sprintf(str,"%d",player.money);
				WriteStr(PLAYER_MONEY_X,PLAYER_MONEY_Y,strlen(BLANK),BLANK);
				WriteStr(PLAYER_MONEY_X,PLAYER_MONEY_Y,strlen(str),str);
				sprintf(str,"%d",computer.money);
				WriteStr(COMPUTER_MONEY_X,COMPUTER_MONEY_Y,strlen(BLANK),BLANK);
				WriteStr(COMPUTER_MONEY_X,COMPUTER_MONEY_Y,strlen(str),str);
 
				refresh();
			}
			else if(player.status == Draw && computer.status == Draw)//本局平局
			{
				WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(BLANK),BLANK);
				WriteStr(PLAYER_RESULT_X,PLAYER_RESULT_Y,strlen(DRAW),DRAW);
				WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(BLANK),BLANK);
				WriteStr(COMPUTER_RESULT_X,COMPUTER_RESULT_Y,strlen(DRAW),DRAW);
				
				sprintf(str,"%d",player.dice_total);
				WriteStr(PLAYER_TOTAL_X,PLAYER_TOTAL_Y,strlen(BLANK),BLANK);
				WriteStr(PLAYER_TOTAL_X,PLAYER_TOTAL_Y,strlen(str),str);
				sprintf(str,"%d",computer.dice_total);
				WriteStr(COMPUTER_TOTAL_X,COMPUTER_TOTAL_Y,strlen(BLANK),BLANK);
				WriteStr(COMPUTER_TOTAL_X,COMPUTER_TOTAL_Y,strlen(str),str);
			
				refresh();
			}
		
			umpire.status = Over;//计算本局输赢结束后表示游戏结束，裁判状态设置为结束
			WriteStr(UMPIRE_STATUS_X,UMPIRE_STATUS_Y,strlen(BLANK),BLANK);
			WriteStr(UMPIRE_STATUS_X,UMPIRE_STATUS_Y,strlen(GAME_OVER),GAME_OVER);
			
		}     
    }
	
    dice_over();//结束游戏
    return 0;
}
