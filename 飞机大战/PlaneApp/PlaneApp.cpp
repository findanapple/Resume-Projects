#include"PlaneApp.h"
#include"../GameConfig/GameConfig.h"
#include"../GunnerBox/GunnerBox.h"
#include<time.h>
#include<string>
#include"../FoePlaneBox/FoePlaneBig.h"
#include"../FoePlaneBox/FoePlaneMid.h"
#include"../FoePlaneBox/FoePlaneSma.h"

WND_PARAM(WND_X, WND_Y, WND_GAME_WIDTH, WND_GAME_HEIGHT, WND_GAME_TITLE)  // 项目属性改成多字节
DYNAMIC_CAEATE(CPlaneApp)

#define CREATE_FOEPLANE(FORTYPE)\
	CFoePlane* pFoe = new FORTYPE;\
	pFoe->InitFoe();\
	m_foebox.m_lstFoe.push_back(pFoe);

int CPlaneApp::m_rand = 0;
CPlaneApp::CPlaneApp() {

}
CPlaneApp::~CPlaneApp() {

}
void CPlaneApp::SetTimer() {
	//设定背景移动的定时器
	::SetTimer(hWnd, TIMER_BACKGROUND_MOVE_TIMERID, TIMER_BACKGROUND_MOVE_INTERVAL, nullptr);
	//定时检测方向键是否按下
	::SetTimer(hWnd, TIMER_PLAYER_CHECKMOVE_TIMERID, TIMER_PLAYER_CHECKMOVE_INTERVAL, nullptr);
	//炮弹移动的定时器
	::SetTimer(hWnd, TIMER_GUNNER_MOVE_TIMERID, TIMER_GUNNER_MOVE_INTERVAL, nullptr);
	//炮弹发射的定时器
	::SetTimer(hWnd, TIMER_GUNNER_SEND_TIMERID, TIMER_GUNNER_SEND_INTERVAL, nullptr);
	//敌人大飞机定时出现
	::SetTimer(hWnd, TIMER_CREATE_FOEBIG_TIMERID, TIMER_CREATE_FOEBIG_INTERVAL, nullptr);
	//敌人中飞机定时出现
	::SetTimer(hWnd, TIMER_CREATE_FOEMID_TIMERID, TIMER_CREATE_FOEMID_INTERVAL, nullptr);
	//敌人小飞机定时出现
	::SetTimer(hWnd, TIMER_CREATE_FOESMA_TIMERID, TIMER_CREATE_FOESMA_INTERVAL, nullptr);
	//敌人飞机移动的定时器
	::SetTimer(hWnd, TIMER_FOESTEP_TIMERID, TIMER_FOESTEP_INTERVAL, nullptr);	
	//检测碰撞的定时器
	::SetTimer(hWnd, TIMER_CHECK_HIT_INTERID, TIMER_CHECK_HIT_INTERVAL, nullptr);
	//定时切换爆炸效果图片
	::SetTimer(hWnd, TIMER_CHANGE_BOOM_INTERID, TIMER_CHANGE_BOOM_INTERVAL, nullptr);
}

void CPlaneApp::On_Init() {
	srand(time(NULL));
	m_score = 0;
	::loadimage(&m_scoreBoard, "./res/cardboard.png");

	//背景的初始化
	m_back.InitBackGround();
	//玩家飞机的初始化
	m_player.InitPlayer();
	
	//启动所有定时器
	SetTimer();
	
}

void CPlaneApp::On_Paint() {
	//每次重绘获取随机数
	CPlaneApp::m_rand = rand();
	//显示背景图
	m_back.ShowBackGround();
	//显示玩家飞机
	m_player.ShowPlayer();
	//显示炮弹
	m_gunbox.ShowAllGunner();
	//显示所有正常敌人飞机
	m_foebox.ShowAllFoe();
	//显示所有爆炸
	m_foebox.ShowAllBoomFoe();
	//显示分数
	ShowScore();
}

void CPlaneApp::On_Close() {

}

void CPlaneApp::On_WM_TIMER(WPARAM wParam, LPARAM lParam) {
	switch (wParam) {
	case TIMER_BACKGROUND_MOVE_TIMERID:   //背景移动的定时器
	{
		m_back.MoveBackGround(TIMER_BACKGROUND_MOVE_STEP);
	}
	break;
	case TIMER_PLAYER_CHECKMOVE_TIMERID:   //定时检测方向键是否按下
	{
		if (GetAsyncKeyState(VK_LEFT)) {  //判断某个方向键是否是按下的状态
			m_player.MovePlayer(VK_LEFT, TIMER_PLAYER_MOVE_STEP);
		}
		if (GetAsyncKeyState(VK_RIGHT)) {
			m_player.MovePlayer(VK_RIGHT, TIMER_PLAYER_MOVE_STEP);
		}
		if (GetAsyncKeyState(VK_UP)) {
			m_player.MovePlayer(VK_UP, TIMER_PLAYER_MOVE_STEP);
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			m_player.MovePlayer(VK_DOWN, TIMER_PLAYER_MOVE_STEP);
		}
	}
	break;
	case TIMER_GUNNER_MOVE_TIMERID://炮弹定时发射
	{
		//发射完炮弹放到炮弹盒子的链表中
		CGunner*pGun=m_player.SendGunner();
		m_gunbox.m_lstGun.push_back(pGun); 
	}
	break;
	case TIMER_GUNNER_SEND_TIMERID://炮弹定时移动
		m_gunbox.MoveAllGunner(TIMER_GUNNER_MOVE_STEP);	
	
	break;
	case TIMER_CREATE_FOEBIG_TIMERID:
	{
		CREATE_FOEPLANE(CFoePlaneBig)
	}break;
	case TIMER_CREATE_FOEMID_TIMERID:
	{
		CREATE_FOEPLANE(CFoePlaneMid)
	}break;
	case TIMER_CREATE_FOESMA_TIMERID:
	{
		CREATE_FOEPLANE(CFoePlaneSma)
	}break;
	case TIMER_FOESTEP_TIMERID:
	{
		m_foebox.MoveAllFoe();
	}break;

	case TIMER_CHECK_HIT_INTERID:
	{
		bool isBoom = false;
		//遍历所有敌人飞机
		list<CFoePlane*>::iterator iteFoe = m_foebox.m_lstFoe.begin();
		while (iteFoe != m_foebox.m_lstFoe.end()) {
			//1.判断是否撞机玩家飞机，如果撞机，gameover
			if (*iteFoe) {
				if ((*iteFoe)->IsHitPlayer(m_player)) {
					GameOver();
					return;
				}
			}
			//判断是否撞机炮弹  循环遍历炮弹
			list<CGunner*>::iterator iteGun = m_gunbox.m_lstGun.begin();
			while (iteGun != m_gunbox.m_lstGun.end()) {
				if (*iteGun) {
					//判断敌人飞机是否撞机了炮弹
					if ((*iteFoe)->IsHitFoe(*iteGun)) {
						delete* iteGun;
						*iteGun = nullptr;
						//删除炮弹连接节点
						iteGun=m_gunbox.m_lstGun.erase(iteGun);

						(*iteFoe)->m_blood -= DEF_GUNNER_HURT;//敌人飞机掉血
						if ((*iteFoe)->m_blood == 0) {
							m_foebox.m_lstFoeBoom.push_back(*iteFoe);//将爆炸的飞机添加到爆炸的链表中

							iteFoe =m_foebox.m_lstFoe.erase(iteFoe);
							isBoom = true;
							m_score += 3;
							break;
						}
						continue;
					}
					iteGun++;
				}
			}
			if (!isBoom)	iteFoe++;
			else isBoom = false;
		}
	}break;
	case TIMER_CHANGE_BOOM_INTERID:
	{
		list<CFoePlane*>::iterator ite=m_foebox.m_lstFoeBoom.begin();
		while (ite != m_foebox.m_lstFoeBoom.end()) {
			if (*ite) {
				(*ite)->m_showid--;
				if ((*ite)->m_showid < 0) {//判断条件小于0是为了显示最后一张爆炸效果图
					//删除敌人爆炸的飞机
					delete* ite;
					*ite = nullptr;
					//删除节点
					ite = m_foebox.m_lstFoeBoom.erase(ite);
					continue;
				}
			}
			ite++;
		}
	}break;
	}
}

void CPlaneApp::GameOver() {
	//停止所有定时器
	::KillTimer(hWnd, TIMER_BACKGROUND_MOVE_TIMERID);	
	::KillTimer(hWnd, TIMER_PLAYER_CHECKMOVE_TIMERID);	
	::KillTimer(hWnd, TIMER_GUNNER_MOVE_TIMERID);	
	::KillTimer(hWnd, TIMER_GUNNER_SEND_TIMERID);	
	::KillTimer(hWnd, TIMER_CREATE_FOEBIG_TIMERID);	
	::KillTimer(hWnd, TIMER_CREATE_FOEMID_TIMERID);	
	::KillTimer(hWnd, TIMER_CREATE_FOESMA_TIMERID); 	
	::KillTimer(hWnd, TIMER_FOESTEP_TIMERID);	
	::KillTimer(hWnd, TIMER_CHECK_HIT_INTERID);

	::MessageBox(hWnd, "Game Over~~", "提示",MB_OK);//弹出框

	//给消息队列发送一个消息 WM_CLOSE
	::PostMessage(hWnd, WM_CLOSE, 0, 0);
}
void CPlaneApp::ShowScore() {
	string strScore("分数=");
	char sc[5] = { 0 };
	_itoa_s(m_score, sc, 10);
	strScore += sc;
	RECT rect = { 0,0,186,60 };
	::settextcolor(RGB(255, 0, 0));//设置文字颜色
	::putimage(0, 0, &m_scoreBoard);
	::drawtext(strScore.c_str(), &rect, DT_CENTER);//显示字符串
	
} 
void CPlaneApp::On_WM_KEYDOWN(WPARAM wparam) {
	//m_player.MovePlayer(wparam, TIMER_PLAYER_MOVE_STEP);
}


